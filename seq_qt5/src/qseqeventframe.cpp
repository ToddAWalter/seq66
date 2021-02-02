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
 * \file          qseqeventframe.cpp
 *
 *  This module declares/defines the base class for plastering
 *  pattern/sequence data information in the data area of the pattern
 *  editor.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2018-08-13
 * \updates       2021-01-31
 * \license       GNU GPLv2 or above
 *
 */

#include "cfg/settings.hpp"             /* SEQ66_QMAKE_RULES indirectly     */
#include "play/sequence.hpp"            /* seq66::sequence                  */
#include "qseqeventframe.hpp"           /* seq66::qseqeventframe            */
#include "qseventslots.hpp"             /* seq66::qseventslots              */

#if defined SEQ66_QMAKE_RULES
#include "forms/ui_qseqeventframe.h"
#else
#include "forms/qseqeventframe.ui.h"
#endif

/*
 *  Do not document the name space.
 */

namespace seq66
{

/**
 *  For correcting the width of the event table.  It tries to account for the
 *  width of the vertical scroll-bar, plus a bit more.
 */

static const int sc_event_table_fix = 48;

/**
 *  Specifies the current hardwired value for set_row_heights().
 */

static const int sc_event_row_height = 18;

/**
 *
 * \param p
 *      Provides the performer object to use for interacting with this sequence.
 *      Among other things, this object provides the active PPQN.
 *
 * \param seqid
 *      Provides the sequence number.  The sequence pointer is looked up using
 *      this number.  This number is also the pattern-slot number for this
 *      sequence and for this window.  Ranges from 0 to 1024.
 *
 * \param parent
 *      Provides the parent window/widget for this container window.  Defaults
 *      to null.
 *
 */

qseqeventframe::qseqeventframe (performer & p, int seqid, QWidget * parent)
  :
    QFrame                  (parent),
    performer::callbacks    (p),
    ui                      (new Ui::qseqeventframe),
    m_seq                   (p.sequence_pointer(seqid)),
    m_eventslots            (new qseventslots(p, *this, m_seq)),
    m_is_dirty              (false)
{
    ui->setupUi(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    /*
     * Sequence Title.
     */

    connect
    (
        ui->m_entry_name, SIGNAL(textChanged(const QString &)),
        this, SLOT(update_seq_name())
    );
    set_seq_title(make_seq_title());

    std::string ts_ppqn = std::to_string(m_seq->get_beats_per_bar());
    ts_ppqn += "/";
    ts_ppqn += std::to_string(m_seq->get_beat_width());
    ts_ppqn += " at ";
    ts_ppqn += std::to_string(m_seq->get_ppqn());
    ts_ppqn += " PPQN";
    set_seq_time_sig_and_ppqn(ts_ppqn);

    std::string channelstr = "Channel ";
    channelstr += std::to_string(int(m_seq->get_midi_channel()) + 1);
    channelstr += " [re 1]";
    set_seq_channel(channelstr);

    /*
     * Measures and Event.  The event-slot object should keep these up-to-date
     * during editing.
     */

    set_seq_lengths(get_lengths());

    /*
     * Event Table and scroll area.  Some setup is already done in the
     * setupUi() function as configured via Qt Creator.
     *
     * "This" object is the parent of eventScrollArea.  The scroll-area
     * contains scrollAreaWidgetContents, which is the parent of
     * eventTableWidget.
     *
     * Note that setRowHeight() will need to be called for any new rows that
     * get added to the table.  However, there is no function for that!
     *
     * ui->eventTableWidget->setRowHeight(16);
     */

    QStringList columns;
    columns << "Time" << "Event" << "Chan" << "Data 0" << "Data 1" << "Link";
    ui->eventTableWidget->setHorizontalHeaderLabels(columns);
    ui->eventTableWidget->setSelectionBehavior
    (
        QAbstractItemView::SelectRows           /* SelectItems          */
    );
    ui->eventTableWidget->setSelectionMode
    (
        QAbstractItemView::SingleSelection      /* MultiSelection       */
    );
    set_row_heights(sc_event_row_height);
    set_column_widths(ui->eventTableWidget->width() - sc_event_table_fix);

    /*
     * Doesn't make the table read-only.  We want that for now, until we can
     * get time to modify events in-place.
     *
     * ui->eventTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
     */

    connect
    (
        ui->eventTableWidget, SIGNAL(currentCellChanged(int, int, int, int)),
        this, SLOT(handle_table_click_ex(int, int, int, int))
    );

    /*
     * Delete button.  Will set to enabled/disabled once fully initialized.
     */

    connect
    (
        ui->button_del, SIGNAL(clicked(bool)),
        this, SLOT(handle_delete())
    );
    ui->button_del->setEnabled(false);

    /*
     * Insert button.
     */

    connect
    (
        ui->button_ins, SIGNAL(clicked(bool)),
        this, SLOT(handle_insert())
    );
    ui->button_ins->setEnabled(true);

    /*
     * Modify button.
     */

    connect
    (
        ui->button_modify, SIGNAL(clicked(bool)),
        this, SLOT(handle_modify())
    );
    ui->button_modify->setEnabled(false);

    /*
     * Save button.
     */

    connect
    (
        ui->button_save, SIGNAL(clicked(bool)),
        this, SLOT(handle_save())
    );
    ui->button_save->setEnabled(false);

    /*
     * Clear button.
     */

    connect
    (
        ui->button_clear, SIGNAL(clicked(bool)),
        this, SLOT(handle_clear())
    );
    ui->button_clear->setEnabled(true);

    /*
     * Dump button.
     */

    connect
    (
        ui->button_dump, SIGNAL(clicked(bool)),
        this, SLOT(handle_dump())
    );
    ui->button_dump->setEnabled(true);

    /*
     * Load the data.
     */

    initialize_table();

    /*
     *  The event editor is now in a tab, and it is not quite as critical as the
     *  pattern editor.  The following setting causes the "File / New" operation
     *  to seem to mysteriously fail.
     *
     *      m_seq->seq_in_edit(true);
     */

    m_seq->set_dirty_mp();
    cb_perf().enregister(this);
}

qseqeventframe::~qseqeventframe()
{
    cb_perf().unregister(this);
    delete ui;
}

/**
 *  Check for dirtiness (perhaps), clear the table and settings, an reload as
 *  if starting again.
 */

bool
qseqeventframe::on_sequence_change (seq::number seqno, bool recreate)
{
    bool result = m_seq && seqno == m_seq->seq_number();
    if (result)
    {
        if (m_is_dirty)
        {
            // ignore and pop up a warning dialog?

            result = false;
        }
        else
        {
            if (recreate)
                initialize_table();
        }
#if defined SEQ66_PLATFORM_DEBUG_TMI
        printf("qseqeventframe::on_sequence_change(%d)\n", seqno);
#endif
    }
    return result;
}

void
qseqeventframe::set_row_heights (int height)
{
    const int rows = ui->eventTableWidget->rowCount();
    for (int r = 0; r < rows; ++r)
        ui->eventTableWidget->setRowHeight(r, height);  /* set_row_height() */
}

/**
 *  Sets the height of each row in the table.
 */

void
qseqeventframe::set_row_height (int row, int height)
{
    ui->eventTableWidget->setRowHeight(row, height);
}

/**
 *  Scales the columns against the provided window width.
 */

void
qseqeventframe::set_column_widths (int total_width)
{
    ui->eventTableWidget->setColumnWidth(0, int(0.15f * total_width));
    ui->eventTableWidget->setColumnWidth(1, int(0.25f * total_width));
    ui->eventTableWidget->setColumnWidth(2, int(0.10f * total_width));
    ui->eventTableWidget->setColumnWidth(3, int(0.140f * total_width));
    ui->eventTableWidget->setColumnWidth(4, int(0.140f * total_width));
    ui->eventTableWidget->setColumnWidth(5, int(0.20f * total_width));
}

/**
 *  Clears, then refills the event table from the qseventslots object.
 */

bool
qseqeventframe::initialize_table ()
{
    bool result = false;
    if (m_eventslots)
    {
        int rows = m_eventslots->event_count();
        if (rows > 0)
        {
            ui->eventTableWidget->clearContents();
            ui->eventTableWidget->setRowCount(rows);
            set_row_heights(sc_event_row_height);
            if (m_eventslots->load_table())
            {
                m_eventslots->select_event(0);      /* first row */
            }
            // ui->button_del->setEnabled(true);
            // ui->button_modify->setEnabled(true);
            ui->button_clear->setEnabled(true);
        }
        else
        {
            ui->button_clear->setEnabled(false);
            ui->button_del->setEnabled(false);
            ui->button_modify->setEnabled(false);
        }
    }
    return result;
}

std::string
qseqeventframe::make_seq_title ()
{
#if defined USE_THIS_CODE // I don't like it!
    std::string title = m_seq->seq_number_string();
    title += ". \"";
    title += m_seq->name();
    title += "\"";
    return title;
#else
    return m_seq->name();
#endif
}

/**
 *  Sets ui->m_entry_name to the title.
 *
 * \param title
 *      The name of the sequence.
 */

void
qseqeventframe::set_seq_title (const std::string & title)
{
    ui->m_entry_name->setText(title.c_str());
}

/**
 *  Handles edits of the sequence title.
 */

void
qseqeventframe::update_seq_name ()
{
    std::string name = ui->m_entry_name->text().toStdString();
    if (cb_perf().set_sequence_name(m_seq, name))
        set_dirty();
}

/**
 *  Sets ui->label_time_sig to the time-signature string.
 *  Also adds the parts-per-quarter-note string.
 *
 *  Combines the set_seq_time_sig() and set_seq_ppqn() from the old
 *  user-interface.
 *
 * \param sig
 *      The time signature of the sequence.
 */

void
qseqeventframe::set_seq_time_sig_and_ppqn (const std::string & sig)
{
    ui->label_time_sig->setText(sig.c_str());
}

void
qseqeventframe::set_seq_channel (const std::string & ch)
{
    ui->label_channel->setText(ch.c_str());
}

/**
 *  Sets the number of measure and the number of events string.
 *
 *      m_eventslots->m_seq->calculate_measures()
 *
 *  Combines set_seq_count() and set_length() into one function.
 */

void
qseqeventframe::set_seq_lengths (const std::string & mevents)
{
    ui->label_measures_ev_count->setText(mevents.c_str());
}

/**
 *  Sets ui->label_category to the category string.
 *
 * \param c
 *      The category string for the current event.
 */

void
qseqeventframe::set_event_category (const std::string & c)
{
    ui->label_category->setText(c.c_str());
}

/**
 *  Sets ui->entry_ev_timestamp to the time-stamp string.
 *
 * \param ts
 *      The time-stamp string for the current event.
 */

void
qseqeventframe::set_event_timestamp (const std::string & ts)
{
    ui->entry_ev_timestamp->setText(ts.c_str());
}

/**
 *  Sets ui->entry_ev_name to the name-of-event string.
 *
 * \param n
 *      The name-of-event string for the current event.
 */

void
qseqeventframe::set_event_name (const std::string & n)
{
    ui->entry_ev_name->setText(n.c_str());
}

/**
 *  Sets ui->entry_ev_data_0 to the first data byte string.
 *
 * \param d
 *      The first data byte string for the current event.
 */

void
qseqeventframe::set_event_data_0 (const std::string & d)
{
    ui->entry_ev_data_0->setText(d.c_str());
}

/**
 *  Sets ui->entry_data_1 to the second data byte string.
 *
 * \param d
 *      The second data byte string for the current event.
 */

void
qseqeventframe::set_event_data_1 (const std::string & d)
{
    ui->entry_ev_data_1->setText(d.c_str());
}

/**
 *  Retrieve the table cell at the given row and column.
 *
 * \param row
 *      The row number, which should be in range.
 *
 * \param col
 *      The column enumeration value, which will be in range.
 *
 * \return
 *      Returns a pointer the table widget-item for the given row and column.
 *      If out-of-range, a null pointer is returned.
 */

QTableWidgetItem *
qseqeventframe::cell (int row, column_id col)
{
    int column = int(col);
    QTableWidgetItem * result = ui->eventTableWidget->item(row, column);
    if (is_nullptr(result))
    {
        /*
         * Will test row/column and maybe add rows on the fly later.
         */

        result = new QTableWidgetItem;
        ui->eventTableWidget->setItem(row, column, result);
    }
    return result;
}

void
qseqeventframe::set_event_line
(
    int row,
    const std::string & evtimestamp,
    const std::string & evname,
    const std::string & evchannel,
    const std::string & evdata0,
    const std::string & evdata1,
    const std::string & linktime
)
{
    QTableWidgetItem * qtip = cell(row, column_id::timestamp);
    if (not_nullptr(qtip))
        qtip->setText(evtimestamp.c_str());

    qtip = cell(row, column_id::eventname);
    if (not_nullptr(qtip))
        qtip->setText(evname.c_str());

    qtip = cell(row, column_id::channel);
    if (not_nullptr(qtip))
        qtip->setText(evchannel.c_str());

    qtip = cell(row, column_id::data_0);
    if (not_nullptr(qtip))
        qtip->setText(evdata0.c_str());

    qtip = cell(row, column_id::data_1);
    if (not_nullptr(qtip))
        qtip->setText(evdata1.c_str());

    qtip = cell(row, column_id::link);
    if (not_nullptr(qtip))
        qtip->setText(linktime.c_str());
}

/**
 *  Sets the "modified" status of the user-interface.  This includes changing
 *  a label, enabling/disabling the Save button, and modifying the event count
 *  and sequence length (in measures).
 *
 * \param flag
 *      If true, the modified status is indicated, otherwise it is cleared.
 *      The default value is true.
 */

void
qseqeventframe::set_dirty (bool flag)
{
    if (flag)
    {
        ui->button_save->setEnabled(true);
        m_is_dirty = true;
    }
    else
    {
        ui->button_save->setEnabled(false);
        m_is_dirty = false;
    }
}

/**
 *  Needs to be defined in cpp file due to being an incomplete type in the
 *  header.
 */

int
qseqeventframe::current_row () const
{
    return m_eventslots->current_row();
}

/**
 *  Needs to be defined in cpp file due to being an incomplete type in the
 *  header.
 */

void
qseqeventframe::current_row (int row)
{

#if defined SEQ66_PLATFORM_DEBUG_TMI
    int checkrow = ui->eventTableWidget->currentRow();
    printf("row %d; checkrow %d\n", row, checkrow);
#endif

    m_eventslots->current_row(row);
}

void
qseqeventframe::handle_table_click_ex
(
    int row, int /*column*/, int /*prevrow*/, int /*prevcolumn*/
)
{
    if (row >= 0)
    {
        m_eventslots->select_event(row);
        current_row(row);
        ui->button_del->setEnabled(true);
        ui->button_modify->setEnabled(true);
    }
}

std::string
qseqeventframe::get_lengths ()
{
    std::string meas_events = std::to_string(m_eventslots->calculate_measures());
    meas_events += " measures, ";
    meas_events += std::to_string(m_eventslots->event_count());
    meas_events += " events";
    return meas_events;
}

/**
 *  Initiates the deletion of the current editable event.  We call both of the
 *  following.  Though they seem redundant, the first call is needed to
 *  hightlight the row visually, and the second makes the actual selection.
 *
 *  -   ui->eventTableWidget->setCurrentIndex(next);
 *  -   ui->eventTableWidget->selectionModel()->select(next, ...);
 *
 *  These are alternatives we tried, FYI only:
 *
 *  -   QItemSelectionModel::ClearAndSelect|QItemSelectionModel::Rows
 *  -   ui->eventTableWidget->viewport().update();
 */

void
qseqeventframe::handle_delete ()
{
    if (m_eventslots)
    {
        editable_event & current = m_eventslots->current_event();
        int row0 = m_eventslots->current_row();
        int row1 = m_eventslots->count_to_link(current);
        bool islinked = row1 >= 0;
        if (islinked && row0 > row1)
            std::swap(row0, row1);
        else
            row1 = row0;

        if (islinked)
            m_eventslots->select_event(row1, false);        /* only/last row */

        bool was_removed = m_eventslots->delete_current_event();
        if (was_removed)
        {
            int cr = row1;
            ui->eventTableWidget->removeRow(row1);
            if (islinked)
            {
                m_eventslots->select_event(row0, false);    /* first row     */
                was_removed = m_eventslots->delete_current_event();
                if (was_removed)
                {
                    cr = row0;
                    ui->eventTableWidget->removeRow(row0);
                }
            }
            if (m_eventslots->empty())
            {
                QModelIndex next = ui->eventTableWidget->model()->index(cr, 0);
                ui->eventTableWidget->setCurrentIndex(next);
                ui->eventTableWidget->selectionModel()->select
                (
                    next, QItemSelectionModel::Rows
                );
                m_eventslots->select_event(cr);
                current_row(cr);
            }
            set_dirty();
        }
        if (m_eventslots->empty())
        {
            ui->button_del->setEnabled(false);
            ui->button_modify->setEnabled(false);
        }
        set_seq_lengths(get_lengths());
    }
}

/**
 *  Initiates the insertion of a new editable event.  The event's location
 *  will be determined by the timestamp and existing events.  Note that we
 *  have to recalibrate the scroll-bar when we insert/delete events.
 *
 *  As a feature, we will allow events to extend the official length of the
 *  sequence.
 *
 *  We have to figure out where the new event goes, its new index into
 *  the container, and add the new table row in the corresponding place.
 */

void
qseqeventframe::handle_insert ()
{
    if (m_eventslots)
    {
        std::string ts = ui->entry_ev_timestamp->text().toStdString();
        std::string name = ui->entry_ev_name->text().toStdString();
        std::string data0 = ui->entry_ev_data_0->text().toStdString();
        std::string data1 = ui->entry_ev_data_1->text().toStdString();
        std::string linktime;                   /* empty, no link time yet  */
        bool has_events = m_eventslots->insert_event(ts, name, data0, data1);
        set_seq_lengths(get_lengths());
        if (has_events)
        {
            std::string chan = m_eventslots->current_event().channel_string();
            int cr = m_eventslots->current_row();
            ui->eventTableWidget->insertRow(cr);
            set_row_height(cr, sc_event_row_height);
            set_event_line(cr, ts, name, chan, data0, data1, linktime);
            ui->button_del->setEnabled(true);
            ui->button_modify->setEnabled(true);
            set_dirty();
        }
    }
}

/**
 *  Passes the edited fields to the current editable event in the eventslot.
 *  Note that there are two cases to worry about.  If the timestamp has not
 *  changed, then we can simply modify the existing current event in place.
 *  Otherwise, we need to delete the old event and insert the new one.
 *  But that is done for us by eventslots::modify_current_event().
 */

void
qseqeventframe::handle_modify ()
{
    if (m_eventslots)
    {
        int cr = current_row();
        const editable_event & ev = m_eventslots->current_event();
        std::string ts = ui->entry_ev_timestamp->text().toStdString();
        std::string name = ui->entry_ev_name->text().toStdString();

        /*
         * Which one is better?  The one use would allow for events on more
         * than one channel in this pattern/loop.
         *
         * std::string chan = std::to_string(int(m_seq->get_midi_channel()));
         */

        std::string chan = ev.channel_string();
        std::string data0 = ui->entry_ev_data_0->text().toStdString();
        std::string data1 = ui->entry_ev_data_1->text().toStdString();
        midipulse lt = c_null_midipulse;
        if (ev.is_linked())
            lt = ev.link_time();

        std::string linktime = m_eventslots->time_string(lt);
        (void) m_eventslots->modify_current_event(cr, ts, name, data0, data1);
        set_seq_lengths(get_lengths());
        set_event_line(cr, ts, name, chan, data0, data1, linktime);
        set_dirty();
    }
}

/**
 *  Handles saving the edited data back to the original sequence.
 *  The event list in the original sequence is cleared, and the editable
 *  events are converted to plain events, and added to the container, one by
 *  one.
 *
 *  Also tells performer to notify its subscribers.  This will cause
 *  qseqeventframe::on_sequence_change() to be called, so be careful of loops!
 *
 * \todo
 *      Could also support writing the events to a new sequence, for added
 *      flexibility.
 *
 * \todo
 *      Make sure that performer now calls the notification apparatus!
 */

void
qseqeventframe::handle_save ()
{
    if (m_eventslots)
    {
        bool ok = m_eventslots->save_events();
        if (ok)
        {
            seq::number seqno = m_seq->seq_number();
            cb_perf().notify_sequence_change(seqno);
            ui->button_save->setEnabled(false);
            m_is_dirty = false;
#if defined SEQ66_PLATFORM_DEBUG
            if (rc().verbose())
            {
                std::string dump = m_eventslots->events_to_string();
                printf("%s", dump.c_str());
            }
#endif
        }
    }
}

void
qseqeventframe::handle_clear ()
{
    if (m_eventslots)
    {
        m_eventslots->clear();
        initialize_table();
        set_dirty();
    }
}

void
qseqeventframe::handle_dump ()
{
    if (m_eventslots)
    {
        std::string dump = m_eventslots->events_to_string();
        printf("%s", dump.c_str());
    }
}


/**
 *  Cancels the edits and closes the dialog box.  In order for removing the
 *  current-highlighting in the mainwd or perfedit windows, some of the work
 *  of handle_close() needs to be done here as well.
 */

void
qseqeventframe::handle_cancel ()
{
    // TO BE DETERMINED
}

}           // namespace seq66

/*
 * qseqeventframe.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

