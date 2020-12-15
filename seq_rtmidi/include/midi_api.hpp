#if ! defined SEQ66_MIDI_API_HPP
#define SEQ66_MIDI_API_HPP

/**
 * \file          midi_api.hpp
 *
 *  An abstract base class for realtime MIDI input/output.
 *
 * \library       seq66 application
 * \author        Gary P. Scavone; modifications by Chris Ahlstrom
 * \date          2016-11-14
 * \updates       2019-12-15
 * \license       See the rtexmidi.lic file.
 *
 *  Declares the following classes:
 *
 *      -   seq66::midi_api
 *      -   seq66::midi_in_api
 *      -   seq66::midi_out_api
 */

#include "midi/midibase.hpp"
#include "rterror.hpp"
#include "rtmidi_types.hpp"             /* SEQ66_NO_INDEX               */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace seq66
{
    class event;
    class midi_info;
    class midibus;

/**
 *  Subclasses of midi_in_api and midi_out_api contain all API- and
 *  OS-specific code necessary to fully implement the rtmidi API.
 *
 *  Note that midi_in_api and midi_out_api are abstract base classes and
 *  cannot be explicitly instantiated.  rtmidi_in and rtmidi_out will
 *  create instances of a midi_in_api or midi_out_api subclass.
 */

class midi_api : public midibase
{

private:

    /**
     *  Contains information about the ports (system or client) enumerated by
     *  the API.
     */

    midi_info & m_master_info;

    /**
     *  Contains a reference to the parent midibus/midibase object.  This
     *  object is needed to get parameters that are peculiar to the port as it
     *  is actually set up, rather than information from the midi_info object.
     */

    midibus & m_parent_bus;

    /**
     *  Although this really is useful only for MIDI input objects,
     *  the split of the midi_api is not as convenient for re-use
     *  as is the split for derived classes like midi_in_jack/midi_out_jack.
     */

    rtmidi_in_data m_input_data;

    /**
     *  Set to true if the port was opened, activated, and connected without
     *  issue.
     */

    bool m_connected;

    /**
     *  Indicates if the user has disabled a port that was previously enabled.
     *  At this time, we don't want to try to tear down the port.  Instead, we
     *  just disable the port via the JACK callback data item rtmidi_in_data
     *  :: is_enabled() in the rtmidi_types module.  Note that, to be
     *  suspended, the port had to be open in the first place.
     */

    bool m_suspended;

protected:

    /**
     *  Holds the last error message, if in force.  This is an original RtMidi
     *  concept.
     */

    std::string m_error_string;

    /**
     *  Holds the error callback function pointer, if any.  This is an
     *  original RtMidi concept.
     */

    rterror_callback m_error_callback;

    /**
     *  Indicates that the first error has happened. This is an original
     *  RtMidi concept.  I have to confess I am not sure how it is/should be
     *  used, yet.
     */

    bool m_first_error_occurred;

    /**
     *  Holds data needed by the error-callback. This is an original RtMidi
     *  concept.  I have to confess I am not sure how it is/should be used,
     *  yet.
     */

    void * m_error_callback_user_data;

public:

    midi_api (midibus & parentbus, midi_info & masterinfo);
    virtual ~midi_api ();

    bool is_input_port () const;
    bool is_virtual_port () const;
    bool is_system_port () const;

public:

    /**
     *  No code; only midi_jack overrides this function at present.
     */

    virtual bool api_connect ()
    {
        return true;
    }

    virtual int api_poll_for_midi () = 0;
    virtual bool api_init_out () = 0;
    virtual bool api_init_out_sub () = 0;
    virtual bool api_init_in () = 0;
    virtual bool api_init_in_sub () = 0;
    virtual bool api_deinit_out () = 0;
    virtual bool api_deinit_in () = 0;
    virtual bool api_get_midi_event (event *) = 0;
    virtual void api_play (event * e24, midibyte channel) = 0;
    virtual void api_sysex (event * e24) = 0;
    virtual void api_continue_from (midipulse tick, midipulse beats) = 0;
    virtual void api_start () = 0;
    virtual void api_stop () = 0;
    virtual void api_flush () = 0;
    virtual void api_clock (midipulse tick) = 0;
    virtual void api_set_ppqn (int ppqn) = 0;
    virtual void api_set_beats_per_minute (midibpm bpm) = 0;

    /*
     * The next two functions are provisional.  Currently useful only in the
     * midi_jack module.
     */

    virtual std::string api_get_bus_name ()
    {
        std::string sm_empty;
        return sm_empty;
    }

    virtual std::string api_get_port_name ()
    {
        std::string sm_empty;
        return sm_empty;
    }

public:

    bool is_port_open () const
    {
        return m_connected;
    }

    bool is_port_suspended () const
    {
        return m_suspended;
    }

    midi_info & master_info ()
    {
        return m_master_info;
    }

    const midi_info & master_info () const
    {
        return m_master_info;
    }

    midibus & parent_bus ()
    {
        return m_parent_bus;
    }

    const midibus & parent_bus () const
    {
        return m_parent_bus;
    }

    void master_midi_mode (bool input);

    /*
     *  A basic error reporting function for rtmidi classes.
     */

    void error (rterror::Type type, const std::string & errorstring);

    /*
     * Moved from the now-removed midi_in_api class.
     */

    void user_callback (rtmidi_callback_t callback, void * userdata);
    void cancel_callback ();

protected:

    void set_port_open ()
    {
        m_connected = true;
    }

    virtual void set_port_suspended (bool flag)
    {
        m_suspended = flag;
    }

    rtmidi_in_data * input_data ()
    {
        return &m_input_data;
    }

};          // class midi_api

}           // namespace seq66

#endif      // SEQ66_MIDI_API_HPP

/*
 * midi_api.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

