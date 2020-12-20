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
 * \file          listsbase.cpp
 *
 *  This module defines some of the more complex functions of the listsbase.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2020-12-10
 * \updates       2020-12-19
 * \license       GNU GPLv2 or above
 *
 *  The listbase provides common code for the clockslist and inputslist
 *  classes.
 */

#include <iostream>                     /* std::cout, etc.                  */
#include "play/listsbase.hpp"           /* seq66::listsbase class           */

/*
 *  This namespace is not documented because it screws up the document
 *  processing done by Doxygen.
 */

namespace seq66
{

/*
 *  The simple constructor and destructor defined in the header file.
 *  A few functions included here for better debugging.
 */

bool
listsbase::add (const std::string & name, const std::string & nickname)
{
    bool result = false;
    io ioitem;
    ioitem.io_enabled = true;
    ioitem.out_clock = e_clock::off;
    if (! name.empty())
    {
        ioitem.io_name = name;
        if (! nickname.empty())
        {
            ioitem.io_nick_name = nickname;
            result = true;
        }
        m_master_io.push_back(ioitem);
    }
    return result;
}

/**
 *  Parses a string of the form:
 *
 *      0   "Name of the Port"
 *
 *  These lines are created by output_port_map_list().  Their format is
 *  strict.
 *
 * \return
 *      Returns true if the line started with a number, followed by text
 *      contained inside double-quotes.
 */

bool
listsbase::add_list_line (const std::string & line)
{
    bool result = false;
    std::string temp = line;
    auto lpos = temp.find_first_of("0123456789");
    if (lpos != std::string::npos)
    {
        int portnum = std::stoi(temp);
        lpos = temp.find_first_of("\"");
        if (lpos != std::string::npos)
        {
            auto rpos = temp.find_last_of("\"");
            std::string portname = temp.substr(lpos + 1, rpos - lpos - 1);
            if (! portname.empty())
            {
                std::string pnum = std::to_string(portnum);
                result = add(portname, pnum);
            }
        }
    }
    return result;
}

void
listsbase::set_name (bussbyte bus, const std::string & name)
{
    if (bus < count())
    {
        std::string nick = extract_nickname(name);
        m_master_io[bus].io_name = name;
        m_master_io[bus].io_nick_name = nick;
    }
}

void
listsbase::set_nick_name (bussbyte bus, const std::string & name)
{
    if (bus < count())
        m_master_io[bus].io_nick_name = name;
}

std::string
listsbase::get_name (bussbyte bus, bool addnumber) const
{
    static std::string s_dummy;
    std::string result = bus < count() ?
        m_master_io[bus].io_name : s_dummy ;

    if (addnumber && ! result.empty())
        result = "[" + std::to_string(int(bus)) + "] " + result;

    return result;
}

std::string
listsbase::get_nick_name (bussbyte bus, bool addnumber) const
{
    static std::string s_dummy;
    std::string result =  bus < count() ?
        m_master_io[bus].io_nick_name : s_dummy ;

    if (addnumber && ! result.empty())
        result = "[" + std::to_string(int(bus)) + "] " + result;

    return result;
}

/**
 *  The nick-name of a port is roughly all the text following the last colon
 *  in the display-name [see midibase::display_name()].  It seems to be the
 *  same text whether the port name comes from ALSA or from a2jmidid when
 *  running JACK.  We don't have any MIDI hardware that JACK detects without
 *  a2jmidid.
 */

std::string
listsbase::extract_nickname (const std::string & name) const
{
    std::string result;
    auto cpos = name.find_last_of(":");
    if (cpos != std::string::npos)
    {
        ++cpos;
        if (std::isdigit(name[cpos]))
        {
            cpos = name.find_first_of(" ", cpos);
            if (cpos != std::string::npos)
                ++cpos;
        }
        else if (std::isspace(name[cpos]))
            ++cpos;

        result = name.substr(cpos);
    }
    else
        result = name;

    return result;
}

/**
 *  This function is used to get the buss number from the main clockslist or
 *  main inputslist, using its nick-name.
 *
 * \param nick
 *      Provides the nick-name to be looked up.  This name is obtained from
 *      the internal clockslist or (pending) inputslist by lookup given a
 *      nominal buss number.
 *
 * \return
 *      Returns the actual buss number that will be used for I/O.
 */

bussbyte
listsbase::bus_from_nick_name (const std::string & nick) const
{
    bussbyte result = c_bussbyte_max;           /* a bad, unusable value    */
    for (int b = 0; b < count(); ++b)
    {
        if (nick == m_master_io[b].io_nick_name)
        {
            result = bussbyte(b);
            break;
        }
    }
    return result;
}

/**
 *  Looks up the nick-name, which should be a string version of the nominal
 *  buss number.  Returns the port name (short name) if found in the list.
 *  This function should be used only on the internal clockslist [returned by
 *  output_port_map() in the clockslist module] or (pending) the internal
 *  inputslist.  Only these lists stored the buss number as a string.  It is a
 *  linear lookup, but the lists are short, usually a half-dozen elements.
 *
 * \param nominalbuss
 *      Provides the external, nominal buss number which is often stored in a
 *      pattern to indicate what output port is to be used.
 */

std::string
listsbase::port_name_from_bus (bussbyte nominalbuss) const
{
    std::string result;
    std::string nick = std::to_string(int(nominalbuss));
    for (const auto & value : m_master_io)
    {
        if (nick == value.io_nick_name)
        {
            result = value.io_name;
            break;
        }
    }
    return result;
}


std::string
listsbase::e_clock_to_string (e_clock e) const
{
    std::string result;
    switch (e)
    {
        case e_clock::disabled:     result = "Disabled";    break;
        case e_clock::off:          result = "Off";         break;
        case e_clock::pos:          result = "Pos";         break;
        case e_clock::mod:          result = "Mod";         break;
        default:                    result = "Unknown";     break;
    }
    return result;
}

std::string
listsbase::port_map_list () const
{
    std::string result;
    if (not_empty())
    {
        for (const auto & value : m_master_io)
        {
            std::string port = value.io_nick_name;
            std::string name = value.io_name;
            std::string temp = port + "   \"" + name + "\"\n";
            result += temp;
        }
    }
    return result;
}

std::string
listsbase::to_string (const std::string & tag) const
{
    std::string result = "I/O List: '" + tag + "'\n";
    int count = 0;
    for (const auto & value : m_master_io)
    {
        std::string temp = std::to_string(count) + ". ";
        temp += value.io_enabled ? "Enabled;  " : "Disabled; " ;
        temp += "Clock = " + e_clock_to_string(value.out_clock);
        temp += "\n   ";
        temp += "Name:     " + value.io_name + "\n   ";
        temp += "Nickname: " + value.io_nick_name + "\n";
        result += temp;
        ++count;
    }
    return result;
}

void
listsbase::show (const std::string & tag) const
{
    std::string listdump = to_string(tag);
    std::cout << listdump << std::endl;
}

}               // namespace seq66

/*
 * listsbase.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
