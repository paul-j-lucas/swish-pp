/*
**      SWISH++
**      src/indexer.c
**
**      Copyright (C) 2000  Paul J. Lucas
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
#include "encoded_char.h"
#include "file_info.h"
#include "ExcludeMeta.h"
#include "IncludeMeta.h"
#include "indexer.h"
#include "iso8859-1.h"
#include "meta_map.h"
#include "stop_words.h"
#include "StoreWordPositions.h"
#include "util.h"
#include "word_info.h"
#include "word_util.h"

// standard
#include <cstring>

using namespace PJL;
using namespace std;

extern unsigned long    num_indexed_words;
extern unsigned long    num_total_words;
#ifdef  FEATURE_word_pos
extern int              word_pos;
#endif
extern word_map         words;

int                     indexer::suspend_indexing_count_ = 0;
indexer*                indexer::text_indexer_ = 0;

//*****************************************************************************
//
// SYNOPSIS
//
        indexer::indexer( char const *mod_name )
//
// DESCRIPTION
//
//      Construct an indexer module by adding its name to the map of indexers.
//
// PARAMETERS
//
//      mod_name    The name of the indexer module.
//
//*****************************************************************************
{
    indexer *&i = map_ref()[ to_lower( mod_name ) ];
    if ( i ) {
        internal_error
            << "indexer::indexer(\"" << mod_name << "\"): "
               "registered more than once" << report_error;
    }
    i = this;
}

//*****************************************************************************
//
// SYNOPSIS
//
        indexer::~indexer()
//
// DESCRIPTION
//
//      Destroy an indexer.
//
// NOTE
//
//      This is out-of-line only because it's virtual.
//
//*****************************************************************************
{
    // do nothing
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ option_stream::spec*
        indexer::all_mods_options( option_stream::spec const *main_spec )
//
// DESCRIPTION
//
//      Build a combined option specification of the main indexing options plus
//      any additional ones of indexing modules.
//
// PARAMETERS
//
//      main_spec   The option specification for the main part of index(1).
//
// RETURN VALUE
//
//      Returns a pointer to an array of option_stream::spec.  It should be
//      deleted after use (with delete[]).
//
//*****************************************************************************
{
    option_stream::spec const *s;

    ////////// Count all options //////////////////////////////////////////////

    int option_count = 0;
    for ( s = main_spec; s->long_name; ++s )
        ++option_count;
    FOR_EACH( map_type, map_ref(), mod )
        if ( s = mod->second->option_spec() )
            while ( s->long_name )
                ++option_count, ++s;

    ////////// Make combined option_spec //////////////////////////////////////

    option_stream::spec *const
        combined_spec = new option_stream::spec[ option_count + 1 ];
    option_stream::spec *c = combined_spec;

    for ( s = main_spec; s->long_name; ++s )
        *c++ = *s;
    FOR_EACH( map_type, map_ref(), mod )
        if ( s = mod->second->option_spec() )
            while ( s->long_name )
                *c++ = *s++;
    c->long_name  = 0;
    c->arg_type   = 0;
    c->short_name = 0;
    return combined_spec;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void indexer::all_mods_post_options()
//
// DESCRIPTION
//
//      This function is called to give all indexer modules a chance to do
//      things just after command-line options have been processed.
//
//*****************************************************************************
{
    TRANSFORM_EACH( map_type, map_ref(), mod )
        mod->second->post_options();
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */
        bool indexer::any_mod_claims_option( option_stream::option const &opt )
//
// DESCRIPTION
//
//      This function is callled to see if any indexing module claims a given
//      option.
//
// RETURN VALUE
//
//      Returns true only if any indexing module claims the option.
//
//*****************************************************************************
{
    TRANSFORM_EACH( map_type, map_ref(), mod )
        if ( mod->second->claims_option( opt ) )
            return true;
    return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void indexer::all_mods_usage( ostream &o )
//
// DESCRIPTION
//
//      Write all indexing-module-specific usage options, if any, to a given
//      ostream.
//
// PARAMETERS
//
//      o   The ostream to write the usage messages to.
//
//*****************************************************************************
{
    FOR_EACH( map_type, map_ref(), mod )
        mod->second->usage( o );
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */
        bool indexer::claims_option( option_stream::option const& )
//
// DESCRIPTION
//
//      See if an indexing module claims an option.  The default doesn't.  A
//      derived indexer that does should override this function.
//
// PARAMETERS
//
//      Not used.
//
// RETURN VALUE
//
//      Returns false.
//
//*****************************************************************************
{
    return false;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ int indexer::find_meta( char const *meta_name )
//
// DESCRIPTION
//
//      Look up a meta name to get its associated unique integer ID; if the
//      meta name didn't exist, add it, or perhaps a reassigned name of it.
//      However, if the name is either among the set of meta names to exclude
//      or not among the set to include, forget it.
//
// PARAMETERS
//
//      meta_name   The meta name to find.
//
// RETURN VALUE
//
//      Returns the ID of the given meta name or Meta_ID_None if the meta name
//      is either excluded or not included.
//
//*****************************************************************************
{
    if ( exclude_meta_names.contains( meta_name ) )
        return Meta_ID_None;

    if ( !include_meta_names.empty() ) {
        //
        // There were meta names explicitly given: see if the meta name is
        // among them.  If not, forget it; if so, possibly reassign the name.
        //
        IncludeMeta::const_iterator const
            m = include_meta_names.find( meta_name );
        if ( m == include_meta_names.end() )
            return Meta_ID_None;
        meta_name = m->second;
    }

    //
    // Look up the meta name to get its associated unique integer ID.
    //
    meta_map::const_iterator const i = meta_names.find( meta_name );
    if ( i != meta_names.end() )
        return i->second;
    //
    // New meta name: add it.  Do this in two statements intentionally because
    // C++ doesn't guarantee that the RHS of assignment is evaluated first.
    //
    int const meta_id = static_cast<int>( meta_names.size() );
    return meta_names[ new_strdup( meta_name ) ] = meta_id;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */ char const* indexer::find_title( mmap_file const& ) const
//
// DESCRIPTION
//
//      Define the default find_title() function that simply returns null
//      indicating that the file has no meaningful title (like plain text
//      files don't).
//
// RETURN VALUE
//
//      Returns null.
//
//*****************************************************************************
{
    return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ void indexer::index_word(
            register char *word, register int len, int meta_id
        )
//
// DESCRIPTION
//
//      Potentially index the given word.
//
// PARAMETERS
//
//      word        The candidate word to be indexed.
//
//      len         The length of the word since it is not null-terminated.
//
//      meta_id     The numeric ID of the meta name the word, if indexed, is to
//                  be associated with.
//
//*****************************************************************************
{
    ++num_total_words;
#ifdef  FEATURE_word_pos
    ++word_pos;
#endif

    if ( len < Word_Hard_Min_Size )
        return;

    if ( suspend_indexing_count_ > 0 ) {
        //
        // A derived indexer class has called suspend_indexing(), so do nothing
        // more.
        //
        // This facility is currently used by HTML_indexer to indicate that the
        // word is within an HTML or XHTML element's begin/end tags whose begin
        // tag's CLASS attribute value is among the set of class names not to
        // index, so do nothing.
        //
        return;
    }

    ////////// Strip chars not in Word_Begin_Chars/Word_End_Chars /////////////

    for ( register int i = len - 1; i >= 0; --i ) {
        if ( is_word_end_char( word[ i ] ) )
            break;
        --len;
    }
    if ( len < Word_Hard_Min_Size )
        return;

    word[ len ] = '\0';

    while ( *word ) {
        if ( is_word_begin_char( *word ) )
            break;
        --len, ++word;
    }
    if ( len < Word_Hard_Min_Size )
        return;

    ////////// Stop-word checks ///////////////////////////////////////////////

    if ( !is_ok_word( word ) )
        return;

    char const *const lower_word = to_lower( word );
    if ( stop_words->contains( lower_word ) )
        return;

    ////////// Add the word ///////////////////////////////////////////////////

    file_info::inc_words();
    ++num_indexed_words;

    word_info &wi = words[ lower_word ];
    ++wi.occurrences_;

    if ( !wi.files_.empty() ) {
        //
        // We've seen this word before: determine whether we've seen it before
        // in THIS file, and, if so, increment the number of occurrences.
        //
        word_info::file &last_file = wi.files_.back();
        if ( last_file.index_ == file_info::current_index() ) {
            ++last_file.occurrences_;
            goto skip_push_back;
        }
    }

    //
    // First time word occurred in current file.
    //
    wi.files_.push_back( word_info::file( file_info::current_index() ) );

skip_push_back:
    word_info::file &last_file = wi.files_.back();
    if ( meta_id != Meta_ID_None )
        last_file.meta_ids_.insert( meta_id );
#ifdef  FEATURE_word_pos
    if ( store_word_positions )
        last_file.add_word_pos( word_pos );
#endif
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */ void indexer::index_words(
            encoded_char_range const &e, int meta_id
        )
//
// DESCRIPTION
//
//      Index the words between the given iterators.  The text is assumed to be
//      plain text.
//
// PARAMETERS
//
//      e           The encoded text to index.
//
//      meta_id     The numeric ID of the meta name the words index are to be
//                  associated with.
//
//*****************************************************************************
{
    char    word[ Word_Hard_Max_Size + 1 ];
    bool    in_word = false;
    int     len;

    encoded_char_range::const_iterator c = e.begin();
    while ( !c.at_end() ) {
        register char const ch = iso8859_1_to_ascii( *c++ );

        ////////// Collect a word /////////////////////////////////////////////

        if ( is_word_char( ch ) ) {
            if ( !in_word ) {
                // start a new word
                word[ 0 ] = ch;
                len = 1;
                in_word = true;
                continue;
            }
            if ( len < Word_Hard_Max_Size ) {
                // continue same word
                word[ len++ ] = ch;
                continue;
            }
            in_word = false;                    // too big: skip chars
            while ( !c.at_end() && is_word_char( iso8859_1_to_ascii( *c++ ) ) )
                ;
            continue;
        }

        if ( in_word ) {
            //
            // We ran into a non-word character, so index the word up to, but
            // not including, it.
            //
            in_word = false;
            index_word( word, len, meta_id );
        }
    }
    if ( in_word ) {
        //
        // We ran into 'end' while still accumulating characters into a word,
        // so just index what we've got.
        //
        index_word( word, len, meta_id );
    }
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* static */ indexer::map_type& indexer::map_ref()
//
// DESCRIPTION
//
//      Define and initialize (exactly once) a static data member for indexer
//      and return a reference to it.  The reason for this function is to
//      guarantee that the map is initialized before its first use across all
//      translation units, something that would not guaranteed if it were a
//      static data member initialized at file scope.
//
//      We also initialize the map with pointers to the singleton instances of
//      all derived class indexers.
//
// RETURN VALUE
//
//      Returns a reference to a static instance of an initialized map_type.
//
// SEE ALSO
//
//      Margaret A. Ellis and Bjarne Stroustrup.  "The Annotated C++
//      Reference Manual."  Addison-Wesley, Reading, MA, 1990.  p. 19.
//
//*****************************************************************************
{
    static map_type m;
    static bool init;
    if ( !init ) {
        init = true;                    // must set this before init_modules()
        init_modules();                 // defined in init_modules.c
        static indexer text( "text" );
        text_indexer_ = &text;
    }
    return m;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */ option_stream::spec const* indexer::option_spec() const
//
// DESCRIPTION
//
//      Return a module-specific option specification.  The default returns
//      none.  A derived indexer that has its own command-line options should
//      override this function.
//
// RETURN VALUE
//
//      Returns null.
//
//*****************************************************************************
{
    return 0;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */ void indexer::post_options()
//
// DESCRIPTION
//
//      The default does nothing after command-line options are processed.
//
//*****************************************************************************
{
    // do nothing
}

//*****************************************************************************
//
// SYNOPSIS
//
        char* indexer::tidy_title( char const *begin, char const *end )
//
// DESCRIPTION
//
//      "Tidy up" a title string by trimming leading and trailing whitespace,
//      squeezing multiple spaces to single spaces, and converting all
//      non-space whitespace characters to spaces.
//
//      Additionally, if the length of the title exceeds Title_Max_Size, then
//      the title is truncated and the last 3 characters of the truncated title
//      are replaced with an ellipsis ("...").
//
// PARAMETERS
//
//      begin   The pointer to the beginning of the title.
//
//      end     The pointer to one past the end of the title.
//
// RETURN VALUE
//
//      Returns the title.
//
//*****************************************************************************
{
    // Remove leading spaces
    while ( begin < end && is_space( *begin ) )
        ++begin;

    // Remove trailing spaces
    while ( begin < --end && is_space( *end ) ) ;
    ++end;

    // Squeeze/convert multiple whitespace characters to single spaces.
    static char title[ Title_Max_Size + 1 ];
    int consec_spaces = 0, len = 0;
    while ( begin < end ) {
        char c = *begin++;
        if ( is_space( c ) ) {
            if ( ++consec_spaces >= 2 )
                continue;
            c = ' ';
        } else
            consec_spaces = 0;

        title[ len++ ] = c;
        if ( len == Title_Max_Size ) {
            ::strcpy( title + Title_Max_Size - 3, "..." );
            break;
        }
    }
    title[ len ] = '\0';
    return title;
}

//*****************************************************************************
//
// SYNOPSIS
//
        /* virtual */ void indexer::usage( ostream& ) const
//
// DESCRIPTION
//
//      Print a module-specific usage message.  The default prints nothing.  A
//      derived indexer that has its own command-line options should override
//      this function.
//
// PARAMETERS
//
//      Not used.
//
//*****************************************************************************
{
    // do nothing
}
/* vim:set et sw=4 ts=4: */
