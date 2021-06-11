#ifndef SEQ66_FINDDEFAULT_H
#define SEQ66_FINDDEFAULT_H

/*
 *  This file is part of seq66, adapted from the PortMIDI project.
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
 * \file        finddefault.h
 *
 *      Provides a function to find the default MIDI device.
 *
 * \library     seq66 application
 * \author      PortMIDI team; new from Chris Ahlstrom
 * \date        2018-04-08
 * \updates     2018-04-08
 * \license     GNU GPLv2 or above
 *
 *  This file is included by files that implement library internals.
 *  However, Seq66 doesn't use it, since it has its own configuration
 *  files, located in ~/.config/seq66/ or in
 *  C:/Users/username/AppData/Local/.
 */

#include "portmidi.h"                   /* PmDeviceID   */

#if defined __cplusplus
extern "C"
{
#endif

#if defined SEQ66_PORTMIDI_FIND_DEFAULT_DEVICE
extern PmDeviceID find_default_device (char * path, int input, PmDeviceID);
#endif

#if defined __cplusplus
}               // extern "C"
#endif

#endif          // SEQ66_FINDDEFAULT_H

/*
 * finddefault.h
 *
 * vim: sw=4 ts=4 wm=4 et ft=c
 */

