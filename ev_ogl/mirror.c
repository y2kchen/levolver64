/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Ken Brakke, brakke@geom.umn.edu              *
*************************************************************/

/*************************************************************
*
*     file:     mirror.c
*
*     purpose: implement mirror symmetry group on  y = 0
*/

#include "include.h"

char *symmetry_name = "mirror";  /* name appearing in datafile. lowercase! */

/*
       Group elements are encoded as one parity bit.
*/


/*******************************************************************
*
*  function: group_wrap
*
*  purpose:  Provide adjusted coordinates for vertices that get
*            wrapped around torus.  Serves as example for user-written
*            symmetry function.
*
*/

void group_wrap(x,y,wrap)
REAL *x;   /* original coordinates */
REAL *y;   /* wrapped coordinates  */
WRAPTYPE wrap;  /* encoded symmetry group element */
{
  int i;        /* which lattice vector */
  int j;        /* space dimension */
  int n;        /* number times wraps around */

  /* copy original coordinates */
  memcpy((char*)y,(char*)x,DIMENSION*sizeof(REAL));

  if ( wrap ) y[1] = -y[1];
}

/********************************************************************
*
*  function: group_compose()
*
*  purpose:  do composition of two group elements
*
*/

WRAPTYPE group_compose(wrap1,wrap2)
WRAPTYPE wrap1,wrap2;  /* the elements to compose */
{
  return (wrap1 + wrap2) & 1;
}


/********************************************************************
*
*  function: group_inverse()
*
*  purpose:  return inverse of group element.
*
*/

WRAPTYPE group_inverse(wrap)
WRAPTYPE wrap;  /* the element to invert */
{
  return wrap;
}
  
/*******************************************************************
*
*  function: group_form_pullback
*
*  purpose:  Pull back differential forms at vertices that get
*            wrapped around torus.  Serves as example for user-written
*            symmetry function.
*
*/

void group_form_pullback(x,xform,yform,wrap)
REAL *x;   /* original coordinates */
REAL *xform;  /* result pullback */
REAL *yform;   /* original form input  */
WRAPTYPE wrap;  /* encoded symmetry group element */
{
  int i;

  for ( i = 0 ; i < web.sdim ; i++ )
    xform[i] = yform[i];

  if ( wrap ) yform[1] = -yform[1];
}

