
/* Microsoft C 5.1 graphics module */

#include "include.h"
#include <graph.h>

/* prototypes */
void reset_mode(void);
void mscgraph_start(void);
void mscgraph_edge(struct graphdata *gdata);
void mscgraph_facet(struct graphdata *gdata,facet_id f_id);
void msc_init(void);

void reset_mode()
{
  _setvideomode(_DEFAULTMODE);
}

void msc_init()
{
  _setvideomode(_HRESBW);     /* CGA graphics */
  atexit(reset_mode);
  _displaycursor(_GCURSORON);
  _setlogorg(319,99);
}

void mscgraph_start() 
{
  if ( init_flag == 0 ) msc_init();

  /* clear screen */
  _clearscreen(_GVIEWPORT);
  innerflag = outerflag = 1;
}

void mscgraph_facet(gdata,f_id)
struct graphdata *gdata;
facet_id f_id; /* unused here */
{
  int x[3],y[3];
  int i,j;
  double a[4],b[4];

  for ( i = 0 ; i < 3 ; i++ )
     {
        for ( j = 0 ; j < 3 ; j++ ) a[j] = gdata[i].x[j];
        a[3] = 1.0;
        matvec_mul(view,a,b,4,4);  /* transform */
        x[i] = (int)(b[1]*130);
        y[i] = (int)(b[2]*70);
     }         

  _moveto(x[0],-y[0]);
  _lineto(x[1],-y[1]);
  _lineto(x[2],-y[2]);
  _lineto(x[0],-y[0]);
}

void mscgraph_edge(gdata)
struct graphdata *gdata;
{
  int x[3],y[3];
  int i,j;
  double a[4],b[4];

  for ( i = 0 ; i < 2 ; i++ )
     {
        for ( j = 0 ; j < 2 ; j++ ) a[j] = gdata[i].x[j];
        a[2] = 0.0;
        a[3] = 1.0;
        matvec_mul(view,a,b,4,4);  /* transform */
        x[i] = (int)(b[0]*130);
        y[i] = (int)(b[1]*70);
     }         

  _moveto(x[0],y[0]);
  _lineto(x[1],y[1]);
}

 
void display()
{
  ENTER_GRAPH_MUTEX
  init_graphics = msc_init;
  finish_graphics = null_function;
  graph_start = mscgraph_start;
  graph_facet = mscgraph_facet;
  graph_edge = mscgraph_edge;
  graph_end = null_function;
  
  graphgen();
  LEAVE_GRAPH_MUTEX
}
