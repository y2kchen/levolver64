
/*
 *  Dear Ken:
 *
 *      Here's a shell for porting the Surface Evolver to the Macintosh.
 *  It gives you a console and also supports an arbitrary number of
 *  resizable graphics windows.  The usual Surface Evolver code will
 *  remain in control of things, using the functions write_to_console()
 *  and read_line_from_console() for its I/O.  It can also create graphics
 *  windows whenever it gets the urge.  The function read_line_from_console(),
 *  maintains the graphics windows (e.g. by accepting mouse clicks from
 *  the user and rotating the image in the window accordingly) and also
 *  takes care of menus, resizing windows, etc.
 *
 *      To install the real Surface Evolver into this shell, you will
 *  need to
 *
 *  (1) Replace run_evolver() with a call to the real Surface Evolver code.
 *
 *  (2) Modify the Surface Evolver to do all its console I/O via
 *      write_to_console() and read_line_from_console().
 *
 *  (3) Modify the graphicsWindowRecord to include variables relevant to
 *      displaying surfaces (e.g. lists of triangles, or whatnot).
 *      You will also need to replace the angular_position and angular_velocity
 *      fields with 3 x 3 orthogonal matrices which serve the same purpose
 *      for 3-d graphics.
 *
 *  (4) Modify draw_contents() to draw your lists of triangles in 3-space.
 *      This shouldn't be hard -- I can provide code for doing this sort
 *      of thing, but if you've already implemented mouse-able 3-d graphics
 *      on the NeXT you probably have your own code ready to go.
 *
 *  (5) Modify do_graphics_click() to handle 3-d rotations instead of 2-d
 *      rotations.  Again, I can provide code if you don't already have some.
 *
 *  (6) Create your own color icons in evolver.pi.rsrc.  You'll have to
 *      rebuild the desktop to get the Mac to recognize the new icons.
 *      To rebuild the desktop, restart the computer with the shift, control,
 *      option and command keys held down (actually some subset will do,
 *      but I don't remember which).
 *
 *  Hmmm . . . I think that's it.  There are various improvements that could
 *  be made to this code (e.g. allowing the user to paste input into the
 *  console), but I'll wait to see what your needs are rather than trying
 *  to implement all possible improvements ahead of time.
 *
 *  Along those lines, please let me know if you have any trouble with this
 *  shell.  I'm looking forward to seeing the Surface Evolver on the Mac
 *  next month.
 *
 *  By the way, parts of this code are far less elegant than they ought to
 *  be (the "correct" solution is probably to do it all in an object-oriented
 *  environment like the THINK Class Library), but it seems to work OK.
 *
 *  Jeff Weeks
 *  February 9, 1993 
 */
 


#include <Sound.h>
#include <Fonts.h>
#include <ToolUtils.h>
#include <Traps.h>
#include <Menus.h>
#include <Controls.h>
#include <Devices.h>
#include <CursorCtl.h>
#include <Resources.h>
#include <Scrap.h>

#include "macgraph.h"
#include <QDOffscreen.h>
#include <string.h>
#include <math.h>

#include "TBUtilities.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif




/*  I couldn't find these traps in the THINK C headers.  Maybe they're  */
/*  hiding in there somewhere.  In the meantime, here they are.         */
#define  QD32TRAP       0xAB03
#define  UNIMPL_TRAP    0xA89F


#define  MENU_BAR_HEIGHT            20
#define  SCROLL_BAR_WIDTH           15
#define  DRAG_INSET                 4
#define  DEFAULT_WINDOW_WIDTH       400
#define  DEFAULT_WINDOW_HEIGHT      250
#define  WINDOW_OFFSET              20
#define  MIN(A,B)                   ((A<B) ? A : B)

#define  MAX_COMMAND_LENGTH         200

enum {  appleMenuID = 128,
        fileMenuID,
        editMenuID};
enum {  quitItem = 1};
enum {  cutItem = 1,
        copyItem,
        pasteItem,
        clearItem,
        selectAllItem};

enum {  aboutAlertID = 128,
        errorAlertID};

/* main parts of program */
static void set_up(void);
extern int old_main(int argc,char **argv);
static void shut_down(void);

/* routines to implement Surface Evolver console */
void write_to_console(char *string);
void read_line_from_console(char *string);
static void create_square(RGBColor square_color);

/* initialization routines */
static void initialize_toolbox(void);
static void compute_screen_center(void);
static void check_system_environment(void);
static void check_pixel_depth(void);
static void set_drag_rect(void);
static void set_window_rects(void);
static void set_up_menus(void);
static void create_console(void);

/* event processing routines */
static void update_menus(void);
static Boolean do_menu_choice(long int menuChoice);
static void show_selection(void);
static void adjust_text_to_scroll_value(void);
static void set_scroll_max(void);
static void displace_gNextWindow(void);
static void do_grow(WindowPtr theWindow, Point pt);
static void do_zoom(WindowPtr theWindow, Point pt, int thePart);
static void adjust_view(void);
static void resize_offscreen_GWorld(WindowPtr theWindow);
static void do_update(WindowPtr theWindow);
static void do_activate(EventRecord *eventPtr);
static void activate_console(void);
static void deactivate_console(void);
static void center_dialog(ResType dialogType, int dialogID);
static void open_offscreen_GWorld(graphicsWindowPeek myWindow);
static void set_window_coords(WindowPtr theWindow);
static void do_cut(void);
static void do_copy(void);
static void do_paste(void);
static void do_clear(void);
static void do_select_all(void);
static void copy_graphics_to_clipboard(void);
static void do_idle(void);
static void error_message(Str255 message);
Boolean IsCancelEvent( EventRecord *theEvent );

/* shut down routines */
static void close_all_windows(void);
static void recursive_close_window(WindowPtr theWindow);
static void close_one_window(WindowPtr theWindow);
static void clear_menu_bar(void);

/* your routines for handling events */
static void initialize_window_data(graphicsWindowPeek myWindow);
static void do_console_click(EventRecord *eventPtr);

static pascal void VActionProc(ControlHandle qtheControl, short thePart);
static void do_graphics_click(EventRecord *eventPtr, WindowPtr theWindow);
static void draw_contents(graphicsWindowPeek myWindow);

/* other */
Boolean AbortInQueue( void);
void break_check(void);
void create_graphics_window(void);

/* global variables */

int             gPixelDepth;    /* pixel depth on main screen */
Point           gScreenCenter;
Rect            gDragRect,
                gWindowSizeLimits = {0x80, 0x80, 0x7FFF, 0x7FFF},
                gCenteredWindow,
                gNextWindow;
MenuHandle      gAppleMenu,
                gFileMenu,
                gEditMenu;
CursHandle      gIBeamCursor;
CursHandle      gWatchCursor;
WindowPtr       gConsoleWindow;
TEHandle        gConsoleEditRec;
ControlHandle   gConsoleScrollBar;
graphicsWindowPeek GraphicsWindowPtr;  /* for communication with macgraph.c */
extern QDGlobals qd;

main()
{ 
    
     MaxApplZone();  /* expand application heap to max */
     
    /*
     *  Set up the Macintosh toolbox and the console window.
     */
    set_up();
 
    /*
     *  Pass control to the standard Surface Evolver code.
     *  The Surface Evolver code will be the same as on any
     *  other platform, except that it should write to the
     *  console using write_to_console(), and read
     *  from the console using read_line_from_console().
     */
    old_main(0,NULL);

    /*
     *  Clean up before quitting.  (This isn't really necessary,
     *  but it seems polite.)
     */
    shut_down();
    return 0;
}

/* break checker, call periodically */

#include <signal.h>

/******************************************************************************
 AbortInQueue           TCL 1.1.1 DLP 9/18/91
 
    Walk the event queue, return TRUE if a Command-'.' is there. The
    event is removed from the queue.
    
 ******************************************************************************/


#include "TBUtilities.h"

 
Boolean AbortInQueue( void)
{
    EvQElPtr    qEvt = (EvQElPtr) GetEvQHdr()->qHead;
    Boolean     foundAbort = FALSE;

    while (qEvt)
    {
            /* Starting at the evtQWhat field, and EvQEl is identical
             to an EventRecord */
            
        if ( IsCancelEvent( (EventRecord*) &qEvt->evtQWhat))
        {
            /* Flush this and all other key-downs from the queue */
            FlushEvents( keyDownMask, 0);
            foundAbort = TRUE;
            break;
        }
        qEvt = (EvQElPtr) qEvt->qLink;
    }
    return foundAbort;
}

/******************************************************************************
 IsCancelEvent
 
    Returns TRUE if event is a command-period keydown event. Uses 
    the technique in TN #263 to handle international keyboards.
    
 ******************************************************************************/


#define kMaskModifiers  0xFE00     /* we need the modifiers without the
                                    command key for KeyTrans */
                        
#define kUpKeyMask      0x0080
#define kMaskASCII1     0x00FF0000 /* get the key out of the ASCII1 byte */
#define kMaskASCII2     0x000000FF /* get the key out of the ASCII2 byte */

Boolean IsCancelEvent( EventRecord *theEvent )
{
    Boolean  isCancel = FALSE;
    long     keyInfo, lowChar, highChar;
    Handle   hKCHR = NULL;

    if ((theEvent->what == keyDown) || (theEvent->what == autoKey))
    {
  
        /* see if the command key is down.  If it is, find out the ASCII
         equivalent for the accompanying key. */

        if (theEvent->modifiers & cmdKey) 
        {
                keyInfo = theEvent->message;
        
            lowChar = keyInfo &  kMaskASCII2;
            highChar = (keyInfo & kMaskASCII1) >> 16;
            if (lowChar == '.' || highChar == '.')
                isCancel = true;
        }
    }
    return isCancel;
}
extern int breakflag;
void break_check()
{ if ( AbortInQueue() )  /* raise(SIGINT);*/
   /* catcher(SIGINT);  this completely messes; tries console window */
    { write_to_console("Will break.\n"); breakflag = 1; }
}

void write_to_console(
    char    *string)
{
    while (*string) {
        /*
         *  Treat a newline \n as a return \r.
         */
        if (*string == '\n')
            TEKey('\r', gConsoleEditRec);
        else
            TEKey(*string, gConsoleEditRec);
        string++;
    }
    show_selection();

    return;
}


void read_line_from_console(
    char    *string)
{
    EventRecord theEvent;
    WindowPtr   theWindow;
    Boolean     full_line_read;
    char        ch;
    int         pos = 0;    /* the position in the string of the next char to be read */

    full_line_read = FALSE;

    while ( ! full_line_read) {

        WaitNextEvent(everyEvent, &theEvent, 0L, NULL);

        switch (theEvent.what) {

        case nullEvent:
            do_idle();
            break;

        case mouseDown:
            switch (FindWindow(theEvent.where, &theWindow)) {
            case inDesk:
                break;
            case inMenuBar:
                update_menus();
                if (do_menu_choice(MenuSelect(theEvent.where))) {
                    strcpy(string, "quit");
                    full_line_read = TRUE;
                }
                break;
            case inSysWindow:
                SystemClick(&theEvent, theWindow);
                break;
            case inContent:
                if (theWindow != FrontWindow())
                    SelectWindow(theWindow);
                else
                    if (theWindow == gConsoleWindow)
                        do_console_click(&theEvent);
                    else
                        do_graphics_click(&theEvent, theWindow);
                break;
            case inDrag:
                DragWindow(theWindow, theEvent.where, &gDragRect);
                break;
            case inGrow:
                do_grow(theWindow, theEvent.where);
                break;
            case inGoAway:
                if (TrackGoAway(theWindow, theEvent.where))
                    close_one_window(theWindow);
                break;
            case inZoomIn:
                do_zoom(theWindow, theEvent.where, inZoomIn);
                break;
            case inZoomOut:
                do_zoom(theWindow, theEvent.where, inZoomOut);
                break;
            }
            break;

        case keyDown:
        case autoKey:
            ch = theEvent.message & charCodeMask;
            if (theEvent.modifiers & cmdKey) {
                update_menus();
                if (do_menu_choice(MenuKey(ch))) {
                    strcpy(string, "quit");
                    full_line_read = TRUE;
                }
            }
            else {
                if ( (ch < 0x1C) || (ch > 0x1F) ) /* don't show arrows */
                  TEKey(ch, gConsoleEditRec);
                show_selection();
                switch (ch) {
                    case '\n':
                    case '\r':
                        string[pos] = 0;
                        full_line_read = TRUE;
                        break;
                    case '\b':
                        if (pos > 0)
                            --pos;
                        break;
                    case 28 : string[pos++] = 29; break;
                    case 29 : string[pos++] = 28; break;  /* kludge */
                    default:
                        if (pos == MAX_COMMAND_LENGTH)
                            SysBeep(10L);
                        else
                            string[pos++] = ch;
                        break;
                }
            }
            break;

        case updateEvt:
            do_update((WindowPtr) theEvent.message);
            break;

        case activateEvt:
            do_activate(&theEvent);
            break;

        case osEvt:
            if ((theEvent.message >> 24) == suspendResumeMessage) {
                if (theEvent.message & resumeFlag) {
                    if (gConsoleWindow == FrontWindow())
                        activate_console();
                }
                else {
                    if (gConsoleWindow == FrontWindow())
                        deactivate_console();
                }
            }
            break;

        default:
            /* ignore all other events */
            break;
        }
    }

    SetCursor(*gWatchCursor);  /* busy signal for user */
    return;
}


void create_graphics_window(void)
{
    graphicsWindowPeek  newWindow;

    newWindow = (graphicsWindowPeek) NewPtr((long) sizeof(graphicsWindowRecord));
    newWindow->magic = MAGIC_VALUE;
    NewCWindow( (Ptr) newWindow,
                &gNextWindow,
                "\pSurface Evolver",
                1,
                zoomDocProc,
                (WindowPtr) (-1),
                1,
                0L);
    displace_gNextWindow();
    initialize_window_data(newWindow);
    set_window_coords((WindowPtr) newWindow);
    open_offscreen_GWorld(newWindow);

    DrawMenuBar();

    GraphicsWindowPtr = newWindow;   /* for communication with macgraph.c */
    return;
}


static void set_up()
{
 
    initialize_toolbox(); 
    compute_screen_center(); 
    check_system_environment();
    check_pixel_depth(); 
    set_drag_rect();
    set_window_rects(); 
    set_up_menus(); 
    gIBeamCursor = GetCursor(iBeamCursor);
    gWatchCursor = GetCursor(watchCursor);
 

    create_console(); 
    create_graphics_window();

    return;
}


static void shut_down()
{
    close_all_windows();
    clear_menu_bar();

    return;
}


static void initialize_toolbox()
{
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(0);
    InitCursor();
    FlushEvents(everyEvent, 0);

    return;
}


static void compute_screen_center()
{
    gScreenCenter.h = (qd.screenBits.bounds.left + qd.screenBits.bounds.right )/2;
    gScreenCenter.v = (qd.screenBits.bounds.top  + qd.screenBits.bounds.bottom + MENU_BAR_HEIGHT)/2;

    return;
}


static void check_system_environment()
{
    SysEnvRec   environment;

    SysEnvirons(2, &environment);
    
    if ( ! environment.hasColorQD
     || NGetTrapAddress(QD32TRAP, ToolTrap)
        == NGetTrapAddress(UNIMPL_TRAP, ToolTrap)) {
        error_message("\pThis program requires 32-bit QuickDraw.");
        ExitToShell();
    }


    return;
}


static void check_pixel_depth()
{
    GDHandle        mainDevice;
    PixMapHandle    mainDevicePixMap;

    mainDevice          = GetMainDevice();
    mainDevicePixMap    = (**mainDevice).gdPMap;
    gPixelDepth         = (**mainDevicePixMap).pixelSize;

    return;
}


static void set_drag_rect()
{
    gDragRect = qd.screenBits.bounds;
    gDragRect.top += MENU_BAR_HEIGHT;
    InsetRect(&gDragRect, DRAG_INSET, DRAG_INSET);

    return;
}


static void set_window_rects()
{
    gCenteredWindow.left    = gScreenCenter.h - DEFAULT_WINDOW_WIDTH/2;
    gCenteredWindow.right   = gScreenCenter.h + DEFAULT_WINDOW_WIDTH/2;
    gCenteredWindow.top     = 40 /*gScreenCenter.v - DEFAULT_WINDOW_HEIGHT/2 */;
    gCenteredWindow.bottom  = gScreenCenter.v + DEFAULT_WINDOW_HEIGHT/6;

    gNextWindow = gCenteredWindow;

    return;
}


static void set_up_menus()
{
    gAppleMenu  = GetMenu(appleMenuID);
    gFileMenu   = GetMenu(fileMenuID);
    gEditMenu   = GetMenu(editMenuID);

    InsertMenu(gAppleMenu, 0);
    InsertMenu(gFileMenu,  0);
    InsertMenu(gEditMenu,  0);

#ifdef powerc
    AddResMenu(gAppleMenu, 'DRVR');
#endif
    DrawMenuBar();

    return;
}


static void create_console()
{
    Rect    r,
            scroll_rect;

    r = qd.screenBits.bounds;

    r.left      += 20;
    r.top       += r.bottom*2/3;
    r.right     -= 20;
    r.bottom    -= 20;

    gConsoleWindow = NewCWindow(
        NULL,
        &r,
        "\pSurface Evolver",
        TRUE,
        zoomDocProc,
        (WindowPtr) (-1),
        FALSE,
        0L);
    SetPort(gConsoleWindow);

    OffsetRect(&r, -r.left, -r.top);
    InsetRect(&r, 4, 4);
    r.bottom -= SCROLL_BAR_WIDTH;
    r.right  -= SCROLL_BAR_WIDTH;

    gConsoleEditRec = TENew(&r, &r);

    scroll_rect = gConsoleWindow->portRect;
    scroll_rect.left    = scroll_rect.right-15;
    scroll_rect.right   += 1;
    scroll_rect.bottom  -= 14;
    scroll_rect.top     -= 1;

    gConsoleScrollBar = NewControl(
                            gConsoleWindow,
                            &scroll_rect,
                            "\p",
                            TRUE,
                            0,
                            0,
                            0,
                            scrollBarProc,
                            0L);

    return;
}


static void update_menus()
{
    if (FrontWindow() == gConsoleWindow) {
        if ((*gConsoleEditRec)->selStart == (*gConsoleEditRec)->selEnd) {
            DisableItem(gEditMenu, cutItem);
            DisableItem(gEditMenu, copyItem);
            DisableItem(gEditMenu, clearItem);
        }
        else {
            EnableItem(gEditMenu, cutItem);
            EnableItem(gEditMenu, copyItem);
            EnableItem(gEditMenu, clearItem);
        }
        DisableItem(gEditMenu, pasteItem);
        EnableItem(gEditMenu, selectAllItem);
    }
    else {
        DisableItem(gEditMenu, cutItem);
        EnableItem(gEditMenu, copyItem);
        DisableItem(gEditMenu, pasteItem);
        DisableItem(gEditMenu, clearItem);
        DisableItem(gEditMenu, selectAllItem);
    }

    return;
}


static Boolean do_menu_choice(
    long int menuChoice)
{
    Str255      DAname;

    switch (HiWord(menuChoice)) {

    case appleMenuID:
        if (LoWord(menuChoice) == 1)
            Alert(aboutAlertID, NULL);
        else {
            GetItem(gAppleMenu, LoWord(menuChoice), DAname);
            OpenDeskAcc(DAname);
        }
        break;

    case fileMenuID:
        switch (LoWord(menuChoice)) {
        case quitItem:
            return(TRUE);
        }
        break;

    case editMenuID:
        switch (LoWord(menuChoice)) {
        case cutItem:
            do_cut();
            break;
        case copyItem:
            do_copy();
            break;
        case pasteItem:
            do_paste();
            break;
        case clearItem:
            do_clear();
            break;
        case selectAllItem:
            do_select_all();
            break;
        }
        break;
    }
    HiliteMenu(0);
    return(FALSE);
}


static void show_selection()
{
    TERec   *te;
    int     visible_lines,
            top_line,
            bottom_line,
            this_line;


    te = *gConsoleEditRec;
   
    /* Guard against overflow of text buffer, which has max size 32K */
    if ( te->teLength > 32000 )
    { int linenum = 0;
      while ( te->lineStarts[linenum] < 1000 )  linenum++;
      TESetSelect(0,te->lineStarts[linenum],gConsoleEditRec);
      TEDelete(gConsoleEditRec);    /* delete lines at start */
      TESetSelect(32766,32766,gConsoleEditRec);  /* cursor back to end */
    }
    
    set_scroll_max();
    adjust_text_to_scroll_value();

    visible_lines   = (te->viewRect.bottom - te->viewRect.top)/te->lineHeight;

    top_line    = GetCtlValue(gConsoleScrollBar);
    bottom_line = top_line + visible_lines;

    if (te->selStart < te->lineStarts[top_line] ||
            te->selStart >= te->lineStarts[bottom_line]) {
        for (this_line = 0; te->selStart >= te->lineStarts[this_line]; this_line++)
            ;
        SetCtlValue(gConsoleScrollBar, this_line - visible_lines/2);
        adjust_text_to_scroll_value();
    }

    return;
}


static void adjust_text_to_scroll_value()
{
    TERec   *te;
    int     oldScroll,
            newScroll,
            delta;

    te = *gConsoleEditRec;

    oldScroll = te->viewRect.top - te->destRect.top;
    newScroll = GetCtlValue(gConsoleScrollBar) * te->lineHeight;
    delta = oldScroll - newScroll;
    if (delta != 0)
      TEScroll(0, delta, gConsoleEditRec);

    return;
}


static void set_scroll_max()
{
    int     lines,
            max;
    TERec   *te;

    te = *gConsoleEditRec;
    lines = te->nLines;
    if (*(*te->hText + te->teLength - 1) == '\r')
        lines++;
    max = lines - (te->viewRect.bottom - te->viewRect.top) / te->lineHeight;
    if (max < 0)
        max = 0;

    SetCtlMax(gConsoleScrollBar, max);

    return; 
}


static void displace_gNextWindow()
{
    /* offset gNextWindow in preparation for the next window */
    OffsetRect(&gNextWindow, -WINDOW_OFFSET, WINDOW_OFFSET);
    if (gNextWindow.left < 0 || gNextWindow.bottom > qd.screenBits.bounds.bottom)
        gNextWindow = gCenteredWindow;

    return;
}


static void do_grow(
    WindowPtr   theWindow,
    Point       pt)
{
    long int    newSize;

    newSize = GrowWindow(theWindow, pt, &gWindowSizeLimits);
    if (newSize) {
        EraseRect(&theWindow->portRect);
        SizeWindow(theWindow, LoWord(newSize), HiWord(newSize), 1);
        if (theWindow == gConsoleWindow)
            adjust_view();
        else
            resize_offscreen_GWorld(theWindow);
        InvalRect(&theWindow->portRect);
    }

    return;
}


static void do_zoom(
    WindowPtr   theWindow,
    Point       pt,
    int         thePart)
{
    if (TrackBox(theWindow, pt, thePart)) {
        EraseRect(&theWindow->portRect);
        ZoomWindow(theWindow, thePart, 0);
        if (theWindow == gConsoleWindow)
            adjust_view();
        else
            resize_offscreen_GWorld(theWindow);
        InvalRect(&theWindow->portRect);
    }

    return;
}


static void adjust_view()
{
    Rect    r;
    int     num_lines;

    r = gConsoleWindow->portRect;
    InsetRect(&r, 4, 4);
    r.bottom -= SCROLL_BAR_WIDTH;
    r.right  -= SCROLL_BAR_WIDTH;
    num_lines = (r.bottom - r.top) / (**gConsoleEditRec).lineHeight;
    r.bottom = r.top + num_lines * (**gConsoleEditRec).lineHeight;

    (**gConsoleEditRec).viewRect = r;

    (**gConsoleEditRec).destRect.right = (**gConsoleEditRec).viewRect.right;

    TECalText(gConsoleEditRec);

    MoveControl(gConsoleScrollBar,
                gConsoleWindow->portRect.right - SCROLL_BAR_WIDTH,
                gConsoleWindow->portRect.top - 1);
    SizeControl(gConsoleScrollBar,
                SCROLL_BAR_WIDTH + 1,
                gConsoleWindow->portRect.bottom - gConsoleWindow->portRect.top - (SCROLL_BAR_WIDTH - 2));
    set_scroll_max();
    adjust_text_to_scroll_value();

    return;
}


static void resize_offscreen_GWorld(
    WindowPtr   theWindow)
{
    set_window_coords(theWindow);
    DisposeGWorld(((graphicsWindowPeek)theWindow)->offscreenGWorld);
    open_offscreen_GWorld((graphicsWindowPeek) theWindow);
    InvalRect(&theWindow->portRect);

    return;
}


static void do_update(
    WindowPtr   theWindow)
{
    GrafPtr savePort;

    GetPort(&savePort);
    SetPort(theWindow);
    BeginUpdate(theWindow);
        DrawGrowIcon(theWindow);
        if (theWindow == gConsoleWindow) {
            TEUpdate(&theWindow->portRect, gConsoleEditRec);
            DrawControls(theWindow);
        }
        else {
            do_drawing(theWindow);
        }
    EndUpdate(theWindow);
    SetPort(savePort);

    return;
}


static void do_activate(
    EventRecord *eventPtr)
{
    WindowPtr   theWindow;

    theWindow = (WindowPtr) eventPtr->message;
    SetPort(theWindow);
    DrawGrowIcon(theWindow);
    if (theWindow == gConsoleWindow) {
        if (eventPtr->modifiers & activeFlag)
            activate_console();
        else
            deactivate_console();
    }

    return;
}


static void activate_console()
{
    gConsoleEditRec[0]->selStart =
    gConsoleEditRec[0]->selEnd = gConsoleEditRec[0]->teLength;
    TEActivate(gConsoleEditRec);
    HiliteControl(gConsoleScrollBar, 0);
/*  TEFromScrap();          */

    return;
}


static void deactivate_console()
{
    TEDeactivate(gConsoleEditRec);
    HiliteControl(gConsoleScrollBar, 255);
/*  ZeroScrap();            */
/*  TEToScrap();            */

    return;
}


static void center_dialog(
    ResType dialogType, /* 'DLOG' or 'ALRT' */
    int     dialogID)
{

    Rect    *boundsRectPtr;

    boundsRectPtr = (Rect *) *GetResource(dialogType, dialogID);
    OffsetRect( boundsRectPtr,
                gScreenCenter.h - (boundsRectPtr->left + boundsRectPtr->right )/2,
                gScreenCenter.v - (boundsRectPtr->top  + boundsRectPtr->bottom)/2);

    return;
}


static void open_offscreen_GWorld(
    graphicsWindowPeek  myWindow)
{
    QDErr   errorCode;
    Rect    *thePortRect;

    thePortRect = &((WindowPtr)myWindow)->portRect;

    /* open the offscreen GWorld... */
    errorCode = NewGWorld(
                &myWindow->offscreenGWorld,
                gPixelDepth,
                thePortRect,
                NULL,
                NULL,
                0L);
    
    if (errorCode) {
        error_message("\pCouldn't open offscreen GWorld.");
        ExitToShell();
    }

    /* for use in scaling drawings, note the size of the window */
    myWindow->scale = MIN(-thePortRect->left, -thePortRect->top);

    return;
}


static void set_window_coords(
    WindowPtr   theWindow)
{
    GrafPtr savePort;

    GetPort(&savePort);
    SetPort(theWindow);

    /* Set the local coordinates of the window so the origin is at the center. */
    SetOrigin(
        (theWindow->portRect.left - theWindow->portRect.right  + SCROLL_BAR_WIDTH)/2,
        (theWindow->portRect.top  - theWindow->portRect.bottom + SCROLL_BAR_WIDTH)/2);

    SetPort(savePort);

    return;
}


void do_drawing(
    WindowPtr   theWindow)
{
    GDHandle        saveGD;
    GWorldPtr       saveGW,
                    offscreenGW;
    graphicsWindowPeek  myWindow;
    Rect            copyRect;

    /* get set up */
    myWindow = (graphicsWindowPeek) theWindow;
    if ( myWindow->magic != MAGIC_VALUE ) return;
    offscreenGW = myWindow->offscreenGWorld;
    LockPixels(offscreenGW->portPixMap);
    GetGWorld(&saveGW, &saveGD);    /* save the current GWorld, so  */
                                    /* we can restore it at the end */
    SetGWorld(offscreenGW, NULL);

    /* draw into the offscreen GWorld */
    EraseRect(&theWindow->portRect);
    draw_contents(myWindow);

    /* restore the current GWorld */
    SetGWorld(saveGW, saveGD);

    /* copy the results back to the real window */
    copyRect = offscreenGW->portRect; /* == theWindow->portRect */
    copyRect.bottom -= SCROLL_BAR_WIDTH;
    copyRect.right  -= SCROLL_BAR_WIDTH;
    CopyBits(   (BitMap*)(offscreenGW->portPixMap[0]),
                &theWindow->portBits,
                &copyRect,
                &copyRect,
                srcCopy,
                theWindow->visRgn);

    UnlockPixels(offscreenGW->portPixMap);

    return;
}


static void do_cut()
{
    if (gConsoleWindow == FrontWindow())
        if (ZeroScrap() == noErr) {
            TECut(gConsoleEditRec);
            show_selection();
            if (TEToScrap() != noErr)
                ZeroScrap();
        }

    return;
}


static void do_copy()
{
    if (gConsoleWindow == FrontWindow()) {
        if (ZeroScrap() == noErr) {
            TECopy(gConsoleEditRec);
            if (TEToScrap() != noErr)
                ZeroScrap();
        }
    }
    else
        copy_graphics_to_clipboard();

    return;
}


static void do_paste()
{
    if (gConsoleWindow == FrontWindow())
        if (TEFromScrap() == noErr) {
            TEPaste(gConsoleEditRec);
            show_selection();
        }

    return;
}


static void do_clear()
{
    if (gConsoleWindow == FrontWindow()) {
        TEDelete(gConsoleEditRec);
        show_selection();
    }

    return;
}


static void do_select_all()
{
    if (gConsoleWindow == FrontWindow())
        TESetSelect(0, 32767, gConsoleEditRec);

    return;
}


/*
 *  The copy_graphics_to_clipboard() function copies the contents of the
 *  current window to the clipboard in PICT format.  You may then switch to
 *  another program such as MacDraw or SuperPaint to edit and or print
 *  the image.  This saves our program from having to fuss with printing.
 */

static void copy_graphics_to_clipboard()
{
    WindowPtr   theWindow;
    Rect        picFrame;
    PicHandle   thePic;

    theWindow = FrontWindow();

    InvertRect(&theWindow->portRect);

    picFrame = theWindow->portRect;
    thePic = OpenPicture(&picFrame);
    draw_contents((graphicsWindowPeek) theWindow);
    ClosePicture();

    HLock((Handle)thePic);
    if (ZeroScrap() == noErr)
        PutScrap(GetHandleSize((Handle)thePic), 'PICT', (char*)(*thePic));

    KillPicture(thePic);

    SystemEdit(3);  /* tell multifinder to update its clipboard */

    InvertRect(&theWindow->portRect);

    return;
}


static void do_idle()
{
    WindowPtr           theWindow;
    graphicsWindowPeek  myWindow;
    Point               pt;

    if (gConsoleWindow == FrontWindow())
        TEIdle(gConsoleEditRec);

    GetMouse(&pt);
    if (gConsoleWindow == FrontWindow() && PtInRect(pt, &(*gConsoleEditRec)->viewRect))
        SetCursor(*gIBeamCursor);
    else
        InitCursor();

    for (   theWindow = FrontWindow();
            theWindow != NULL;
            theWindow = (WindowPtr) ((WindowPeek)theWindow)->nextWindow
        )

        if (theWindow != gConsoleWindow) {
            myWindow = (graphicsWindowPeek) theWindow;
            if (myWindow->continuous_redraw) {
                do_drawing(theWindow);
            }
            if (myWindow->need_redraw) {
                do_drawing(theWindow);
                myWindow->need_redraw = 0;
            }
        }

    return;
}


static void error_message(Str255 message)
{
    ParamText(message, "\p", "\p", "\p");
    Alert(errorAlertID, NULL);

    return;
}


static void close_all_windows()
{
    TEDispose(gConsoleEditRec);
    DisposeControl(gConsoleScrollBar);
    DisposeWindow(gConsoleWindow);

    recursive_close_window(FrontWindow());

    return;
}


static void recursive_close_window(
    WindowPtr   theWindow)
{
    if (theWindow != NULL) {
        /* first close the windows behind theWindow */
        recursive_close_window((WindowPtr)(((WindowPeek)theWindow)->nextWindow));

        /* now close theWindow */
        close_one_window(theWindow);
    }

    return;
}


static void close_one_window(
    WindowPtr   theWindow)
{
    DisposeGWorld(((graphicsWindowPeek)theWindow)->offscreenGWorld);
    CloseWindow(theWindow);
    DisposPtr((char*)theWindow);

    return;
}


static void clear_menu_bar()
{
    ClearMenuBar();
    DrawMenuBar();

    ReleaseResource((Handle)gAppleMenu);
    ReleaseResource((Handle)gFileMenu);
    ReleaseResource((Handle)gEditMenu);

    return;
}


static void initialize_window_data(
    graphicsWindowPeek  myWindow)
{
    myWindow->continuous_redraw = FALSE;
    return;
}

RoutineDescriptor stupid_mac_rd = BUILD_ROUTINE_DESCRIPTOR(kRegisterBased,VActionProc);
/* first arg is procInfo,   supposed to be  calling convention*/

static void do_console_click(
    EventRecord *eventPtr)
{
    Point   pt;
    Boolean shiftDown;
    int     thePart;
    int     old_value,
            delta;

    pt = eventPtr->where;
    GlobalToLocal(&pt);

    thePart = FindControl(pt, gConsoleWindow, NULL);

    if (thePart == 0) {     /* not in the scroll bar */

        shiftDown = (eventPtr->modifiers & shiftKey) != 0;
        gConsoleEditRec[0]->selStart =
        gConsoleEditRec[0]->selEnd = gConsoleEditRec[0]->teLength;

        TEClick(pt, shiftDown, gConsoleEditRec);

    }
    else {
        switch (thePart) {
            case inUpButton: 
            case inDownButton: 
            case inPageUp: 
            case inPageDown:
                VActionProc(NULL,thePart);    
                /* (void) TrackControl(gConsoleScrollBar, pt, &stupid_mac_rd ); */
                break;

            case inThumb:
                old_value = GetCtlValue(gConsoleScrollBar);
                if (TrackControl(gConsoleScrollBar, pt, NULL) != 0) {
                    delta = old_value - GetCtlValue(gConsoleScrollBar);
                    if (delta != 0)
                        TEScroll(0, delta * (*gConsoleEditRec)->lineHeight, gConsoleEditRec);
                }
                break;
        }
    }

    return;
}


static pascal void VActionProc(
    ControlHandle   theControl,
    short               thePart)
{
    TERec   *te;
    int     delta;

    if (thePart != 0) {
        switch (thePart) {
            case inUpButton:
                delta = -1;
                break;
            case inDownButton:
                delta = +1;
                break;
            case inPageUp:
                te = *gConsoleEditRec;
                delta = - (te->viewRect.bottom - te->viewRect.top) / te->lineHeight;
                break;
            case inPageDown:
                te = *gConsoleEditRec;
                delta = (te->viewRect.bottom - te->viewRect.top) / te->lineHeight;
                break;
        }

        SetCtlValue(gConsoleScrollBar, GetCtlValue(gConsoleScrollBar) + delta);
        adjust_text_to_scroll_value();
    }
    return;
}

extern void fix_ctm(double** view,double dx,double dy);
extern double **view;

static void do_graphics_click(
    EventRecord *eventPtr,
    WindowPtr   theWindow)
{
    Point               p0,
                        p1;
    graphicsWindowPeek  myWindow;

    myWindow = (graphicsWindowPeek) theWindow;

    p0 = eventPtr->where;
    GlobalToLocal(&p0);

    while (StillDown())  ;  /* wait until upclick */
    
    GetMouse(&p1);
    fix_ctm(view,p1.h-p0.h,p0.v-p1.v);  /* note vertical is upside down */
    do_drawing(theWindow);

    return;
}


static void draw_contents(
    graphicsWindowPeek  myWindow)
{
    SetCursor(*gWatchCursor);  /* busy signal for user */
    do_display();  /* in macgraph.c */
    return;
}
