#define _XOPEN_SOURCE 500

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include <GL/glx.h>
#include <X11/extensions/XInput2.h>

#include <ent.h>

typedef float length_xy[2]; // always in meters

struct xent_context
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

struct xent_context *
xent_context_alloc (
    struct ent_table * entities)
{
	struct xent_context * c = calloc (1, sizeof (struct xent_context));

	if (c)
	{
		c->entities = entities;
	}

	return c;
}

int
xent_context_setup (
    struct xent_context * c)
{
	GLint att[] =
	{
		GLX_RGBA,
		GLX_DEPTH_SIZE, 24,
		GLX_DOUBLEBUFFER,
		None
	};

	c->xdisplay = XOpenDisplay (NULL);
	if (c->xdisplay == NULL)
	{
		printf ("failed to open display\n");
		return -1;
	}

	int error, event;

	if (!XQueryExtension (
	            c->xdisplay,
	            "XInputExtension",
	            &c->opcode,
	            &event,
	            &error))
	{
		XCloseDisplay (c->xdisplay);
		c->xdisplay = NULL;
		printf ("failed to find XInput extension\n");
		return -1;
	}

	int major = 2, minor = 2;
	XIQueryVersion (c->xdisplay, &major, &minor);
	if (major * 1000 + minor < 2002)
	{
		XCloseDisplay (c->xdisplay);
		c->xdisplay = NULL;
		printf ("XInput v%d.%d\n", major, minor);
		return -1;
	}
	else
	{
		printf ("XInput v%d.%d\n", major, minor);
	}

	Window xroot = DefaultRootWindow (c->xdisplay);
	XVisualInfo * vi = glXChooseVisual (c->xdisplay, 0, att);
	if (vi == NULL)
	{
		XCloseDisplay (c->xdisplay);
		c->xdisplay = NULL;
		printf ("failed to get visual\n");
		return -1;
	}
	else
	{
		printf ("Xvisual %p selected\n", (void *)vi->visualid);
	}

	Colormap cmap = XCreateColormap (c->xdisplay, xroot, vi->visual, AllocNone);
	XSetWindowAttributes swa =
	{
		.colormap = cmap,
		.event_mask = ExposureMask | KeyPressMask,
	};
	c->xwindow =
	    XCreateWindow (
	        c->xdisplay, xroot,
	        0, 0, 600, 600,
	        0, vi->depth, InputOutput, vi->visual,
	        CWColormap | CWEventMask, &swa);
	if (!c->xwindow)
	{
		XCloseDisplay (c->xdisplay);
		c->xdisplay = NULL;
		printf ("failed to find XInput extension\n");
		return -1;
	}

	c->deleteWindow = XInternAtom (c->xdisplay, "WM_DELETE_WINDOW", False);
	XSetWMProtocols (c->xdisplay, c->xwindow, &c->deleteWindow, 1);
	XIDeviceInfo * info;
	int ndevices;
	info = XIQueryDevice (c->xdisplay, XIAllDevices, &ndevices);

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
	XISelectEvents (c->xdisplay, c->xwindow, &eventmask, 1);

	XMapWindow (c->xdisplay, c->xwindow);
	XStoreName (c->xdisplay, c->xwindow, "Simple App");

	c->glx = glXCreateContext (c->xdisplay, vi, NULL, GL_TRUE);
	glXMakeCurrent (c->xdisplay, c->xwindow, c->glx);

	return 0;
}

int
xent_context_teardown (struct xent_context * c)
{
	if (c->xdisplay)
	{
		if (c->glx)
		{
			glXMakeCurrent (c->xdisplay, None, NULL);
			glXDestroyContext (c->xdisplay, c->glx);
			c->glx = NULL;
		}

		if (c->xwindow)
		{
			XDestroyWindow (c->xdisplay, c->xwindow);
			c->xwindow = 0;
		}

		XCloseDisplay (c->xdisplay);
		c->xdisplay = NULL;
	}

	return 0;
}

void xent_context_free (struct xent_context * c)
{
	if (c)
	{
		free (c);
	}
}

int xent_context_paint (struct xent_context * c)
{
	XGetWindowAttributes (c->xdisplay, c->xwindow, &c->xwindow_attributes);

	glViewport (
	    0, 0,
	    c->xwindow_attributes.width, c->xwindow_attributes.height);

	length_xy screen_size =
	{
		WidthMMOfScreen (c->xwindow_attributes.screen) * 0.001,
		HeightMMOfScreen (c->xwindow_attributes.screen) * 0.001,
	};

	c->pixel[0] = screen_size[0] / WidthOfScreen (c->xwindow_attributes.screen);
	c->pixel[1] = screen_size[1] / HeightOfScreen (c->xwindow_attributes.screen);

	//length_xy visual_size =
	//{
	//0.5 * c->pixel[0] * c->xwindow_attributes.width,
	//0.5 * c->pixel[1] * c->xwindow_attributes.height
	//};
	//printf ("%0.3f x %0.3f\n", visual_size[0], visual_size[1]);

	// TODO: paint(visual_size);
	glClearColor (1.0, 1.0, 1.0, 1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glXSwapBuffers (c->xdisplay, c->xwindow);

	return 0;
}

struct ticker
{
	long frame;
	struct timeval time_started;
	unsigned long tick_offset;
};

struct ticker * ticker_alloc()
{
	struct ticker * t = calloc (1, sizeof (struct ticker));
	if (t)
	{
		gettimeofday (&t->time_started, NULL);
		t->tick_offset = t->time_started.tv_usec * 60 / 1000000;
	}
	return t;
}

void ticker_free (struct ticker * t)
{
	free (t);
}

int
ticker_sleep (struct ticker * t)
{
	t->frame += 1;

	struct timeval tick =
	{
		.tv_sec = t->time_started.tv_sec + (t->tick_offset + t->frame) / 60,
		.tv_usec = ((t->tick_offset + t->frame) % 60) * 1000000 / 60,
	};

	int go_to_sleep = 1;

	while (go_to_sleep)
	{
		struct timeval now;

		int error = gettimeofday (&now, NULL);
		if (error)
		{
			printf ("cannot get time of day: %x\n", errno);
			return -1;
		}

		printf ("time %ld.%06ld\n", now.tv_sec, now.tv_usec);

		long interval = (tick.tv_usec - now.tv_usec)
		                + 1000000 * (tick.tv_sec - now.tv_sec);
		if (interval < 0)
		{
			printf ("interval too small..?\n");
			break;
		}
		else if (interval > 1000000 / 60)
		{
			printf ("interval too large..?\n");
			interval = 1000000 / 60;
		}

		printf ("sleeping for 0.%06ld\n", interval);

		if (usleep ((useconds_t)interval))
		{
			if (errno == EINVAL)
			{
				go_to_sleep = 0;
				break;
			}
			else if (errno == EINTR)
			{
				break;
			}
		}
		else
		{
			go_to_sleep = 0;
		}
	}

	return 0;
}

enum input_type
{
	input_type_touch = 1,
	input_type_mouse,
	input_type_key,
};

typedef int input_id;

enum input_action
{
	input_action_begin = 1,
	input_action_update,
	input_action_end,
};

int xent_context_input (struct xent_context * c)
{
	int error = 0;

	while (!error && XPending (c->xdisplay))
	{
		XEvent xevent;

		XNextEvent (c->xdisplay, &xevent);

		enum input_type input_type = 0;
		input_id input_id = 0;
		length_xy input_pos = {0, 0};
		enum input_action input_action = 0;

		if (xevent.type == ClientMessage)
		{
			if ((Atom)xevent.xclient.data.l[0] == c->deleteWindow)
			{
				printf ("window closed\n");
				c->stopping = true;
			}
		}
		else if (xevent.xcookie.type == GenericEvent &&
		         xevent.xcookie.extension == c->opcode &&
		         XGetEventData (c->xdisplay, &xevent.xcookie))
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
				input_pos[0] = c->pixel[0] *
				               (device_event->event_x -
				                c->xwindow_attributes.width / 2);
				input_pos[1] = -c->pixel[1] *
				               (device_event->event_y -
				                c->xwindow_attributes.height / 2);
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

		XFreeEventData (c->xdisplay, &xevent.xcookie);

		// TODO: input_pos, input_action, input_type, input_id
		//error = input_system_event (input_system, &input);
	}

	return error;
}

int
main()
{
	struct ent_table * entities = ent_table_alloc();
	if (!entities)
	{
		return -1;
	}

	struct xent_context * c = xent_context_alloc (entities);
	if (!c)
	{
		return -1;
	}

	if (xent_context_setup (c) == -1)
	{
		return -1;
	}

	unsigned long frame = 0;
	int error = 0;
	int status = 0;

	while (!c->stopping)
	{
		error = xent_context_paint (c);
		if (error)
		{
			break;
		}

		frame += 1;
		if (! (frame % 60))
		{
			printf ("frame %ld\n", frame);
		}

		error = xent_context_input (c);
		if (error)
		{
			break;
		}

		// TODO: iterate state based on input etc.
	}

	assert (xent_context_teardown (c) != -1);
	xent_context_free (c);
	ent_table_free (entities);
	return (error || status < 0) ? -1 : 0;
}
