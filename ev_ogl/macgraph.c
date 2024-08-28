/*************************************************************
*  This file is for the Surface Evolver source code.         *
*  Macintosh graphics routines.                              *
*  Uses routines from Jeff Weeks' sample shell.              *
*************************************************************/
/* All coordinates in absolute pixels in window */


#include "include.h"
#include "macgraph.h"


static int maxx,maxy;  /* max viewport coordinates */
static double xscale,yscale;  /* for scaling to screen size */

static RGBColor edgecolor;
void macgraph_facet ARGS(( struct tsort *));
void init_macgraph ARGS((void));
void macgraph_edge ARGS(( struct tsort *));
void finish_macgraph ARGS((void));
void close_macgraph ARGS((void));

static void get_a_color(int c,double gray, RGBColor *color)
{
  color->red = (unsigned short)(0x0FFFF*(gray+.3)/1.3*rgb_colors[c][0]);
  color->green = (unsigned short)(0x0FFFF*(gray+.3)/1.3*rgb_colors[c][1]);
  color->blue = (unsigned short)(0x0FFFF*(gray+.3)/1.3*rgb_colors[c][2]);
}

void init_macgraph()
{ RGBColor background;
  PolyHandle theTriangle;

  if ( GraphicsWindowPtr == NULL )
    kb_error(1983,"Mac graphics window not initialized.\n",UNRECOVERABLE);
  xscale = GraphicsWindowPtr->scale/1.5;
  yscale = GraphicsWindowPtr->scale/1.5;
  init_flag = 1;
  get_a_color(fillcolor,1.0,&edgecolor);
  
  /* clear screen, in case not called thru window event */
  theTriangle = OpenPoly();
  MoveTo(-1000,-1000);
  LineTo(-1000,1000);
  LineTo(1000,1000);
  LineTo(1000,-1000);
  ClosePoly();
  get_a_color(WHITE,1.0,&background);
  RGBForeColor(&background);
  PaintPoly(theTriangle);
  KillPoly(theTriangle);
 }

void macgraph_edge(t)
struct tsort *t;
{
  int p[2][2];
  int i;
  RGBColor color;

  if ( t->ecolor[0] == CLEAR ) return;
  

  for ( i = 0 ; i < 2 ; i++ )
    {
      p[i][0] = (int)(t->x[i][0]*xscale);
      p[i][1] = -(int)(t->x[i][1]*yscale);
    }       
  PenSize(1,1);
  get_a_color(t->color,1.0,&color);
  RGBForeColor(&color);
  MoveTo(p[0][0],p[0][1]);
  LineTo(p[1][0],p[1][1]);
  return;
 }

void macgraph_facet(t)
struct tsort *t;
{
  int n = FACET_VERTS;      /* vertices in polygon */
  int p[6][2];              /* vertex coords       */
  int i;
  RGBColor  color;
  PolyHandle theTriangle;

  PenSize(1,1);
  for ( i = 0 ; i < FACET_VERTS ; i++ )
       {
         p[i][0] = (int)(t->x[i][0]*xscale);
         p[i][1] = -(int)(t->x[i][1]*yscale);
       }
  p[FACET_VERTS][0] = p[0][0];  /* convenient */
  p[FACET_VERTS][1] = p[0][1];  /* convenient */
 
  if ( shading_flag && color_flag )
          get_a_color(t->color,gray_level(t->normal),&color);
  else if ( color_flag )
          get_a_color(t->color,1.0,&color);
  else
          get_a_color(WHITE,gray_level(t->normal),&color);
  theTriangle = OpenPoly();
  MoveTo(p[0][0],p[0][1]);
  LineTo(p[1][0],p[1][1]);
  LineTo(p[2][0],p[2][1]);
  LineTo(p[0][0],p[0][1]);
  ClosePoly();
  if ( web.hide_flag )  
    { RGBForeColor(&color);
      PaintPoly(theTriangle);
    }
  if ( edgeshow_flag ) /* all grid edges */
    {  RGBForeColor(&edgecolor);
       FramePoly(theTriangle);
     }
  KillPoly(theTriangle);
 
  /* show designated edges */
  PenSize(1,1);
  for ( i = 0 ; i < 3 ; i++ )
    { if ( t->ecolor[i] == CLEAR ) continue;
      if ( t->etype[i]&EBITS == INVISIBLE_EDGE ) continue;
      get_a_color(t->ecolor[i],1.0,&color);
      RGBForeColor(&color);
      MoveTo(p[i][0],p[i][1]);
      LineTo(p[i+1][0],p[i+1][1]);
    }

}

void finish_macgraph()
{
}

void close_macgraph()
{
  init_flag = 0;
}

void do_display()
{
  init_graphics = init_macgraph;
  finish_graphics = finish_macgraph;
  close_graphics = close_macgraph;
  graph_start = painter_start;
  graph_edge  = painter_edge;
  display_edge = macgraph_edge;
  graph_facet = painter_facet;
  display_facet = macgraph_facet;
  graph_end = painter_end;
  
  graphgen();

}

void display()  /* called by evolver when want update */
{
   /* GraphicsWindowPtr->need_redraw = 1; */
   /* just sets flag */
   
   do_drawing((WindowPtr)GraphicsWindowPtr);
 }

void graph_new_surface() {}
 
