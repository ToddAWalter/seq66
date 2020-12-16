/**
 * \file          midi_api.cpp
 *
 *    A class for a generic MIDI API.
 *
 * \library       seq66 application
 * \author        Gary P. Scavone; refactoring by Chris Ahlstrom
 * \date          2016-11-14
 * \updates       2020-12-15
 * \license       See the rtexmidi.lic file.  Too big for a header file.
 *
 *  In this refactoring, we had to adapt the existing Seq66
 *  infrastructure to how the "RtMidi" library works.  We also had to
 *  refactor the RtMidi library significantly to fit it within the working
 *  mode of the Seq66 application and libraries.
 */

#include "cfg/settings.hpp"             /* seq66::rcsettings ...            */
#include "midi/event.hpp"
#include "midi_api.hpp"
#include "midi_info.hpp"
#include "midibus_rm.hpp"
#include "util/basic_macros.hpp"        /* errprint() etc.                  */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace seq66
{

/*
 *  midi_api section
 */

/**
 *  Principle constructor.
 */

midi_api::midi_api (midibus & parentbus, midi_info & masterinfo) :
    m_master_info               (masterinfo),
    m_parent_bus                (parentbus),
    m_input_data                (),
    m_connected                 (false),
    m_suspended                 (false),
    m_error_string              (),
    m_error_callback            (0),
    m_first_error_occurred      (false),
    m_error_callback_user_data  (0)
{
    // no code
}

/**
 *  Destructor, needed because it is virtual.
 */

midi_api::~midi_api ()
{
    // no code
}

/**
 *  \return
 *      Returns true if the port is an input port.
 */

bool
midi_api::is_input_port () const
{
    return parent_bus().is_input_port();
}

/**
 *  A virtual port is what Seq24 called a "manual" port.  It is a MIDI port
 *  that an application can create as if it is a real ALSA port.
 *
 *  \return
 *      Returns true if the port is an input port.
 */

bool
midi_api::is_virtual_port () const
{
    return parent_bus().is_virtual_port();
}

/**
 *  A system port is one that is independent of the devices and applications
 *  that exist.  In the ALSA subsystem, the only system port is the "announce"
 *  port.
 *
 *  \return
 *      Returns true if the port is an system port.
 */

bool
midi_api::is_system_port () const
{
    return parent_bus().is_system_port();
}

/**
 *  Provides an error handler that can support an error callback.
 *
 * \throw
 *      If the error is not just a warning, then an rterror object is thrown.
 *
 * \param type
 *      The type of the error.
 *
 * \param errorstring
 *      The error message, which gets copied if this is the first error.
 */

void
midi_api::error (rterror::Type type, const std::string & errorstring)
{
    if (m_error_callback)
    {
        if (m_first_error_occurred)
            return;

        m_first_error_occurred = true;

        const std::string errorMessage = errorstring;
        m_error_callback(type, errorMessage, m_error_callback_user_data);
        m_first_error_occurred = false;
        return;
    }

    if (type == rterror::WARNING)
    {
        errprint(errorstring.c_str());
    }
    else if (type == rterror::DEBUG_WARNING)
    {
#if defined SEQ66_PLATFORM_DEBUG
        errprint(errorstring.c_str());
#endif
    }
    else
    {
        errprint(errorstring.c_str());

        /*
         * Not a big fan of throwing errors, especially since we currently log
         * errors in rtmidi to the console.  Might make this a build option.
         *
         * throw rterror(errorstring, type);
         */
    }
}

/**
 * \getter m_master_info.midi_mode()
 *      This function makes it a bit simpler on the caller.
 */

void
midi_api::master_midi_mode (bool input)
{
    m_master_info.midi_mode(input);
}

#if defined SEQ66_USER_CALLBACK_SUPPORT

/**
 *  Wires in a MIDI input callback function.
 *
 *  We moved it into the base class, trading convenience for the chance of
 *  confusion.
 *
 * \param callback
 *      Provides the callback function.
 *
 * \param userdata
 *      Provides the user data needed by the callback function.
 */

void
midi_api::user_callback (rtmidi_callback_t callback, void * userdata)
{
    if (m_input_data.using_callback())
    {
        m_error_string = "callback function is already set";
        error(rterror::WARNING, m_error_string);
        return;
    }
    if (is_nullptr(callback))
    {
        m_error_string = "callback function is null";
        error(rterror::WARNING, m_error_string);
        return;
    }
    m_input_data.user_callback(callback);
    m_input_data.user_data(userdata);
    m_input_data.using_callback(true);
}

/**
 *  Removes the MIDI input callback and some items related to it.
 *
 *  We moved it into the base class, trading convenience for the chance of
 *  confusion.
 */

void
midi_api::cancel_callback ()
{
    if (m_input_data.using_callback())
    {
        m_input_data.user_callback(nullptr);
        m_input_data.user_data(nullptr);
        m_input_data.using_callback(false);
    }
    else
    {
        m_error_string = "no callback function was set";
        error(rterror::WARNING, m_error_string);
    }
}

#endif      // defined SEQ66_USER_CALLBACK_SUPPORT

}           // namespace seq66

/*
 * midi_api.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

