#if ! defined SEQ66_CLINSMANAGER_HPP
#define SEQ66_CLINSMANAGER_HPP

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
 * \file          clinsmanager.hpp
 *
 *  This module declares/defines the main module for the JACK/ALSA "qt5"
 *  implementation of this application.
 *
 * \library       clinsmanager application
 * \author        Chris Ahlstrom
 * \date          2020-08-31
 * \updates       2020-08-31
 * \license       GNU GPLv2 or above
 *
 *  This is an attempt to change from the hoary old (or, as H.P. Lovecraft
 *  would style it, "eldritch") gtkmm-2.4 implementation of Seq66.
 */

#include <memory>                       /* std::unique_ptr<>                */

#include "sessions/smanager.hpp"        /* seq66::smanager                  */

#if defined SEQ66_NSM_SUPPORT
#include "nsm/nsmclient.hpp"            /* seq66::nsmclient                 */
#endif

/**
 *  The potential list of capabilities is
 *
 *  -   switch:       Client is capable of responding to multiple `open`
 *                    messages without restarting.
 *  -   dirty:        Client knows when it has unsaved changes.
 *  -   progress:     Client can send progress updates during time-consuming
 *                    operations.
 *  -   message:      Client can send textual status updates.
 *  -   optional-gui: Client has an optional GUI.
 */

#define SEQ66_NSM_CLI_CAPABILITIES      ":message:"

namespace seq66
{

/**
 *
 */

class clinsmanager : public smanager
{

private:

    bool m_nsm_active;

#if defined SEQ66_NSM_SUPPORT

    /**
     *  The optional NSM client.  This item is not in the base class,
     *  smanager, because that class is meant to allow the option of building
     *  without NSM, but still simplifying the application's main() function.
     */

    std::unique_ptr<nsmclient> m_nsm_client;

#endif

public:

    clinsmanager (const std::string & caps = SEQ66_NSM_CLI_CAPABILITIES);
    virtual ~clinsmanager ();

    virtual bool create_session
    (
        int argc        = 0,
        char * argv []  = nullptr
    ) override;
    virtual bool close_session (bool ok = true) override;
    virtual void show_message (const std::string & msg) const override;
    virtual void show_error (const std::string & msg = "") const override;
    virtual bool run () override;
    virtual void session_manager_name (const std::string & mgrname) override;

};          // class clinsmanager

}           // namespace seq66

#endif      // SEQ66_CLINSMANAGER_HPP

/*
 * clinsmanager.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
