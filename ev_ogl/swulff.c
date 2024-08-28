/*********************************************************************

	       string_wulff method

*********************************************************************/

/*********************************************************************
*
* function: string_wulff_value()
*
* purpose:  method value
*
*/

REAL string_wulff_value(e_info)
struct qinfo *e_info;
{ int m,k;
  double value = 0.0;
  REAL z[MAXCOORD];  /*  pointers to coord and tangent */
 
  for ( k = 0 ; k < web.sdim ; k++ ) z[web.sdim+k] = e_info->sides[0][k];
  for ( m = 0 ; m < web.gauss_order ; m++ )
  { for ( k = 0 ; k < web.sdim ; k++ ) z[k] = e_info->gauss_pt[m][k]; 
    value += gauss1Dwt[m]*eval(e_info->method->expr[0],z);
  }
  return value;
}

/*********************************************************************
*
* function: string_wulff_grad()
*
* purpose:  method gradient
*
*/


REAL string_wulff_grad(e_info)
struct qinfo *e_info;
{ int m,j,k;
  double value = 0.0;
  double val;
  REAL derivs[MAXCOORD];
  REAL z[MAXCOORD];  /*  pointers to coord and tangent */

  for ( j = 0 ; j < web.sdim ; j++ ) 
    for ( m = 0 ; m < 2 ; m++ )
      e_info->grad[m][j] = 0.0;

  for ( k = 0 ; k < web.sdim ; k++ ) z[web.sdim+k] = e_info->sides[0][k];
  for ( m = 0 ; m < web.gauss_order ; m++ )
  { for ( k = 0 ; k < web.sdim ; k++ ) z[k] = e_info->gauss_pt[m][k]; 
    eval_all(e_info->method->expr[0],z,web.sdim,&val,derivs);
    value += gauss1Dwt[m]*val;
    for ( j = 0 ; j < web.sdim ; j++ )
     { e_info->grad[0][j] += gauss1Dwt[m]*(gauss1Dpt[m]*derivs[j]
                              - derivs[j+web.sdim]);
       e_info->grad[1][j] += gauss1Dwt[m]*((1-gauss1Dpt[m])*derivs[j]
                              + derivs[j+web.sdim]);
     }
  }
  return value;
}
