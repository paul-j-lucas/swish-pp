/*
**      SWISH++
**      src/mod/mail/mod_mail.cpp
**
**      Copyright (C) 2000-2016  Paul J. Lucas
**
**      This program is free software; you can redistribute it and/or modify
**      it under the terms of the GNU General Public License as published by
**      the Free Software Foundation; either version 2 of the License, or
**      (at your option) any later version.
**
**      This program is distributed in the hope that it will be useful,
**      but WITHOUT ANY WARRANTY; without even the implied warranty of
**      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**      GNU General Public License for more details.
**
**      You should have received a copy of the GNU General Public License
**      along with this program; if not, write to the Free Software
**      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

// local
#include "config.h"
#include "AssociateMeta.h"
#include "config.h"
#include "encoded_char.h"
#include "FilterAttachment.h"
#include "IncludeMeta.h"
#include "iso8859-1.h"
#include "meta_id.h"
#ifdef WITH_ENCODED_WORD
#include "encodings/encodings.h"
#endif /* WITH_ENCODED_WORD */
#ifdef WITH_HTML
#include "mod/html/mod_html.h"
#endif /* WITH_HTML */
#include "mod_mail.h"
#ifdef WITH_RTF
#include "mod/rtf/mod_rtf.h"
#endif /* WITH_RTF */
#include "pjl/less.h"
#include "TitleLines.h"
#include "util.h"
#include "Verbosity.h"
#include "word_util.h"

// standard
#include <algorithm>                    /* for copy() */
#include <cctype>
#include <cstring>
#include <memory>                       /* for unique_ptr */
#include <string>
#include <unistd.h>                     /* for unlink(2) */
#include <vector>

using namespace PJL;
using namespace std;

FilterAttachment                    attachment_filters;
mail_indexer::boundary_stack_type   mail_indexer::boundary_stack_;
bool                                mail_indexer::did_last_header_;

////////// local functions ////////////////////////////////////////////////////

/**
 * Checks to see if the character (or character sequence) is a newline (or
 * "internet newline," i.e., a CR-LF pair).
 *
 * @param c The pointer to start comparing at; it is assumed not to be at
 * "end".
 * @param end The pointer to the end of the range to check.
 * @return Returns \c true only if it's a newline.
 */
inline bool is_newline( char const *c, char const *end ) {
  return (c[0] == '\r' && c+1 != end && c[1] == '\n') || *c == '\n';
}

/**
 * Calls an external filter program to convert the encoded character range into
 * plain text that we know how to index.
 *
 * @param f The filter to use.
 * @param e The encoded character range to filter and index.
 */
static void index_via_filter( filter *f, encoded_char_range const &e ) {
  extern string temp_file_name_prefix;
  //
  // Create a temporary file containing the decoded bytes of an attachment.
  //
  string const temp_file_name = temp_file_name_prefix + "att";
  ofstream temp_file( temp_file_name.c_str(), ios::out | ios::binary );
  if ( !temp_file ) {
could_not_filter:
    if ( verbosity > 3 )
      cout << " (could not filter attachment)";
    return;
  }
  ::copy( e.begin(), e.end(),
    ostream_iterator<encoded_char_range::value_type>( temp_file )
  );
  temp_file.close();

  //
  // Substitute the temporary file's name into the filter command, execute the
  // filter creating a text file, and delete the temporary file containing the
  // original attachment.
  //
  f->substitute( temp_file_name );
  char const *const new_file_name = f->exec();
  ::unlink( temp_file_name.c_str() );

  if ( new_file_name ) {
    //
    // The filter worked, so now index the post-filtered file that is assumed
    // to be plain text.
    //
    static indexer *const text = indexer::find_indexer( "text" );
    mmap_file const file( new_file_name );
    if ( file && !file.empty() )
      text->index_file( file );
  } else {
    goto could_not_filter;
  }
}

////////// member functions ///////////////////////////////////////////////////

bool mail_indexer::boundary_cmp( char const *c, char const *end,
                                 char const *boundary ) {
  if ( c == end || *c != '-' || ++c == end || *c++ != '-' )
    return false;
  while ( *boundary && c != end && *boundary++ == *c++ )
    ;
  return !*boundary;
}

inline void mail_indexer::new_file() {
  boundary_stack_.clear();
  did_last_header_ = false;
}

char const* mail_indexer::find_title( mmap_file const &file ) const {
  new_file();
  unsigned lines = 0;

  for ( auto c = file.begin(); c != file.end(); ) {
    if ( is_newline( c, file.end() ) || ++lines > num_title_lines ) {
      //
      // We either ran out of headers or didn't find the Subject header within
      // first num_title_lines lines of file: forget it.
      //
      break;
    }

    //
    // Find the newline ending the header and its value and see if it's the
    // Subject header.
    //
    char const *const nl = find_newline( c, file.end() );
    if ( nl == file.end() )
      break;
    if ( move_if_match( c, nl, "subject:", true ) )
      return tidy_title( c, nl );       // found the Subject

    c = skip_newline( nl, file.end() );
  } // for

  //
  // The file has less than num_title_lines lines and no Subject was found.
  //
  return nullptr;
}

mail_indexer::message_type mail_indexer::index_headers( char const *&c,
                                                        char const *end ) {
  key_value kv;
  message_type type;

  while ( parse_header( c, end, &kv ) ) {

    ////////// Deal with Content-Transfer-Encoding ////////////////////////////

    if ( ::strcmp( kv.key.get(), "content-transfer-encoding" ) == 0 ) {
      unique_ptr<char[]> const lower_ptr(
        to_lower_r( kv.value_begin, kv.value_end )
      );
      char const *const value = lower_ptr.get();
      if ( ::strstr( value, "binary" ) ) {
        type.encoding_ = Binary;
      } else if ( ::strstr( value, "base64" ) ) {
#ifdef WITH_BASE64
        type.encoding_ = encoding_base64;
#else
        // Since Base64 encoding wasn't compiled in, we need to make it not
        // indexable: set the encoding to binary.
        //
        type.encoding_ = Binary;
#endif /* WITH_BASE64 */
#ifdef WITH_QUOTED_PRINTABLE
      } else if ( ::strstr( value, "quoted-printable" ) ) {
        type.encoding_ = encoding_quoted_printable;
      }
#else
      // Treat quoted-printable as plain text since it *is* mostly plain text.
      // (This is the best that can be done if the encoding isn't compiled in
      // and it's better than treating the text as binary and not indexing it
      // at all.)
      //
#endif /* WITH_QUOTED_PRINTABLE */
      continue;
    }

    ////////// Deal with Content-Type /////////////////////////////////////////

    if ( !::strcmp( kv.key.get(), "content-type" ) ) {
      unique_ptr<char[]> const lower_ptr(
        to_lower_r( kv.value_begin, kv.value_end )
      );
      char const *const value = lower_ptr.get();

      //
      // Extract the MIME type.
      //
      char const *s = value;
      while ( *s && isspace( *s ) ) ++s;
      if ( !*s )                        // all whitespace: weird
        continue;
      string const mime_type( s, ::strcspn( s, "; \n\r\t" ) );

      //
      // Extract the charset, if any.
      //
      char const *charset = ::strstr( value, "charset=" );
      if ( charset && *(charset += 8) ) {
        if ( *charset == '"' )
          ++charset;
        if ( !::strncmp( charset, "us-ascii", 8 ) )
          type.charset_ = US_ASCII;
        else if ( !::strncmp( charset, "iso8859-1", 9 ) )
          type.charset_ = ISO_8859_1;
#ifdef WITH_UTF7
        else if ( !::strncmp( charset, "utf-7", 5 ) )
          type.charset_ = charset_utf7;
#endif /* WITH_UTF7 */
#ifdef WITH_UTF8
        else if ( !::strncmp( charset, "utf-8", 5 ) )
          type.charset_ = charset_utf8;
#endif /* WITH_UTF8 */
        else {
          type.charset_ = CHARSET_UNKNOWN;
          goto not_indexable;
        }
      }

      //
      // See if there's a filter for the MIME type: if so, set up to use it.
      // Note that a filter can override our built-in handling of certain MIME
      // types.
      //
      if ( auto const f = attachment_filters[ mime_type ] ) {
        type.content_type_ = ct_external_filter;
        //
        // Just in case there were two Content-Type headers (weird, yes; but we
        // have to be robust) and we previously set the filter, delete the old
        // filter first so there won't be a memory leak.
        //
        delete type.filter_;
        type.filter_ = new filter( *f );
        continue;
      }

      //
      // See if it's the text/"something" or "message/rfc822".
      //
      if ( mime_type == "text/plain" )
        type.content_type_ = ct_text_plain;
      else if ( mime_type == "text/enriched" )
        type.content_type_ = ct_text_enriched;
      else if ( mime_type == "text/html" )
        type.content_type_ = ct_text_html;
      else if ( ::strstr( value, "vcard" ) )
        type.content_type_ = ct_text_vcard;
      else if ( mime_type == "message/rfc822" )
        type.content_type_ = ct_message_rfc822;

      //
      // See if it's multipart/"something", i.e., mixed, alternative, or
      // parallel: we have to extract the boundary string.
      //
      else if ( ::strstr( value, "multipart/" ) ) {
        char const *b = ::strstr( value, "boundary=" );
        if ( !b || !*(b += 9) )         // weird case
          goto not_indexable;
        //
        // Erase everything (including any surrounding quotes) except the
        // boundary string from the value.
        //
        string boundary( kv.value_begin + (b - value), kv.value_end );
        if ( boundary[0] == '"' )
          boundary.erase( 0, 1 );
        if ( boundary[ boundary.length() - 1 ] == '"' )
          boundary.erase( boundary.size()-1, 1 );
        //
        // Push the boundary onto the stack.
        //
        boundary_stack_.push_back( boundary );
        type.content_type_ = ct_multipart;
      } else {
        //
        // It's not a Content-Type we know anything about, so it's not
        // indexable.
        //
not_indexable:
        type.content_type_ = ct_unknown;
      }
    }

    ////////// Index the value of the header //////////////////////////////////

    //
    // Potentially index the words in the value of the header where they are
    // associated with the name of the header as a meta name.
    //
    meta_id_type meta_id = Meta_ID_None;
    if ( associate_meta ) {
      //
      // Do not index the words in the value of the header if either the name
      // of the header (canonicalized to lower case) is among the set of meta
      // names to exclude or not among the set to include.
      //
      if ( (meta_id = find_meta( kv.key.get() )) == Meta_ID_None )
        continue;
    }

    encoded_char_range const e(
      kv.value_begin, kv.value_end, nullptr,
#ifdef WITH_ENCODED_WORD
      encoding_encoded_word
#else
      nullptr
#endif /* WITH_ENCODED_WORD */
    );
    indexer::index_words( e, meta_id );
  } // while

  return type;
}

void mail_indexer::index_words( encoded_char_range const &e, meta_id_type ) {
  auto c = e.begin();
  message_type const type( index_headers( c.pos(), c.end_pos() ) );

  if ( type.content_type_ == ct_unknown || type.encoding_ == Binary ) {
    //
    // The attachment is something we can't index so just skip over it.
    //
    return;
  }

  //
  // Create a new encoded_char_range having the same range but the
  // Content-Transfer-Encoding given in the headers.
  //
  encoded_char_range const e2(
    c.pos(), c.end_pos(), type.charset_, type.encoding_
  );

  switch ( type.content_type_ ) {

    case ct_external_filter:
      index_via_filter( type.filter_, e2 );
      break;

    case ct_message_rfc822:
      index_words( e2 );
      break;

    case ct_multipart:
      index_multipart( c.pos(), c.end_pos() );
      boundary_stack_.pop_back();
      break;

#ifdef WITH_RTF
    case ct_text_enriched: {
      static indexer &rtf = *indexer::find_indexer( "RTF" );
      rtf.index_words( e2 );
      break;
    }
#endif /* WITH_RTF */

#ifdef WITH_HTML
    case ct_text_html: {
      static indexer &html = *indexer::find_indexer( "HTML" );
      html.index_words( e2 );
      break;
    }
#endif /* WITH_HTML */

    case ct_text_plain:
      indexer::index_words( e2 );
      break;

    case ct_text_vcard:
      index_vcard( c.pos(), c.end_pos() );
      break;

    case ct_unknown:
      // do nothing
      break;
  } // switich
}

bool mail_indexer::parse_header( char const *&c, char const *end,
                                 key_value *kv ) {
  if ( did_last_header_ )
    return did_last_header_ = false;

  char const *header_begin, *header_end, *nl;

  while ( true ) {
    if ( (nl = find_newline( c, end )) == end )
      return false;
    //
    // Parse a header by looking for the terminating ':'.
    //
    header_begin = c;
    while ( c != nl && *c != ':' )
      ++c;
    //
    // We have to check for the special case of the "From" header that doesn't
    // have the ':', i.e., the one in the envelope, not the letter, and ignore
    // it.
    //
    if ( c >= header_begin + 4 /* 4 == strlen( "From" ) */ &&
         !::strncmp( header_begin, "From ", 5 ) ) {
      if ( (c = skip_newline( nl, end )) == end )
        return false;
      continue;
    }

    if ( c == nl )                      // didn't find it: weird
      return false;
    header_end = c;
    break;
  } // while

  //
  // Parse a value.
  //
  if ( ++c == end )                     // skip past the ':'
    return false;
  kv->value_begin = c;
  while ( true ) {
    if ( (c = skip_newline( nl, end )) == end )
      break;
    //
    // See if the value is folded across multiple lines: if the first character
    // on the next line isn't whitespace, then the value isn't folded (it's the
    // next header).
    //
    if ( !isspace( *c ) )
      goto more_headers;
    //
    // The first character on the next line is whitespace: see how much of the
    // rest of the next line is also whitepace.
    //
    do {
      if ( *c == '\r' || *c == '\n' ) {
        //
        // The entire next line is whitespace: consider it the end of all the
        // headers and therefore also the end of this value.  Also skip the
        // blank line.
        //
        c = skip_newline( c, end );
        goto last_header;
      }
    } while ( ++c != end && isspace( *c ) );

    //
    // The next line has at least one non-leading non-whitespace character;
    // therefore, it is a continuation of the current header's value:
    // reposition "nl" and start over.
    //
    if ( (nl = find_newline( c, end )) == end )
      break;
  } // while

last_header:
  did_last_header_ = true;

more_headers:
  kv->value_end = nl;
  //
  // Canonicalize the name of the header to lower case.
  //
  kv->key.reset( to_lower_r( header_begin, header_end ) );
  return true;
}

///////////////////////////////////////////////////////////////////////////////
/* vim:set et sw=2 ts=2: */
