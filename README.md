# TRIAD-6
A fantasy ternary computer that will provide retro gaming and application support.

This project is currently a very work-in-progress. However, that means one can still learn as the project gets developed.

As of right now, a desktop PC or a powerful embedded system is needed to run sufficiently.  The minimum right now is a Raspberry Pi 4 (which can operate between 34-35 to 61-63 FPS depending on instruction makeup, though could change to lower or higher as new features get added).

## Currently Supported Renderers
- OpenGL 2.1 (Should run on most desktop PCs and even some embedded systems like the Raspberry Pi 4.  Some extensions are used.)
- OpenGL ES 3.0 (This should run on more modern embedded systems like the Raspberry Pi 4.)

## System Requirements
- Current minimum on a non-desktop PC is the Raspberry Pi 4 and other systems similar in class.  Must be able to run at least 25-30 FPS or it will be unusable (good examples of systems unlikely to run well include Raspberry Pi 3 and earlier as well as others).  That's because simulating a ternary computer on a binary computer accurately is very CPU intensive (at least not without any optimization).

- Current recommended for full performance is likely any modern desktop PC (probably the earliest in this class can be from mid to late 2000s), though untested.  Though DirectX 9 support will likely not exist; OpenGL 2.1 should suffice in those cases.  Systems that can meet 60 FPS at least on average easily are recommended.
