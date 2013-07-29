#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

char **ealloc(int len) {
	char **result = calloc(len, sizeof(char *));
	if (!result)
		exit(1);
	return result;
}

void mumblecpy(char **dst, char **src, int len) {
	memcpy(dst, src, len * sizeof(char *));
}

int poppush(Display *dpy, Window win, char *old, char *new) {
	XTextProperty prop;
	char **returned_list, **new_list;
	int i, len;
	XGetWMName(dpy, win, &prop);
	XmbTextPropertyToTextList(dpy, &prop, &returned_list, &len);
	XFree(prop.value);
	if (old == NULL) {
		new_list = ealloc(len + 1);
		mumblecpy(new_list + 1, returned_list, len);
		len += 1;
		i = 0;
	} else {
		for (i = 0; i < len; i ++) {
			if (!strcmp(returned_list[i], old))
				break;
		}
		if (i == len) {
			XFreeStringList(returned_list);
			return 1;
		}
		if (i != 0) {
			new_list = ealloc(len);
			mumblecpy(new_list + 1, returned_list, i);
			mumblecpy(new_list + i + 1, returned_list + i + 1, len - i - 1);
		} else {
			new_list = returned_list;
		}
	}
	if (new != NULL) {
		char *old = new_list[0];
		new_list[0] = new;
		XmbTextListToTextProperty(dpy, new_list, len, XTextStyle, &prop);
		new_list[0] = old;
	} else {
		XmbTextListToTextProperty(dpy, new_list + 1, len - 1, XTextStyle, &prop);
	}
	XSetWMName(dpy, win, &prop);
	XFree(prop.value);
	if (new_list != returned_list)
		free(new_list);
	XFreeStringList(returned_list);
	return 0;
}



#define BUF_SIZE 256
int main(int argc, char **argv) {
	Display *dpy;
	Window win;
	char buf1[BUF_SIZE] = "", buf2[BUF_SIZE] = "";
	char *buf = buf1, *old_buf = NULL;
	size_t len;

	install_sighandlers();

	if (!(dpy = XOpenDisplay(NULL))) {
		fprintf(stderr, "failed to open display.");
		return 1;
	}
	win = DefaultRootWindow(dpy);

	if (argc > 1) {
		/* set WM_NAME to the first command-line argument, permanently */
		XTextProperty prop;
		XmbTextListToTextProperty(dpy, argv + 1, 1, XTextStyle, &prop);
		XSetWMName(dpy, win, &prop);
		XSync(dpy, 0);
	} else {
		while (!halt_now && fgets(buf, BUF_SIZE, stdin)) {
			buf[BUF_SIZE - 1] = 0;
			len = strlen(buf);
			if (buf[len-1] == '\n') 
				buf[len-1] = 0;
			poppush(dpy, win, old_buf, buf);
			XSync(dpy, 0);
			old_buf = buf;
			buf = (old_buf == buf1) ? buf2 : buf1;
		}
		poppush(dpy, win, old_buf, NULL);
		XSync(dpy, 0);
		
	}
	XCloseDisplay(dpy);
	return 0;
}
