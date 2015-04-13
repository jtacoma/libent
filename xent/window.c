#define _XOPEN_SOURCE 500

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glx.h>
#include <X11/extensions/XInput2.h>

#include <ent.h>

#include "xent.h"
#include "window.h"

typedef float length_xy[2]; // always in meters

struct window
{
	struct ent_table * entities;
	Display * xdisplay;
	Window xwindow;
	GLXContext glx;
	int opcode;
	length_xy pixel;
	XWindowAttributes xwindow_attributes;
	Atom deleteWindow;
	bool stopping;
};

struct window *
window_alloc (
    struct ent_table * entities)
{
	struct window * w = calloc (1, sizeof (struct window));

	if (w)
	{
		w->entities = entities;
	}

	return w;
}

void
window_free (
    struct window * w)
{
	if (w)
	{
		window_teardown (w);
		free (w);
	}
}

int
window_setup (
    struct window * w)
{
	GLint att[] =
	{
		GLX_RGBA,
		GLX_DEPTH_SIZE, 24,
		GLX_DOUBLEBUFFER,
		None
	};

	w->xdisplay = XOpenDisplay (NULL);
	if (w->xdisplay == NULL)
	{
		printf ("failed to open display\n");
		return -1;
	}

	int error, event;

	if (!XQueryExtension (
	            w->xdisplay,
	            "XInputExtension",
	            &w->opcode,
	            &event,
	            &error))
	{
		XCloseDisplay (w->xdisplay);
		w->xdisplay = NULL;
		printf ("failed to find XInput extension\n");
		return -1;
	}

	int major = 2, minor = 2;
	XIQueryVersion (w->xdisplay, &major, &minor);
	if (major * 1000 + minor < 2002)
	{
		XCloseDisplay (w->xdisplay);
		w->xdisplay = NULL;
		printf ("XInput v%d.%d\n", major, minor);
		return -1;
	}
	else
	{
		printf ("XInput v%d.%d\n", major, minor);
	}

	Window xroot = DefaultRootWindow (w->xdisplay);
	XVisualInfo * vi = glXChooseVisual (w->xdisplay, 0, att);
	if (vi == NULL)
	{
		XCloseDisplay (w->xdisplay);
		w->xdisplay = NULL;
		printf ("failed to get visual\n");
		return -1;
	}
	else
	{
		printf ("Xvisual %p selected\n", (void *)vi->visualid);
	}

	Colormap cmap = XCreateColormap (w->xdisplay, xroot, vi->visual, AllocNone);
	XSetWindowAttributes swa =
	{
		.colormap = cmap,
		.event_mask = ExposureMask | KeyPressMask,
	};
	w->xwindow =
	    XCreateWindow (
	        w->xdisplay, xroot,
	        0, 0, 600, 600,
	        0, vi->depth, InputOutput, vi->visual,
	        CWColormap | CWEventMask, &swa);
	if (!w->xwindow)
	{
		XCloseDisplay (w->xdisplay);
		w->xdisplay = NULL;
		printf ("failed to find XInput extension\n");
		return -1;
	}

	w->deleteWindow = XInternAtom (w->xdisplay, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (w->xdisplay, w->xwindow, &w->deleteWindow, 1);
	XIDeviceInfo * info;
	int ndevices;
	info = XIQueryDevice (w->xdisplay, XIAllDevices, &ndevices);

	for (int i = 0; i < ndevices; i++)
	{
		XIDeviceInfo * dev = &info[i];

		for (int j = 0; j < dev->num_classes; j++)
		{
			XIAnyClassInfo * cls = dev->classes[j];
			XITouchClassInfo * t = (XITouchClassInfo *)cls;

			if (cls->type == XITouchClass
			        && t->mode == XIDirectTouch)
			{
				printf ("direct touch input: %s\n", dev->name);
			}
		}
	}

	unsigned char mask[XI_LASTEVENT] = {0};
	XIEventMask eventmask =
	{
		.deviceid = XIAllMasterDevices,
		.mask_len = sizeof (mask),
		.mask = mask,
	};
	XISetMask (mask, XI_ButtonPress);
	XISetMask (mask, XI_ButtonRelease);
	XISetMask (mask, XI_Motion);
	XISetMask (mask, XI_KeyPress);
	XISetMask (mask, XI_TouchBegin);
	XISetMask (mask, XI_TouchUpdate);
	XISetMask (mask, XI_TouchEnd);
	XISelectEvents (w->xdisplay, w->xwindow, &eventmask, 1);

	XMapWindow (w->xdisplay, w->xwindow);
	XStoreName (w->xdisplay, w->xwindow, "Simple App");

	w->glx = glXCreateContext (w->xdisplay, vi, NULL, GL_TRUE);
	glXMakeCurrent (w->xdisplay, w->xwindow, w->glx);

	return 0;
}

int
window_teardown (struct window * w)
{
	if (w->xdisplay)
	{
		if (w->glx)
		{
			glXMakeCurrent (w->xdisplay, None, NULL);
			glXDestroyContext (w->xdisplay, w->glx);
			w->glx = NULL;
		}

		if (w->xwindow)
		{
			XDestroyWindow (w->xdisplay, w->xwindow);
			w->xwindow = 0;
		}

		XCloseDisplay (w->xdisplay);
		w->xdisplay = NULL;
	}

	return 0;
}

int window_paint (struct window * w)
{
	XGetWindowAttributes (w->xdisplay, w->xwindow, &w->xwindow_attributes);

	glViewport (
	    0, 0,
	    w->xwindow_attributes.width, w->xwindow_attributes.height);

	length_xy screen_size =
	{
		WidthMMOfScreen (w->xwindow_attributes.screen) * 0.001,
		HeightMMOfScreen (w->xwindow_attributes.screen) * 0.001,
	};

	w->pixel[0] = screen_size[0] / WidthOfScreen (w->xwindow_attributes.screen);
	w->pixel[1] = screen_size[1] / HeightOfScreen (w->xwindow_attributes.screen);

	length_xy visual_size =
	{
		0.5 * w->pixel[0] * w->xwindow_attributes.width,
		0.5 * w->pixel[1] * w->xwindow_attributes.height
	};
	printf ("%0.3f x %0.3f\n", visual_size[0], visual_size[1]);

	// TODO: paint(visual_size);
	glClearColor (1.0, 1.0, 1.0, 1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glXSwapBuffers (w->xdisplay, w->xwindow);

	return 0;
}

int window_input (struct window * w)
{
	int error = 0;

	while (!error && XPending (w->xdisplay))
	{
		XEvent xevent;

		XNextEvent (w->xdisplay, &xevent);

		enum input_type input_type = 0;
		input_id input_id = 0;
		length_xy input_pos = {0, 0};
		enum input_action input_action = 0;

		if (xevent.type == ClientMessage)
		{
			if ((Atom)xevent.xclient.data.l[0] == w->deleteWindow)
			{
				printf ("window closed\n");
				w->stopping = true;
			}
		}
		else if (xevent.xcookie.type == GenericEvent &&
		         xevent.xcookie.extension == w->opcode &&
		         XGetEventData (w->xdisplay, &xevent.xcookie))
		{
			XIDeviceEvent const * device_event =
			    (XIDeviceEvent const *)xevent.xcookie.data;

			switch (xevent.xcookie.evtype)
			{
			case XI_TouchBegin:
			case XI_TouchEnd:
			case XI_TouchUpdate:
				input_type = input_type_touch;
				input_id = device_event->detail;
				break;

			case XI_ButtonPress:
			case XI_ButtonRelease:
			case XI_Motion:
				input_type = input_type_mouse;
				input_id = *device_event->buttons.mask;
				break;

			case XI_KeyPress:
				input_type = input_type_key;
				break;

			default:
				printf ("ignoring XI event %x\n", xevent.xcookie.evtype);
				continue;
			}

			switch (xevent.xcookie.evtype)
			{
			case XI_ButtonPress:
			case XI_ButtonRelease:
			case XI_Motion:
			case XI_TouchBegin:
			case XI_TouchEnd:
			case XI_TouchUpdate:
				input_pos[0] = w->pixel[0] *
				               (device_event->event_x -
				                w->xwindow_attributes.width / 2);
				input_pos[1] = -w->pixel[1] *
				               (device_event->event_y -
				                w->xwindow_attributes.height / 2);
				break;
			}

			switch (xevent.xcookie.evtype)
			{
			case XI_ButtonPress:
			case XI_KeyPress:
			case XI_TouchBegin:
				input_action = input_action_begin;
				break;

			case XI_Motion:
			case XI_TouchUpdate:
				input_action = input_action_update;
				break;

			case XI_ButtonRelease:
			case XI_TouchEnd:
				input_action = input_action_end;
				break;

			default:
				printf ("ignoring XI event %x\n",
				        xevent.xcookie.evtype);
				continue;
			}
		}
		else
		{
			printf ("ignoring XI event %x, fix me!\n",
			        xevent.xcookie.type == GenericEvent);
			continue;
		}

		XFreeEventData (w->xdisplay, &xevent.xcookie);

		// TODO: input_pos, input_action, input_type, input_id
		//error = input_system_event (input_system, &input);
		(void)input_pos;
		(void)input_action;
		(void)input_type;
		(void)input_id;
	}

	return error;
}

bool
window_stopping (
    struct window const * w)
{
	return w && w->stopping;
}
