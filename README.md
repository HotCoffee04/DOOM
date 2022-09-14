# Coffee Doom

Welcome to yet another port of Doom!


## The goal of this port

The goal of Coffee Doom is to make DOOM available on Linux and Windows while not altering anything about the source code besides the implementation files (``i_video.c``,``i_sound.c``,``i_system.c``...).
In this way Coffee Doom can be a base for further modifications of the game, allowing to mess with the source code without having to go through the trouble of first porting it to a modern operating system.
Ultimately, this port was made as a way to learn SDL2, the library I used to port the video and audio.
While I couldn't implement some things, for example the music, I'm still proud of this port

## How it plays
Coffee Doom doesn't implement anything new, the controls are the same as the 1993 release on DOS, you can move with keyboard and mouse but to change key bindings you'll have to write the "``default.cfg``" yourself because this port doesn't have a setup executable.
By default the game launches at a stretched resolution of `4:3` to emulate how people used to play in the 90s, that's because even if doom internal resolution was `16:9` old monitors stretched it to fit the whole screen.
You can disable this feature by launching Coffee Doom with the parameter "``-no_stretch``".
You can launch the game in full screen with the parameter "``-fullscreen``" otherwise the game will start as a maximized window.

## Compiling and running


### On Linux
Enter the directory and run ``make``, then run ``coffeedoom`` inside the ``\linux`` folder.

### On Windows
Still have to port it to Windows.

