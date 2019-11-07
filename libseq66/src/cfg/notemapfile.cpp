/*
 *  This file is part of seq66.
 *
 *  seq66 is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  seq66 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with seq66; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          notemapfile.cpp
 *
 *  This module declares/defines the base class for managing the reading and
 *  writing of the mute-group sections of the "rc" file.
 *
 * \library       sequencer64 application
 * \author        Seq24 team; modifications by Chris Ahlstrom
 * \date          2019-11-05
 * \updates       2019-11-06
 * \license       GNU GPLv2 or above
 *
 */

#include <iomanip>                      /* std::setw()                      */

#include "cfg/notemapfile.hpp"          /* seq66::notemapfile class         */
#include "cfg/rcsettings.hpp"           /* seq66::rcsettings class          */
#include "util/calculations.hpp"        /* seq66::string_to_bool()          */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *  Principal constructor.
 *
 * \param mapper
 *      Provides the notemapper reference to be acted upon.
 *
 * \param filename
 *      Provides the name of the mute-groups file; this is usually a full path
 *      file-specification to the "mutes" file using this object.
 *
 * \param rcs
 *      The configfile currently requires and rcsetting object, but it is not
 *      yet used here.
 */

notemapfile::notemapfile
(
    notemapper & mapper,
    const std::string & filename,
    rcsettings & rcs
) :
    configfile      (filename, rcs),
    m_note_mapper   (mapper)
{
    // Empty body
}

/**
 *  A rote destructor.
 */

notemapfile::~notemapfile ()
{
    // ~configfile() called automatically
}

/**
 *  Parse the ~/.config/seq66/seq66.rc file-stream or the
 *  ~/.config/seq66/seq66.mutes file-stream.
 *
 *  [mute-group]
 *
 *      The mute-group starts with a line that indicates up to 32 mute-groups are
 *      defined. A common value is 1024, which means there are 32 groups times 32
 *      keys.  But this value is currently thrown away.  This value is followed by
 *      32 lines of data, each contained 4 sets of 8 settings.  See the seq66-doc
 *      project on GitHub for a much more detailed description of this section.
 */

bool
notemapfile::parse_stream (std::ifstream & file)
{
    bool result = true;
    file.seekg(0, std::ios::beg);                   /* seek to start    */

    /*
     * [comments] Header commentary is skipped during parsing.  However, we
     * now try to read an optional comment block.  This block is part of the
     * notemap container, not part of the rcsettings object.
     */

    std::string s = parse_comments(file);
    if (! s.empty())
    {
        mapper().comments_block().set(s);
        if (rc().verbose()
            std::cout << s;
    }

    s = get_variable(file, "[notemap-flags]", "map-type");
    if (! s.empty())
        mapper().map_type(s);

    s = get_variable(file, "[notemap-flags]", "gm-channel");
    if (! s.empty())
        mapper().gm_channel(std::to_int(s));

    s = get_variable(file, "[notemap-flags]", "reverse");
    if (! s.empty())
        mapper().map_type(string_to_bool(s));

    /*
     * This function gets the position before the first "Drum" section.
     * But it also, like line_after(), gets the line().
     *
     * MOVE TO CONFIGFILE
     */

    int note = (-1);
    int position = find_tag(file, "[ Drum");
    good = position > 0;
    if (good)
    {
        printf("drum line %s\n", line().c_str();  // JUST A TEST
        note = get_tag_value(line());
    }
    if (note == (-1))
    {
        errprint("No [Drum 00] tag value found");
        good = false;
    }
    while (good)                        /* not at end of section?   */
    {
        std::string tag = "[Drum ";
        tag += std::to_string(note);
        tag += "]";
        good = line_after(file, tag);
        if (good)
        {
            good = /// parse_mutes_stanza();
            if (good)
                good = next_data_line(file);
        }
        ++note;
    }
    return result;
}

/**
 *  Get the number of sequence definitions provided in the [mute-group]
 *  section.  See the rcfile class for full information.
 *
 * \param p
 *      Provides the performance object to which all of these options apply.
 *
 * \return
 *      Returns true if the file was able to be opened for reading.
 *      Currently, there is no indication if the parsing actually succeeded.
 */

bool
notemapfile::parse ()
{
    bool result = true;
    std::ifstream file(name(), std::ios::in | std::ios::ate);
    if (file.is_open())
    {
        result = parse_stream(file);
    }
    else
    {
        errprintf
        (
            "notemap::parse(): error opening %s for reading", name().c_str()
        );
        result = false;
    }
    return result;
}

/**
 *  Writes the [mute-group] section to the given file stream.
 *
 * \param file
 *      Provides the output file stream to write to.
 *
 * \param separatefile
 *      If true, the [mute-group] section is being written to a separate
 *      file.  The default value is false.
 *
 * \return
 *      Returns true if the write operations all succeeded.
 */

bool
notemapfile::write_stream (std::ofstream & file)
{
    file
        << "# Seq66 0.90.1 (and above) mute-groups configuration file\n"
        << "#\n"
        << "# " << name() << "\n"
        << "# Written on " << current_date_time() << "\n"
        << "#\n"
        << "# This file can be used to convert the percussion of non-GM devices\n"
        << "# to GM, as best as permitted by GM percussion.\n"
        << "\n"
        ;

    /*
     * [comments]
     */

    file <<
        "[Seq66]\n\n"
        "config-type = \"drums\"\n"
        "version = 0\n"
        "\n"
        "# The [comments] section can document this file.  Lines starting\n"
        "# with '#' and '[' are ignored.  Blank lines are ignored.  Show a\n"
        "# blank line by adding a space character to the line.\n\n"
        "[comments]\n\n" << rc_ref().comments_block().text() << "\n"
        <<
        "# This file holds the drum-note mapping configuration for Seq66.\n"
        "# It is always stored in the main configuration directory.  To use\n"
        "# this file, ... we need to add a user-nterface for it.  TODO!\n"
        "#\n"
        "# map-type: drum; indicates what kind of mapping is done, open for\n"
        "# future expansion.\n"
        "# \n"
        "# gm-channel: Indicates the channel to be enforced for the converted\n"
        "# events.\n"
        ;

	bool result = write_map_entries(file);
	if (result)
	{
		file
			<< "\n# End of " << name() << "\n#\n"
			<< "# vim: sw=4 ts=4 wm=4 et ft=dosini\n"
			;
	}
	else
		file_error("failed to write", name());

    return result;
}

/**
 *  This options-writing function is just about as complex as the
 *  options-reading function.
 *
 * \return
 *      Returns true if the write operations all succeeded.
 */

bool
notemapfile::write ()
{
    std::ofstream file(name(), std::ios::out | std::ios::trunc);
    bool result = file.is_open();
    if (result)
    {
        pathprint("Writing mute-groups configuration", name());
        result = write_stream(file);
        file.close();
    }
    else
    {
        file_error("Error opening for writing", name());
    }
    return result;
}

/**
 *  The default long format for writing mute groups.
 */

static const char * const sg_scanf_fmt_1 =
    "%d [ %d %d %d %d %d %d %d %d ] [ %d %d %d %d %d %d %d %d ] "
      " [ %d %d %d %d %d %d %d %d ] [ %d %d %d %d %d %d %d %d ]";

/**
 *  Writes the [mute-group] section to the given file stream.  This can also
 *  be called by the rcfile object to just dump the data into that file.
 *
 * \param file
 *      Provides the output file stream to write to.
 *
 * \return
 *      Returns true if the write operations all succeeded.
 */

bool
notemapfile::write_map_entries (std::ofstream & file)
{
    bool result = file.is_open();
    if (result)
    {
        for (const auto & stz : rc_ref().mute_groups().list())
        {
            int gmute = stz.first;
            const mutegroup & m = stz.second;
//          std::string stanza = write_stanza_bits(m.get(), usehex);
            if (! stanza.empty())
            {
                file << std::setw(2) << gmute << " " << stanza << std::endl;
            }
            else
            {
                result = false;
                break;
            }
        }
    }
    return result;
}

/**
 *  We need the format of a mute-group stanza to be more flexible.
 *  Should it match the set size?
 *
 *  We want to support the misleading format "[ b b b... ] [ b b b...] ...",
 *  as well as a new format "[ 0xbb ] [ 0xbb ] ...".
 *
 *  This function handles the current line of data from the mutes file.
 */

bool
notemapfile::parse_mutes_stanza ()
{
    int group = string_to_int(line());
    bool result = group >= 0 && group < 512;            /* a sanity check   */
    if (result)
    {
        midibooleans groupmutes;
        result = parse_stanza_bits(groupmutes, line());
        if (result)
            result = rc_ref().mute_groups().load(group, groupmutes);
    }
    return result;
}

}           // namespace seq66

/*
 * notemapfile.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

