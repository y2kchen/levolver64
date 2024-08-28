/******************************************************************
* These functions calculate the length of an edge in the Klein
* model of the hyperbolic plane.
*/

#include "include.h"

#define SQR(a) ((a)*(a))
#define PI 3.14159265358979

double acosh();

double coshKleinLength();

double sinhKleinLength();

double klein_length();

double calc_grad();

void klein_length_grad();

double angle_klein();

double area_klein();

void grad_area_klein();

/*****************************************************************
* This function takes the inverse hyperbolic cosine of a number
* who's value is greater than or equal to one and returns the
* positve answer.
*/
double acosh(value)
double value;       /* the value who's acosh is being taken */
{
  if(value>=1)
    return(log(value + sqrt(SQR(value) - 1)));
  else
    return(0);
}

/****************************************************************
* This function finds the hyperbolic cosine of an edge in the Klein
* model of the hyperbolic plane. It is used to calculate edge
* lengths and angles in the model.
*/
double coshKleinLength(head, tail)
double *head;       /* coordinates of edge */
double *tail;
{
  double num;       /* numerator and den0minator of returned value */
  double den;
  double temp;
  int i, j;         /* iterators */

  num = 1;
  for(i=0;i<web.sdim;i++)
    num -= head[i] * tail[i];

  den = 1;
  for(i=0;i<web.sdim;i++)
    den -= SQR(head[i]);
  temp = 1;
  for(i=0;i<web.sdim;i++)
    temp -= SQR(tail[i]);
  if((den*=temp)>0)
    den = sqrt(den);
  else
    return(0);

  return(num/den);
}

/***********************************************************************
* This function finds the hyperbolic sine of an edge in the klein model
* of hyperbolic space.  It is used to calculate angles in the model.
*/
double sinhKleinLength(head, tail)
double *head;        /* The coordinates of the endpoints of the edge */
double *tail;
{
  double num,        /* Squared numerator and denominator of */
         den,        /* returned value */
         temp;       /* temp storage variable */
  int    i, j;       /* loop iterators */

  num = 0;
  for(i=0;i<web.sdim;i++)
    num += SQR(head[i] - tail[i]);
  for(i=0;i<web.sdim;i++)
    for(j=1;j<(web.sdim - i);j++)
      num -= SQR(head[i]*tail[(i+j)%web.sdim] -
		 head[(i+j)%web.sdim]*tail[i]);

  den  = 1;
  temp = 1;
  for(i=0;i<web.sdim;i++) {
    den  -= SQR(head[i]);
    temp -= SQR(tail[i]);
  }

  if((den *= temp)>0)
    return(sqrt(num/den));
  else
    return(0);
}

/***********************************************************************
* This function finds the length of an edge in the klein model of
* hyperbolic space.
*/
double klein_length(head, tail)
double *head;        /* coordinates of edge */
double *tail;
{
  return(acosh(coshKleinLength(head, tail)));
}

/***********************************************************************
* This function calculates the value of the gradient at the point head
* for the coordinate coord.
*/
double calc_grad(head, tail, coord)
double *head;        /* head is point gradient is taken */
double *tail;        /* tail is direction gradient is pointing */
int    coord;        /* coordinate of partial that is being taken */
{
  double temp1,      /* temporary storage variables for common expressions */
         temp2,
         num,        /* numerator and denominator of returned number */
         den;
  int i,j,k;         /* loop iterators */

  temp1 = head[coord] - tail[coord];
  num = temp1;
  den = SQR(temp1);
  for(i=1;i<web.sdim;i++) { j=((coord+i)%web.sdim);
    temp2 = tail[coord] * head[j] -
      tail[j]*head[coord];
    num += head[j]*temp2;
    den += SQR((tail[j] - head[j])) - SQR(temp2);
  }

  for(i=1;i<web.sdim;i++) {
    k = (coord+i)%web.sdim;
    for(j=1;j<(web.sdim - i);j++)
      den += SQR(tail[k]*head[(k+j)%web.sdim] -
		 tail[(k+j)%web.sdim]*head[k]);
  }

  temp1 = 1;
  for(i=0;i<web.sdim;i++)
    temp1 -= SQR(head[i]);

  if((den>0)&(temp1>0))
    return(num/(sqrt(den)*temp1));
  else
    return(1000.0);
}

/**********************************************************************
* This function finds the contribution to the gradient of the edge with
* endpoints tail and head at those endpoints.
*/
void klein_length_grad(head, tail, head_grad, tail_grad)
double *head, *tail;            /* coordinates of edge */
double *head_grad, *tail_grad;  /* gradients of head and tail */
{
  int i;
  double aa,bb,ab,den;
  aa = 1-dot(head,head,web.sdim);
  bb = 1 - dot(tail,tail,web.sdim);
  ab = 1-dot(head,tail,web.sdim);
  den = sqrt(ab*ab - aa*bb);
  for ( i = 0 ; i < web.sdim ; i++ )
    { head_grad[i] -= (ab*head[i]/aa- tail[i])/den;
      tail_grad[i] -= (ab*tail[i]/bb - head[i])/den;
    }
/*
  for(i=0;i<web.sdim;i++) {
    head_grad[i] -= calc_grad(head, tail, i);
    tail_grad[i] -= calc_grad(tail, head, i);
  }
*/
}

/**********************************************************************
* This function finds the angle at vertex between the lines made by
* vertex and pt1 and vertex and pt2 in the klein model of hyperbolic
* space. Its calculation is based on the hyperbolic law of cosines.
*/
double angle_klein(vertex, pt1, pt2)
double *vertex,
       *pt1,
       *pt2;
{
  double num,         /* numerator and denominator of returned value */
         den;

  num = coshKleinLength(vertex, pt1) * coshKleinLength(vertex, pt2) -
    coshKleinLength(pt1, pt2);

  den = sinhKleinLength(vertex, pt1) * sinhKleinLength(vertex, pt2);

  if(den!=0)
    return(acos(num/den));
  else
    return(0.0);
}

/***********************************************************************
* This function finds the area of a triangle in the klein model of the
* hyperbolic plane by using the formula:
*    AREA = PI - angle1 - angle2 - angle3
*/
double klein_area(triangle)
double **triangle;
{
  double area;          /* returned area */
  int i;                /* loop iterator */

  area = PI;
  for(i=0;i<3;i++)
    area -= angle_klein(triangle[i], triangle[(i+1)%3],
			   triangle[(i+2)%3]);

  return(area);
}

/**************************************************************************
* This function calculates the area gradient at the vertices of a triangle.
*/
void klein_area_grad(triangle, triangle_grad)
double **triangle;         /* the face */
double **triangle_grad;    /* the gradients */
{
  double grad,             /* returned value */
         temp;             /* temporary storage of gradient parts */

  int    vertex,           /* loop iterator */
         coord,
         pt1,              /* the triangle vertices who's gradient is not */
         pt2;              /* being taken */

  double coshs[3],           /* the coshs of the sides */
         sinhs[3],           /* the sinhs of the sides */
         angles[3];          /* the angles of the triangle */
  
  MAT2D(grads,6,MAXCOORD);     /* dimensionx6 matrix of length gradients */

  coshs = vector(0, 3);
  sinhs = vector(0, 3);
  angles = vector(0,3);
  grads = dmatrix(0, 6, 0, web.sdim);

  for(vertex=0;vertex<3;vertex++) {
    coshs[vertex] = coshKleinLength(triangle[vertex], triangle[(vertex+1)%3]);
    sinhs[vertex] = sinhKleinLength(triangle[vertex], triangle[(vertex+1)%3]);
    angles[vertex] = angle_klein(triangle[vertex], triangle[(vertex+1)%3],
			    triangle[(vertex+2)%3]);
    klein_length_grad(triangle[vertex], triangle[(vertex+1)%3], grads[vertex],
		      grads[((vertex+1)%3)+3]);
  }

  for(vertex=0;vertex<3;vertex++) {
    pt1 = (vertex+1)%3;
    pt2 = (vertex+2)%3;
    for(coord=0;coord<web.sdim;coord++) {
      grad = -grads[vertex+3][coord]*sinhs[vertex]*
	(coshs[pt1]*coshs[pt2] - coshs[vertex]) -
	  grads[vertex][coord]*sinhs[pt2]*(coshs[vertex]*coshs[pt1] -
					   coshs[pt2]);
      grad /= SQR(sinhs[vertex]*sinhs[pt1])*sin(angles[vertex]);

      temp = -grads[vertex][coord]*sinhs[pt1]*
	(coshs[pt2]*coshs[vertex] - coshs[pt1]) +
	grads[vertex+3][coord]*sinhs[vertex]*sinhs[pt1]*sinhs[pt2];
      temp /= SQR(sinhs[vertex]*sinhs[pt1])*sin(angles[pt1]);

      grad += temp;
      
      temp = -grads[vertex+3][coord]*sinhs[pt2]*
	(coshs[vertex]*coshs[pt1] - coshs[pt2]) +
	grads[vertex][coord]*sinhs[vertex]*sinhs[pt1]*sinhs[pt2];
      temp /= SQR(sinhs[pt2]*sinhs[pt1])*sin(angles[pt2]);

      grad += temp;
      
      triangle_grad[vertex][coord] -= grad;
    }
  }

}

