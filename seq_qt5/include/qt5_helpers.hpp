#if ! defined SEQ66_QT5_HELPERS_HPP
#define SEQ66_QT5_HELPERS_HPP

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
 * \file          qt5_helpers.hpp
 *
 *  This module declares/defines some helpful free functions to support Qt and
 *  C++ integration.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2018-03-14
 * \updates       2025-01-23
 * \license       GNU GPLv2 or above
 *
 */

#include "ctrl/keymap.hpp"              /* seq66::qt_modkey_ordinal()       */
#include "ctrl/keystroke.hpp"           /* seq66::keystroke wrapper class   */

/*
 *  Currently disabled because it's nearly impossible to get the absolute
 *  position of the mouse correct with multiple monitors.
 */

#undef  SEQ66_SHOW_GENERIC_TOOLTIPS

/**
 *  The install_scroll_filter_function is currently unused.
 */

#undef  SEQ66_INSTALL_SCROLL_FILTER

class QAction;
class QComboBox;
class QIcon;
class QKeyEvent;
class QLayoutItem;
class QLineEdit;
class QMenu;
class QPushButton;
class QScrollArea;
class QSpinBox;
class QTimer;
class QWidget;

/*
 * Don't document the namespace.
 */

namespace seq66
{

class combolist;

/*
 *  Free constants in the seq66 namespace.  These values are simply visible
 *  booleans for using file dialogs.
 */

const bool SavingFile = true;
const bool OpeningFile = false;
const bool ConfigFile = true;
const bool NormalFile = false;

/*
 * Free functions in the seq66 namespace.
 */

extern std::string qt_get_color (QPushButton * button);
extern std::string qt_set_color
(
    const std::string & colorname, QPushButton * button
);
extern void qt_set_icon (const char * pixmap_array [], QPushButton * button);
extern std::string qt_icon_theme ();
extern bool qt_prompt_ok
(
    const std::string & text,
    const std::string & info
);
extern void qt_info_box (QWidget * self, const std::string & msg);
extern void qt_error_box (QWidget * self, const std::string & msg);
extern keystroke qt_keystroke
(
    QKeyEvent * event,
    keystroke::action rp,
    bool testing = false
);
extern QString qt (const std::string & text);
extern void qt_set_layout_visibility (QLayoutItem * item, bool visible);
extern QTimer * qt_timer
(
    QObject * self,
    const std::string & name,
    int redraw_factor,
    const char * slotname
);
extern void enable_combobox_item
(
    QComboBox * box, int index, bool enabled = true
);
extern void set_combobox_item
(
    QComboBox * box, int index,
    const std::string & text
);
extern bool fill_combobox
(
    QComboBox * box,
    const combolist & clist,
    std::string value           = "",
    const std::string & prefix  = "",
    const std::string & suffix  = ""
);

#if defined SEQ66_INSTALL_SCROLL_FILTER
extern bool install_scroll_filter (QWidget * monitor, QScrollArea * target);
#endif

extern void set_spin_value (QSpinBox * spin, int value);
extern QAction * new_qaction (const std::string & text, const QIcon & micon);
extern QAction * new_qaction (const std::string & text, QObject * parent);
extern QMenu * new_qmenu (const std::string & text, QWidget * parent = nullptr);
extern bool show_open_midi_file_dialog (QWidget * parent, std::string & file);
extern bool show_import_midi_file_dialog (QWidget * parent, std::string & file);
extern bool show_select_project_dialog
(
    QWidget * parent,
    std::string & selecteddir,
    std::string & selectedfile
);
extern bool show_playlist_dialog
(
    QWidget * parent,
    std::string & file,
    bool saving
);
extern bool show_text_file_dialog (QWidget * parent, std::string & file);
extern bool show_exe_file_dialog (QWidget * parent, std::string & file);
extern bool show_file_dialog
(
    QWidget * parent,
    std::string & selectedfile,
    const std::string & prompt      = "",
    const std::string & filterlist  = "",
    bool saving                     = false,
    bool forceconfig                = false,
    const std::string & extension   = "",
    bool promptoverwrite            = true
);
extern bool show_file_select_dialog
(
    QWidget * parent,
    const std::string & extension,
    std::string & selecteddir,
    std::string & selectedfile
);
extern bool show_folder_dialog
(
    QWidget * parent,
    std::string & selectedfolder,
    const std::string & prompt      = "",
    bool forcehome                  = false
);
extern void tooltip_with_keystroke
(
    QWidget * widget,
    const std::string & keyname,
    int duration = -1
);
extern void tooltip_for_filename
(
    QLineEdit * lineedit,
    const std::string & filespec,
    int duration = -1
);

#if defined SEQ66_SHOW_GENERIC_TOOLTIPS

extern void generic_tooltip
(
    QWidget * widget,
    const std::string & tiptext,
    int x, int y,
    int msduration = -1
);

#endif

extern bool is_empty (const QLineEdit * lineedit);

}               // namespace seq66

#endif          // SEQ66_QT5_HELPERS_HPP

/*
 * qt5_helpers.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

