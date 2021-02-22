# XViRoot - XLib Video Wallpaper
## Why?
- Video wallpapers - yay!
- Also uses a minimal amount of resources

## Features
- Play any videos as your wallpaper
- Sound is annoying? Set the volume or disable the audio completely!
- Don't want to loop the same video over and over? Just play several videos or don't loop at all
- Shuffling is of course also possible (in which case the video will be looped)
- I don't know why, but playing songs is also possible
- Clicky stuff:
    - click on the left side to play the previous video
    - click in the middle to play / pause
    - click on the right side to play the next video
    - right click to close
    - scroll to adjust volume
- Multi monitor support:
    - select any monitor to play on, or just specify a position and size

## Installation
- dependency libxine needs to be installed
- Clone the repo
- run `make build`
- copy / move the `xviroot` executable into any bin folder, e.g. `/bin`

## Usage
- Any specified file is added to the video queue
- `--help` or `-h` displays the help
- `volume` or `-v` [0-100] sets the volume
- `ignore_audio` or `-ia` does not play audio
- `loop` or `-l` to loop
- `shuffle` or `-sh` to shuffle
- `screen` or `-s` to set the screen to be played on (-1 to use the linux root), default is the default screen
- set `x`, `y`, `width` and `height` to specify a position / size
- e.g.: `xviroot loop -ia screen -1 x 30 y 60 width 500 somevideo.mp4 othervideo.mp4`