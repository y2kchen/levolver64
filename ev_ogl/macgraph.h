/* macgraph.h */

/* stuff needed by macmain.c and macgraph.c in common */

#include <windows.h>

/*
 *  Data is attached to a graphics window using the "piggyback
 *  technique" described on pp. 36-38 of Macintosh C Programming
 *  Primer, volume 2.
 */

typedef struct {

    /* We embed a WindowRecord.                                 */

    WindowRecord    w;

    /* All drawing will be done to an offscreen GWorld          */
    /* and then copied to the window, to avoid flicker.         */

    GWorldPtr       offscreenGWorld;

    /* The data stored by your program should be independent    */
    /* of the size of the window.  To do the actual drawing,    */
    /* your program can use the following "scale" factor, which */
    /* is defined to be half the height or width of the window, */
    /* whichever is smaller.  Thus, for example, a circle of    */
    /* radius "scale" will just fit in the window.              */

    int             scale;

    /* Should the contents of the window be continuously        */
    /* redrawn during idle time?  (I.e. should something in the */
    /* window be moving?)                                       */

    int             continuous_redraw;
    int             need_redraw;
    int    magic;   /* so we don't do_drawing() on some other window */
    /*  Whatever other data is associated to a window can follow here. */

} graphicsWindowRecord, *graphicsWindowPeek;
#define MAGIC_VALUE 0x71423245
extern graphicsWindowPeek GraphicsWindowPtr;
extern void do_drawing(WindowPtr theWindow);
extern void do_display(void);
