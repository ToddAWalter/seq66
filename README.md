# README for Seq66 0.99.9 2023-09-21

__Seq66__: MIDI sequencer/live-looper with a hardware-sampler grid interface;
pattern banks, triggers, and playlists for song management; scale and chord
aware piano-roll; song layout for creative composition; and control/status
via MIDI automation for live performance.  Mute-groups enable/disable multiple
patterns at once.  Supports the Non/New Session Manager; can also run headless.
Works in a space as small as 725x500 pixels (less if window decoration removed).
It does not support audio samples, just MIDI.

__Seq66__ is a major refactoring of Sequencer64/Kepler34/Seq24 with modern C++
and new features.  Linux and Windows users can build this application from
source code.  See the extensive INSTALL file.  Includes a comprehensive PDF
user-manual.

Support sites (still in progress):

    *   https://ahlstromcj.github.io/
    *   https://github.com/ahlstromcj/ahlstromcj.github.io/wiki

The release now includes an installer for the 64-bit Windows version of Seq66.

![Alt text](doc/latex/images/main-window/main-windows.png?raw=true "Seq66")

# Major Features

##  User interface

    *   Qt 5 (cross-platform).  Loop-button gird. Qt style-sheet support.
    *   Tabs and external windows for patterns, sets, mute-groups, song
        layout, event-editing, play-lists, and session information.
    *   Low-frequency oscillator (LFO) to modify continuous controller
        and velocity values.
    *   A "fixer" for expansion/compression/alignment of note patterns.
    *   Colorable pattern slots; the color palette can be saved and modified.
    *   Horizontal and vertical zoom in the pattern and song editors.
    *   A headless/daemon version can be built.

##  Configuration files

    *   Supports configuration files: '.rc', '.usr', '.ctrl', '.mutes',
        '.playlist', '.drums' (note-mapping), '.palette', and Qt '.qss'.
    *   Separates MIDI control and mute-group setting into their own files.
    *   Unified keystroke and MIDI controls in the '.ctrl' file; defines MIDI
        controls for automation/display of Seq66 status in grid controllers
        (e.g. LaunchPad).  Sample '.ctrl' files provided for Launchpad Mini.

##  Non/New Session Manager

    *   Support for NSM/New Session Manager, RaySession, Agordejo....
    *   Handles starting, stopping, hiding, and session saving.
    *   Displays details about the session.

##  Multiple Builds

    *   ALSA/JACK: `qseq66` using an rtmidi-based library
    *   Command-line/headless: `seq66cli`
    *   PortMidi: `qpseq66`
    *   Windows: `qpseq66.exe`

##  More Features

    *   Transposable triggers to re-use patterns more comprehensively.
    *   Song import/export from/to stock MIDI (SMF 0 or 1).
    *   Improved non-U.S. keyboard support.
    *   Many demonstration and test MIDI files.
    *   See **Recent Changes** below, and the **NEWS** file.

##  Internal

    *   More consistent use of modern C++, auto, and lambda functions.
    *   Additional performer callbacks to reduce polling.
    *   A ton of clean-up and refactoring.

Seq66 uses a Qt 5 user-interface based on Kepler34 and the Seq66 *rtmidi*
(Linux) and *portmidi* (Windows) engines.  MIDI devices are detected,
inaccessible devices are ignored, with playback (e.g. to the Windows wavetable
synth). It is built easily via *GNU Autotools*, *Qt Creator* or *qmake*, using
*MingW*.  See the INSTALL file for build-from-source instructions for Linux or
Windows, and using a conventional source tarball.

## Recent Changes

    *   Version 0.99.9:
        *   Added an "Input Bus Routing" feature, where each pattern can be
            set to receive events from a given input buss. Selectable from
            the grid-slot popup menu.
        *   Fixed nasty segfault opening new file while Editor tab open.
        *   Fixed bug: port-mapping Remap and Restart did not work due to
            timing.
        *   Fixed error in "quiet" startup that would cause immediate exit.
        *   Related to issue #115: Added ability to select a line in the data
            pane and grab a handle to change its value.
        *   Refactored and extended zoom support, added it to event and data
            panes.
        *   Adding more seqroll keystokes (and HTML help). Enabled Esc to
            exit paint mode if not playing.
        *   Added live-note mapping (needs testing!), refactoring set-record
            code.
        *   Implemented automation for BBT/HMS toggling, FF/Rewind, Undo/Redo,
            Play-set Copy/Paste, and Record Toggle.
        *   Added a build-time option to add a show/hide button in the
            main window to allow making the window to take up much less space.
        *   Added HTML help files to data/share/doc/info, other documentation
            upgrades.
    *   Version 0.99.8:
        *   Issue #112: A new pattern now displays in the MIDI controller.
        *   Issue #114: Adding display of shortcut keys to tool tips.
        *   Added a Pattern tab to Edit / Preferences for new-pattern settings
            and jitter/randomization.
        *   Added automation for the main window Loop L/R button.
        *   Fixed seqroll drawing errors introduced in adding time-sig support.
        *   Fixed incomplete data-pane refresh in scrolling with arrow-keys.
        *   Fixed not setting up SIGINT, which prevented a proper shutdown.
        *   Fixed a couple corrupted data/midi/FM/*.mid files.
        *   Changing playlist setting enables Session Restart button.
        *   Removed coloring of record-style and -mode buttons. Added coloring
            of event-editor "Store" button to denote saving is needed.
        *   Refactoring quantization alterations for future upgrades. Added
            an option to jitter the notes in the seqroll.
        *   Enforced that configuration files are stored in the "home"
            directory.
        *   The usual raft of humiliating bug-fixes. A small sample: Updating
            the event list when recording stops; fixing record button in pattern
            editor; fixing note-selection refresh; and about a dozen more.

    See the "NEWS" file for changes in earlier versions.  Some proposed features
    will be pushed off to Seq66v2; see the bottom of the TODO file. Version
    2 is probably a year or two away :-( So many things to improve!

// vim: sw=4 ts=4 wm=2 et ft=markdown
