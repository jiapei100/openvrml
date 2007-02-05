// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML Mozilla plug-in
// Copyright 2004, 2005, 2006  Braden N. McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# include <iostream>
# include <sstream>
# include <boost/lexical_cast.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/thread/thread.hpp>
# include <unistd.h>
// Must include before X11 headers.
# include <boost/numeric/conversion/converter.hpp>
# include <gtk/gtk.h>
# include <openvrml/browser.h>

# ifdef HAVE_CONFIG_H
#   include "config.h"
# endif

# include "gtkvrmlbrowser.h"
# include "command_istream.h"
# include "plugin_streambuf.h"

using namespace openvrml_player;
using namespace boost::multi_index::detail; // for scope_guard

namespace {

    const char application_name[] = "OpenVRML XEmbed Control";

    //
    // We don't already know what the URI of the initial stream is until we
    // start getting that data from the browser.  This is a placeholder that
    // is used to identify the plugin_streambuf that will be used to receive
    // the initial stream data.
    //
    const char initial_stream_uri[] = "x-openvrml-initial:";
}

namespace openvrml_player {

    struct command_istream_reader {
        command_istream_reader(command_istream & in,
                               GtkVrmlBrowser & vrml_browser,
                               GMainLoop & command_channel_loop):
            in_(&in),
            vrml_browser_(&vrml_browser),
            command_channel_loop_(&command_channel_loop)
        {}

        void operator()() const throw ()
        {
            using std::string;

            string command_line;
            while (getline(*this->in_, command_line)) {
                using std::istringstream;

                istringstream command_line_stream(command_line);
                string command;
                command_line_stream >> command;
                if (command == "get-url-result") {
                    using boost::shared_ptr;

                    std::string url;
                    int result;
                    command_line_stream >> url >> result;

                    shared_ptr<plugin_streambuf> streambuf =
                        uninitialized_plugin_streambuf_map_.find(url);
                    assert(streambuf);
                    streambuf->set_get_url_result(result);
                } else if (command == "new-stream") {
                    using boost::shared_ptr;

                    size_t stream_id;
                    std::string type, url;
                    command_line_stream >> stream_id >> type >> url;

                    shared_ptr<plugin_streambuf> streambuf =
                        uninitialized_plugin_streambuf_map_.find(url);

                    static bool got_initial_stream = false;
                    if (!streambuf) {
                        if (!got_initial_stream) {
                            g_assert(uninitialized_plugin_streambuf_map_.size()
                                     == 1);
                            streambuf =
                                uninitialized_plugin_streambuf_map_.front();
                            got_initial_stream = true;
                        } else {
                            g_warning("Attempt to create an unrequested "
                                      "stream.");
                            continue;
                        }
                    }
                    streambuf->init(stream_id, url, type);
                } else if (command == "destroy-stream") {
                    size_t stream_id;
                    command_line_stream >> stream_id;
                    plugin_streambuf_map_t::iterator pos = 
                        plugin_streambuf_map.find(stream_id);
                    if (pos == plugin_streambuf_map.end()) {
                        g_warning("Attempt to destroy a nonexistent stream.");
                        continue;
                    }
                    pos->second->buf_.set_eof();
                    plugin_streambuf_map.erase(pos);
                } else if (command == "write") {
                    size_t stream_id, length;
                    command_line_stream >> stream_id >> length;
                    plugin_streambuf_map_t::const_iterator pos =
                        plugin_streambuf_map.find(stream_id);
                    if (pos == plugin_streambuf_map.end()) {
                        g_warning("Attempt to write to a nonexistent stream.");
                        continue;
                    }
                    for (size_t i = 0; i < length; ++i) {
                        pos->second->buf_.put(this->in_->get());
                    }
                } else if (command == "load-url") {
                    string url;
                    command_line_stream >> url;

                    const gchar * urls[] = { g_strdup(url.c_str()), 0 };
                    scope_guard urls_guard =
                        make_guard(g_free,
                                   const_cast<void *>(
                                       static_cast<const void *>(urls[0])));
                    boost::ignore_unused_variable_warning(urls_guard);

                    gtk_vrml_browser_load_url(this->vrml_browser_, urls, 0);
                }
            }

            //
            // Got EOF from the command stream. Time to shut down.
            //
            // First, mark any outstanding uninitialized streams as failed so
            // that another thread doesn't block waiting for them.
            //
            while (!uninitialized_plugin_streambuf_map_.empty()) {
                uninitialized_plugin_streambuf_map_.front()->fail();
            }

            //
            // Tell the command_channel_loop thread to quit.  (This is our
            // "producer" thread; it's also the thread that spawned us.)
            //
            g_main_loop_quit(this->command_channel_loop_);
        }

    private:
        command_istream * const in_;
        GtkVrmlBrowser * const vrml_browser_;
        GMainLoop * const command_channel_loop_;
    };
}

namespace {

    gboolean version, initial_stream;
    gchar ** args;

    GOptionEntry options[] = {
        {
            "version",
            'v',
            0,
            G_OPTION_ARG_NONE,
            &version,
            "Print the version information and exit",
            0
        },
        {
            "initial-stream",
            'i',
            0,
            G_OPTION_ARG_NONE,
            &initial_stream,
            "Expect an initial stream",
            0
        },
        {
            G_OPTION_REMAINING,
            '\0',
            0,
            G_OPTION_ARG_STRING_ARRAY,
            &args,
            "the embedder's window ID",
            "XID"
        },
        { 0, '\0', 0, G_OPTION_ARG_NONE, 0, 0, 0 }
    };

    struct initial_stream_reader {
        initial_stream_reader(
            const boost::shared_ptr<plugin_streambuf> & streambuf,
            GtkVrmlBrowser & vrml_browser):
            streambuf_(streambuf),
            vrml_browser_(&vrml_browser)
        {}

        void operator()() const throw ()
        {
            class plugin_istream : public openvrml::resource_istream {
                boost::shared_ptr<plugin_streambuf> streambuf_;

            public:
                explicit plugin_istream(
                    const boost::shared_ptr<plugin_streambuf> & streambuf):
                    openvrml::resource_istream(streambuf.get()),
                    streambuf_(streambuf)
                {}

                virtual ~plugin_istream() throw ()
                {}

            private:
                virtual const std::string do_url() const throw (std::bad_alloc)
                {
                    return this->streambuf_->url();
                }

                virtual const std::string do_type() const
                    throw (std::bad_alloc)
                {
                    return this->streambuf_->type();
                }

                virtual bool do_data_available() const throw ()
                {
                    return this->streambuf_->data_available();
                }
            } in(this->streambuf_);
            gtk_vrml_browser_set_world(this->vrml_browser_, in);
        }

    private:
        boost::shared_ptr<plugin_streambuf> streambuf_;
        GtkVrmlBrowser * vrml_browser_;
    };

    struct command_channel_loop {
        command_channel_loop(GIOChannel & command_channel,
                             command_istream & command_in,
                             GtkVrmlBrowser & vrml_browser):
            command_channel_(&command_channel),
            command_in_(&command_in),
            vrml_browser_(&vrml_browser)
        {}

        void operator()() const throw ()
        {
            using boost::function0;
            using boost::scoped_ptr;
            using boost::thread;

            GMainContext * const main_context = g_main_context_new();
            scope_guard main_context_guard = make_guard(g_main_context_unref,
                                                        main_context);
            boost::ignore_unused_variable_warning(main_context_guard);
            GMainLoop * const main_loop = g_main_loop_new(main_context, false);
            scope_guard main_loop_guard = make_guard(g_main_loop_unref,
                                                     main_loop);
            boost::ignore_unused_variable_warning(main_loop_guard);

            GSource * const command_watch =
                g_io_create_watch(this->command_channel_,
                                  GIOCondition(G_IO_IN | G_IO_HUP));
            scope_guard command_watch_guard = make_guard(g_source_unref,
                                                         command_watch);
            boost::ignore_unused_variable_warning(command_watch_guard);

            static const GDestroyNotify notify = 0;
            g_source_set_callback(
                command_watch,
                reinterpret_cast<GSourceFunc>(::command_data_available),
                static_cast<command_streambuf *>(this->command_in_->rdbuf()),
                notify);
            const guint command_watch_id = g_source_attach(command_watch,
                                                           main_context);
            g_assert(command_watch_id != 0);

            //
            // The command_istream_reader thread is our "consumer" thread.
            //
            function0<void> read_commands =
                command_istream_reader(*this->command_in_,
                                       *this->vrml_browser_,
                                       *main_loop);
            const scoped_ptr<thread>
                command_reader_thread(new thread(read_commands));

            g_main_loop_run(main_loop);

            //
            // If we're here, our consumer thread is done.  Join it and tell
            // the GTK+ main loop to quit.
            //
            command_reader_thread->join();

            gtk_main_quit();
        }

    private:
        GIOChannel * const command_channel_;
        command_istream * const command_in_;
        GtkVrmlBrowser * const vrml_browser_;
    };

    G_GNUC_INTERNAL void
    command_channel_shutdown(GIOChannel * command_channel);

    G_GNUC_INTERNAL void
    request_channel_shutdown(GIOChannel * request_channel);
}

int main(int argc, char * argv[])
{
    using std::cout;
    using std::cerr;
    using std::endl;
    using std::vector;
    using boost::function0;
    using boost::ref;
    using boost::shared_ptr;
    using boost::thread;
    using boost::thread_group;

    using namespace openvrml_player;

    g_thread_init(0);
    gdk_threads_init();

    g_set_application_name(application_name);

    gtk_init(&argc, &argv);
    gtk_gl_init(&argc, &argv);

    GError * error = 0;
    scope_guard error_guard = make_guard(g_error_free, ref(error));

    GOptionContext * const context =
        g_option_context_new("- render VRML worlds");
    const gchar * const translation_domain = 0;
    g_option_context_add_main_entries(context, options, translation_domain);
    g_option_context_add_group(context, gtk_get_option_group(true));
    gboolean succeeded = g_option_context_parse(context, &argc, &argv, &error);
    if (!succeeded) {
        if (error) { g_critical(error->message); }
        return EXIT_FAILURE;        
    }

    if (version) {
        cout << application_name << ' ' << PACKAGE_VERSION << endl;
        return EXIT_SUCCESS;
    }

    if (!::args) {
        cerr << argv[0] << ": missing required XID argument" << endl;
        return EXIT_FAILURE;
    }

    GdkNativeWindow socket_id;
    try {
        socket_id = boost::lexical_cast<GdkNativeWindow>(::args[0]);
    } catch (const boost::bad_lexical_cast & ex) {
        cerr << argv[0] << ": expected integer value for XID argument" << endl;
        return EXIT_FAILURE;
    }

    GIOChannel * const request_channel = g_io_channel_unix_new(1); // stdout
    g_return_val_if_fail(request_channel, EXIT_FAILURE);
    scope_guard request_channel_guard = make_guard(request_channel_shutdown,
                                                   request_channel);
    boost::ignore_unused_variable_warning(request_channel_guard);
    GIOStatus status = g_io_channel_set_encoding(request_channel,
                                                 0, // binary (no encoding)
                                                 &error);
    if (status != G_IO_STATUS_NORMAL) {
        if (error) { g_critical(error->message); }
        return EXIT_FAILURE;
    }

    GtkWidget * const window =  gtk_plug_new(socket_id);
    g_return_val_if_fail(window, EXIT_FAILURE);

    GtkWidget * const vrml_browser = gtk_vrml_browser_new(request_channel);
    g_return_val_if_fail(vrml_browser, EXIT_FAILURE);
    gtk_container_add(GTK_CONTAINER(window), vrml_browser);    

    gtk_widget_show_all(window);

    thread_group threads;

    GIOChannel * const command_channel = g_io_channel_unix_new(0); // stdin
    g_return_val_if_fail(command_channel, EXIT_FAILURE);
    scope_guard command_channel_guard = make_guard(command_channel_shutdown,
                                                   command_channel);
    boost::ignore_unused_variable_warning(command_channel_guard);
    
    error = 0;
    status = g_io_channel_set_encoding(command_channel,
                                       0, // binary (no encoding)
                                       &error);
    if (status != G_IO_STATUS_NORMAL) {
        if (error) { g_critical(error->message); }
        return EXIT_FAILURE;
    }

    command_istream command_in;

    function0<void> command_channel_loop_func =
        command_channel_loop(*command_channel,
                             command_in,
                             *GTK_VRML_BROWSER(vrml_browser));
    threads.create_thread(command_channel_loop_func);

    if (::initial_stream) {
        const shared_ptr<plugin_streambuf> initial_stream(
            new plugin_streambuf(::initial_stream_uri));
        uninitialized_plugin_streambuf_map_.insert(::initial_stream_uri,
                                                   initial_stream);
        const function0<void> initial_stream_reader_func =
            initial_stream_reader(initial_stream,
                                  *GTK_VRML_BROWSER(vrml_browser));
        threads.create_thread(initial_stream_reader_func);
    }

    gtk_main();

    threads.join_all();

    error_guard.dismiss();
}

namespace {

    void command_channel_shutdown(GIOChannel * const command_channel)
    {
        using boost::ref;

        static const gboolean flush = true;
        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, ref(error));
        const GIOStatus status = g_io_channel_shutdown(command_channel,
                                                       flush,
                                                       &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) { g_critical(error->message); }
        }
        g_io_channel_unref(command_channel);
        error_guard.dismiss();
    }

    void request_channel_shutdown(GIOChannel * const request_channel)
    {
        using boost::ref;

        static const gboolean flush = false;
        GError * error = 0;
        scope_guard error_guard = make_guard(g_error_free, ref(error));
        GIOStatus status = g_io_channel_shutdown(request_channel,
                                                 flush,
                                                 &error);
        if (status != G_IO_STATUS_NORMAL) {
            if (error) { g_critical(error->message); }
        }
        g_io_channel_unref(request_channel);
        error_guard.dismiss();
    }
}