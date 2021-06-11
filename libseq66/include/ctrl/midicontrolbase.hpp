#ifndef SEQ66_MIDICONTROLBASE_HPP
#define SEQ66_MIDICONTROLBASE_HPP

/*
 *  This file is part of seq66.
 *
 *  seq66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  seq66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with seq66; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          midicontrolbase.hpp
 *
 *  This module declares/defines the base class for handling MIDI control
 *  I/O of the application.
 *
 * \library       seq66 application
 * \author        C. Ahlstrom
 * \date          2019-11-25
 * \updates       2021-06-09
 * \license       GNU GPLv2 or above
 *
 *  Provides the base class for midicontrolout.
 *
 * Warning:
 *
 *      It is NOT a base class for midicontrol or midicontrolin!
 */

#include "midi/midibytes.hpp"           /* seq66::bussbyte data type        */

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *  Provides some management support for MIDI control... on I/O.  Many thanks
 *  to igorangst!
 */

class midicontrolbase
{

    friend class midicontrolfile;
    friend class performer;

private:

    /**
     *  A name to use for showing the contents of the container.
     */

    std::string m_name;

    /**
     *  Provides the MIDI I/O buss, that is the port number for MIDI I/O.
     *  This value defaults to 0, and the user must be sure to avoid using
     *  this buss value for music, or redefine the buss.  This is the nominal
     *  buss, which is read and saved, but not used for I/O; see m_true_buss
     *  instead.
     */

    bussbyte m_buss;                    /* SEQ66_MIDI_CONTROL_IN/OUT_BUSS   */

    /**
     * The true buss, which exists on the system.
     */

    bussbyte m_true_buss;

    /**
     *  Indicates that this container is "empty".
     */

    bool m_is_blank;

    /**
     *  Indicates that this container is enabled or disabled.
     */

    bool m_is_enabled;

    /**
     *  Offset provides a way to utilize a different portion of a controller
     *  such as the Launchpad Mini.  Currently just set to 0 while we work
     *  things out.
     */

    int m_offset;

    /**
     *  Provides the number of rows, useful when the runtime number of rows
     *  differs from that specified in the configuration file.  We at least
     *  want to avoid segfaults.
     */

    int m_rows;

    /**
     *  Provides the number of rows, useful when the runtime number of rows
     *  differs from that specified in the configuration file.  We at least
     *  want to avoid segfaults.
     */

    int m_columns;

public:

    midicontrolbase (const std::string & name = "");
    virtual ~midicontrolbase () = default;
    virtual bool initialize (int buss, int rows, int columns);      /* base */

    const std::string & name () const
    {
        return m_name;
    }

    bussbyte nominal_buss () const
    {
        return m_buss;
    }

    bussbyte true_buss () const
    {
        return m_true_buss;
    }

    bool is_blank () const
    {
        return m_is_blank;
    }

    bool is_enabled () const
    {
        return m_is_enabled;
    }

    bool is_disabled () const
    {
        return ! is_enabled();
    }

    int offset () const
    {
        return m_offset;
    }

    int rows () const
    {
        return m_rows;
    }

    int columns () const
    {
        return m_columns;
    }

protected:

    void nominal_buss (bussbyte b)
    {
        m_buss = b;
    }

    void true_buss (bussbyte b)
    {
        m_true_buss = b;
    }

    void is_blank (bool flag)
    {
        m_is_blank = flag;
    }

    void is_enabled (bool flag)
    {
        m_is_enabled = flag;
    }

    void offset (int o)
    {
        if (o >= 0)                 /* more verification later  */
            m_offset = o;
    }

    void rows (int r)
    {
        if (r > 0)                  /* more verification later  */
            m_rows = r;
    }

    void columns (int c)
    {
        if (c > 0)                  /* more verification later  */
            m_columns = c;
    }

};          // class midicontrolbase

}           // namespace seq66

#endif      // SEQ66_MIDICONTROLBASE_HPP

/*
 * midicontrolbase.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

