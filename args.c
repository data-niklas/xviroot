#include <stdio.h>
#include "args.h"
#include <unistd.h>
#include "arraylist.h"
#include <X11/Xlib.h>

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
        else if (!strcmp(argv[i], "screen"))config.xlib.screen = atoi(argv[++i]);
        else if (!strcmp(argv[i], "volume") || !strcmp(argv[i], "-v"))config.player.volume = atoi(argv[++i]);
        else if (!strcmp(argv[i], "loop") || !strcmp(argv[i], "-l"))config.player.loop = True;
        else if (!strcmp(argv[i], "ignore_audio") || !strcmp(argv[i], "-ia"))config.player.ignore_audio = True;
        else if (!strcmp(argv[i], "shuffle") || !strcmp(argv[i], "-s"))config.player.shuffle = True;
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

