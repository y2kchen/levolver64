#define REAL double
extern double **dmatrix();

/* dot product */
REAL dot(a,b,n)
REAL *a,*b;
int n;  /* number of items */
{
  REAL x = 0.0;
  for (  ; --n >= 0 ;  ) x += (*(a++))*(*(b++));
  return x;
}

/* matrix times vector multiplication */
void matvec_mul(a,b,c,rows,cols)
REAL **a,*b,*c;
{
  int i,j;

  for ( i = 0 ; i < rows ; i++ )
     { c[i] = 0.0;
        for ( j = 0 ; j < cols ; j++ )
          c[i] += a[i][j]*b[j];
     }
}

/* vector times matrix multiplication */
void vecmat_mul(a,b,c,rows,cols)
REAL *a,**b,*c;
int rows,cols;
{
  int i,j;

  for ( i = 0 ; i < cols ; i++ )
     { c[i] = 0.0;
        for ( j = 0 ; j < rows ; j++ )
          c[i] += a[j]*b[j][i];
     }
}

/* matrix by matrix multiplication */
/* output: c = a*b */
void mat_mult(a,b,c,imax,jmax,kmax)
REAL **a,**b,**c;  /* not assumed distinct */
{
  REAL **temp;  /* temporary storage, if needed */
  int i,j,k;

  if ( (a == c) || (b == c) )
     {
        temp = dmatrix(0,imax-1,0,jmax-1);  /* temporary storage */
        for ( i = 0 ; i < imax ; i++ )
          for ( j = 0 ; j < jmax ; j++ )
             for ( k = 0 ; k < kmax ; k++ )
                temp[i][k] += a[i][j]*b[j][k];
        matcopy(c,temp,imax,kmax);
        free_matrix(temp);
     }
  else
     {
        for ( i = 0 ; i < imax ; i++ )
          for ( k = 0 ; k < kmax ; k++ )
             {
                c[i][k] = 0.0;
                for ( j = 0 ; j < jmax ; j++ )
                  c[i][k] += a[i][j]*b[j][k];
             }
     }
}

/* quadratic form evaluation */
double quadratic_form(a,b,c,n)
REAL *a,**b,*c;
{ int i,j;
  double sum = 0.0;
  double temp;

  for ( i = 0 ; i < n ; i++ )
     { temp = b[i][0]*c[0];
        for ( j = 1 ; j < n ; j++ )
          temp += b[i][j]*c[j];
        sum += a[i]*temp;
     }

  return sum;
}

