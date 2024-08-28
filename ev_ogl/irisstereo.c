
/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Ken Brakke, brakke@susqu.edu                 *
*************************************************************/


/*******************************************************************
*
*  File:  irisstereo.c
*
*  Contents:  Device-specific routines for graphing facets 
*                 with iris.  Uses z-buffering for removing 
*                 hidden surfaces. Split-screen display for 
*                 stereo viewing.
*/


#include "include.h"
#include <gl.h>

static long win_id;  /* graphics window id */

/* screen corners */
static REAL scrx[4],scry[4];

static initz_flag = 0;

/* stereo projection parameters, one unit being about 4 inches */
REAL eye = 0.5;    /* eye displacement from centerline */
REAL D    = 12.0;    /* eye distance from screen */

void iris_init()
{
}

void iris_finish()
{
}

void init_irisz()
{
  foreground();
  keepaspect(1,1);
  prefposition(128,1151,0,1023);    /* full screen height for stereo */
  win_id = winopen("evolver");
  doublebuffer();
  gconfig();
  zbuffer(TRUE);
 
  /* screen corners */
  scrx[0] = scrx[1] = -1.4;
  scrx[2] = scrx[3] =  1.4;
  scry[0] = scry[3] =  1.4;
  scry[1] = scry[2] = -1.4;
  ortho(scrx[0],scrx[2],scry[0],scry[2],-200.0,200.0);
  initz_flag = 1;
}     

void irisz_start()
{

  if ( initz_flag == 0 ) init_irisz();
  /* clear screen */

  color(GREEN);
  clear();
  zclear();
}     


void irisz_edge(g)
struct graphdata *g;
{  
  int i,j,k;
  REAL a[4],b[2][4];

  /* transform */
  a[3] = 1.0;
  for ( i = 0 ; i < 2 ; i++ )
     {
        for ( j = 0 ; j < 3 ; j++ ) a[j] = g[i].x[j];
        matvec_mul(view,a,b[i],4,4);  /* transform */
     } 

    /* display */
    color(RED);
    bgnline();
    for ( k = 0 ; k < 2 ; k++ )
      v3d(b[k]);
    endline();

}


void irisz_facet(g,f_id)
struct graphdata *g;
facet_id f_id;
{  
  int i,j,k;
  REAL a[4],b[3][4];
  REAL tmp[3][4];

  /* transform */
  a[3] = 1.0;
  for ( i = 0 ; i < 3 ; i++ )
     {
        for ( j = 0 ; j < 3 ; j++ ) a[j] = g[i].x[j];
        matvec_mul(view,a,tmp[i],4,4);  /* transform */
        /* rotate from my axes to iris axes */
        tmp[i][3] = tmp[i][0];
        tmp[i][0] = tmp[i][1];
        tmp[i][1] = tmp[i][2];
        tmp[i][2] = tmp[i][3];
     } 

    /* display upper view */
    for ( i = 0 ; i < 3 ; i++ )
    /* stereo projection */
      { b[i][0] = eye + D*(tmp[i][0] - eye)/(D - tmp[i][2]);
         b[i][1] = D*tmp[i][1]/(D - tmp[i][2]);
         b[i][1] = b[i][1]/2 + 0.7;  /* upper split screen */
         b[i][2] = tmp[i][2];
      }
    if ( web.hide_flag )
      {
         color(WHITE);
         bgnpolygon();
         for ( k = 0 ; k < 3 ; k++ )
            v3d(b[k]);
         endpolygon();
      }

    for ( i = 0 ; i < 3 ; i++ )
      b[i][2] += 0.005;  /* get edges in front of facets */

  if ( edgeshow_flag )
      { 
         color(fillcolor);
         bgnclosedline();
         for ( k = 0 ; k < 3 ; k++ )
            v3d(b[k]);
         endclosedline();
      }

  for ( k = 0 ; k < 3 ; k++ )
     { if ( t->etype[k]&EBITS == INVISIBLE_EDGE ) continue;
        if ( t->ecolor[k] == CLEAR ) continue;
         color(t->ecolor[k]);
         bgnline();
         v3d(b[i]);
         v3d(b[(i+1)%3]);
         endline();
     }

    /* display lower view */
    for ( i = 0 ; i < 3 ; i++ )
    /* stereo projection */
      { b[i][0] = -eye + D*(tmp[i][0] + eye)/(D - tmp[i][2]);
         b[i][1] = D*tmp[i][1]/(D - tmp[i][2]);
         b[i][1] = b[i][1]/2 - 0.75;  /* lower split screen */
         b[i][2] = tmp[i][2];
      }
    if ( web.hide_flag )
      {
         color(WHITE);
         bgnpolygon();
         for ( k = 0 ; k < 3 ; k++ )
            v3d(b[k]);

         endpolygon();
      }

    for ( i = 0 ; i < 3 ; i++ )
      b[i][2] += 0.005;  /* get edges in front of facets */

  if ( edgeshow_flag )
      { 
         color(fillcolor);
         bgnclosedline();
         for ( k = 0 ; k < 3 ; k++ )
            v3d(b[k]);
         endclosedline();
      }

  /* designated edges */
  for ( k = 0 ; k < 3 ; k++ )
     { if ( t->etype[k]&EBITS == INVISIBLE_EDGE ) continue;
        if ( t->ecolor[k] == CLEAR ) continue;
         color(t->ecolor[k]);
         bgnline();
         v3d(b[i]);
         v3d(b[(i+1)%3]);
         endline();
     }
}


void irisz_end()
{
  swapbuffers();
}

iris_close()
{
  winclose(win_id);
  init_flag = 0;
  win_id = -1;
}

void display()
{
  ENTER_GRAPH_MUTEX
  graph_start = irisz_start;
  graph_facet = irisz_facet;
  graph_edge  = irisz_edge;
  graph_end = irisz_end;
  
  init_graphics = iris_init;
  finish_graphics = iris_finish;
  close_graphics = iris_close;
  graphgen();
  LEAVE_GRAPH_MUTEX
}
