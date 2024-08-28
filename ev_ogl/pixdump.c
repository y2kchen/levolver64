/**********************************************************************
*
*  File: pixdump.c
*
*  Purpose: Creates data file for Pixar from current triangulation.
*
*/

#include "include.h"

/* Needed Pixar defines. */
#define QUAD 1
#define NQUAD 2
#define CQUAD 3
#define CNQUAD 4


/*******************************************************************************
*
*  Function: pixdump()
*
*  purpose:  Create data and command files for Pixar display with chreyes
*
*  Input:    Prompts user for a picture name.
*            Prompts user for normal interpolation on/off.
*            Asks if user wants surfaces of different bodies
*               colored according to a colormap file.
*
*  Output:   name.go - command file
*            name.quad - quadrilateral data in ASCII format
*            name.quad.fbi - quadrilateral data in internal format
*
*  Return value: number of quadrilaterals
*/

int pixdump()
{
  FILE *pfd = NULL;
  char name[50];       /* base picture name  */
  char quadname[60];   /* quadrilateral file */
  char goname[60];     /* command file name  */
  int i,j;
  facetedge_id fe;
  facet_id f_id;
  FILE *mfd;
  REAL    norm[3];
  int  quadcount = 0;  /* number of facets done */
  float *quadbase;     /* array of coordinates */
  char response[120];
  /* maprow *colormap; */
  REAL map[4];  /* color of this vertex */
  body_id b0_id,b1_id;
  char *gotype;
  int perquad;
  FILE *mapfd;
  int b;
  REAL *c;
  int qtype;
  REAL **verts; /* for adjusted triangle vertices */

  printf("Enter name of picture: ");
  gets(name);
  printf("Do normal interpolation? ");
  gets(response); 
  if ( toupper(response[0]) == 'Y' ) normflag = 1;
    else normflag = 0;
  printf("Do inner, outer, or all surfaces? (i,o,a)");
  gets(response);
  switch ( response[0] )
    {
      case 'i' : innerflag = 1; outerflag = 0; break;
      case 'o' : innerflag = 0; outerflag = 1; break;
      default  : innerflag = 1; outerflag = 1; break;
    }
  printf("Do body colors? ");
  gets(response);
  if ( toupper(response[0]) == 'Y' )
    {  colorflag = 1;
       printf("Enter name of colormap file: ");
       gets(response);
      if ( cmapname[0] == 0 )
      { outstring("No colormap used.\n"); colorflag = 0; }
      else
      {
       mapfd = path_open(response);
       if ( mapfd )
        {
         colormap = (maprow *)temp_calloc(4*web.bodycount,sizeof(REAL));
         for ( b = 0 ; b < web.bodycount ; b++ )
          { c = colormap[b];
            if ( fscanf(mapfd,"%lf %lf %lf %lf",c,c+1,c+2,c+3) != 4 ) break;
          }
         if ( b < web.bodycount ) 
           { sprintf(errmsg,"Colormap file has only %d entries for %d bodies.\n",b,web.bodycount);
             error(errmsg,WARNING);
           }
        }
       }
   }
  else colorflag = 0;

  /* write quadrilateral data */
  strcpy(quadname,name);
  strcat(quadname,".quad");
  pfd = fopen(quadname,"w"); 
  if (pfd == NULL) { perror(quadname); return 0; } 

  if ( normflag ) 
     if ( colorflag )
       { fprintf(pfd,"CNPOLY");
         qtype = CNQUAD;
         gotype = "cnpoly";
         perquad = (3+3+4)*4;
       }
     else
       { fprintf(pfd,"NPOLY");
         qtype = NQUAD;
         gotype = "npoly";
         perquad = (3+3)*4;
       }
  else if ( colorflag )
       { fprintf(pfd,"CPOLY");
         qtype = CQUAD;
         gotype = "cpoly";
         perquad = (3+4)*4;
       }
     else 
       { fprintf(pfd,"POLY");
         qtype = QUAD;
         gotype = "poly";
         perquad = (3)*4;
       }
  fprintf(pfd,"\n");

  quadbase = (float *)temp_calloc(web.skel[FACET].maxcount,
                                              perquad*sizeof(float));
  quadcount = 0;

  verts = dmatrix(0,2,0,2);
  FOR_ALL_FACETS(f_id)
    { 
      int nbrs;  /* number of neighboring bodies */

      if ( get_fattr(f_id) & NODISPLAY ) continue;
      fe = get_facet_fe(f_id);
      nbrs =  (valid_id(get_facet_body(f_id)) ? 1 : 0) 
                  + (valid_id(get_facet_body(facet_inverse(f_id))) ? 1 : 0);
      if ( (nbrs >= 2) && !innerflag ) continue;
      if ( (nbrs < 2) && !outerflag ) continue;
      if ( colorflag ) /* get vertex color */
        { 
          b0_id = get_facet_body(f_id);
          b1_id = get_facet_body(facet_inverse(f_id));
          if ( !valid_id(b0_id) && !valid_id(b1_id) ) 
             for ( i = 0 ; i < 4 ; i++ ) map[i] = 0.0;
          if ( valid_id(b0_id) && !valid_id(b1_id) ) 
             for ( i = 0 ; i < 4 ; i++ ) map[i] = colormap[loc_ordinal(b0_id)][i];
          if ( !valid_id(b0_id) && valid_id(b1_id) ) 
             for ( i = 0 ; i < 4 ; i++ ) map[i] = colormap[loc_ordinal(b1_id)][i];
          if ( valid_id(b0_id) && valid_id(b1_id) ) 
             for ( i = 0 ; i < 4 ; i++ ) 
               map[i] = (colormap[loc_ordinal(b1_id)][i]+colormap[loc_ordinal(b1_id)][i])/2;
        }
          
      get_facet_verts(f_id,verts,NULL);
      for ( i = 0 ; i < 3 ; i++ )
        { 
          if ( normflag ) calc_vertex_normal(fe,norm);
          fprintf(pfd,"%5.3f %5.3f %5.3f  ",verts[i][0],verts[i][1],verts[i][2]); 
          for ( j = 0 ; j < 3 ; j++ ) quadbase[quadcount++] = verts[i][j];
          if ( normflag )
            { fprintf(pfd,"%5.3f %5.3f %5.3f    ",norm[0],norm[1],norm[2]);
              for ( j = 0 ; j < 3 ; j++ ) quadbase[quadcount++] = norm[j];
            }
          if ( colorflag ) 
            { fprintf(pfd,"%5.3f %5.3f %5.3f %5.3f   ",map[0],map[1],map[2],map[3]);
              for ( j = 0 ; j < 4 ; j++ ) quadbase[quadcount++] = map[j];
            }
    
          fe = get_next_edge(fe);
        }
      fprintf(pfd,"%5.3f %5.3f %5.3f  ",verts[2][0],verts[2][1],verts[2][2]);
      for ( j = 0 ; j < 3 ; j++ ) quadbase[quadcount++] = verts[2][j];
      if ( normflag )
        { fprintf(pfd,"%5.3f %5.3f %5.3f    ",norm[0],norm[1],norm[2]);
          for ( j = 0 ; j < 3 ; j++ ) quadbase[quadcount++] = norm[j];
        }
      if ( colorflag ) 
        { fprintf(pfd,"%5.3f %5.3f %5.3f %5.3f   ",map[0],map[1],map[2],map[3]);
          for ( j = 0 ; j < 4 ; j++ ) quadbase[quadcount++] = map[j];
        }
      fprintf(pfd,"\n");
    }
  fclose(pfd);
  free_matrix(verts);

#ifdef XXXX
  /* write command file, copying from minim.go, changing data file name */
  mfd = fopen("/n/poincare/usr1/brakke/minim.go","r");
  if ( mfd == NULL ) { perror("minim.go"); return 0; }
  strcpy(goname,name);
  strcat(goname,".go");
  pfd = fopen(goname,"w");
  if (pfd == NULL) { perror(goname); return 0; }
  while ( fgets(line,100,mfd) )
    { 
      if ( normflag && (strncmp(line,"smooth",6) == 0) )
         sprintf(line,"smooth on");
      if ( strncmp(line,"gprim",5) == 0 )  /* geometry file */
          sprintf(line+13,"%s %s        \n",gotype,quadname);
      if ( strncmp(line,"modelx",6) == 0 ) /* viewing transform, transposed for Pixar */
        { char *c;
          for ( c = line + 12, i = 0 ; i < 3 ; i++ )
           for ( j = 0 ; j < 4 ; j++, c += 5 )
            sprintf(c,"%4.0f ",128*view[j][i]);
          for ( j = 0 ; j < 3 ; j++, c += 5 )
            sprintf(c,"%4.0f ",128*view[j][3]);
          sprintf(c,"%4.0f ",1.0);
          c += 5;
          *c = 0;
        }
      if ( colorflag && (strncmp(line,"color",5) == 0) )
             continue; /* omit global color */
      fputs(line,pfd);
    }
  fclose(pfd);
  /* pfd = fopen(quadname,"w");  erase quad file
  fclose(pfd); */
  fclose(mfd);
#endif

  /* write out data in internal .fbi format */
  strcat(quadname,".fbi");
   /*QuadtoFile(qtype,quadcount/perquad,quadbase,quadname);*/

  temp_free((char *)quadbase);
  if ( colorflag ) temp_free((char *)colormap);

  return  quadcount/perquad;
}


