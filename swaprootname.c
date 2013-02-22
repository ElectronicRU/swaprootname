#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BUF_SIZE 256
int main(int argc, char **argv) {
	Display *dpy = XOpenDisplay(NULL);
	Window win;

	if (!dpy) {
		fprintf(stderr, "failed to open display.");
		return 1;
	}
	win = DefaultRootWindow(dpy);

	if (argc > 1) {
		XTextProperty prop;
		/* what are we, fucking echo? we will NOT glue them */
		Xutf8TextListToTextProperty(dpy, argv + 1, 1, XUTF8StringStyle, &prop);
		XSetWMName(dpy, win, &prop);
		XSync(dpy, 0);
	} else {
		XTextProperty prop, orig_prop;
		char buf[BUF_SIZE], *ptr=buf;
		size_t len;
		XGetWMName(dpy, win, &orig_prop);
		while (fgets(buf, BUF_SIZE, stdin)) {
			len = strlen(buf);
			if (buf[len-1] == '\n') buf[len-1] = 0;
			XmbTextListToTextProperty(dpy, &ptr, 1, XTextStyle, &prop);
			XSetWMName(dpy, win, &prop);
			XSync(dpy, 0);
		}
		XSetWMName(dpy, win, &orig_prop);
		XSync(dpy, 0);
		
	}
	XCloseDisplay(dpy);
	return 0;
}
