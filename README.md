# PC98Launcher

## What is it?

PC98Launcher is graphical game browser and launcher for the PC-98 range of personal computers from NEC.

It runs on the PC-98 hardware itself, either emulated or real physical system; it is __not__ a Window/Mac/Linux application like [launchbox](https://www.launchbox-app.com/) or [RetroArch](https://github.com/libretro/RetroArch).

Specifically, it is written to use the 256 colour graphics hardware found on late PC-9801 and early to late PC-9821 machines.

   * It creates a browseable list of all the game directories on your drive(s)
   * It can load and display additional metadata per game (i.e developer, genre, release date etc)
   * It can load and display screenshots or artwork per game (i.e box art, screenshots, etc)
   * It can export an audit file of all the found games
   * It can launch any game for which a start file is either found (e.g. start.bat), or which has been defined in metadata (e.g. run.com, go.exe, etc)

## Sample Use

The image below takes you to a sample movie of the launcher application in action via **YouTube**:

[![PC98Launcher in action](http://img.youtube.com/vi/ItZVIFY-c10/0.jpg)](https://
www.youtube.com/watch?v=ItZVIFY-c10)

*Click to the video above to see the launcher in action*

## What does it look like?

Starting up and scanning for games:

![PC98Launcher startup progress](git/git_what_1.png)

Main user interface:

![PC98Launcher startup progress](git/git_what_2.png)

Browsing game artwork:

![PC98Launcher startup progress](git/git_what_3.png)

![PC98Launcher startup progress](git/git_what_4.png)

----

## How do I run it?

You will need a system that meets these requirements:

   * A PC-9801 or PC-9821 with the 256 colour PEGC graphics hardware
   * A minimum of 4MB of RAM
   * A hard drive
   * A VCPI (EMM386 or VEM486) and DPMI server (DPMI.EXE from PC-98 DOS 6.22 works well)

You then need to copy the zipped binaries (see below) to a location on your PC-98 hard drive and unzip them to a directory of your choice.

The directory should have the following after unzipping:

   * launcher.exe
   * launcher.ini
   * l.bat
   * dpmi.exe
   * go32-v2.exe
   * assets\font8x16.bmp
   * assets\logo.bmp
   * assets\light\\*.bmp

You don't need to set anything in config.sys or autoexec.bat.

Edit launcher.ini with a normal text editor and set the `gamedirs` variable to all of the locations of games on your hard drive(s).

The following settings are available for launcher.ini:

   * gamedirs=A:\Path1,B:\Path2,B:\Path3 - List the directories which contain your game subdirectories
   * verbose=0|1 - Enable text mode logging for troubleshooting purposes
   * savedirs=0|1 - Save the scraped list of games to a text file at start
   * preload_names=0|1 - For each found game, attempt to load the metadata file to get its real name. This will slow initial scraping down.
   * keyboard_test=0|1 - Before starting the UI, prompt the user to do a quick input test

If you have your games under folders such as `A:\Games\Arkanoid` and `A:\Games\Dark` for example, then you only need to add the path `A:\Games`. You may add up to 16 comma seperated game paths, and these can be for different drives if you wish.

<u>Just run `l.bat` to start the application.</u>


#### Binaries

The latest pre-compiled binaries as well as pre-made metadata and screenshots can be found on my website under the [Tech Stuff](https://www.target-earth.net/wiki/doku.php?id=blog:tech#retro_computing_gaming) wiki:

   * [www.target-earth.net - Tech Stuff wiki](https://www.target-earth.net/wiki/doku.php?id=blog:tech)

----

## How to build

The code for PC98Launcher is all standard ANSI C, built using GCC and a custom version of DJGPP targetting the PC-98 hardware, rather than PC/DOS hardware.

A full guide of how to set up a build environment is on my wiki:

   * [www.target-earth.net - PC-98 Dev tools wiki](https://www.target-earth.net/wiki/doku.php?id=blog:pc98_devtools)


----

## Adding metadata & images


### Metadata files

You can add additional metadata to your game by placing a text file in each game directory named `launch.dat`.

The contents of this file is as follows:

```
[defaults]
name=
developer=
midi_mpu=
midi_serial=
year=
genre=
images=
series=
start=
alt_start=

```

The fields `name`, `developer` and `genre` are all simple text, maximimum of 32 characters.

The fields `midi_mpu` and `midi_serial` are integers, where __1__ means supported and any other value means unsupported.

The `images` field is a comma seperated list of the artwork available. The path is relative to the game directory, so `image01.bmp` will be expanded to include the drive and path that the game is located under (A:\Games\gamename\image01.bmp, for example).

Images will be shown in the order they are listed, so place the image you want shown by default as the first item in the list.

The `series` field is a text name of the larger game series in which the game is based, useful for those games in which there are more than one game (Doom and Doom II, for example). You can use the __filter__ option within the application to find all games within the same series, as long as they are tagged up with the correct metadata.

The `genre` field notes the type of gameplay within the game (RPG, Action, Sports, Puzzle, etc). As with `series` you can use the application __filter__ facility to restrict the display of games to just one type of genre if desired. Again, you need to set this metadata in order to make use of it within the interface, otherwise it will be treated as blank.

Finally, the `start` and `alt_start` data determines which exe should be launched when the game is selected. By convention I would reccomend the main game executable or batch file is set for the `start` value, and any stand-alone cinematics, or sound configuration utlity is set for the `alt_start` value.

An example is shown below:

```
[default]
name=Advanced Power Dolls 2
developer=Kogado Studio Inc.
midi_mpu=0
midi_serial=1
year=1996
genre=Strategy
images=cover.bmp,screen1.bmp,screen2.bmp,box.bmp
series=Power Dolls
start=game.exe
alt_start=config.exe
```


### Converting images to useable BMP files

The BMP files used by the application need to be 8bpp but of a limited palette so that the user interface does not suffer from colour issues (approximately 40 colours are reserved by the user interface and fonts).

You can use any image processing application you want, but it must output images of the following specifications:

   * BMP
   * Uncompressed
   * 8bpp, indexed/paletted colour
   * Maximum of 208 colours
   * No larger than 320x200 (but they may be smaller in either dimension, if desireable, e.g. for vertical boxart)


If you have the [ImageMagick](https://www.imagemagick.org/) tools available on your system, you can batch convert files using the following syntax:

```
convert INPUT.JPG -resize 320x200 -depth 8 -colors 208 -alpha OFF -compress none BMP3:OUTPUT.BMP
```