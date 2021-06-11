#if ! defined SEQ66_MASTERMIDIBUS_HPP
#define SEQ66_MASTERMIDIBUS_HPP

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
 * \file          mastermidibus.hpp
 *
 *  This module declares the right version of the mastermidibus header for the
 *  current API.
 *
 * \library       seq66 application
 * \author        Chris Ahlstrom
 * \date          2016-11-28
 * \updates       2019-02-09
 * \license       GNU GPLv2 or above
 *
 */

#include "seq66-config.h"

#if defined SEQ66_RTMIDI_SUPPORT
#include "mastermidibus_rm.hpp"         /* seq66::mastermidibus for RtMidi  */
#elif defined SEQ66_PORTMIDI_SUPPORT
#include "mastermidibus_pm.hpp"         /* seq66::mastermidibus, PortMidi   */
#elif defined SEQ66_WINDOWS_SUPPORT
#include "mastermidibus_pm.hpp"         /* Windows uses PortMIDI now        */
#else
#include "mastermidibus_rm.hpp"         /* seq66::mastermidibus default     */
#endif

#endif      // SEQ66_MASTERMIDIBUS_HPP

/*
 * mastermidibus.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

