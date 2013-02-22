#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

sig_atomic_t halt_now = 0;

static void sighandler(int sig) {
	halt_now = 1;
}

static void install_sighandlers(void) {
	struct sigaction handler;
	handler.sa_handler = sighandler;
	sigemptyset(&handler.sa_mask);
	handler.sa_flags = SA_NODEFER;

	sigaction(SIGINT, &handler, NULL);
	sigaction(SIGTERM, &handler, NULL);
}

#define BUF_SIZE 256
int main(int argc, char **argv) {
	Display *dpy;
	Window win;

	install_sighandlers();

	if (!(dpy = XOpenDisplay(NULL))) {
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
		while (!halt_now && fgets(buf, BUF_SIZE, stdin)) {
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
