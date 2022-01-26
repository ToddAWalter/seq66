#if ! defined SEQ66_MASTERMIDIBUS_RM_HPP
#define SEQ66_MASTERMIDIBUS_RM_HPP

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
 * \file          mastermidibus_rm.hpp
 *
 *  This module declares/defines the base class for MIDI I/O for Linux, Mac,
 *  and Windows, via the RtMidi API.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2021-07-19
 * \license       GNU GPLv2 or above
 *
 *  This mastermidibus module is the Linux (and, soon, JACK) version of the
 *  mastermidibus module using the completely refactored RtMidi library.
 */

#include "midi/mastermidibase.hpp"      /* seq66::mastermidibase ABC        */
#include "rtmidi_info.hpp"              /* seq66::rtmidi_info, new class    */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *  The class that "supervises" all of the midibus objects.  This
 *  implementation uses the PortMidi library, which supports Linux and
 *  Windows, but not JACK or Mac OSX.
 */

class mastermidibus final : public mastermidibase
{
    friend class midi_alsa_info;
    friend class midi_jack_info;

private:

    /**
     *  Holds the basic MIDI input and output information for later re-use in
     *  the construction of midibus objects.
     */

    rtmidi_info m_midi_master;

    /**
     *  Indicates we are running with JACK MIDI enabled, and need to
     *  use each port's ability to poll for and get MIDI events, rather
     *  than use ALSA's method of calling functions from the "MIDI master"
     *  object.
     */

    bool m_use_jack_polling;

public:

    mastermidibus () = delete;
    mastermidibus (int ppqn, midibpm bpm);
    virtual ~mastermidibus ();

    virtual bool activate () override;

protected:

    virtual bool api_get_midi_event (event * in) override;
    virtual int api_poll_for_midi () override;
    virtual void api_init (int ppqn, midibpm bpm) override;

    /**
     *  Provides MIDI API-specific functionality for the set_ppqn() function.
     */

    virtual void api_set_ppqn (int p) override
    {
        m_midi_master.api_set_ppqn(p);
    }

    /**
     *  Provides MIDI API-specific functionality for the
     *  set_beats_per_minute() function.
     */

    virtual void api_set_beats_per_minute (midibpm b) override
    {
        m_midi_master.api_set_beats_per_minute(b);
    }

    virtual void api_flush () override
    {
        m_midi_master.api_flush();
    }

    virtual void api_port_start (mastermidibus & masterbus, int bus, int port)
    {
        m_midi_master.api_port_start(masterbus, bus, port);
    }

private:

    void port_list (const std::string & tag);
    midibus * make_virtual_output_bus (int bus);
    midibus * make_virtual_input_bus (int bus);
    midibus * make_output_bus (int bus);
    midibus * make_input_bus (int bus);

};          // class mastermidibus

}           // namespace seq66

#endif      // SEQ66_MASTERMIDIBUS_RM_HPP

/*
 * mastermidibus_rm.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

