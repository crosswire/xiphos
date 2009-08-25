//copyright xiphos; license gpl2

#ifndef signals_h
#define signals_h

#ifdef __cplusplus
extern "C" {
#endif

enum {
	LINK_MESSAGE,
	HTML_TITLE,
	HTML_OPEN_URI,
	HTML_DOM_KEY_DOWN,
	HTML_DOM_KEY_UP,
	HTML_DOM_MOUSE_DOWN,
	HTML_DOM_MOUSE_UP,
	HTML_DOM_MOUSE_DBL_CLICK,
	HTML_DOM_MOUSE_OVER,
	HTML_DOM_MOUSE_OUT,
	TITLE_CHANGED,
	POPUPMENU_REQUESTED,
	  HTML_DOM_KEY_PRESS,
	  HTML_DOM_MOUSE_CLICK,
	  HTML_DOM_ACTIVATE,
	  HTML_DOM_FOCUS_IN,
	  HTML_DOM_FOCUS_OUT,
	LAST_SIGNAL
};

extern guint signals[LAST_SIGNAL];

#ifdef __cplusplus
}
#endif

#endif
