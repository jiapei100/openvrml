//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fstream>

#include "Doc.h"
#include "doc2.hpp"
#include "System.h"

namespace OpenVRML {

/**
 * @class Doc
 *
 * @brief A class to contain document references.
 *
 * This is just a shell until a real http protocol library is found...
 */

/**
 * @var char * Doc::d_url
 *
 * @brief The URL.
 */

/**
 * @var std::ostream * Doc::d_ostream
 *
 * @brief A pointer to a std::ostream used for writing the resource.
 */

/**
 * @var FILE * Doc::d_fp
 *
 * @brief A file descriptor for reading the local copy of the resource.
 */

/**
 * @var char * Doc::d_tmpfile
 *
 * @brief Name of the temporary file created for the local copy of the resource.
 */

/**
 * @brief Constructor.
 *
 * @param url       an HTTP or file URL.
 * @param relative  the Doc that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
Doc::Doc(const std::string & url, const Doc * relative): d_url(0), d_ostream(0),
        d_fp(0), d_tmpfile(0) {
    if (url.length() > 0) {
        this->seturl(url.c_str(), relative);
    }
}

/**
 * @brief Constructor.
 *
 * @param url       an HTTP or file URL.
 * @param relative  the Doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
Doc::Doc(const std::string & url, const Doc2 * relative): d_url(0),
        d_ostream(0), d_fp(0), d_tmpfile(0) {
    if (url.length() > 0) {
        this->seturl(url.c_str(), relative);
    }
}

/**
 * @brief Destructor.
 */
Doc::~Doc()
{
  delete [] d_url;
  delete d_ostream;
  if (d_tmpfile)
    {
      theSystem->removeFile(d_tmpfile);
      delete [] d_tmpfile;
      d_tmpfile = 0;
    }
}

namespace {
    const char * stripProtocol(const char *url)
    {
      const char *s = url;

#ifdef _WIN32
      if (strncmp(s+1,":/",2) == 0) return url;
#endif

      // strip off protocol if any
      while (*s && isalpha(*s)) ++s;

      if (*s == ':')
        return s + 1;

      return url;
    }

    bool isAbsolute(const char *url)
    {
      const char *s = stripProtocol(url);
      return ( *s == '/' || *(s+1) == ':' );
    }
}

/**
 * @brief Set the URL.
 *
 * @param url       the new URL.
 * @param relative  the Doc that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
void Doc::seturl(const char * url, const Doc * relative)
{
  delete [] d_url;
  d_url = 0;

  if (url)
  {
      const char *path = "";

#ifdef _WIN32
// Convert windows path stream to standard URL
	  char *p = (char *)url;
	  for(;*p != '\0';p++)
		  if(*p == '\\')*p = '/';
#endif

      if ( relative && ! isAbsolute(url) )
	    path = relative->urlPath();

      d_url = new char[strlen(path) + strlen(url) + 1];
      strcpy(d_url, path);

      if (strlen(url)>2 && url[0] == '.' && url[1] == '/')
        strcat(d_url, url+2); // skip "./"
      else
        strcat(d_url, url);
  }
}

/**
 * @brief Set the URL.
 *
 * @param url       the new URL.
 * @param relative  the Doc2 that @p url is relative to, or 0 if @p url is an
 *                  absolute URL.
 */
void Doc::seturl(const char * const url, const Doc2 * const relative)
{
    delete [] this->d_url;
    this->d_url = 0;

    if (url) {
        std::string path;

#ifdef _WIN32
        // Convert windows path stream to standard URL
        char *p = (char *)url;
        for (; *p != '\0'; p++) { if (*p == '\\') { *p = '/'; } }
#endif

        if (relative && !isAbsolute(url)) { path = relative->urlPath(); }

        this->d_url = new char[path.length() + strlen(url) + 1];
        strcpy(this->d_url, path.c_str());

        if (strlen(url) > 2 && url[0] == '.' && url[1] == '/') {
            strcat(this->d_url, url + 2); // skip "./"
        } else {
            strcat(this->d_url, url);
        }
    }
}

/**
 * @brief Get the URL.
 *
 * @return the URL.
 */
const char * Doc::url() const { return d_url; }

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      without its extension.
 *
 * @return the portion of the last path element preceding the last '.' in the
 *      path, or an empty string if the last path element is empty.
 */
const char * Doc::urlBase() const
{
  if (! d_url) return "";

  static char path[1024];
  char *p, *s = path;
  strncpy(path, d_url, sizeof(path)-1);
  path[sizeof(path)-1] = '\0';
  if ((p = strrchr(s, '/')) != 0)
    s = p+1;
  else if ((p = strchr(s, ':')) != 0)
    s = p+1;

  if ((p = strrchr(s, '.')) != 0)
    *p = '\0';

  return s;
}

/**
 * @brief Get the portion of the path likely to correspond to a file name
 *      extension.
 *
 * @return the portion of the last path element succeeding the last '.' in the
 *      path, or an empty string if the last path element includes no '.'.
 */
const char * Doc::urlExt() const
{
  if (! d_url) return "";

  static char ext[20];
  char *p;

  if ((p = strrchr(d_url, '.')) != 0)
    {
      strncpy(ext, p+1, sizeof(ext)-1);
      ext[sizeof(ext)-1] = '\0';
    }
  else
    ext[0] = '\0';

  return &ext[0];
}

/**
 * @brief Get the URL without the last component of the path.
 *
 * In spite of its name, this method does not return the URL's path.
 *
 * @return the portion of the URL including the scheme, the authority, and all
 *      but the last component of the path.
 */
const char * Doc::urlPath() const
{
  if (! d_url) return "";

  static char path[1024];

  strcpy(path, d_url);
  char *slash;
  if ((slash = strrchr(path, '/')) != 0)
    *(slash+1) = '\0';
  else
    path[0] = '\0';
  return &path[0];
}

/**
 * @brief Get the URL scheme.
 *
 * @return the URL scheme.
 */
const char * Doc::urlProtocol() const
{
  if (d_url)
    {
      static char protocol[12];
      const char *s = d_url;

#ifdef _WIN32
      if (strncmp(s+1,":/",2) == 0) return "file";
#endif

      for (unsigned int i=0; i<sizeof(protocol); ++i, ++s)
	{
	  if (*s == 0 || ! isalpha(*s))
	    {
	      protocol[i] = '\0';
	      break;
	    }
	  protocol[i] = tolower(*s);
	}
      protocol[sizeof(protocol)-1] = '\0';
      if (*s == ':')
	return protocol;
    }

  return "file";
}

/**
 * @brief Get the fragment identifier.
 *
 * @return the fragment identifier, including the leading '#', or an empty
 *      string if there is no fragment identifier.
 */
const char * Doc::urlModifier() const
{
  char *mod = d_url ? strrchr(d_url,'#') : 0;
  return mod;
}

/**
 * @brief Get the fully qualified name of a local file that is the downloaded
 *      resource at @a d_url.
 *
 * @return the fully qualified name of a local file that is the downloaded
 *      resource at @a d_url.
 */
const char * Doc::localName()
{
  static char buf[1024];
  if (filename(buf, sizeof(buf)))
    return &buf[0];
  return 0;
}

/**
 * @brief Get the path of the local file that is the downloaded resource at
 *      @a d_url.
 *
 * @return the path of the local file that is the downloaded resource at
 *      @a d_url.
 */
const char * Doc::localPath()
{
  static char buf[1024];
  if (filename(buf, sizeof(buf)))
    {
      char *s = strrchr(buf, '/');
      if (s) *(s+1) = '\0';
      return &buf[0];
    }
  return 0;
}

/**
 * @brief Converts a url into a local filename.
 *
 * @retval fn   a character buffer to hold the local filename.
 * @param nfn   the number of elements in the buffer @p fn points to.
 */
bool Doc::filename(char * fn, int nfn)
{
  fn[0] = '\0';

  char *e = 0, *s = (char*) stripProtocol(d_url);

  if ((e = strrchr(s,'#')) != 0)
    *e = '\0';

  const char *protocol = urlProtocol();

  // Get a local copy of http files
  if (strcmp(protocol, "http") == 0)
    {
      if (d_tmpfile)		// Already fetched it
	s = d_tmpfile;
      else if ((s = (char *) theSystem->httpFetch(d_url)))
	{
	  d_tmpfile = new char[strlen(s)+1];
	  strcpy(d_tmpfile, s);
	  free(s);		// assumes tempnam or equiv...
	  s = d_tmpfile;
	}
    }

  // Unrecognized protocol (need ftp here...)
  else if (strcmp(protocol, "file") != 0)
    s = 0;

#ifdef _WIN32
  // Does not like "//C:" skip "// "
   if(s)
	   if(strlen(s)>2 && s[0] == '/' && s[1] == '/')s=s+2;
#endif

  if (s)
    {
      strncpy( fn, s, nfn-1 );
      fn[nfn-1] = '\0';
    }

  if (e) *e = '#';

  return s && *s;
}


#ifdef macintosh

inline char convertHex(char c)
{
    static char converted;
    if (c>='0' && c<='9')
        converted = c-'0';
    else
        if (c>='A' && c<='F')
            converted = c-'A'+10;
        else
            converted = c-'a'+10;
    return converted;
}

char* decodePath(const char* path)
{
    static char converted[256];
    strcpy (converted, path);

    char * a = converted;
    char * b = converted;

    while(*a) {
	    if (*a == '%') {
		    a++;
		    if (*a)
		    	*b = convertHex(*a++) * 16;
		    if (*a)
		    	*b = *b+convertHex(*a);
		    a++, b++;
		} else {
		    *b++ = *a++;
		}
    }
    *b++ = 0;

    return &converted[0];
}


char* Doc::convertCommonToMacPath( char *fn, int nfn )
{
  /* Note that only full paths can be use on the Mac to
     retrieve files correctly, so this function assumes
     that the viewer, e.g. Lookat, has provided the Browser with
     a file path in the form of a URL (optionally without the protocol
     if it is a local path) */

  static char macfn[256];

  // We start at index 3 in order to skip the /// at the start
  // of a legal Mac file protocol URL

  if ( !((nfn > 3) && (fn[0] == '/') && (fn[1] == '/') && (fn[2] == '/')) ) {
    return fn; // its either a tmp file from a URL transfer or its an incorrect path
  }

  int macfnpos = 0;
  for ( int i = 3; i < nfn; i++ ) {
    if ( fn[i] == '/' ) {
      macfn[macfnpos] = ':';
      macfnpos++;
    }
    else {
      if ( fn[i] == '.' ) {
         if ( (i+2 < nfn) && (fn[i+1] == '.') && (fn[i+2] == '/') ) {
           // replace "../" with an extra :
           macfn[macfnpos] = ':';
           macfnpos++;
           i=i+2;
        }
        else {
          macfn[macfnpos] = fn[i];
          macfnpos++;
        }
      }
      else {
        macfn[macfnpos] = fn[i];
        macfnpos++;
      }
    }
  }
  return decodePath(macfn);
}
#endif /* macintosh */

/**
 * @brief Open a file.
 *
 * @return a pointer to a FILE struct for the opened file.
 *
 * Having both fopen and outputStream is dumb.
 */
FILE *Doc::fopen(const char *mode)
{
  if (d_fp)
    theSystem->error("Doc::fopen: %s is already open.\n", d_url ? d_url : "");

  char fn[256];
  if (filename(fn, sizeof(fn)))
    {
      if (strcmp(fn, "-") == 0)
	{
	  if (*mode == 'r')
	    d_fp = stdin;
	  else if (*mode == 'w')
	    d_fp = stdout;
	}
      else
	{
# ifdef macintosh
      d_fp = ::fopen( convertCommonToMacPath(fn, sizeof(fn)), mode );
# else
      d_fp = ::fopen( fn, mode );
# endif
	}
    }

  return d_fp;
}

/**
 * @brief Close a file.
 *
 * Closes the file opened with Doc::fopen.
 */
void Doc::fclose()
{
  if (d_fp && (strcmp(d_url, "-") != 0) && (strncmp(d_url, "-#", 2) != 0))
    ::fclose(d_fp);

  d_fp = 0;
  if (d_tmpfile)
    {
# ifdef macintosh
      theSystem->removeFile(convertCommonToMacPath(d_tmpfile, sizeof(d_tmpfile)));
# else
      theSystem->removeFile(d_tmpfile);
# endif
      delete [] d_tmpfile;
      d_tmpfile = 0;
    }
}

/**
 * @brief Get an output stream for writing to the resource.
 *
 * @return an output stream.
 */
std::ostream & Doc::outputStream()
{
  d_ostream = new std::ofstream(stripProtocol(d_url), std::ios::out);
  return *d_ostream;
}

} // namespace OpenVRML
