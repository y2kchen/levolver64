

/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Ken Brakke, brakke@geom.umn.edu              *
*************************************************************/

/*********************************************************************
*
*    file:      simpequi.c
*
*    Contents:  Equiangulation for simplex model.
*               Propagating Delaunay triangulation on bare vertices.
*/

#include "include.h"

/*********************************************************************
*
*  function: simplex_equiangulate()
*
*  purpose: equiangulation for simplex representation.
*
*/

void simplex_equiangulate()
{
  vertex_id v_id;
  vertex_id face[MAXCOORD+1],*v;
  int dim;
  int i,j,k;
  int count = 0;
  REAL temperature;
  facet_id f_id;
  edge_id e_id;
   REAL **side,**kvector;
   REAL *x[MAXCOORD];
   int kcomp = binom_coeff(SDIM,web.dimension);
   vertex_id tempv;

  /* use one old simplex to establish orientation of starter new simplex */
  side = dmatrix(0,web.dimension-1,0,SDIM-1);
  kvector = dmatrix(0,1,0,kcomp-1);
  f_id = web.skel[FACET].used;   /* get one old simplex */
  v = get_facet_vertices(f_id);
  x[0] = get_coord(v[0]);
  for ( k = 1 ; k <= web.dimension ; k++ )
     {  x[k] = get_coord(v[k]);
        for ( j = 0 ; j < SDIM ; j++ )
          side[k-1][j] = x[k][j] - x[0][j];
     }
  exterior_product(side,kvector[0],web.dimension,SDIM);

  /* get initial face */
  face[0] = v[0]; /* get a starter vertex */
  for ( dim = 0 ; dim < web.dimension ; dim++ )
   { v_id = find_other_vertex(face,dim,NULLID);
     if ( !valid_id(v_id) )
       { 
         sprintf(errmsg,"Cannot find simplex-completing vertex for\n");
         for ( j = 0 ; j <= dim ; j++ ) 
           sprintf(errmsg+strlen(errmsg)," %d",ordinal(face[j])+1);
         kb_error(1619,errmsg,RECOVERABLE);
       }
     face[dim+1] = v_id;
   }
  x[0] = get_coord(face[0]);
  for ( k = 1 ; k <= web.dimension ; k++ )
     {  x[k] = get_coord(face[k]);
        for ( j = 0 ; j < SDIM ; j++ )
          side[k-1][j] = x[k][j] - x[0][j];
     }
  exterior_product(side,kvector[1],web.dimension,SDIM);
  if ( dot(kvector[0],kvector[1],kcomp) < 0.0 )
    { /* need to flip orientation */
      v_id = face[0]; face[0] = face[1]; face[1] = v_id;
    }
  free_matrix(side);
  free_matrix(kvector);

  /* get rid of old */
  FOR_ALL_FACETS(f_id)
    free_element(f_id);
  FOR_ALL_EDGES(e_id)
    free_element(e_id);

  /* jiggle to remove degeneracy */
  save_coords();
  temperature = 1e-5;
  FOR_ALL_VERTICES(v_id)
    { REAL *x;
      x = get_coord(v_id);
      for ( i = 0 ; i < SDIM ; i++ )
        x[i] += gaussian()*temperature*web.max_len*overall_size;
    }

  /* start face stack */
  init_face_stack(web.dimension-1);
  dim = web.dimension-1;
  v_id = face[dim+1];
  for ( i = 0 ; i <= dim+1 ; i++ )
    { if ( i == dim+1 )  /* for proper orientation */ 
        { tempv = face[1]; face[1] = face[0]; face[0] = tempv; }
      face[dim+1] = face[i]; face[i] = v_id; 
      push_face(face);
      face[i] = face[dim+1];
    }
  tempv = face[1]; face[1] = face[0]; face[0] = tempv;  /* fix order */

  /* add to list */
  f_id = new_facet();
  v = get_facet_vertices(f_id);
  for ( i = 0 ; i <= web.dimension ; i++ ) v[i] = face[i];
  count++;

  /* go through and complete each face on stack */
  while ( pop_face(face) )
   { v_id = find_other_vertex(face,dim,face[dim+1]);
     if ( !valid_id(v_id) )
       { 
         sprintf(errmsg,"Cannot find simplex-completing vertex for\n");
         for ( j = 0 ; j <= dim ; j++ ) 
           sprintf(errmsg+strlen(errmsg)," %d",ordinal(face[j])+1);
         kb_error(1620,errmsg,RECOVERABLE);
       }
     for ( i = 0 ; i <= dim ; i++ )
       { face[dim+1] = face[i];
         face[i] = v_id;
         push_face(face);
         face[i] = face[dim+1];
       }

     /* add to list */
     f_id = new_facet();
     v = get_facet_vertices(f_id);
     for ( i = 0 ; i < web.dimension ; i++ ) v[i] = face[i];
sprintf(msg,"simplex: %d %d %d %d\n",ordinal(v[0])+1,ordinal(v[1])+1,
ordinal(v[2])+1,ordinal(v[3])+1);
outstring(msg);
     v[web.dimension] = v_id;
     count++;
   }
 end_face_stack();
 sprintf(msg,"Rebuilt simplices: %d\n",count);
    outstring(msg);

 simplex_delauney_test();

 restore_coords();  /* undo jiggling */
}
     
static vertex_id *face_stack;
static int face_stack_size;
static int face_dim;
static int face_stack_max;

void init_face_stack(dim)
int dim;  /* dimension of faces */
{
  face_stack_max = web.skel[VERTEX].count*2*(dim+2);
  face_stack = (vertex_id *)temp_calloc(face_stack_max,sizeof(vertex_id)); 
  face_stack_size = 0;
  face_dim = dim;
}

void end_face_stack()
{
  temp_free((char *)face_stack);
}

int pop_face(face)
vertex_id *face;  /* to copy vertex list for face into */
{ int i;
  if ( face_stack_size <= 0 ) return 0;
  memcpy((char*)face,(char*)(face_stack + face_stack_size - (face_dim+2)),
     (face_dim+2)*sizeof(vertex_id));
  if ( valid_id(face[face_dim+1]) )
     face_stack_size -= face_dim+2;  /* pop if already done one side */

#ifdef UBUG
sprintf(msg,"pop %d:",face_stack_size); 
for ( i = 0 ; i <= face_dim+1 ; i++ ) 
  sprintf(msg+strlen(msg)," %d",ordinal(face[i])+1);
strcat(msg,"\n");
outstring(msg);
#endif

}

void push_face(face)
vertex_id *face;  /* face to put on list if not already there */
{ int i,j;
  vertex_id v_id,vsort[MAXCOORD+1],*v;
  conmap_t conmap[MAXCONPER];
  conmap_t *vconmap;
  int swaps;

  /* see if all on same constraint */
  vconmap = get_v_constraint_map(face[0]);
  for ( j = 0,i=0 ; j <= vconmap[0] ; j++ )
   { if ( vconmap[j] & CON_HIT_BIT ) conmap[i++] = vconmap[j];
   }
  conmap[0] = i;
  for ( i = 1 ; i <= face_dim ; i++ )
  { vconmap = get_v_constraint_map(face[i]);
    for ( j = 1 ; j <= conmap[0] ; j++ )
    { for ( k = 1 ; k <= vconmap[0] ; k++ )
        if ( conmap[j] == vconmap[k] ) break;
      if ( k > vconmap[0] ) { conmap[j] = conmap[j+1]; conmap[0]--; j-- }
    }
  }
  if ( conmap[0] && valid_id(face[face_dim+1]) )  /* outer edge */
    { /* add to face list */
      edge_id e_id = new_edge(NULLID,NULLID,NULLID);
      v = get_edge_vertices(e_id);
      for ( j = 0 ; j <= face_dim ; j++ )
        v[j] = face[j];
      set_attr(e_id,CONSTRAINT);
      set_attr(e_id,BDRY_ENERGY);  /* maybe not, but won't hurt */
      set_attr(e_id,BDRY_CONTENT);  /* maybe not, but won't hurt */
      set_e_conmap(e_id,conmap);
      return; /* don't add to face list */
    }

  /* sort face vertices for easy comparison */
  /* insertion sort */
  for ( i = 0, swaps = 0 ; i <= face_dim ; i++ )
    { for ( j = i ; j > 0 ; j-- )
        if ( ordinal(vsort[j-1]) < ordinal(face[i]) ) break;
        else { vsort[j] = vsort[j-1]; swaps++; }
      vsort[j] = face[i];
    }
  if ( !(swaps & 1) ) /* force odd swaps */
    { v_id = vsort[0]; vsort[0] = vsort[1]; vsort[1] = v_id; }
  vsort[face_dim+1] = face[face_dim+1];

#ifdef UBUG
fprintf(outfd,"push %d ",face_stack_size); 
for ( i = 0 ; i <= face_dim+1 ; i++ ) fprintf(outfd," %d",ordinal(vsort[i])+1);
fprintf(outfd,"\n");
#endif

  /* see if already have */
  /* crude linear search */
  for ( i = 0 ; i < face_stack_size ; i += face_dim+2 )
    { for ( j = 0 ; j <= face_dim ; j++ )
        if ( !equal_id(face_stack[i+j],vsort[j]) ) break;
      if ( j > face_dim ) /* found */
        break;
    }
  if ( i < face_stack_size ) /* found */
   { 
#ifdef UBUG
fprintf(outfd,"found\n");
#endif
     if ( valid_id(face_stack[i+face_dim+1]) )
       { /* now have both sides, so drop from list */
         face_stack_size -= face_dim+2;
         if ( face_stack_size > i ) /* move top face into empty slot */
           memcpy((char*)(face_stack+i),(char*)(face_stack + face_stack_size),
                       (face_dim+2)*sizeof(vertex_id));
       }
     else /* have just first simplex */
       face_stack[i+face_dim+1] = face[face_dim+1];
   }
  else /* not found, add to stack */
   {
#ifdef UBUG
fprintf(outfd,"not found\n");
#endif
     if ( face_stack_size >= face_stack_max )
       kb_error(1621,"Internal error: Face stack overflow.\n",RECOVERABLE);

     v_id = vsort[0]; vsort[0] = vsort[1]; vsort[1] = v_id; /* even swap */
     memcpy((char*)(face_stack + face_stack_size),(char*)vsort,
                                 (face_dim+2)*sizeof(vertex_id)); 
     face_stack_size += face_dim+2; 
    } 
}

vertex_id void_test(v,dim)
vertex_id *v;
int dim;
{
   REAL *x[MAXCOORD+1];
   REAL ss[MAXCOORD];  /* squares of sides */
   int k,j;
   REAL rr;  /* square radius of void */
   REAL center[MAXCOORD];
   REAL lam[MAXCOORD];
   vertex_id v_id,bad_v = NULLID;
  REAL **mat = dmatrix(0,web.dimension-1,0,web.dimension-1);
  REAL **side = dmatrix(0,SDIM-1,0,SDIM-1);

   /* first, calculate center of void */
   x[0] = get_coord(v[0]);
   for ( k = 1 ; k <= dim ; k++ )
     {  x[k] = get_coord(v[k]);
        for ( j = 0 ; j < SDIM ; j++ )
          side[k-1][j] = x[k][j] - x[0][j];
     }
   for ( k = 0 ; k < dim ; k++ )
     { 
       for ( j = 0 ; j <= k ; j++ )
         mat[j][k] = mat[k][j] = SDIM_dot(side[j],side[k]);
       ss[k] = mat[k][k];
     }
   mat_inv(mat,dim);
   matvec_mul(mat,ss,lam,dim,dim);
   rr = dot(lam,ss,dim)/4;
   vec_mat_mul(lam,side,center,dim,SDIM);
   for ( k = 0 ; k < SDIM ; k++ )
     center[k] = x[0][k] + center[k]/2;

   /* now see if any other vertices are in the void */
   FOR_ALL_VERTICES(v_id)
     { REAL *y;
       REAL z[MAXCOORD];

       for ( k = 0 ; k <= dim ; k++ )
         if ( equal_id(v_id,v[k]) ) break;
       if ( k <= dim ) continue;  /* skip vertices in facet */

       y = get_coord(v_id);
       for ( j = 0 ; j < SDIM ; j++ )
         z[j] = y[j] - center[j];
       if ( SDIM_dot(z,z) >= rr - 1e-10 ) continue;
#if 1      
sprintf(msg,"Void violation by %g\n",-SDIM_dot(z,z) + rr);
    outstring(msg);
#endif
       bad_v = v_id;
       break;
     }
 free_matrix(side);
 free_matrix(mat);
 return bad_v;
}

void simplex_delauney_test()
{
  facet_id f_id;
  
  FOR_ALL_FACETS(f_id)
  {
   vertex_id v_id, *v = get_facet_vertices(f_id);
   v_id = void_test(v,web.dimension);
   if ( valid_id(v_id) )
     { sprintf(msg,"Vertex %d inside void of facet %d\n",ordinal(v_id)+1,
           ordinal(f_id)+1);
       outstring(msg);
     }
  }

}

vertex_id find_other_vertex(v,dim,otherv)
vertex_id *v;  /* vertices of face */
int dim;  /* dimension of face */
vertex_id otherv; /* known vertex for other simplex on face */
{
   REAL *x[MAXCOORD+1];
   int k,j,i;
   REAL RRv;  /* square radius of void */
   REAL minRR;
   REAL rr;  /* square radius of face void */
   REAL center[MAXCOORD];
   REAL lam[MAXCOORD];
   REAL **qform,**temp,**side,**mat;
   REAL ss[MAXCOORD];  /* squares of sides */
   REAL *otherx;
   vertex_id v_id;
#define MAXKEEPERS 15
   struct keeper {
     vertex_id v;  /* completers of face */
     REAL z[MAXCOORD];
     REAL center[MAXCOORD];
     REAL RR;
   } keeper[MAXKEEPERS];
   int  keepers = 0;
   vertex_id best_v;

/*
   qform = dmatrix(0,SDIM-1,0,SDIM-1);
   temp = dmatrix(0,SDIM-1,0,SDIM-1);
   side = dmatrix(0,dim-1,0,SDIM-1);
   mat = dmatrix(0,dim-1,0,dim-1);
*/
   qform = dmatrix(0,MAXCOORD,0,MAXCOORD);
   temp  = dmatrix(0,MAXCOORD,0,MAXCOORD);
   side  = dmatrix(0,MAXCOORD,0,MAXCOORD);
   mat   = dmatrix(0,MAXCOORD,0,MAXCOORD);

   /* first, calculate center of void */
   x[0] = get_coord(v[0]);
   for ( k = 1 ; k <= dim ; k++ )
     {  x[k] = get_coord(v[k]);
        for ( j = 0 ; j < SDIM ; j++ )
          side[k-1][j] = x[k][j] - x[0][j];
     }
   for ( k = 0 ; k < dim ; k++ )
     { 
       for ( j = 0 ; j <= k ; j++ )
         mat[j][k] = mat[k][j] = SDIM_dot(side[j],side[k]);
       ss[k] = mat[k][k];
     }
   mat_inv(mat,dim);
   matvec_mul(mat,ss,lam,dim,dim);
   rr = dot(lam,ss,dim)/4;
   vec_mat_mul(lam,side,center,dim,SDIM);
   for ( j = 0 ; j < SDIM ; j++ )
      center[j] /= 2.0;

    /* quadratic form for evaluating b^2 */
   mat_mult(mat,side,temp,dim,dim,SDIM);
   tr_mat_mul(side,temp,qform,dim,SDIM,SDIM);
   for ( j = 0 ; j < SDIM ; j++ )
     for ( k = 0 ; k < SDIM ; k++ )
       { qform[j][k] = -qform[j][k];
         if ( j == k ) qform[j][k] += 1.0;
       }        

   /* go thru all other vertices */
   if ( valid_id(otherv) ) 
     { otherx = get_coord(otherv);
       keeper[keepers].v = otherv;
       for ( j = 0 ; j < SDIM ; j++ )
         keeper[keepers].z[j] = otherx[j] - x[0][j];
       keepers++;
     }
   FOR_ALL_VERTICES(v_id)
    { REAL *y,z[MAXCOORD],aa,bb,b,c,cvec[MAXCOORD],bvec[MAXCOORD];
      REAL vcenter[MAXCOORD];
      REAL zc[MAXCOORD];

      /* skip those in current face */
      for ( j = 0 ; j <= dim ; j++ )
        if ( equal_id(v_id,v[j]) ) break;
      if ( j <= dim ) continue;
      if ( equal_id(v_id,otherv) ) continue;

      y = get_coord(v_id);
      for ( j = 0 ; j < SDIM ; j++ )
        z[j] = y[j] - x[0][j];  /* relative to v[0] */
      matvec_mul(qform,z,bvec,SDIM,SDIM);
      bb = SDIM_dot(z,bvec);
      if ( fabs(bb) < 1E-14 ) continue;
      for ( j = 0 ; j < SDIM ; j++ )
        zc[j] = z[j] - center[j];
      aa = SDIM_dot(zc,zc);
      RRv = rr + (aa - rr)*(aa - rr)/4/bb;
      c = (aa - rr)/2/bb;
      for ( j = 0 ; j < SDIM ; j++ )
        vcenter[j] = center[j] + c*bvec[j];

      if ( RRv < 0.0 ) continue; /* really infinite */
      /* see if new void contains any previous keepers */
      for ( i = 0 ; i < keepers ; i++ )
        { REAL dd;
          for ( j = 0 ; j < SDIM ; j++ )
            cvec[j] = keeper[i].z[j] - vcenter[j];
          dd = SDIM_dot(cvec,cvec);
          if ( dd < RRv ) goto cont;
        }
      /* see if any previous keepers displaced */
      for ( i = (valid_id(otherv)?1:0) ; i < keepers ; i++ )
        { REAL dd;
          for ( j = 0 ; j < SDIM ; j++ )
            cvec[j] = keeper[i].center[j] - z[j];
          dd = SDIM_dot(cvec,cvec);
          if ( dd < keeper[i].RR ) 
            keeper[i] = keeper[--keepers]; /* pop */
        }
      /* add to keeper list */
      if ( keepers >= MAXKEEPERS )
        { kb_error(1622,"Too many keepers.\n",WARNING); continue; }

      keeper[keepers].v = v_id;
      keeper[keepers].RR = RRv;
      for ( j = 0 ; j < SDIM ; j++ )
        { keeper[keepers].z[j] = z[j];
          keeper[keepers].center[j] = vcenter[j];
        }
      keepers++;

cont: ;
    }
  free_matrix(mat);
  free_matrix(temp);
  free_matrix(qform);
  free_matrix(side);

  /* recheck all voids */
 {
  vertex_id vv[MAXCOORD+1];
  best_v = NULLID;     
  for ( i = 0 ; i <= dim ; i++ ) vv[i] = v[i];
  for ( i =  (valid_id(otherv)?1:0) ; i < keepers ; i++ )
   { vv[dim+1] = keeper[i].v;
     v_id = void_test(vv,dim+1);
     if ( valid_id(v_id) )
       { keeper[i] = keeper[--keepers]; i--; }  /* pop and retest */
   }
  }
  for ( i =  (valid_id(otherv)?1:0), minRR = 1e30 ; i < keepers ; i++ )
   { if ( keeper[i].RR < minRR )
       { minRR = keeper[i].RR;
         best_v = keeper[i].v; 
       } /* keep smallest void */
   }

#ifdef UBUG
fprintf(outfd,"search "); 
for ( i = 0 ; i <= dim ; i++ ) fprintf(outfd," %d",ordinal(v[i])+1);

if ( valid_id(best_v) )
  sprintf(msg,"  found %d\n",ordinal(best_v)+1);
else sprintf(msg,"  vertex not found.\n");
    outstring(msg);
#endif

  return best_v;
}


