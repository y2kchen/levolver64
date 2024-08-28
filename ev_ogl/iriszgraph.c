/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Ken Brakke, brakke@susqu.edu                 *
*************************************************************/

/*******************************************************************
*
*  File:  iriszgraph.c
*
*  Contents:  Device-specific routines for graphing facets 
*                 with iris.  Uses z-buffering for removing 
*                 hidden surfaces.
*/


#include "include.h"
#include <gl.h>
#include <device.h>

static long win_id;  /* graphics window id */

/* screen corners */
static REAL scrx[4],scry[4];

static initz_flag = 0;  /* z buffer */

/* gl matrices have vector on left! */
Matrix vt3 =  /* gl transform matrix */
  { {0.,0.,1.,0.},
     {1.,0.,0.,0.},
     {0.,-1.,0.,0.},
     {0.,0.,0.,1.} };

Matrix vt2 =  /* gl transform matrix */
  { {1.,0.,0.,0.},
     {0.,-1.,0.,0.},
     {0.,0.,1.,0.},
     {0.,0.,0.,1.} };

static float rgba[16][4]; 

static prev_timestamp; /* for remembering surface version */

static dindex = 1;  /* display list object index */

void draw_screen ARGS((void));

void iris_init()
{
}

void iris_finish()
{
}

/* lighting model parameters */
float material[20] = { LMNULL };
/* float lmodel[20] = { TWOSIDE,1.0,LMNULL }; */
float lmodel[20] = {LMNULL};
float lights[8][20] = { 
    {AMBIENT,0.4,0.4,0.4,POSITION,0.5,0.0,2.0,0.0,LCOLOR,0.4,0.4,0.4,LMNULL} ,
    {AMBIENT,1.0,1.0,1.0,LMNULL} 
    };
void init_irisz ARGS((void));

void init_irisz()
{ int i,j;

  for ( i = 0 ; i < 16 ; i++ )
     for ( j = 0 ; j < 4 ; j++ )
        rgba[i][j] = (float)rgb_colors[i][j];

  foreground();
  keepaspect(1,1);
/*  prefposition(551,1151,423,1023); */    /* x1,x2,y1,y3 */
  win_id = winopen("evolver");
  doublebuffer(); 
  RGBmode();
  gconfig();
  zbuffer(TRUE);

  /* lighting */
  mmode(MPROJECTION);
  lmdef(DEFMATERIAL,1,0,material);
  lmdef(DEFLMODEL,1,3,lmodel);
  lmdef(DEFLIGHT,1,10,lights[0]); /* for ambient plus directional */
  lmdef(DEFLIGHT,2,10,lights[1]); /* for all ambient */
  lmbind(LIGHT0,1);
  lmbind(MATERIAL,1);
  lmbind(LMODEL,1);
  lmcolor(LMC_COLOR);
  shademodel(FLAT);
  lmcolor(LMC_AD);
 
  /* screen corners */
  scrx[0] = scrx[1] = -1.4;
  scrx[2] = scrx[3] =  1.4;
  scry[0] = scry[3] =  1.4;
  scry[1] = scry[2] = -1.4;
  initz_flag = 1;
}     

void draw_screen()
{
  Matrix viewf;
  int i,j;

  reshapeviewport();

  mmode(MPROJECTION);
  ortho(scrx[0],scrx[2],scry[0],scry[2],-200.0,200.0);
  if ( SDIM == 2 ) multmatrix(vt2); /* upside down */
  else multmatrix(vt3);  /* upside down and rotate axes */
  for ( i = 0 ; i < 4 ; i++ )
     for ( j = 0 ; j < 4 ; j++ )
        viewf[i][j] = view[j][i];  /* transpose */
  mmode(MVIEWING);
  loadmatrix(viewf); 

  /* clear screen and zbuffer */
  czclear(0xFFFcFdFe,getgdesc(GD_ZMAX));

  if ( shading_flag ) lmbind(LMODEL,1);
  else lmbind(LMODEL,0);

  callobj(dindex);
  swapbuffers();
}

void irisz_start()
{
  if ( initz_flag == 0 ) init_irisz();

  if ( graph_timestamp != prev_timestamp ) 
      makeobj(dindex);  /* start display list */
}     


void irisz_edge(g,e_id)
struct graphdata *g;
edge_id e_id;
{  
  int k;
  int e_color;

  e_color = g[0].color;
  if ( e_color == CLEAR ) return;
  if ( (e_color < 0) || (e_color >= IRIS_COLOR_MAX) )
     e_color = DEFAULT_EDGE_COLOR;

    /* display */
    c3f(rgba[e_color]);
    bgnline();
    for ( k = 0 ; k < 2 ; k++ )
      v3d(g[k].x);
    endline();

}

float norm[3] = { .6,.8,0.};

void irisz_facet(g,f_id)
struct graphdata *g;
facet_id f_id;
{  
  int i,k;
  REAL len;

         /* need normal for lighting */
         for ( i = 0 ; i < 3 ; i++ )
         { int ii = (i+1)%3;
            int iii = (i+2)%3;
            norm[i] = ((g[1].x[ii]-g[0].x[ii])*(g[2].x[iii]-g[0].x[iii])
                 -  (g[1].x[iii]-g[0].x[iii])*(g[2].x[ii]-g[0].x[ii]));
         }
         len = sqrt(dotf(norm,norm,3));
         if ( len <= 0.0 ) return;
         for ( i = 0 ; i < 3 ; i++ ) norm[i] /= len;

    if ( web.hide_flag && (g[0].color != CLEAR) && (g[0].color != UNSHOWN) )
      {
         if ( color_flag ) c3f(rgba[g->color]);
         else c3f(rgba[WHITE]);
         n3f(norm);
         bgnpolygon();
         for ( k = 0 ; k < 3 ; k++ )
             v3d(g[k].x);
         endpolygon();
         if ( g->color != g->backcolor )
            { REAL x[MAXCOORD];
              c3f(rgba[g->backcolor]);
              for ( i = 0 ; i < 3 ; i++ ) norm[i] = -norm[i];
              n3f(norm);
              bgnpolygon();
              for ( k = 0 ; k < 3 ; k++ )
              { for ( i = 0 ; i < SDIM ; i++ )
                      x[i] = g[k].x[i] - 0.0001*overall_size*norm[i];
                 v3d(x);
              }
              endpolygon();
            }
      }

  for ( k = 0 ; k < 3 ; k++ )
     { 
        if ( g[k].ecolor == CLEAR ) continue;
        if ( !edgeshow_flag || (g[0].color == UNSHOWN) )
        { if ( g[k].etype&EBITS == INVISIBLE_EDGE ) continue;
        }
         c3f(rgba[g[k].ecolor]);
         n3f(norm);
         bgnline();
         v3d(g[k].x);
         v3d(g[(k+1)%3].x);
         endline();
     }

}


void irisz_end()
{
  closeobj();
  draw_screen();
  prev_timestamp = graph_timestamp;
}

void iris_close()
{
  winclose(win_id);
  init_flag = 0;
  initz_flag = 0;
  win_id = -1;
}

void display()
{
#ifdef MOUSING
  REAL x1=0.0,y1=0.0,x2=0.0,y2=0.0;
#endif
  ENTER_GRAPH_MUTEX
  graph_start = irisz_start;
  graph_facet = irisz_facet;
  graph_edge  = irisz_edge;
  graph_end = irisz_end;
  
  init_graphics = iris_init;
  finish_graphics = iris_finish;
  close_graphics = iris_close;
  if ( graph_timestamp != prev_timestamp )
     graphgen();
  draw_screen();

#ifdef MOUSING
  /* events until key */
  outstring("Move surface with left mouse button; escape with right button.\n");
  qreset();
  qdevice(LEFTMOUSE);  /* left mouse button */
  qdevice(MIDDLEMOUSE);
  qdevice(RIGHTMOUSE);
  unqdevice(MOUSEX);
  unqdevice(MOUSEY);
  qreset();
  for (;;)
    { short value;
      int dev;

      dev = qread(&value);
      switch (dev )
         { 
            case LEFTMOUSE:
              if ( value == 1 ) /* down */ 
                { x1 = getvaluator(MOUSEX);
                  y1 = getvaluator(MOUSEY);
printf("x1: %f    y1: %f  \n",(DOUBLE)x1,(DOUBLE)y1);
                }
              else  /* up */
                { x2 = getvaluator(MOUSEX);
                  y2 = getvaluator(MOUSEY);
printf("x2: %f    y2: %f    mouse value %d \n",(DOUBLE)x2,(DOUBLE)y2,value);
                  fix_ctm(view,x2-x1,y2-y1);
                  draw_screen();
                }
              break;

            case REDRAW: 
              draw_screen();
              break;

            case INPUTCHANGE:
            case DEPTHCHANGE:
              break;

            default:  sprintf(msg,"Break event %d\n",dev );
                outstring(msg);
                goto gexit;
         }
    }
gexit:
  unqdevice(LEFTMOUSE);  /* left mouse button */
  unqdevice(MIDDLEMOUSE);
  unqdevice(RIGHTMOUSE);
  unqdevice(MOUSEX);
  unqdevice(MOUSEY);

#endif
  LEAVE_GRAPH_MUTEX
}

void graph_new_surface() {}

