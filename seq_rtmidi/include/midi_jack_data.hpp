#if ! defined SEQ66_MIDI_JACK_DATA_HPP
#define SEQ66_MIDI_JACK_DATA_HPP

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
 * \file          midi_jack_data.hpp
 *
 *    Object for holding the current status of JACK and JACK MIDI data.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2017-01-02
 * \updates       2022-09-14
 * \license       See above.
 *
 *  GitHub issue #165: enabled a build and run with no JACK support.
 */

#include "util/basic_macros.h"          /* nullptr and other macros         */
#include "midi/midibytes.hpp"           /* seq66::midibyte, other aliases   */
#include "rtmidi_types.hpp"             /* seq66::rtmidi_in_data class      */

#if defined SEQ66_JACK_SUPPORT

#include <jack/jack.h>
#include <jack/ringbuffer.h>


/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *  Contains the JACK MIDI API data as a kind of scratchpad for this object.
 *  This guy needs a constructor taking parameters for an rtmidi_in_data
 *  pointer.
 */

class midi_jack_data
{
    /**
     *  Holds data about JACK transport, to be used in midi_jack ::
     *  jack_frame_offset(). These values are a subset of what appears in the
     *  jack_position_t structure in jack/types.h.
     */

    static jack_nframes_t sm_jack_frame_rate;
    static double sm_jack_ticks_per_beat;
    static double sm_jack_beats_per_minute;
    static double sm_jack_frame_factor;

    /**
     *  Holds the JACK sequencer client pointer so that it can be used by the
     *  midibus objects.  This is actually an opaque pointer; there is no way
     *  to get the actual fields in this structure; they can only be accessed
     *  through functions in the JACK API.  Note that it is also stored as a
     *  void pointer in midi_info::m_midi_handle.  This item is the single
     *  JACK client created by the midi_jack_info object.
     */

    jack_client_t * m_jack_client;

    /**
     *  Holds the JACK port information of the JACK client.
     */

    jack_port_t * m_jack_port;

    /**
     *  Holds the size of data for communicating between the client
     *  ring-buffer and the JACK port's internal buffer.
     */

    jack_ringbuffer_t * m_jack_buffsize;

    /**
     *  Holds the data for communicating between the client ring-buffer and
     *  the JACK port's internal buffer.
     */

    jack_ringbuffer_t * m_jack_buffmessage;

    /**
     *  The last time-stamp obtained.  Use for calculating the delta time, I
     *  would imagine.
     */

    jack_time_t m_jack_lasttime;

#if defined SEQ66_MIDI_PORT_REFRESH

    /**
     *  An unsigned 32-bit port ID that starts out as null_system_port_id(),
     *  and, at least in JACK can be filled with actual internal port number
     *  assigned during port registration.
     */

    jack_port_id_t m_internal_port_id;

#endif

    /**
     *  Holds special data peculiar to the client and its MIDI input
     *  processing. This data consists of the midi_queue message queue and a
     *  few boolean flags.
     */

    rtmidi_in_data * m_jack_rtmidiin;

public:

    midi_jack_data ();
    ~midi_jack_data ();

    /*
     *  Frame offset-related functions.
     */

    static bool recalculate_frame_factor (const jack_position_t & pos);
    static jack_nframes_t jack_frame_offset (jack_nframes_t F, midipulse p);

    static jack_nframes_t jack_frame_rate ()
    {
        return sm_jack_frame_rate;
    }

    static double jack_ticks_per_beat ()
    {
        return sm_jack_ticks_per_beat;
    }

    static double jack_beats_per_minute ()
    {
        return sm_jack_beats_per_minute;
    }

    static double jack_frame_factor ()
    {
        return sm_jack_frame_factor;
    }

    static void jack_frame_rate (jack_nframes_t nf)
    {
        sm_jack_frame_rate = nf;
    }

    static void jack_ticks_per_beat (double tpb)
    {
        sm_jack_ticks_per_beat = tpb;
    }

    static void jack_beats_per_minute (double bp)
    {
        sm_jack_beats_per_minute = bp;
    }

    /*
     *  Basic member access. Getters and setters.
     */

    jack_client_t * jack_client ()
    {
        return m_jack_client;
    }

    void jack_client (jack_client_t * jc)
    {
        m_jack_client = jc;
    }

    jack_port_t * jack_port ()
    {
        return m_jack_port;
    }

    void  jack_port (jack_port_t * jp)
    {
        m_jack_port = jp;
    }

    bool valid_buffer () const
    {
        return not_nullptr(m_jack_buffmessage);
    }

    rtmidi_in_data * jack_rtmidiin () const
    {
        return m_jack_rtmidiin;
    }

    void jack_rtmidiin (rtmidi_in_data * rid)
    {
        m_jack_rtmidiin = rid;
    }

    jack_ringbuffer_t * jack_buffsize ()
    {
        return m_jack_buffsize;
    }

    void jack_buffsize (jack_ringbuffer_t * jrb)
    {
        m_jack_buffsize = jrb;
    }

    jack_ringbuffer_t * jack_buffmessage ()
    {
        return m_jack_buffmessage;
    }

    void jack_buffmessage (jack_ringbuffer_t * jrb)
    {
        m_jack_buffmessage = jrb;
    }

    jack_time_t jack_lasttime () const
    {
        return m_jack_lasttime;
    }

    void jack_lasttime (jack_time_t jt)
    {
        m_jack_lasttime = jt;
    }

#if defined SEQ66_MIDI_PORT_REFRESH

    jack_port_id_t internal_port_id () const
    {
        return m_internal_port_id;
    }

    void internal_port_id (uint32_t id)
    {
        m_internal_port_id = id;
    }

#endif

};          // class midi_jack_data

}           // namespace seq66

#endif      // SEQ66_JACK_SUPPORT

#endif      // SEQ66_MIDI_JACK_DATA_HPP

/*
 * midi_jack_data.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

