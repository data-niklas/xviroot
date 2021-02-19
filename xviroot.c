#include <xine.h>
#include <xine/xineutils.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xinerama.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

#include "xviroot.h"
#include "args.h"
#include "arraylist.h"

static Atom ATOM_DESKTOP, ATOM_ATOM, ATOM_STRING, ATOM_WINDOW_TYPE, ATOM_STATE, ATOM_STICKY, ATOM_BELOW;

//Callbacks
static void dest_size_cb(void *data, int video_width, int video_height, double video_pixel_aspect,
                         int *dest_width, int *dest_height, double *dest_pixel_aspect)
{

    *dest_width = in.xlib.width;
    *dest_height = in.xlib.height;
    *dest_pixel_aspect = in.xlib.pixel_aspect;
}

/* this will be called by xine when it's about to draw the frame */
static void frame_output_cb(void *data, int video_width, int video_height,
                            double video_pixel_aspect, int *dest_x, int *dest_y,
                            int *dest_width, int *dest_height,
                            double *dest_pixel_aspect, int *win_x, int *win_y)
{
    *dest_x = 0;
    *dest_y = 0;
    *win_x = in.xlib.x;
    *win_y = in.xlib.y;
    *dest_width = in.xlib.width;
    *dest_height = in.xlib.height;
    *dest_pixel_aspect = in.xlib.pixel_aspect;
}

static void xine_event_cb(void *user_data,
                          const xine_event_t *event)
{
    switch (event->type)
    {
    case XINE_EVENT_UI_PLAYBACK_FINISHED:
    {
        int newindex = next_track();
        xine_close(in.player.stream);
        in.player.playing = False;
        if (newindex == tracks())
        {
            if (in.player.loop)
                newindex = 0;
            else
            {
                finish();
                exit(0);
            }
        }
        in.player.index = newindex;
        play();
        break;
    }
    }
}

//Initialization
void init_xine()
{
    in.player.xine = xine_new();
    xine_init(in.player.xine);

    x11_visual_t vis;
    vis.display = in.player.dpy;
    vis.d = in.xlib.window;
    vis.screen = DefaultScreen(in.xlib.dpy);
    vis.dest_size_cb = dest_size_cb;
    vis.frame_output_cb = frame_output_cb;
    vis.user_data = NULL;

    char *vo_driver = "auto";
    char *ao_driver = "auto";
    if (in.player.ignore_audio)
        ao_driver = "none";
    in.player.vo_port = xine_open_video_driver(in.player.xine, vo_driver, XINE_VISUAL_TYPE_X11, (void *)&vis);
    in.player.ao_port = xine_open_audio_driver(in.player.xine, ao_driver, NULL);

    in.player.stream = xine_stream_new(in.player.xine, in.player.ao_port, in.player.vo_port);
    xine_set_param(in.player.stream, XINE_PARAM_GAPLESS_SWITCH, 1);

    in.player.queue = xine_event_new_queue(in.player.stream);
    xine_event_create_listener_thread(in.player.queue, xine_event_cb, NULL);
}

void create_window()
{

    if (XineramaIsActive(in.xlib.dpy))
    {
        if (in.xlib.screen > -1)
        {
            int screens;
            XineramaScreenInfo *infos = XineramaQueryScreens(in.xlib.dpy, &screens);
            for (int i = 0; i < screens; i++)
            {
                XineramaScreenInfo info = infos[i];
                if (info.screen_number == in.xlib.screen)
                {
                    in.xlib.x += info.x_org;
                    in.xlib.y += info.y_org;
                    if (in.xlib.width == 0 || in.xlib.height == 0)
                    {
                        in.xlib.width = info.width;
                        in.xlib.height = info.height;
                    }
                    break;
                }
            }
        }
    }

    int default_screen = DefaultScreen(in.xlib.dpy);

    if (in.xlib.width == 0 || in.xlib.height == 0)
    {
        in.xlib.width = DisplayWidth(in.xlib.dpy, default_screen);
        in.xlib.height = DisplayHeight(in.xlib.dpy, default_screen);
    }
    in.xlib.root = XRootWindow(in.xlib.dpy, default_screen);

    XSetWindowAttributes attr;
    attr.override_redirect = True;
    in.xlib.window = XCreateWindow(in.xlib.dpy, in.xlib.root, in.xlib.x, in.xlib.y, in.xlib.width, in.xlib.height, 0, DefaultDepth(in.xlib.dpy, default_screen), InputOutput, DefaultVisual(in.xlib.dpy, default_screen), CWOverrideRedirect, &attr);

    double res_h = (DisplayWidth(in.xlib.dpy, default_screen) * 1000 / DisplayWidthMM(in.xlib.dpy, default_screen));
    double res_v = (DisplayHeight(in.xlib.dpy, default_screen) * 1000 / DisplayHeightMM(in.xlib.dpy, default_screen));
    in.xlib.pixel_aspect = res_v / res_h;

    XChangeProperty(in.xlib.dpy, in.xlib.window, ATOM_WINDOW_TYPE, ATOM_ATOM, 32, PropModeReplace, (void *)&ATOM_DESKTOP, 1);
    XChangeProperty(in.xlib.dpy, in.xlib.window, ATOM_STATE, ATOM_ATOM, 32, PropModeAppend, (void *)&(Atom[]){ATOM_STICKY, ATOM_BELOW}, 2);
    XSelectInput(in.xlib.dpy, in.xlib.window, StructureNotifyMask | ExposureMask | ButtonPressMask);
    XMapWindow(in.xlib.dpy, in.xlib.window);
    XLowerWindow(in.xlib.dpy, in.xlib.window);
    XFlush(in.xlib.dpy);
}

void get_atoms()
{
    ATOM_WINDOW_TYPE = XInternAtom(in.xlib.dpy, "_NET_WM_WINDOW_TYPE", 0);
    ATOM_DESKTOP = XInternAtom(in.xlib.dpy, "_NET_WM_WINDOW_TYPE_DESKTOP", 0);
    ATOM_STATE = XInternAtom(in.xlib.dpy, "_NET_WM_STATE", 0);
    ATOM_STICKY = XInternAtom(in.xlib.dpy, "_NET_WM_STATE_STICKY", 0);
    ATOM_BELOW = XInternAtom(in.xlib.dpy, "_NET_WM_STATE_BELOW", 0);
    ATOM_ATOM = XInternAtom(in.xlib.dpy, "ATOM", 0);
    ATOM_STRING = XInternAtom(in.xlib.dpy, "STRING", 0);
}

//Player methods
int next_track()
{
    if (in.player.shuffle)
    {
        return rand() % tracks();
    }
    else
    {
        return in.player.index + 1;
    }
}

char *get_track(int index)
{
    return (char *)arraylist_get(in.player.videos, index);
}

int tracks()
{
    return arraylist_size(in.player.videos);
}

void play()
{
    in.player.playing = True;
    if (!xine_open(in.player.stream, get_track(in.player.index)) || !xine_play(in.player.stream, 0, 0))
    {
        puts("Error occured");
        exit(1);
    }
    xine_set_param(in.player.stream, XINE_PARAM_AUDIO_VOLUME, in.player.volume);
}

static int start_pos, start_time;
void play_pause()
{
    if (in.player.playing)
    {
        int length;
        xine_get_pos_length(in.player.stream, &start_pos, &start_time, &length);
        xine_stop(in.player.stream);
    }
    else
    {
        xine_play(in.player.stream, start_pos, start_time);
    }
    in.player.playing = !in.player.playing;
}

void inc_volume()
{
    in.player.volume += 5;
    if (in.player.volume > 100)
        in.player.volume = 100;
    xine_set_param(in.player.stream, XINE_PARAM_AUDIO_VOLUME, in.player.volume);
}

void dec_volume()
{
    in.player.volume -= 5;
    if (in.player.volume < 0)
        in.player.volume = 0;
    xine_set_param(in.player.stream, XINE_PARAM_AUDIO_VOLUME, in.player.volume);
}

void event_loop()
{
    XEvent event;
    while (1)
    {
        XNextEvent(in.xlib.dpy, &event);
        switch (event.type)
        {
        case Expose:
        {
            if (event.xexpose.count == 0)
            {
                xine_port_send_gui_data(in.player.vo_port, XINE_GUI_SEND_EXPOSE_EVENT, (void *)&event);
            }
            break;
        }
        case UnmapNotify:
        {
            xine_port_send_gui_data(in.player.vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, NULL);
            break;
        }
        case MapNotify:
        {
            xine_port_send_gui_data(in.player.vo_port, XINE_GUI_SEND_DRAWABLE_CHANGED, (void *)in.xlib.window);
            break;
        }
        case ButtonPress:
        {
            switch (event.xbutton.button)
            {
            case Button1:
                play_pause();
                break;
            case Button3:
                finish();
                exit(0);
                break;
            case Button4:
                inc_volume();
                break;
            case Button5:
                dec_volume();
                break;
            }
            break;
        }
        }
    }
}

void finish()
{
    //And free
    xine_event_dispose_queue(in.player.queue);
    xine_dispose(in.player.stream);
    xine_close_audio_driver(in.player.xine, in.player.ao_port);
    xine_close_video_driver(in.player.xine, in.player.vo_port);
    xine_exit(in.player.xine);
    XCloseDisplay(in.player.dpy);

    XDestroyWindow(in.xlib.dpy, in.xlib.window);
    XCloseDisplay(in.xlib.dpy);
    exit(0);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    in = parse_args(argc, argv);
    get_atoms();
    create_window();
    init_xine();
    if (tracks() == 0)
    {
        puts("No videos specified");
        return 1;
    }
    in.player.index = -1;
    in.player.index = next_track();
    play();
    event_loop();
    finish();
    return 0;
}