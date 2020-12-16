#if ! defined SEQ66_RTMIDI_TYPES_HPP
#define SEQ66_RTMIDI_TYPES_HPP

/**
 * \file          rtmidi_types.hpp
 *
 *  Type definitions pulled out for the needs of the refactoring.
 *
 * \library       seq66 application
 * \author        Gary P. Scavone; severe refactoring by Chris Ahlstrom
 * \date          2016-11-20
 * \updates       2020-12-14
 * \license       See the rtexmidi.lic file.  Too big for a header file.
 *
 *  The lack of hiding of these types within a class is a little to be
 *  regretted.  On the other hand, it does make the code much easier to
 *  refactor and partition, and slightly easier to read.
 */

#include <string>                           /* std::string                  */
#include <vector>                           /* std::vector container        */

#include "midi/event.hpp"                   /* seq66::event namespace       */
#include "midi/midibytes.hpp"               /* seq66::midibyte alias        */

/**
 * This was the version of the RtMidi library from which this reimplementation
 * was forked.  However, the divergence from RtMidi by this library is now
 * very great... only the idea of selecting the MIDI API at runtime, and the
 * queuing and call-back mechanism, have been preserved.
 */

#define SEQ66_RTMIDI_VERSION "2.1.1"        /* revision at fork time        */

/**
 *  Like the SEQ66_NO_BUS and SEQ66_NO_PORT macros in
 *  libseq66/include/app_limits.h, this value indicates an unspecified or
 *  invalid index into the list of available ports.
 */

#define SEQ66_NO_INDEX          (-1)        /* good values start at 0       */

/**
 *  Default size of the MIDI queue.
 */

#define SEQ66_DEFAULT_QUEUE_SIZE    100

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *    MIDI API specifier arguments.  These items used to be nested in
 *    the rtmidi class, but that only worked when RtMidi.cpp/h were
 *    large monolithic modules.
 */

enum rtmidi_api
{
    RTMIDI_API_UNSPECIFIED,     /**< Search for a working compiled API.     */
    RTMIDI_API_LINUX_ALSA,      /**< Advanced Linux Sound Architecture API. */
    RTMIDI_API_UNIX_JACK,       /**< JACK Low-Latency MIDI Server API.      */

#if defined USE_RTMIDI_API_ALL

    /*
     * We're not supporting these until we get a simplified
     * seq66-friendly API worked out.
     */

    RTMIDI_API_MACOSX_CORE,     /**< Macintosh OS-X Core Midi API.          */
    RTMIDI_API_WINDOWS_MM,      /**< Microsoft Multimedia MIDI API.         */
    RTMIDI_API_DUMMY,           /**< A compilable but non-functional API.   */

#endif

    RTMIDI_API_MAXIMUM          /**< A count of APIs; an erroneous value.   */

};

/**
 *  Provides a handy capsule for a MIDI message, based on the
 *  std::vector<unsigned char> data type from the RtMidi project.
 *
 *  Please note that the ALSA module in seq66's rtmidi infrastructure
 *  uses the seq66::event rather than the seq66::midi_message object.
 *  For the moment, we will translate between them until we have the
 *  interactions between the old and new modules under control.
 */

class midi_message
{

public:

    /**
     *  Holds the data of the MIDI message.  Callers should use
     *  midi_message::container rather than using the vector directly.
     *  Bytes are added by the push() function, and are safely accessed
     *  (with bounds-checking) by operator [].
     */

    using container = std::vector<midibyte>;

private:

    /**
     *  Holds the event status and data bytes.
     */

    container m_bytes;

    /**
     *  Holds the (optional) timestamp of the MIDI message.
     */

    double m_timestamp;

public:

    midi_message ();

    midibyte operator [] (int i) const
    {
        return (i >= 0 && i < int(m_bytes.size())) ? m_bytes[i] : 0 ;
    }

#if defined USE_MIDI_MESSAGE_AT_ACCESS

    midibyte & at (int i)
    {
        return m_bytes.at(i);       /* can throw an exception */
    }

    const midibyte & at (int i) const
    {
        return m_bytes.at(i);       /* can throw an exception */
    }

#endif

    const char * array () const
    {
        return reinterpret_cast<const char *>(&m_bytes[0]);
    }

    const midibyte * data () const
    {
        return m_bytes.data();
    }

    int count () const
    {
        return int(m_bytes.size());
    }

    bool empty () const
    {
        return m_bytes.empty();
    }

    void push (midibyte b)
    {
        m_bytes.push_back(b);
    }

    double timestamp () const
    {
        return m_timestamp;
    }

    void timestamp (double t)
    {
        m_timestamp = t;
    }

    bool is_sysex () const
    {
        return m_bytes.size() > 0 ? event::is_sysex_msg(m_bytes[0]) : false ;
    }

    void show () const;

};          // class midi_message

/**
 *  MIDI caller callback function type definition.  Used to be nested in the
 *  rtmidi_in class.  The timestamp parameter has been folded into the
 *  midi_message class (a wrapper for std::vector<unsigned char>), and the
 *  pointer has been replaced by a reference.
 */

using rtmidi_callback_t = void (*)
(
    midi_message & message,             /* includes the timestamp already */
    void * userdata
);

/**
 *  Provides a queue of midi_message structures.  This entity used to be a
 *  plain structure nested in the midi_in_api class.  We made it a class to
 *  encapsulate some common operations to save a burden on the callers.
 */

class midi_queue
{

private:

    unsigned m_front;
    unsigned m_back;
    unsigned m_size;
    unsigned m_ring_size;
    midi_message * m_ring;

public:

    midi_queue ();
    ~midi_queue ();

    bool empty () const
    {
        return m_size == 0;
    }

    int count () const
    {
        return int(m_size);
    }

    bool full () const
    {
        return m_size == m_ring_size;
    }

    const midi_message & front () const
    {
        return m_ring[m_front];
    }

    bool add (const midi_message & mmsg);
    void pop ();
    midi_message pop_front ();
    void allocate (unsigned queuesize = SEQ66_DEFAULT_QUEUE_SIZE);
    void deallocate ();

};          // class midi_queue

/**
 *  The rtmidi_in_data structure is used to pass private class data to the
 *  MIDI input handling function or thread.  Used to be nested in the
 *  rtmidi_in class.
 */

class rtmidi_in_data
{

private:

    /**
     *  Provides a queue of MIDI messages. Used when not using a JACK callback
     *  for MIDI input.
     */

    midi_queue m_queue;

    /**
     *  A one-time flag that starts out true and is falsified when the first
     *  MIDI messages comes in to this port.  It simply resets the delta JACK
     *  time.
     */

    bool m_first_message;
    void * m_api_data;

#if defined SEQ66_USER_CALLBACK_SUPPORT

    /**
     *  We currently don't use this feature, and do not plan to.  Keep it
     *  around, Justin Case.
     */

    bool m_using_callback;
    rtmidi_callback_t m_user_callback;
    void * m_user_data;

#endif

    bool m_continue_sysex;

    /**
     *  This flag is used in order to allow the JACK callbacks to not respond
     *  to disabled ports.  Ultimately, we want be able to remove ports and
     *  disassociate them from the JACK callbacks... a future endeavor.
     */

    bool m_is_enabled;

public:

    rtmidi_in_data ();

    const midi_queue & queue () const
    {
        return m_queue;
    }

    midi_queue & queue ()
    {
        return m_queue;
    }

    bool is_enabled () const
    {
        return m_is_enabled;
    }

    void is_enabled (bool flag)
    {
        m_is_enabled = flag;
    }

    bool first_message () const
    {
        return m_first_message;
    }

    void first_message (bool flag)
    {
        m_first_message = flag;
    }

    bool continue_sysex () const
    {
        return m_continue_sysex;
    }

    void continue_sysex (bool flag)
    {
        m_continue_sysex = flag;
    }

#if defined SEQ66_USER_CALLBACK_SUPPORT

    bool using_callback () const
    {
        return m_using_callback;
    }

    void using_callback (bool flag)
    {
        m_using_callback = flag;
    }

#endif

    const void * api_data () const
    {
        return m_api_data;
    }

    void * api_data ()
    {
        return m_api_data;
    }

    void api_data (void * dataptr)
    {
        m_api_data = dataptr;
    }

#if defined SEQ66_USER_CALLBACK_SUPPORT

    const void * user_data () const
    {
        return m_user_data;
    }

    void * user_data ()
    {
        return m_user_data;
    }

    void user_data (void * dataptr)
    {
        m_user_data = dataptr;
    }

    rtmidi_callback_t user_callback () const
    {
        return m_user_callback;
    }

    /**
     * \setter m_user_callback
     *      This should be done immediately after opening the port to avoid
     *      having incoming messages written to the queue instead of sent to
     *      the callback function.
     */

    void user_callback (rtmidi_callback_t cbptr)
    {
        m_user_callback = cbptr;
    }

#endif

};          // class rtmidi_in_data

}           // namespace seq66

#endif      // SEQ66_RTMIDI_TYPES_HPP

/*
 * rtmidi_types.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

