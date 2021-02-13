# README for Seq66 0.92.0 (Sequencer64 refactored for C++/14 and Qt 5)

Chris Ahlstrom
2015-09-10 to 2021-02-13

__Seq66__ is a MIDI sequencer and live MIDI looper with a hardware sampler-like
grid pattern interface, MIDI automation for live performance, with sets and
playlists for song management, a scale/chord-aware piano-roll interface, a
song editor for creative composition, and control via mouse, keystrokes, and
MIDI.  Supports NSM (New Session Manager) on Linux, can also be run headless.

__Seq66__ is a refactoring of the Qt version of Sequencer64/Kepler34, a reboot
of __Seq24__ with modern C++ and new features.  Linux users can build this
application from the source code.  See the INSTALL file.  Windows users can
get an installer package on GitHub.  A PDF user-manual is also provided.

# Major Features (also see **Recent Changes** below):

##  User interface

    *   Qt 5 (good cross-platform support).  No "grid of sets", but
        unlimited external windows.
    *   Tabs for management of sets, mute-groups, song mode, pattern
        editing, event-editing, play-lists, and session information.
    *   The live frame uses buttons matching Qt theming.
    *   A color for each sequence can be chosen to make them stand out.
        The color number is saved in a *SeqSpec* associated with the track.
        The color palette can be saved and modified.

##  Configuration files

    *   Separated MIDI control and mute-group setting into their own files,
        with support for hex notation.
    *   Supported configuration files: ".rc", ".usr", ".ctrl", ".mutes",
        ".playlist", ".drums" (note-mapping), and ".palette".
    *   Unified keystroke control and MIDI control into a ".ctrl" file. It
        defines MIDI In controls for controlling Seq66, and MIDI Out controls
        for displaying Seq66 status in grid controllers (e.g.  LaunchPad).
        Basic 4x8 and 8x8 ".ctrl" files for the Launchpad Mini provided.

##  Non Session Manager

    *   Support for this manager is essentially complete.
    *   Handles stopping and saving
    *   Handle display of details about the session.

##  Internal

    *   More consistent use of modern C++, auto, and lambda functions.
    *   Additional performer callbacks to reduce the need for polling.

##  Multiple Builds

    *   ALSA/JACK: `qseq66`
    *   Command-line/daemon: `seq66cli`
    *   PortMidi: `qpseq66`
    *   Windows: `qpseq66.exe`

Seq66 uses a Qt 5 user-interface based on Kepler34 and the Seq66 *rtmidi*
(Linux) and *portmidi* (Windows) engines.  MIDI devices are detected,
inaccessible devices are ignored, with playback (e.g. to the Windows wavetable
synth). It is built easily via *GNU Autotools*, *Qt Creator* or *qmake*, using
*MingW*.  See the INSTALL file for build-from-source instructions for Linux or
Windows, and using a conventional source tarball.

## Recent Changes

    *   Version 0.92.0:
        *   Fixed issue #34: "seq66 does not follow jack_transport tempo changes"
        *   Fixed issues with applying 'usr' buss and instrument names to the
            pattern-editor menus.
        *   Fixing serious issues with the event editor. Now deletes both
            linked notes.
        *   Added mute-group-out to show mutegroups on Launchpad Mini buttons.
        *   Tightened up mute-group handling, configuration file handling,
            play-list handling, and MIDI display device handling.
        *   Stream-line the format of the 'ctrl' file by removing columns for
            the values of "enabled" and "channel".  Will detect older formats
            and fix them.
        *   A PDF user manual is now provided in the doc directory, installed
            to /usr/local/share/doc/seq66-0.92 with "make install".
    *   Version 0.91.6:
        *   Massively updated the Mutes tab.
        *   More documentation.
        *   More fixes to the song editor.
        *   Added more files to the creation setup at the first run of Seq66.
    *   Version 0.91.5:
        *   Added vertical zoom to the pattern editor (V keys and buttons).
        *   Added more control over the coloring of notes.
        *   Still improving the port-mapper feature.
        *   Added quotes to file-paths in the 'rc' configuration file.
        *   Many fixes to seqedit, perfedit.  Way too many to mention them
            all.  Changed the 4/4 and length selections to be editable.
        *   Getting serious about rewriting the user manual in this project.
    *   Version 0.91.4:
        *   Improved port naming and provide an option for short or long port
            names.
        *   Improved safety in NSM sessions.
        *   Major refactoring the color handling.  Colors have changed!!!
    *   Version 0.91.3:
        *   Added check to not apply last mute-group if in Song mode.
        *   Provisional support for playing multiple sets at once and for
            auto-arming the selected set when loaded.
        *   Added a configurable number for virtual MIDI input/output ports.
        *   Provides an option to for multiple-set playback and auto-arming
            of a newly-selected set.
        *   Fixed bug in string-to-int conversion uncovered by automatic
            mute-group restore.
        *   Refactoring port naming and I/O lists.
        *   Minor play-list fixes.
    *   Version 0.91.2:
        *   Fix developer bug causing playlists to not load properly.
        *   Fix crash when 'rc' file specifies empty mutes and ctrl files.
    *   Version 0.91.1:
        *   More fixes for mute-group (mutes) handling.
        *   Ability to save the last-active mute-group for restoring on startup.
        *   Added a button to toggle insert mode in pattern and song editors.
        *   Robustness enhancement to NSM support.
    *   Version 0.91.0:
        *   Non Session Manager support essentially complete.  The
            refactoring to do this was massive.
        *   All too many bug fixes and minor improvements.
        *   Added --copy-dt-needed-entries to qseq66 Makefile.am to fix
            linkage errors that cropped up in debug builds.
        *   Got the CLI version building, needs a lot of testing.
        *   Playlist editing from the user-interface much improved.

    See the "NEWS" file for changes in earlier versions.

# vim: sw=4 ts=4 wm=4 et ft=markdown
