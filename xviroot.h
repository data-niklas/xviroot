#ifndef XVIROOT_H
#define XVIROOT_H

#include <xine.h>
#include <xine/xineutils.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <paths.h>

#include "arraylist.h"


typedef struct {
    Window window;
    Window root;
    Display*dpy;
    int screen;
    int x;
    int y;
    int width;
    int height;
    double pixel_aspect;
} xlib_t;

typedef struct {
    xine_t *xine;
    xine_stream_t*stream;
    xine_audio_port_t*ao_port;
    xine_video_port_t*vo_port;
    xine_event_queue_t* queue;
    Display*dpy;

    int volume;

    int ignore_audio;
    int loop;
    int shuffle;
    int playing;
    int index;
    arraylist * videos;
} player_t;

typedef struct{
    xlib_t xlib;
    player_t player;
} instance_t;

static instance_t in;
void create_window();

char * get_track(int index);
int tracks();
int next_track();
int prev_track();
void next();
void prev();
void play();
void play_pause();
void inc_volume();
void dec_volume();
void finish();
#endif