
/* vgagraph.c */


#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include <sys/machdep.h>
#include <sys/comcrt.h>
#include "include.h"

double zzz; /* for storing bomb */

#define VGA13

#ifdef VGA13
#define MAXX 320
#define MAXY 200
#define MAG  70.0
#define VBUFFSTART 0x4000
#define SW_VGAMODE SW_VGA13
#endif

#ifdef VGA12
#define MAXX 640
#define MAXY 480
#define MAG  70.0
#define VBUFFSTART 0x4000
#define SW_VGAMODE SW_VGA12
#endif

#define ZOFFSET 1000.0

int oldmode;
unsigned char *vgabuff;
int vgafd;
struct port_io_struct iodata[4];

REAL vgaviewx[HOMDIM][HOMDIM];
REAL *vgaview[HOMDIM] = { vgaviewx[0],vgaviewx[1],vgaviewx[2],vgaviewx[3]};

REAL vgafixx[HOMDIM][HOMDIM] = { { 0.0, MAG, 0.0, MAXX/2.0},
                     { 0.0, 0.0, -MAG, MAXY/2.0},
                     {-MAG, 0.0, 0.0, ZOFFSET/2.0},
                     { 0.0, 0.0, 0.0, 1.0}
                   } ;  /* for changing view to vgaview */
REAL *vgafix[HOMDIM] = { vgafixx[0],vgafixx[1],vgafixx[2],vgafixx[3]};
float zbuff[MAXY][MAXX];
char colors[256][3] = {  /* rgb */ {0,0,0},{0,0,21},{0,21,0},{0,21,21},
  {21,0,0},{21,0,21},{21,0,42},{21,21,21},{42,42,42},{42,42,63},
  {42,63,42},{42,63,63},{63,42,42},{63,42,63},{63,63,42},{63,63,63} };

#define EDGECOLOR 0 
#define LINECOLOR 4

double dzdx,dzdy; /* slopes in z linear equation */
double zbase;    /* constant in z linear equation */

char *mathmsg[7] = { "","Domain","Singular","Overflow","Underflow",
  "Total loss of significance","Partial loss of significance"};

int matherr(ex)
struct exception *ex;
{
  if ( vgafd )
    {   
      ioctl(vgafd,MODESWITCH|oldmode,0);
      close(vgafd);
      vgafd = 0;
    }
  sprintf(errmsg,"Matherr in %s(%f): %s\n",ex->name,ex->arg1,mathmsg[ex->type]);
  kb_error(errmsg,RECOVERABLE);
  return 0;
}


void vga_line(head,tail,color)
REAL *head,*tail;
int color;
{
  int k,row,col;
  REAL x,y,z,xdiff,ydiff,dx,dy;
  int npts;
  char *pixel;
  int *zspot;
  REAL *temp;

   if ( head[1] < tail[1] ) { temp = head; head = tail; tail = temp; }
   xdiff = head[0] - tail[0]; ydiff = head[1] - tail[1];
   if ( xdiff > ydiff ) { dx = 1.0, dy = ydiff/xdiff; npts = (int)xdiff; }
   else if ( xdiff > -ydiff ) { dx =  xdiff/ydiff; dy = 1.0; npts = (int)ydiff;}
   else if ( xdiff < 0.0 ) { dx = -1.0; dy = -ydiff/xdiff; npts = -(int)xdiff; }
   else { npts = 0; /* degenerate case */ }

   x = tail[0]+.5; y = tail[1]+.5; 
   for ( k = 0; k < npts ; x += dx, y += dy, k++ )
     {
       row = (int)y; col = (int)x;
       if ( (row<0)||(row>=MAXY)||(col<0)||(col>=MAXX) )continue;
       z = zbase + dzdx*col + dzdy*row;
       pixel = vgabuff + row*MAXX + col;
       zspot = zbuff[row] + col;
       if ( z < *zspot ) /* plot */
            { 
 zzz = z;
 if ( fabs(z) > 1e8 ) continue;
              *zspot = z;
              *pixel = (char)color;
            }
     }
}


void draw_triangle(coords,color)
double *coords[3];
int color;
{
  double *temp,*top,*mid,*low;
  double norm[3];
  double side1[3],side2[3],side3[3]; /* side vectors of triangle */
  int row,col;
  int midflag;
#define MIDLEFT 1
#define MIDRIGHT 2
  double mleft,bleft,mright,bright; /* side line m and b */
  int i;
  int toprow,midrow,lowrow;
  int xleft, xright;
  double z;
  int *zspot;
  unsigned char *pixel;

  /* sort vertices vertically */
  if ( coords[0][1] > coords[1][1] )
    { temp = coords[0]; coords[0] = coords[1]; coords[1] = temp; }
  if ( coords[1][1] > coords[2][1] )
    { temp = coords[1]; coords[1] = coords[2]; coords[2] = temp; }
  if ( coords[0][1] > coords[1][1] )
    { temp = coords[0]; coords[0] = coords[1]; coords[1] = temp; }

  top = coords[0]; mid = coords[1]; low = coords[2];

  if ( color == CLEAR ) goto just_edges;

  /* get normal */
  for ( i = 0 ; i < 3 ; i++ )
    { side1[i] = mid[i] - top[i];
      side2[i] = low[i] - top[i];
      side3[i] = low[i] - mid[i];
    }
  norm[0] = side1[1]*side2[2] - side1[2]*side2[1];
  norm[1] = side1[2]*side2[0] - side1[0]*side2[2];
  norm[2] = side1[0]*side2[1] - side1[1]*side2[0];
  if ( fabs(norm[2]) < 1e-9 ) return;  /* edge-on triangle */
  if ( norm[2] < 0.0 ) midflag = MIDLEFT;
  else
    { midflag = MIDRIGHT;
      for ( i = 0 ; i < 3 ; i++ ) norm[i] = -norm[i];
    }

  /* z slopes */
  dzdx = -norm[0]/norm[2];
  dzdy = -norm[1]/norm[2];
  zbase = top[2] - dzdx*top[0] - dzdy*top[1];

  /* find key scan lines */
  toprow = (int)ceil(top[1]);  /* round up */
  if ( toprow >= MAXY ) return;  /* triangle off screen */
  if ( toprow < 0 ) toprow = 0;

  lowrow = (int)ceil(low[1]);  /* round up */
  if ( lowrow < 0 ) return;  /* triangle off screen */
  if ( lowrow > MAXY ) lowrow = MAXY;

  midrow = (int)ceil(mid[1]);  /* round up */
  if ( midrow < 0 ) midrow = 0;
  
  /* set up side lines */
  if ( midflag == MIDLEFT )
    { if ( fabs(side1[1]) < .01 ) {mleft = 0.0; bleft = top[0];}
      else
        {
          mleft = side1[0]/side1[1];
          bleft = (top[0]*mid[1] - top[1]*mid[0])/side1[1];
        }
      if ( fabs(side2[1]) < .01 ) {mright = 0.0; bright = top[0];}
      else
        {
          mright = side2[0]/side2[1];
          bright = (top[0]*low[1] - top[1]*low[0])/side2[1];
        }
    }
  else
    { if ( fabs(side2[1]) < .01 ) {mleft = 0.0; bleft = top[0];}
      else
        {
          mleft = side2[0]/side2[1];
          bleft = (top[0]*low[1] - top[1]*low[0])/side2[1];
        }
      if ( fabs(side1[1]) < .01 ) {mright = 0.0; bright = top[0];}
      else
        {
          mright = side1[0]/side1[1];
          bright = (top[0]*mid[1] - top[1]*mid[0])/side1[1];
        }
    }

  /* scan convert */
  for ( row = toprow; row < lowrow ; row++ )
    {
      if ( row == midrow ) /* change side line */
        { if ( midflag == MIDLEFT )
            {
              if ( fabs(side3[1]) < .01 ) {mleft = 0.0; bleft = mid[0];}
              else
                {
                  mleft = side3[0]/side3[1];
                  bleft = (mid[0]*low[1] - mid[1]*low[0])/side3[1];
                }
            }
          else 
            {
              if ( fabs(side3[1]) < .01 ) {mright = 0.0; bright = mid[0];}
              else
                {
                  mright = side3[0]/side3[1];
                  bright = (mid[0]*low[1] - mid[1]*low[0])/side3[1];
                }
            }
        }
      xleft = (int)(row*mleft + bleft +1.0);
      if ( xleft < 0 ) xleft = 0;  /* clip */
      xright = (int)(row*mright + bright);
      if ( xright >= MAXX ) xright = MAXX-1;  /* clip */
      z = zbase + dzdx*xleft + dzdy*row;
      pixel = vgabuff + row*MAXX + xleft;
      zspot = zbuff[row] + xleft;
      for ( col = xleft ; col <= xright ; col++, zspot++,pixel++, z += dzdx )
        { if ( z < *zspot ) /* plot */
            { *zspot = z;
              *pixel = (unsigned char)color;
            }
        }
   }

just_edges:
   if ( edgeshow_flag )
     {
       vga_line(mid,top,fillcolor);
       vga_line(low,top,fillcolor);
       vga_line(low,mid,fillcolor);
     }
}

void setcolors()
{
  int retval;
  int i,k;

  memset((char*)iodata,0,sizeof(iodata));
  iodata[0].dir = OUT_ON_PORT;
  iodata[0].port = 0x3C8; /*  reg */
  iodata[0].data = 0;     /* first color */
  retval = ioctl(vgafd,VGAIO,iodata);
  if ( retval == -1 ) perror("PEL address write");
  for ( k = 0 ; k < 256 ; k++ )
    {
      for (  i = 0 ; i < 3 ; i++ )
        {
          iodata[i].dir = OUT_ON_PORT;
          iodata[i].port = 0x3C9; /*  reg */
          iodata[i].data = colors[k][i];  
        } 
      retval = ioctl(vgafd,VGAIO,iodata);
      if ( retval == -1 ) perror("outport");
    }
}
  

int inport(index)
{
  int retval;
 
  memset((char *)iodata,0,sizeof(iodata));
  iodata[0].dir = OUT_ON_PORT;
  iodata[0].port = 0x3D4; /*  reg */
  iodata[0].data = (char)index;  
  iodata[1].dir = IN_ON_PORT;
  iodata[1].port = 0x3D5; /*  reg */
  retval = ioctl(vgafd,VGAIO,iodata);
  if ( retval == -1 ) perror("inport");
  return iodata[1].data;
}
  
void outport(index,value)
{
  int retval;

  memset((char*)iodata,0,sizeof(iodata));
  iodata[0].dir = OUT_ON_PORT;
  iodata[0].port = 0x3D4; /*  reg */
  iodata[0].data = (char)index;  
  iodata[1].dir = OUT_ON_PORT;
  iodata[1].port = 0x3D5; /*  reg */
  iodata[1].data = (char)value;  
  retval = ioctl(vgafd,VGAIO,iodata);
  if ( retval == -1 ) perror("outport");
}
  
void vgagraph_start()
{
  int i,k;
  float highvalue = 1e30;
  
  memset((char*)zbuff,((char *)&highvalue)[3],sizeof(zbuff));
  /* farthest depth; with kludge to fill with high values */

  /* set up transform matrix to device coordinates */
  if ( web.sdim == 2 )
    { /* get x-y on screen instead of y-z */
      vgafix[0][0] = MAG; vgafix[1][1] = -MAG; vgafix[2][2] = MAG;
      vgafix[0][1] = vgafix[1][2] = vgafix[2][0] = 0.0;
    }
  else
    { /* get y-z on screen */
      vgafix[0][0] = vgafix[1][1] = vgafix[2][2] = 0.0;
      vgafix[0][1] = MAG; vgafix[1][2] = vgafix[2][0] = -MAG;
    }
  mat_mult(vgafix,view,vgaview,HOMDIM,HOMDIM,HOMDIM);

  for ( k = 64 ; k < 128 ; k++ )
   { /* yellow scale */
     colors[k][0] = (char)(k-64);
     colors[k][1] = (char)(k-64);
   }

  vgafd = open("/dev/vga",O_WRONLY);
  oldmode = ioctl(vgafd,VGA_GET,0);
  vgabuff = VBUFFSTART + (char *)ioctl(vgafd,MAPVGA,0);
  ioctl(vgafd,SWAPVGA,0);
  ioctl(vgafd,SW_VGAMODE,0);

  setcolors();

}

void vgagraph_end()
{
  getchar();

  ioctl(vgafd,MODESWITCH|oldmode,0);
  close(vgafd);
  vgafd = 0;
}


void vgagraph_facet(gdata,f_id)
struct graphdata *gdata;
facet_id f_id;
{
  double coordsx[FACET_VERTS][HOMDIM];
  double *coords[FACET_VERTS];
  int i,k;

  for ( i = 0 ; i < FACET_VERTS ; i++ )
    {
      coords[i] = coordsx[i];
      matvec_mul(vgaview,gdata[i].x,coords[i],HOMDIM,HOMDIM); 
    } 
  if ( gdata[0].color != CLEAR ) 
     draw_triangle(coords,gdata[0].color);

  /* show designated edges */
  for ( k = 0 ; k < 3 ; k++ )
    { if ( gdata[k].etype&EBITS == INVISIBLE_EDGE ) continue;
      if ( gdata[k].ecolor == CLEAR ) continue;
      vga_line(coords[k],coords[(k+1)%3],gdata[k].ecolor);
    }
}

void vgagraph_edge(gdata)
struct graphdata *gdata;
{
  int i,j;
  REAL a[MAXCOORD+1],b[2][MAXCOORD+1];

  if ( gdata[0].color == CLEAR ) return;
  for ( i = 0 ; i < 2 ; i++ )
    {
      for ( j = 0 ; j < web.sdim ; j++ ) a[j] = gdata[i].x[j];
      for ( ; j < HOMDIM-1 ; j++ ) a[j] = 0.0;
      a[HOMDIM-1] = 1.0;
      matvec_mul(vgaview,a,b[i],HOMDIM,HOMDIM);
      /* transform */
      b[i][2] -= ZOFFSET/100;  /* bias lines in front of facets */
    }       
  if ( web.sdim > 2 )
   {
     if ( b[1][0]-b[0][0] != 0.0 )
       dzdx = (b[1][2]-b[0][2])/(b[1][0]-b[0][0]);
     else dzdx = 0.0;
     if ( b[1][1]-b[0][1] != 0.0 )
       dzdy = (b[1][2]-b[0][2])/(b[1][1]-b[0][1]);
     else dzdy = 0.0;
     zbase = b[0][2] - b[0][0]*dzdx - b[0][1]*dzdy;
   }
  else zbase = dzdx = dzdy = 0.0;
  vga_line(b[0],b[1],gdata[0].color);
}

void display()
{
  graph_start = vgagraph_start;
  graph_edge  = vgagraph_edge;
  graph_facet = vgagraph_facet;
  graph_end = vgagraph_end;
  
  graphgen();
}
