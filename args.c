#include <stdio.h>
#include "args.h"
#include <unistd.h>
#include "arraylist.h"
#include <X11/Xlib.h>

void print_help(){
    puts("\
Any specified file is added to the video queue\n\
`--help` or `-h` displays the help\n\
`volume` or `-v` [0-100] sets the volume\n\
`ignore_audio` or `-ia` does not play audio\n\
`loop` or `-l` to loop\n\
`shuffle` or `-sh` to shuffle\n\
`screen` or `-s` to set the screen to be played on (-1 to use the linux root), default is the default screen\n\
set `x`, `y`, `width` and `height` to specify a position / size\n\
e.g.: `xviroot loop -ia screen -1 x 30 y 60 width 500 somevideo.mp4 othervideo.mp4`");
}

instance_t parse_args(int argc, char*argv[]){
    instance_t config;
    config.player.playing = False;
    config.player.loop = False;
    config.player.shuffle = False;
    config.player.index = 0;
    config.xlib.x= 0;
    config.xlib.y = 0;
    config.xlib.width = 0;
    config.xlib.height = 0;
    config.player.ignore_audio = 0;
    config.xlib.dpy = XOpenDisplay(NULL);
    config.player.dpy = XOpenDisplay(NULL);
    config.xlib.screen = DefaultScreen(config.player.dpy);

    config.player.volume = 100;


    config.player.videos = arraylist_create();
    for (int i = 1; i < argc;i++){
        if (!strcmp(argv[i], "x"))config.xlib.x= atoi(argv[++i]);
        else if (!strcmp(argv[i], "y"))config.xlib.y = atoi(argv[++i]);
        else if (!strcmp(argv[i], "width"))config.xlib.width = atoi(argv[++i]);
        else if (!strcmp(argv[i], "height"))config.xlib.height = atoi(argv[++i]);
        else if (!strcmp(argv[i], "screen") || !strcmp(argv[i], "-s"))config.xlib.screen = atoi(argv[++i]);
        else if (!strcmp(argv[i], "volume") || !strcmp(argv[i], "-v"))config.player.volume = atoi(argv[++i]);
        else if (!strcmp(argv[i], "loop") || !strcmp(argv[i], "-l"))config.player.loop = True;
        else if (!strcmp(argv[i], "ignore_audio") || !strcmp(argv[i], "-ia"))config.player.ignore_audio = True;
        else if (!strcmp(argv[i], "shuffle") || !strcmp(argv[i], "-sh"))config.player.shuffle = True;
        else if (!strcmp(argv[i], "help") || !strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")){
            print_help();
            XCloseDisplay(config.xlib.dpy);
            XCloseDisplay(config.player.dpy);
            exit(0);
        }
        else{
            if (!access(argv[i], F_OK)){
                //Probably is a video
                arraylist_add(config.player.videos, strdup(argv[i]));
            }
            else{
                printf("Unknown argument %s\n",argv[i]);
            }
        }
    }

    return config;
}

