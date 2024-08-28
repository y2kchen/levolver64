/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Eric Haines, wrath.cs.cornell.edu!eye!erich  *
*************************************************************/

/*******************************************************************
*
*  File:  hpzgraph.c
*
*  Contents:  Device-specific routines for graphing facets
*                 with HP98731 equipment.  Uses z-buffering for removing
*                 hidden surfaces, and a light source for shading.
*                 Change gopen() call for other devices.
*/


#include <starbase.c.h>
/* undefine stuff that Starbase uses that the evolver also uses, so as to
 * avoid compilation warnings.
 */
#undef DOT
#undef LINEAR
#undef QUADRATIC
#include "include.h"
#define  TRUE 1
#define  FALSE 0


/* screen corners */
static  int      Fd ;
static  int      Curr_buffer = 0 ;

static initz_flag = 0;

void hp_init()
{
}

void hp_finish()
{
}

void init_hpz()
{
     Fd = gopen( "/dev/crt", OUTDEV, "hp98731", THREE_D|MODEL_XFORM ) ;

     /* > Add these two commands for a smaller viewport (lower left corner)
     vdc_extent( Fd, 0.0, 0.0, 0.0, 1.25, 1.0, 1.0 ) ;
     view_port( Fd, 0.0, 0.0, 0.5, 0.5 ) ;
     */

     clip_indicator( Fd, 4 ) ;

     buffer_mode        ( Fd, 1 ) ;

     shade_mode         ( Fd, 6, 1 ) ;

     double_buffer     ( Fd, 3, 12 ) ;

     interior_style    ( Fd, 1, 0 ) ;

     vertex_format     ( Fd, 0,0,0,0,0 ) ;

     hidden_surface    ( Fd, 1, 0 ) ;
     bf_control( Fd, 1, 0) ;      /* turn reverse normals on */
     light_ambient( Fd, 0.3, 0.3, 0.3 ) ;
     light_source( Fd, 1, 0, 0.7, 0.7, 0.7, 1.0, 1.0, -1.0 ) ;
     light_switch( Fd, 3 ) ;

     clear_view_surface( Fd ) ;

     /* screen corners */
     view_volume( Fd, -1.4,-1.4,-2.0,1.4,1.4,2.0) ;
     initz_flag = 1;
}

void hpz_start()
{
     if ( initz_flag == 0 ) init_hpz();

     clear_view_surface( Fd ) ;
     zbuffer_switch( Fd, 1 ) ;    /* clear z-buffer */
}


void hpz_edge(g)
struct graphdata *g;
{
register int i,j,k;
register REAL    b[2][4];
REAL a[4];

     /* transform */
     a[3] = 1.0;
     for ( i = 0 ; i < 2 ; i++ )
     {
          for ( j = 0 ; j < 3 ; j++ ) a[j] = g[i].x[j];
          matvec_mul(view,a,b[i],4,4);  /* transform */
     }

     /* display */
     line_color( Fd, 1.0, 0.0, 0.0 ) ;
     move3d( Fd, (float)b[0][0], (float)b[0][1], (float)b[0][2] ) ;
     draw3d( Fd, (float)b[1][0], (float)b[1][1], (float)b[1][2] ) ;
}


void hpz_facet(g,f_id)
struct graphdata *g;
facet_id f_id;
{
register  int i,j,k;
register REAL b[3][4];
register REAL a[4];
float    clist[9];

     /* transform */
     a[3] = 1.0;
     for ( i = 0 ; i < 3 ; i++ ) {
          a[0] = g[i].x[0];
          a[1] = g[i].x[1];
          a[2] = g[i].x[2];
          matvec_mul(view,a,b[i],4,4);  /* transform */
          /* rotate to view */
          b[i][3] = b[i][0];
          b[i][0] = b[i][1];
          b[i][1] = b[i][2];
          b[i][2] = b[i][3];
     }
     /* display */
     if ( web.hide_flag ) {
          fill_color( Fd, 1.0, 1.0, 1.0 ) ;
          for ( k = 0 ; k < 3 ; k++ ) {
                for ( i = 0 ; i < 3 ; i++ ) {
                     clist[k*3+i] = (float)b[k][i];
                }
          }
          polygon3d( Fd, clist, 3, 0 ) ;
     }

     if ( ridge_color_flag ) {
          for ( i = 0, j = 1 ; i < 3 ; i++, j = (j+1)%3 ) {
                if ( g[i].color == RIDGE ) {
                     line_color( Fd, 1.0, 0.0, 0.0 ) ;
                } else {
                     line_color( Fd, 0.0, 0.0, 1.0 ) ;
                }

                move3d( Fd, (float)b[i][0], (float)b[i][1], (float)b[i][2] ) ;
                draw3d( Fd, (float)b[j][0], (float)b[j][1], (float)b[j][2] ) ;
          }
     }
     else {
          line_color( Fd, 1.0, 0.0, 0.0 ) ;
          move3d( Fd, (float)b[2][0], (float)b[2][1], (float)b[2][2] ) ;
          for ( k = 0 ; k < 3 ; k++ ) {
                draw3d( Fd, (float)b[k][0], (float)b[k][1], (float)b[k][2] ) ;
          }
     }
}


void hpz_end()
{
     Curr_buffer = !Curr_buffer ;
     dbuffer_switch( Fd, Curr_buffer ) ;
     make_picture_current(Fd);
}

void display()
{
  ENTER_GRAPH_MUTEX
     init_graphics = hp_init;
     finish_graphics = hp_finish;
     graph_start = hpz_start;
     graph_facet = hpz_facet;
     graph_edge  = hpz_edge;
     graph_end = hpz_end;

     graphgen();
  LEAVE_GRAPH_MUTEX
}
