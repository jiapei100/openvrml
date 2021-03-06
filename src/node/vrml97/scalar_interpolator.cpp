// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "scalar_interpolator.h"

namespace {

    class OPENVRML_LOCAL scalar_interpolator_node :
        public openvrml::node_impl_util::abstract_node<scalar_interpolator_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::scalar_interpolator_metatype;

        class set_fraction_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(scalar_interpolator_node & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<openvrml::mffloat> key_;
        exposedfield<openvrml::mffloat> key_value_;
        openvrml::sffloat value_changed_;
        sffloat_emitter value_changed_emitter_;

    public:
        scalar_interpolator_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~scalar_interpolator_node() OPENVRML_NOTHROW;
    };


    /**
     * @class scalar_interpolator_node
     *
     * @brief ScalarInterpolator node instances.
     */

    /**
     * @var class scalar_interpolator_node::scalar_interpolator_metatype
     *
     * @brief Class object for ScalarInterpolator nodes.
     */

    /**
     * @internal
     *
     * @class scalar_interpolator_node::set_fraction_listener
     *
     * @brief set_fraction eventIn handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  scalar_interpolator_node.
     */
    scalar_interpolator_node::set_fraction_listener::
    set_fraction_listener(scalar_interpolator_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<scalar_interpolator_node>(node),
        sffloat_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    scalar_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param fraction  fraction.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    scalar_interpolator_node::set_fraction_listener::
    do_process_event(const openvrml::sffloat & fraction, const double timestamp)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            using std::vector;

            scalar_interpolator_node & node =
                dynamic_cast<scalar_interpolator_node &>(this->node());

            const vector<float> & key = node.key_.mffloat::value();
            const vector<float> & key_value = node.key_value_.mffloat::value();

            vector<float>::size_type n = key.size() - 1;
            if (fraction.value() < key[0]) {
                node.value_changed_.value(key_value[0]);
            } else if (fraction.value() > key[n]) {
                node.value_changed_.value(key_value[n]);
            } else {
                for (vector<float>::size_type i = 0; i < n; ++i) {
                    if (key[i] <= fraction.value()
                        && fraction.value() <= key[i + 1]) {
                        float v1 = key_value[i];
                        float v2 = key_value[i + 1];

                        const float f =
                            (fraction.value() - key[i])
                            / (key[i + 1] - key[i]);
                        node.value_changed_.value(v1 + f * (v2 - v1));
                        break;
                    }
                }
            }

            // Send the new value
            node::emit_event(node.value_changed_emitter_, timestamp);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var scalar_interpolator_node::set_fraction_listener scalar_interpolator_node::set_fraction_listener_
     *
     * @brief set_fraction eventIn handler.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<scalar_interpolator_node>::exposedfield<openvrml::mffloat> scalar_interpolator_node::key_
     *
     * @brief key exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<scalar_interpolator_node>::exposedfield<openvrml::mffloat> scalar_interpolator_node::key_value_
     *
     * @brief keyValue exposedField.
     */

    /**
     * @var openvrml::sffloat scalar_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut value.
     */

    /**
     * @var openvrml::sffloat_emitter scalar_interpolator_node::value_changed_emitter_
     *
     * @brief value_changed eventOut emitter.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    scalar_interpolator_node::
    scalar_interpolator_node(const openvrml::node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<scalar_interpolator_node>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    scalar_interpolator_node::~scalar_interpolator_node()
        OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::scalar_interpolator_metatype::id =
    "urn:X-openvrml:node:ScalarInterpolator";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::scalar_interpolator_metatype::
scalar_interpolator_metatype(openvrml::browser & browser):
    node_metatype(scalar_interpolator_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::scalar_interpolator_metatype::
~scalar_interpolator_metatype() OPENVRML_NOTHROW
{}

# define SCALAR_INTERPOLATOR_INTERFACE_SEQ                             \
    ((eventin,      sffloat, "set_fraction",  set_fraction_listener_)) \
    ((exposedfield, mffloat, "key",           key_))                   \
    ((exposedfield, mffloat, "keyValue",      key_value_))             \
    ((eventout,     sffloat, "value_changed", value_changed_emitter_)) \
    ((exposedfield, sfnode,  "metadata",      metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              scalar_interpolator_metatype,
                                              scalar_interpolator_node,
                                              SCALAR_INTERPOLATOR_INTERFACE_SEQ)
