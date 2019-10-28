#if ! defined SEQ66_EVENTLIST_HPP
#define SEQ66_EVENTLIST_HPP

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
 * \file          eventlist.hpp
 *
 *  This module provides a stand-alone module for the event-list container
 *  used by the application.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2015-09-19
 * \updates       2019-10-27
 * \license       GNU GPLv2 or above
 *
 *  This module extracts the event-list functionality from the sequencer
 *  module, so that it is easier to try to replace it with some better
 *  container later.
 *
 *  We should leverage "for-each" functionality.
 *
 *  List versus Map:  #if defined or derivation from an interface?  For our
 *  purposes, #if defined might be simplest, and we only want to pick the
 *  fastest one, ultimately.
 *
 *  It turns out the the std::multimap implementation is a little bit faster in
 *  release mode, and a lot faster in debug mode.  Why?  Probably because
 *  the std::list implementation calls std::list::sort() a lot, and the
 *  std::multimap implementation is a lot faster at sorting.  But since the
 *  map iterator is slower, we stick with std::list.
 *
 *  But, based on this article:
 *
 * https://baptiste-wicht.com/posts/2012/12/cpp-benchmark-vector-list-deque.html
 *
 *  We will now try std::vector for the event list.
 */

#include <algorithm>                    /* std::sort(), std::merge()    */
#include <string>                       /* std::string                  */
#include <vector>                       /* std::vector                  */

#include "midi/event.hpp"

/*
 *  Do not document a namespace; it breaks Doxygen.
 */

namespace seq66
{

/**
 *  The number of MIDI notes supported.  The notes range from 0 to 127.
 */

const int c_num_keys = 128;

/**
 *  The eventlist class is a receptable for MIDI events.
 */

class eventlist
{
    friend class editable_events;       // access to event_key class
    friend class midifile;              // access to print()
    friend class sequence;              // any_selected_notes()

public:

    /**
     * Actions.  These variables represent actions that can be applied to a
     * selection of notes.  One idea would be to add a swing-quantize action.
     * We will reserve the value here, for notes only; not yet used or part of
     * the action menu.
     */

    enum class edit
    {
        select_all_notes = 1,
        select_all_events,
        select_inverse_notes,
        select_inverse_events,
        quantize_notes,
        quantize_events,
        randomize_events,
        tighten_events,
        tighten_notes,
        transpose_notes,            /* basic transpose          */
        reserved,                   /* later: quantize_swing    */
        transpose_harmonic,         /* harmonic transpose       */
        expand_pattern,
        compress_pattern,
        select_even_notes,
        select_odd_notes,
        swing_notes                 /* swing quantize           */
    };

    /**
     *  This enumeration is used in selecting events and note.  Se the
     *  select_note_events() and select_events() functions.
     */

    enum class select
    {
        selecting,      /**< Selection in progress.                     */
        select_one,     /**< To select a single event.                  */
        selected,       /**< The events are selected.                   */
        would_select,   /**< The events would be selected.              */
        deselect,       /**< To deselect event under the cursor.        */
        toggle,         /**< Toggle selection under cursor.             */
        remove,         /**< To remove one note under the cursor.       */
        onset,          /**< Kepler34, To select a single onset.        */
        is_onset        /**< New, from Kepler34, onsets selected.       */
    };

private:

    /**
     *  Provides a key value for an event map.  Its types match the
     *  m_timestamp and get_rank() function of this event class.
     */

    class event_key
    {

    private:

        midipulse m_timestamp;  /**< The primary key-value for the key. */
        int m_rank;             /**< The sub-key-value for the key.     */

    public:

        event_key (midipulse tstamp, int rank);
        event_key (const event & e);
        bool operator < (const event_key & rhs) const;
        event_key (const event_key & ek) = default;
        event_key & operator = (const event_key & ek) = default;
    };

public:

    using Events = std::vector<event>;
    using iterator = Events::iterator;
    using const_iterator = Events::const_iterator;
    using reverse_iterator = Events::reverse_iterator;
    using const_reverse_iterator = Events::const_reverse_iterator;

private:

    /**
     *  This list holds the current pattern/sequence events.
     */

    Events m_events;

    /**
     *  Holds the length of the sequence holding this event-list,
     *  in pulses (ticks).  See sequence::m_length.
     */

    midipulse m_length;

    /**
     *  Provides the number of ticks to shave off of the end of painted notes.
     *  Also used when the user attempts to shrink a note to zero (or less
     *  than zero) length.
     */

    const midipulse mc_note_off_margin;

    /**
     *  A flag to indicate if an event was added or removed.  We may need to
     *  give client code a way to reload the sequence.  This is currently an
     *  issue when a seqroll and an eventedit/eventslots are active for the
     *  same sequence.
     */

    bool m_is_modified;

    /**
     *  A new flag to indicate that a tempo event has been added.  Legacy
     *  behavior forces the tempo to be written to the track-0 sequence,
     *  but we don't want to do that if the MIDI file (or the current event
     *  list) contains a tempo event.
     */

    bool m_has_tempo;

    /**
     *  A new flag to indicate that a time-signature event has been added.
     *  Legacy behavior forces the time-signature to be written to the track-0
     *  sequence, but we don't want to do that if the MIDI file (or the
     *  current event list) contains a time-signature event.
     */

    bool m_has_time_signature;

public:

    eventlist ();
    eventlist (const eventlist & a_rhs);
    eventlist & operator = (const eventlist & a_rhs);
    virtual ~eventlist ()
    {
        // No code needed
    }

    iterator begin ()
    {
        return m_events.begin();
    }

    const_iterator cbegin () const
    {
        return m_events.cbegin();
    }

    iterator end ()
    {
        return m_events.end();
    }

    const_iterator cend () const
    {
        return m_events.cend();
    }

    /**
     *  Returns the number of events stored in m_events.  We like returning
     *  an integer instead of size_t, and rename the function so nobody is
     *  fooled.
     */

    int count () const
    {
        return int(m_events.size());
    }

    midipulse get_max_timestamp () const;

    bool empty () const
    {
        return m_events.empty();
    }

    /**
     *  Adds an event to the internal event list in a sorted manner.  Note
     *  that, for speed, it is better to call append() for each event, and
     *  then later sort them.
     *
     * \param e
     *      Provides the event to be added to the list.
     *
     * \return
     *      Returns true.  We assume the insertion succeeded, and no longer
     *      care about an increment in container size.  It's a multimap, so it
     *      always inserts, and if we don't have memory left, all bets are off
     *      anyway.
     */

    bool add (const event & e)
    {
        bool result = append(e);
        sort();                         /* by time-stamp and "rank" */
        return result;
    }

    bool append (const event & e);

    midipulse get_length () const
    {
        return m_length;
    }

    midipulse note_off_margin () const
    {
        return mc_note_off_margin;
    }

    bool is_modified () const
    {
        return m_is_modified;
    }

    bool has_tempo () const
    {
        return m_has_tempo;
    }

    bool has_time_signature () const
    {
        return m_has_time_signature;
    }

    /**
     * \setter m_is_modified
     *      This function may be needed by some of the sequence editors.
     *      But use it with great caution.
     */

    void unmodify ()
    {
        m_is_modified = false;
    }

    /**
     *  Provides a wrapper for the iterator form of erase(), which is the
     *  only one that sequence uses.  Currently, no check on removal is
     *  performered.  Sets the modified-flag.
     *
     * \param ie
     *      Provides the iterator to the event to be removed.
     *
     * \return
     *      Returns an iterator to the next element, or end() if the container
     *      is now empty.
     */

    iterator remove (iterator ie)
    {
        iterator result = m_events.erase(ie);
        m_is_modified = true;
        return result;
    }

    /**
     *  Provides a wrapper for clear().  Sets the modified-flag.
     */

    void clear ()
    {
        if (! m_events.empty())
        {
            m_events.clear();
            m_is_modified = true;
        }
    }

    void merge (eventlist & el, bool presort = true);

    /**
     *  Sorts the event list.  For the vector, equivalent elements are not
     *  guaranteed to keep their original relative order [see
     *  std::stable_sort(), which we could try at some point].
     */

    void sort ()
    {
        std::sort(m_events.begin(), m_events.end());
    }

    /**
     *  Dereference access for list or map.
     *
     * \param ie
     *      Provides the iterator to the event to which to get a reference.
     */

    static event & dref (iterator ie)
    {
        return *ie;
    }

    /**
     *  Dereference const access for list or map.
     *
     * \param ie
     *      Provides the iterator to the event to which to get a reference.
     */

    static const event & cdref (const_iterator ie)
    {
        return *ie;
    }

private:                                /* internal quantization functions  */

    bool add (Events & evlist, const event & e);
    void merge (const Events & evlist);

private:                                /* functions for friend sequence    */

    /*
     * The following functions provide internal for-loops that do not
     * involved data from the caller.
     */

    void link_new ();
    void clear_links ();
#if defined USE_FILL_TIME_SIG_AND_TEMPO
    void scan_meta_events ();
#endif
    void verify_and_link (midipulse slength = 0);
    bool edge_fix (midipulse snap, midipulse seqlength);
    bool quantize_events
    (
        midibyte status, midibyte cc, int snap,
        int divide, bool fixlink
    );
    midipulse adjust_timestamp (midipulse t, bool isnoteoff);
    bool move_selected_notes (midipulse delta_tick, int delta_note);
    bool randomize_selected
    (
        midibyte status, midibyte control, int plus_minus
    );
    bool randomize_selected_notes (int jitter, int range);
    bool link_new_note (event & eon, event & eoff);
    bool link_note (event & eon, event & eoff);
    void link_tempos ();
    void clear_tempo_links ();
    bool mark_selected ();
    void mark_out_of_range (midipulse slength);
    void mark_all ();
    void unmark_all ();
    bool remove_event (event & e);
    bool remove_marked ();
    void unpaint_all ();
    int count_selected_notes () const;
    bool any_selected_notes () const;
    int count_selected_events (midibyte status, midibyte cc) const;
    bool any_selected_events (midibyte status, midibyte cc) const;
    void select_all ();
    void unselect_all ();
    int select_events
    (
        midipulse tick_s, midipulse tick_f,
        midibyte status, midibyte cc, select action
    );
    int select_note_events
    (
        midipulse tick_s, int note_h,
        midipulse tick_f, int note_l, select action
    );
    bool event_in_range
    (
        const event & e, midibyte status,
        midipulse tick_s, midipulse tick_f
    ) const;
    bool get_selected_events_interval
    (
        midipulse & first, midipulse & last
    ) const;
    bool stretch_selected (midipulse delta);
    bool grow_selected (midipulse delta, int snap);
    midipulse trim_timestamp (midipulse t) const;
    midipulse clip_timestamp
    (
        midipulse ontime, midipulse offtime, int snap
    ) const;

#if defined USE_STAZED_SELECTION_EXTENSIONS
    int select_linked (midipulse tick_s, midipulse tick_f, midibyte status);
#endif

    void print () const;

    const Events & events () const
    {
        return m_events;
    }

    void set_length (midipulse len)
    {
        m_length = len;
    }

};          // class eventlist

}           // namespace seq66

#endif      // SEQ66_EVENTLIST_HPP

/*
 * eventlist.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

