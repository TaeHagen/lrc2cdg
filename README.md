# lrc2cdg
Lrc2cdg is an open source converter written in c++ to convert lrc files to cdg files for karaoke machines. Also supports [emscripten](https://github.com/kripken/emscripten) for use on the web.

## Quick Start

1. Create an LRC file here: https://stage.elsdoerfer.com/elrcmaker/#. 
2. Drag an mp3 file onto the page, then click the Import button at the top right corner of the page.
3. Next, import the lyrics by pasting the text lyrics in the new window that appears. Click the "Parse & Load" button when finished.
4. Play the song, and click the Space bar on your keyboard when you hear a word.
5. When the song is finished, click on Export and save the lrc file.
6. Go to our [webpage](https://taehagen.github.io/lrc2cdg/Emscripten%20compiled/) and upload the lrc file. A new window will immediately appear allowing you to download a CDG file.
7. Move this file to a USB stick, along with the MP3 file with the exact same filename. (same filename, different extensions)
8. Put that USB stick into your karaoke machine, and sing. You might need to switch the mode to USB on your karaoke machine.

## Using lrc2cdg
To use lrc2cdg you will need a lrc file. Lrc files are not easy to find but easy to make. We use [Enhanced LRC maker](https://stage.elsdoerfer.com/elrcmaker/#) (Click the help button in the top right for instructions)
### Web interface
Go to our [webpage](https://taehagen.github.io/lrc2cdg/Emscripten%20compiled/) and pick the lrc file. A download box will popup with the cdg file. You can choose the color of the text that shows up on the karaoke machine before choosing your file.
### CLI
After installing lrc2cdg(instructions below), use the command as follows:
`lrc2cdg infile.lrc outfile.cdg r g b`
The R, G, and B arguments are optional.

## Viewing these files on a karaoke machine
Most karaoke machines have a usb port. Get a usb stick and plug it in to your computer. Add 2 files to the usb stick, songname.mp3 and songname.cdg. They both must have the same name(but different extensions) so the karaoke machine will play them together. Remove the usb stick from your computer and plug it into the karaoke machine. You may need to change the mode of the karaoke machine to USB.

## Building
To build, g++ and make must be installed. Clone the repository, cd to the repostory, and run `make && make install`. This will build lrc2cdg. To build it for the web with emscripten, emscripten build tools must be in the path. Then run `make emscripten`. Move lrc2cdg.js and lrc2cdg.wasm to the Emscripten Compiled folder, then serve that folder with a web server and navigate to the server. Clearing chrome's cache may be nesesary between changes.

## Modifying
In CDG, there are 12x6 tiles. For text, lrc2cdg uses 4 tiles for one character.
All the characters are stored in values.h, in the array called alphabet. The array is ordered a-z, with special characters after z. The location of special characters is defined in the drawString function. If you are unhappy with the font, you can change the font in the alphabet array. The characters in the alphabet array are 24x12px with 1 representing color, 0 representing black. You can also add other characters by adding an entry to the alphabet array and adding a special character in the drawString function.

## CD+G Specification
The program is written around this [CD+G Specification](https://jbum.com//cdg_revealed.html)