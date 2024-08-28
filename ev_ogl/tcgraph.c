/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Ken Brakke, brakke@susqu.edu                 *
*************************************************************/

/* TurboC 2.0 graphics module */
/* All coordinates in absolute pixels in current viewport */

#include <graphics.h>
#include "include.h"

static int maxx,maxy;  /* max viewport coordinates */
static double xscale,yscale;  /* for scaling to screen size */
static int maxcolor;
static int backcolor;
static  int graphmode;

/* PORTING NOTE: set environment variable BGIPATH to directory
    with BGI graphics drivers. */
void restoremode(){ restorecrtmode();}

void init_tcgraph()
{
  int graphdriver = DETECT;
  int xasp,yasp;  /* aspect ratio */

  if ( init_flag == 0 )
     {
        initgraph(&graphdriver,&graphmode,getenv("BGIPATH"));
        if ( graphdriver < 0 )
          {
             switch ( graphdriver )
                {
                  case -2: sprintf(errmsg,"Cannot detect a graphics card.");
                              error(errmsg,RECOVERABLE);
                  case -3: sprintf(errmsg,"Cannot find BGI driver file.\n");
                              error(errmsg,RECOVERABLE);
                  case -4: sprintf(errmsg,"Invalid driver.");
                              error(errmsg,RECOVERABLE);
                  case -5: sprintf(errmsg,"Insufficient memory to load driver.");
                              error(errmsg,RECOVERABLE);
                }
          }
        setgraphmode(getmaxmode());
        atexit(restoremode);
        maxx = getmaxx(); maxy = getmaxy();
        maxcolor = getmaxcolor();
        if ( maxcolor > 2 )  backcolor = BLUE;
        else backcolor = BLACK;
        setbkcolor(backcolor);
        if ( graphmode == 4 ) /* CGA hi res */
            xscale = 2*maxy/3;     /* exact for CGA on plasma */
        else
         {
            getaspectratio(&xasp,&yasp);  
            xscale = maxy/3*(long)yasp/(long)xasp;  
         }
        yscale = maxy/3;
        init_flag = 1;
     }

  /* clear screen */
  clearviewport();
}

void tcgraph_edge(t)
struct tsort *t;
{
  int x[MAXCOORD],y[MAXCOORD];
  int i;
  
  if ( t->color == CLEAR ) return;
  setcolor(t->color);

  for ( i = 0 ; i < 2 ; i++ )
     {
        x[i] = (int)(t->x[i][0]*xscale) + maxx/2;
        y[i] = maxy/2 - (int)(t->x[i][1]*yscale);
     }         

  moveto(x[0],y[0]);
  lineto(x[1],y[1]);
}

static float light[3] = { 0.0, -1.0, 0.0 };
static int     gray[16]  /* Toshiba color to gray scale map */
     = { 0, 1, 4, 5, 8, 2, 6, 9, 3, 12, 13, 7, 10, 11, 14, 15 };

void tcgraph_facet(t)
struct tsort *t;
{
  int n = FACET_VERTS;        /* vertices in polygon */
  int x[6][2];     /* vertex coords         */
  int i,j,k,color;
  color = t->color;
  
  if ( (web.modeltype == QUADRATIC) && valid_id(t->f_id) )
     {
        double a[MAXCOORD+1],b[MAXCOORD+1],*y;
        facetedge_id fe;

        a[web.sdim] = 1.0;

        n = FACET_CTRL;
        fe = get_facet_fe(t->f_id);
        for ( i = 0 ; i < FACET_CTRL ; i += 2 )
          { 
             y = get_coord(get_fe_tailv(fe));
             for ( j = 0 ; j < web.sdim ; j++ ) a[j] = y[j];
             matvec_mul(view,a,b,HOMDIM,HOMDIM);  /* transform */
             x[i][0] = (int)(b[1]*xscale) + maxx/2;
             x[i][1] = maxy/2 - (int)(b[2]*yscale);
 
             y = get_coord(get_fe_midv(fe));
             for ( j = 0 ; j < web.sdim ; j++ ) a[j] = y[j];
             matvec_mul(view,a,b,HOMDIM,HOMDIM);  /* transform */
             x[i+1][0] = (int)(b[1]*xscale) + maxx/2;
             x[i+1][1] = maxy/2 - (int)(b[2]*yscale);

             fe = get_next_edge(fe);
          }
     }
  else
     for ( i = 0 ; i < web.sdim ; i++ )
      {
         x[i][0] = (int)(t->x[i][0]*xscale) + maxx/2;
         x[i][1] = maxy/2 - (int)(t->x[i][1]*yscale);
      }
  if (color != CLEAR)
  {
     if ( web.hide_flag )  
      {
        setfillstyle(SOLID_FILL,color);
        if ( graphmode == 4 ) setcolor(0);
        else if ( edgeshow_flag ) setcolor(fillcolor);  
              else setcolor(color);  /* don't show edges */
      }
     else
      { setfillstyle(EMPTY_FILL,backcolor);
         setcolor(fillcolor);
      }
     fillpoly(n,(int *)x);
  }
  /* show designated edges */
  for ( k = 0 ; k < 3 ; k++ )
     { if ( t->etype[k]&EBITS == INVISIBLE_EDGE ) continue;
        if ( t->ecolor[k] == CLEAR ) continue;
         setcolor(t->ecolor[k]);
         moveto(x[k][0],x[k][1]);
         lineto(x[(k+1)%3][0],x[(k+1)%3][1]);
     }
}

void display()
{
  init_graphics = init_tcgraph;
  finish_graphics = null_function;
  graph_edge  = painter_edge;
  display_edge = tcgraph_edge;
  if ( web.dimension == STRING )
     {
        graph_start = init_graphics;
        graph_end    = null_function;
     }
  else
     { 
        graph_start = painter_start;
        graph_facet = painter_facet;
        display_facet = tcgraph_facet;
        graph_end = painter_end;
     }
  
  graphgen();
}
