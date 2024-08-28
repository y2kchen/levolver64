/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Ken Brakke, brakke@susqu.edu                 *
*************************************************************/

/******************************************************************
*
*  File:  mpi_main.c
*
*  Purpose:  Master program for MPI Evolver
*
*  Written for MPI version 1.10, in particular MPICH
*/

#include "mpi.h"
#include "include.h"

char *infotypes[] = { "ID_REQUEST","ID_RESPONSE","DONE_REQUESTS","EXIM_INFO"};

int mytid; /* self */
int alltids[1000];  /* task ids */
int typesize[10] = { 0,sizeof(REAL) };  /* type sizes for combiners */ 

long distrib_timestamp = -1;  /* topological structure distributed */
int did_update;  /* set if quant initializer does update */

int mpi_corona_state;  /* how much corona active */
#define NO_CORONA 0
#define THIN_CORONA 1
#define FAT_CORONA  2

struct mpi_command message;
int num_nodes;
int start,end;
int pernode[NUMELEMENTS];
int to_do_count[NUMELEMENTS];
int to_do_max[NUMELEMENTS];
element_id *to_do[NUMELEMENTS];
int old_main ARGS((int , char** ));
void  mpi_fetch_remote_elements ARGS((int));
void  mpi_fill_import_list ARGS((int,element_id*,int));
int mpi_node_main ARGS((void));
void mpi_synch_pressures ARGS((void));
void mpi_node_synch_pressures ARGS((void));
void mpi_synch_vertices ARGS((void));
void mpi_synch_edges ARGS((void));
void mpi_node_parallel_exec ARGS((int));
void mpi_node_node_exec ARGS((int));
void mpi_add_import ARGS((element_id));
void mpi_add_remote_element ARGS((struct element *));
void mpi_add_export ARGS((element_id,int));
void mpi_synch_volgrads ARGS((void));
void mpi_node_calc_volgrads ARGS((int));
void mpi_node_calc_content ARGS((int));
void mpi_node_calc_leftside ARGS((void));
void mpi_node_calc_rightside ARGS((int));
void mpi_node_lagrange_adjust ARGS((int));
void mpi_node_volume_restore ARGS((int,REAL,int));
void mpi_node_synch_coord ARGS((void));
void mpi_node_report ARGS((void));
void mpi_node_hessian_fill ARGS((int));
void mpi_node_hessian_move ARGS((REAL,int));
void mpi_synch_corona ARGS((void));
void mpi_node_synch_corona ARGS((void));
void mpi_dissolve_corona ARGS((void));
void mpi_node_synch_elements ARGS((void));
void mpi_node_dissolve_corona ARGS((void));
void mpi_node_vertex_average ARGS((int));

char *bigbuf;  /* big enough for any incoming message expected */ 
int bigbufsize;
void check_bigbufsize(size)
{ if ( bigbufsize < size )
  { 
    bigbufsize = size+1024;  /* room for a little growth */
    bigbuf = kb_realloc(bigbuf,bigbufsize);
  }
}


/* element attributes */
int mpi_export_voffset;
int mpi_export_eoffset;
int mpi_export_foffset;
int mpi_export_boffset;
int mpi_export_feoffset;

/* lists of pointers to imported elements local structures */
/* keeps full index list for each neighbor (should be only 2 or 3) */
struct remote_list_t remote_elements[NUMELEMENTS];

/***************************************************/
/* Keeping track of which tasks have what elements */

/* What this task has imported from others */
struct import_list_t {
     int count;  /* elements occupied */
     int max;    /* elements allocated */
     element_id *id_list;
     struct element *buf;  /* buffer space for import/export */
     int bufsize; /* in bytes */
};
struct import_list_t **import_elements;
/* indexed by other task as first index, element type as second */

/* What this task is exporting to others */
struct export_list_t {
     int count;  /* occupied */
     int max;    /* allocated */
     element_id *id_list;  /* exported elements */
     struct element *buf;  /* buffer space for import/export */
     int bufsize; /* in bytes */
};
struct export_list_t **export_elements;
/* indexed by other task as first index, element type as second */

void set_export_attr(id,task)
element_id id;
int task;
{ unsigned short *p = mpi_export_attr_ptr(id);
  int k;
  if ( id_machine(id) != this_machine )
	return; /* don't mark imported elements! */
  for ( k = 0 ; (p[k] != 0) && (k < MPI_EXPORT_MAX) ; k++ )
    if ( p[k] == task ) return;
  if ( k == MPI_EXPORT_MAX )
    kb_error(3333,"MPI export task exceeds MPI_EXPORT_MAX\n",RECOVERABLE);
  *p = task;
}

/***************************************************/


/***********************************************************************
* 
* function: main()
*
* purpose: Set up MPI, 
*          Master task calls Evolver main
*          Others listen for messages.
*/
int main(argc,argv)
int argc;
char *argv[];
{ 
  int retval;
  int type;
  int task;
  
  msgmax = 1000;
  msg = my_list_calloc(1,msgmax,ETERNAL_BLOCK);
  
  /* enroll in MPI */
  MPI_Init(&argc,&argv); 
  MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD,&mytid);
  this_machine = mytid;
  procs_requested = nprocs; 
  num_nodes = nprocs - 1;
  if ( nprocs >= (1 << MACHINE_ID_BITS) )
  {
    sprintf(errmsg,
     "Number of machines requested, %d, exceeds max representable in id, %d\n",
        nprocs,(1 << MACHINE_ID_BITS)-1 );

    kb_error(3108,errmsg,UNRECOVERABLE);
  }

sprintf(msg,"Rank %d task: pid %d\n",mytid,getpid());
outstring(msg);

  infos = (struct message_info *)calloc(nprocs*NUMELEMENTS,
                                        sizeof(struct message_info));
  /* allocate base remote_elements lists */
  for ( type = VERTEX ; type < NUMELEMENTS ; type++ )
  { remote_elements[type].count = (int *)calloc(nprocs,sizeof(int));
    remote_elements[type].max   = (int *)calloc(nprocs,sizeof(int));
    remote_elements[type].ibase = 
      (struct element ***)calloc(nprocs,sizeof(struct element **));
  }

  /* allocate import/export list structures */
  if ( !import_elements )
  { import_elements =
      (struct import_list_t**)calloc(sizeof(struct import_list_t*),nprocs);
    import_elements[0] =
      (struct import_list_t*)calloc(sizeof(struct import_list_t),
              nprocs*NUMELEMENTS);
    for ( task = 1 ; task < nprocs ; task++ )
      import_elements[task] = import_elements[0] + task*NUMELEMENTS;

    export_elements =
      (struct export_list_t**)calloc(sizeof(struct export_list_t*),nprocs);
    export_elements[0] =
      (struct export_list_t*)calloc(sizeof(struct export_list_t),
              nprocs*NUMELEMENTS);
    for ( task = 1 ; task < nprocs ; task++ )
      export_elements[task] = export_elements[0] + task*NUMELEMENTS;
  }

  if ( mytid == 0 )  /* master */
  {
    sprintf(msg,"Using %d nodes.\n",nprocs);
    outstring(msg);

    retval = old_main(argc,argv);
    message.cmd = mpi_QUIT;
    MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  }
  else /* listen for instructions from master */
  { 
    old_main(argc,argv);
    mpi_node_main();
  }

  MPI_Finalize();
  return 0;
}

void mpi_my_exit()
{ 
   message.cmd = mpi_QUIT;
   MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
   MPI_Finalize();
   exit(0);   
}

/**************************************************************************
*
* function: mpi_node_main()
*
* purpose: listen for messages from master
*/
int mpi_node_main()
{
  mpi_corona_state = NO_CORONA;

  /* event loop of commands from root */
  for (;;)
   { MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE,0,MPI_COMM_WORLD);
     if ( mpi_debug )
        printf("Task %d got command %d\n",this_machine,message.cmd);
     switch(message.cmd)
       {
         case mpi_DEBUG: mpi_debug = message.mode; break;
         case mpi_PAR_EXEC: mpi_node_parallel_exec(message.count); break;
         case mpi_NODE_EXEC: 
            if ( message.task == this_machine )
              mpi_node_node_exec(message.count);
             break;
         case mpi_CALC_ENERGY: mpi_node_calc_energy(); break;
         case mpi_CALC_FORCE: mpi_node_calc_force(); break;
         case mpi_MOVE_VERTICES: 
            mpi_node_move_vertices(message.mode,message.t); 
            break;
         case mpi_CONVERT_FORMS: 
            mpi_node_convert_forms_to_vectors(message.mode);
            break;
         case mpi_LAGRANGE_ADJUST: 
            mpi_node_lagrange_adjust(message.count);
            break;
         case mpi_SAVE_COORDS: mpi_node_save_coords(message.mode);
            break;
         case mpi_UNSAVE_COORDS: mpi_node_unsave_coords(message.mode);
            break;
         case mpi_RESTORE_COORDS: mpi_node_restore_coords(message.mode);
            break;
         case mpi_UPDATE_DISPLAY: 
            local_update_display();
            MPI_Barrier(MPI_COMM_WORLD); 
            break;
         case mpi_EDGE_REFINE: mpi_node_refine(); break;
         case mpi_CALC_CONTENT: mpi_node_calc_content(message.mode); break;
         case mpi_CALC_VOLGRADS:
            mpi_node_calc_volgrads(message.mode);
            break;
         case mpi_VGRAD_END: vgrad_end(); break;
         case mpi_CALC_LEFTSIDE: mpi_node_calc_leftside(); break;
         case mpi_CALC_RIGHTSIDE: mpi_node_calc_rightside(message.count); break;
         case mpi_VOLUME_RESTORE: 
            mpi_node_volume_restore(message.count,message.t,message.mode); 
            break;
         case mpi_TEMP_FREE_ALL: 
             temp_free_all();
             MPI_Barrier(MPI_COMM_WORLD); 
             break;
         case mpi_COUNT_REPORT: mpi_node_report(); break;
         case mpi_SYNCH_ELEMENTS: mpi_node_synch_elements(); break;
         case mpi_SYNCH_PRESSURES: mpi_node_synch_pressures(); break;
         case mpi_SYNCH_CORONA: mpi_node_synch_corona(); break;
         case mpi_DISSOLVE_CORONA: mpi_node_dissolve_corona(); break;
         case mpi_HESSIAN_FILL: mpi_node_hessian_fill(message.mode); break;
         case mpi_HESSIAN_MOVE: mpi_node_hessian_move(message.t,message.mode); 
               break;
         case mpi_VERTEX_AVERAGE: mpi_node_vertex_average(message.mode);
              break;
         case mpi_QUIT: 
               /* Program finished. Exit MPI before stopping */
               return 1;
         default: sprintf(errmsg,"Undefined message %d to task %d\n",
                    message.cmd,mytid);
                  kb_error(3335,errmsg,RECOVERABLE);
               return 2;
       }
   }

  /* Program finished. Return to main. */

  return 0;
}

/***************************************************************************

 Handling lists of remote elements.

****************************************************************************/
/***************************************************************************
*
* function: element_id_comp()
*
* purpose: comparison of element id's for purposes of sorting in machine
*          order and eliminating duplicates.
*/
int element_id_comp(a,b)
char *a;
char *b;
{ element_id aa = *(element_id*)a;
  element_id bb = *(element_id*)b;
  int m1 = id_machine(aa);
  int m2 = id_machine(bb);
  if ( m1 < m2 ) return -1;
  if ( m1 > m2 ) return  1;
  if ( positive_id(aa) < positive_id(bb) ) return -1;
  if ( positive_id(aa) > positive_id(bb) ) return  1;
  return 0;
}

/***************************************************************************
*
* function: mpi_node_fe_fixup()
*
* purpose: Re-integrate fe links with imported elements.
*          Goes through both import and export lists.
*/
void mpi_node_fe_fixup()
{
  facet_id fe,ffe,next_fe,prev_fe,last_forward,last_backward;
  edge_id e_id;
  int i,task;

  /* strip absent facetedges from edges */
  for ( task = 0 ; task < nprocs ; task++ )
  { for ( i = 0 ; i < import_elements[task][EDGE].count ; i++ )
    { e_id= import_elements[task][EDGE].id_list[i];
      fe = get_edge_fe(e_id);
      if ( !valid_id(fe) ) continue;
      if ( elptr(fe) == NULL )
      { set_edge_fe(e_id,NULLID);
        continue;
      }
      /* go around loop forwards */
      last_forward = last_backward = NULLID;
      ffe = fe;
      do
      {
        next_fe = get_next_facet(ffe);
        if ( !valid_id(next_fe) || (elptr(next_fe) == NULL) )
        { last_forward = ffe;
          break;
        }
        ffe = next_fe;
      } while ( !equal_id(fe,ffe) );
      ffe = fe;
      do
      {
        prev_fe = get_prev_facet(ffe);
        if ( !valid_id(prev_fe) || (elptr(prev_fe) == NULL) )
        { last_backward = ffe;
          break;
        }
        ffe = prev_fe;
      } while ( !equal_id(fe,ffe) );
      if ( valid_id(last_forward) && !valid_id(last_backward) )
        kb_error(3388,"last_forward without last_backward\n",RECOVERABLE);
      if ( !valid_id(last_forward) && valid_id(last_backward) )
        kb_error(3389,"last_backward without last_forward\n",RECOVERABLE);
      if ( valid_id(last_forward) && valid_id(last_backward) )
      { set_next_facet(last_forward,last_backward);
        set_prev_facet(last_backward,last_forward);
      }
    } /* end import edges */
    for ( i = 0 ; i < export_elements[task][EDGE].count ; i++ )
    { e_id= export_elements[task][EDGE].id_list[i];
      fe = get_edge_fe(e_id);
      if ( !valid_id(fe) ) continue;
      if ( elptr(fe) == NULL )
      { set_edge_fe(e_id,NULLID);
        continue;
      }
      /* go around loop forwards */
      last_forward = last_backward = NULLID;
      ffe = fe;
      do
      {
        next_fe = get_next_facet(ffe);
        if ( !valid_id(next_fe) || (elptr(next_fe) == NULL) )
        { last_forward = ffe;
          break;
        }
        ffe = next_fe;
      } while ( !equal_id(fe,ffe) );
      ffe = fe;
      do
      {
        prev_fe = get_prev_facet(ffe);
        if ( !valid_id(prev_fe) || (elptr(prev_fe) == NULL) )
        { last_backward = ffe;
          break;
        }
        ffe = prev_fe;
      } while ( !equal_id(fe,ffe) );
      if ( valid_id(last_forward) && !valid_id(last_backward) )
        kb_error(3906,"last_forward without last_backward\n",RECOVERABLE);
      if ( !valid_id(last_forward) && valid_id(last_backward) )
        kb_error(3929,"last_backward without last_forward\n",RECOVERABLE);
      if ( valid_id(last_forward) && valid_id(last_backward) )
      { set_next_facet(last_forward,last_backward);
        set_prev_facet(last_backward,last_forward);
      }
    } /* end import edges */
  }
  
  /* give each edge a valid facetedge */
  for ( task = 0 ; task < nprocs ; task++ )
  { for ( i = 0 ; i < import_elements[task][FACETEDGE].count ; i++ )
    { fe = import_elements[task][FACETEDGE].id_list[i];
      e_id = get_fe_edge(fe);
      ffe = get_edge_fe(e_id);
      if ( !valid_id(ffe) || (elptr(ffe) == NULL) )
       { set_edge_fe(e_id,fe);
         set_next_facet(fe,fe);
         set_prev_facet(fe,fe);
       }
    }
    for ( i = 0 ; i < export_elements[task][FACETEDGE].count ; i++ )
    { fe = export_elements[task][FACETEDGE].id_list[i];
      e_id = get_fe_edge(fe);
      ffe = get_edge_fe(e_id);
      if ( !valid_id(ffe) || (elptr(ffe) == NULL) )
      { set_edge_fe(e_id,fe);
        set_next_facet(fe,fe);
        set_prev_facet(fe,fe);
      }
    }
  }

  /* make sure all facetedges linked in */
  for ( task = 0 ; task < nprocs ; task++ )
  { for ( i = 0 ; i < import_elements[task][FACETEDGE].count ; i++ )
    { fe = import_elements[task][FACETEDGE].id_list[i];
      e_id = get_fe_edge(fe);
      ffe = get_edge_fe(e_id);
      next_fe = ffe;
      do
      { if ( equal_id(fe,next_fe) ) break;
        next_fe = get_next_facet(next_fe);
      } while ( !equal_id(ffe,next_fe));
      if ( !equal_id(fe,next_fe) )
      { /* splice in */
        set_next_facet(fe,get_next_facet(ffe));
        set_prev_facet(fe,ffe);
        set_prev_facet(get_next_facet(ffe),fe);
        set_next_facet(ffe,fe);
      }
    }        
    for ( i = 0 ; i < export_elements[task][FACETEDGE].count ; i++ )
    { fe = export_elements[task][FACETEDGE].id_list[i];
      e_id = get_fe_edge(fe);
      ffe = get_edge_fe(e_id);
      next_fe = ffe;
      do
      { if ( equal_id(fe,next_fe) ) break;
        next_fe = get_next_facet(next_fe);
      } while ( !equal_id(ffe,next_fe));
      if ( !equal_id(fe,next_fe) )
      { /* splice in */
        set_next_facet(fe,get_next_facet(ffe));
        set_prev_facet(fe,ffe);
        set_prev_facet(get_next_facet(ffe),fe);
        set_next_facet(ffe,fe);
      }
    }        
  }

  /* now make sure geometric order is right */ 
  for ( task = 0 ; task < nprocs ; task++ )
  { for ( i = 0 ; i < import_elements[task][EDGE].count ; i++ )
      fe_reorder(import_elements[task][EDGE].id_list[i]);
    for ( i = 0 ; i < import_elements[task][EDGE].count ; i++ )
      fe_reorder(import_elements[task][EDGE].id_list[i]);
  }
}  /* end mpi_node_fe_fixup() */

/*************************************************************************/
/* THIN_CORONA setup *****************************************************/

/***************************************************************************
*
* function: mpi_synch_vertices()
*
* purpose: Set up import lists of vertices needed from other machines 
*          for THIN_CORONA. 
*/

void mpi_synch_vertices()
{ edge_id e_id;
  int count=0,alloc=0;
  element_id *needs=NULL;
  int i,task;
  
  if ( mpi_debug )
    printf("Task %d synch vertices.\n",this_machine);

  /* build lists of needed elements */
  /* vertices needed by local edges */
  MFOR_ALL_EDGES(e_id)
  { vertex_id v_id;
    v_id = get_edge_tailv(e_id);
    if ( count > alloc-2 )
    { needs = (element_id*)temp_realloc((char*)needs,
        (2*alloc+1000)*sizeof(element_id));
      alloc = 2*alloc+1000;
    }
    task = id_machine(v_id);
    if ( task != this_machine )
      needs[count++] = v_id;
    v_id = get_edge_headv(e_id);
    task = id_machine(v_id);
    if ( task != this_machine )
      needs[count++] = v_id;
  }
     
  mpi_fill_import_list(VERTEX,needs,count);
  mpi_fetch_remote_elements(VERTEX);

  /* make sure vertices connected locally */
  for ( task = 0 ; task < nprocs ; task++ )
  { for ( i = 0 ; i < import_elements[task][EDGE].count ; i++ )
    { e_id = import_elements[task][EDGE].id_list[i];
      set_vertex_edge(get_edge_tailv(e_id),e_id);
      set_vertex_edge(get_edge_headv(e_id),inverse_id(e_id));
    }
  }
  FOR_ALL_EDGES(e_id)
  { set_vertex_edge(get_edge_tailv(e_id),e_id);
    set_vertex_edge(get_edge_headv(e_id),inverse_id(e_id));
  }

  temp_free((char*)needs);
}  /* end synch_vertices() */

/***************************************************************************
*
* function: mpi_synch_edges()
*
* purpose: Set up import lists of edges needed from other machines 
*          for THIN_CORONA.  Only for SOAPFILM model.
*
*/

void mpi_synch_edges()
{ edge_id e_id;
  facet_id f_id;
  int count=0,alloc=0;
  element_id *needs=NULL;
  
  if ( web.representation != SOAPFILM ) 
     return;

  if ( mpi_debug )
    printf("Task %d synch edges.\n",this_machine);

  /* build lists of needed edges */
  FOR_ALL_FACETS(f_id)
  { facetedge_id fe_id = get_facet_fe(f_id);
    facetedge_id start_fe = fe_id;

    do 
    { int task;
      if ( count > alloc-2 )
      { needs = (element_id*)temp_realloc((char*)needs,
           (2*alloc+1000)*sizeof(element_id));
        alloc = 2*alloc+1000;
      }
      e_id = get_fe_edge(fe_id);
      task = id_machine(e_id);
      if ( task != this_machine )
        needs[count++] = e_id;
      fe_id = get_next_edge(fe_id);
    } while ( !equal_id(fe_id,start_fe) );
  }

  if ( mpi_debug )
    printf("Task %d needs %d edges.\n",this_machine,count);
  
  mpi_fill_import_list(EDGE,needs,count);
  mpi_fetch_remote_elements(EDGE);

  temp_free((char*)needs);
}


/***************************************************************************
*
* function: mpi_synch_elements()
*
* purpose: Set up lists of elements needed from other machines.  These
*          lists are separate from the native list, and have read-only
*          values.  Initial setup.
*
* arguments: int corona_level
*               0 - no import/export
*               1 - home facets have adjacent edges and vertices
*               2 - home vertices have adjacent facets and their parts.
*/
void mpi_synch_elements(corona_level)
int corona_level;
{ 
  message.cmd = mpi_SYNCH_ELEMENTS;
  message.mode = corona_level;
  if ( (mpi_corona_state < corona_level) && (corona_level >= THIN_CORONA) )
  { MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
    mpi_node_synch_elements();
  }
  if ( (mpi_corona_state < corona_level) && (corona_level >= FAT_CORONA) )
    mpi_synch_corona();
  mpi_corona_state = corona_level;
  top_timestamp = ++global_timestamp;
}

/***************************************************************************
*
* function: mpi_node_synch_elements()
*
* purpose: Set up lists of elements needed from other machines.  These
*          lists are separate from the native list, and have read-only
*          values.  Initial setup.
*
*          Initial state should be no corona.
*          Final state is thin corona.
*/
void mpi_node_synch_elements()
{
  if ( mpi_debug )
    printf("task %d synch_elements\n",this_machine);


  mpi_synch_edges();
  MPI_Barrier(MPI_COMM_WORLD);
  mpi_synch_vertices();
  MPI_Barrier(MPI_COMM_WORLD);

  top_timestamp = ++global_timestamp;
  
} /* end mpi_node_synch_elements() */

/*  End THIN_CORONA setup ************************************************/
/*************************************************************************/

/************************************************************************
*
* function: mpi_fetch_remote_elements()
*
* purpose: Fetch remote elements on import list of ids.
*/

void mpi_fetch_remote_elements(type)
int type; /* of element */
{ int i,k,task;
  int nodes_done;
  int asked = 0;  /* number of requests sent out */
  int sent_done = 0; /* whether have sent DONE_REQUESTS */
  int my_requests_fulfilled = 0;
  
  /* now send nonblocking messages to others */
  for ( task = 0 ; task < nprocs ; task++ )
  { MPI_Request request;  /* so MPI_Wait can check completion, if desired */
    int n = import_elements[task][type].count;

    if ( n == 0 ) continue;
    infos[task].count = n;
    infos[task].size = web.sizes[type];
    infos[task].etype = type;
    infos[task].type = ID_REQUEST;

    if ( mpi_debug )
       printf("Task %d requesting %d elements of type %d from task %d.\n",
          this_machine,n,type,task);

    MPI_Isend(&infos[task],sizeof(struct message_info),MPI_BYTE,task,INFO_TAG,
                 MPI_COMM_WORLD,&request);
    MPI_Isend(import_elements[task][type].id_list,n*sizeof(element_id),MPI_BYTE,
          task,IDLIST_TAG,MPI_COMM_WORLD,&request);
    asked++;
  }

  /* get incoming requests and answer */
  my_requests_fulfilled = 0;
  nodes_done = 0;  /* for root to keep track */
  for(;;)
  { struct message_info info;
    MPI_Status status;  /* MPI_SOURCE,MPI_TAG fields for message */
    MPI_Request request;  /* so MPI_Wait can check completion, if desired */

    if ( (this_machine != 0) && !sent_done && (my_requests_fulfilled == asked) )
    { struct message_info dinfo; 
      dinfo.type = DONE_REQUESTS;
      if ( mpi_debug ) 
        printf("Task %d sending DONE_REQUESTS\n",this_machine);
      MPI_Isend(&dinfo,sizeof(struct message_info),MPI_BYTE,0,INFO_TAG,
                 MPI_COMM_WORLD,&request);
      sent_done = 1;
    }

    if ( mpi_debug ) 
       printf("Task %d listening...\n",this_machine);
    MPI_Recv(&info,sizeof(struct message_info),MPI_BYTE,MPI_ANY_SOURCE,
         INFO_TAG,MPI_COMM_WORLD,&status);
    if ( mpi_debug )
       printf("Task %d got reply info.type %s\n",this_machine,
           infotypes[info.type-ID_REQUEST]);
    if ( info.type == DONE_REQUESTS )
    { if ( this_machine == 0 )  /* tell everybody else */
      { nodes_done++; 
        if ( nodes_done >= nprocs-1 )
        {
          info.count = 0;
          info.type = DONE_REQUESTS;
          for ( task = 1 ; task < nprocs ; task++ )
          {
            if ( mpi_debug )
              printf("Task %d sending DONE_REQUESTS to task %d.\n",
                 this_machine,task);
            MPI_Send(&info,sizeof(struct message_info),MPI_BYTE,task,INFO_TAG,
               MPI_COMM_WORLD);
          }
          break;
        } 
      }
      else
        break; /* nonroot done */
    }

    if ( info.type == ID_REQUEST )
    { /* if get here, then wait for corresponding id list */
      element_id *id_buf;
      struct element *element_buf;
      struct element *spot;

      task = status.MPI_SOURCE;
      if ( web.sizes[type] != info.size )
      { sprintf(errmsg,
         "%s structure sizes disagree among MPI tasks:\n this task %d: %d vs task %d %s: %d.\n",
	       typenames[type],this_machine,web.sizes[type],task,
               typenames[info.etype],info.size);
        kb_error(3909,errmsg,  RECOVERABLE);
      }
      /* allocate export structures */
      export_elements[task][type].count = 
        export_elements[task][type].max = info.count;
      id_buf = export_elements[task][type].id_list = 
         (element_id*)mycalloc(info.count,sizeof(element_id));
      element_buf = export_elements[task][type].buf = 
           (struct element *)mycalloc(info.count,web.sizes[type]);

      if ( mpi_debug )
         printf("Task %d listening for element id list.\n",this_machine);
      MPI_Recv(id_buf,info.count*sizeof(element_id),MPI_BYTE,status.MPI_SOURCE,
           IDLIST_TAG,MPI_COMM_WORLD,&status);
  
      /* Send desired elements back */
      for ( k = 0, spot = element_buf ; k < info.count ; k++ )
      { int ty = id_type(id_buf[k]);
        memcpy(spot,elptr(id_buf[k]),web.sizes[ty]);
        spot = (struct element*)((char*)spot + web.sizes[ty]);
        set_export_attr(id_buf[k],status.MPI_SOURCE);
      }
      info.type = ID_RESPONSE;
      info.count = (char*)spot-(char*)element_buf;
      if ( mpi_debug )
        printf("Task %d sending element data back to task %d\n",this_machine,
           status.MPI_SOURCE);
      MPI_Send(&info,sizeof(struct message_info),MPI_BYTE,status.MPI_SOURCE,
           INFO_TAG,MPI_COMM_WORLD);
      MPI_Send(element_buf,info.count,MPI_BYTE,status.MPI_SOURCE,
           RESPONSE_TAG,MPI_COMM_WORLD);
    }
    else if ( info.type == ID_RESPONSE )
    { struct element *element_buf;
      int task = status.MPI_SOURCE;
      element_buf = import_elements[task][type].buf;
      if ( mpi_debug ) 
        printf("Task %d listening for element data from task %d\n",this_machine,
            task);
      MPI_Recv(element_buf,info.count,MPI_BYTE,task,RESPONSE_TAG,
           MPI_COMM_WORLD,&status);
      if ( mpi_debug )
        printf("Task %d got element data.\n",this_machine);
      for ( k = 0, i = 0 ; k < info.count ; i++ )
      { struct element *ep = (struct element*)((char*)element_buf + k);
        if ( !equal_element(import_elements[task][type].id_list[i],ep->self_id) )
        { sprintf(errmsg,
            "Disagreeing element in MPI response. Wanted %08X, got %08X\n",
               import_elements[task][type].id_list[i],ep->self_id);
          kb_error(3114,errmsg, WARNING);
        }
        else
          mpi_add_remote_element(ep);
        k += web.sizes[id_type(ep->self_id)];
        
      }
      my_requests_fulfilled++;
    }
  };

} /* end fetch_remote_elements() */

/***************************************************************************
*
* function: mpi_fill_import_list()
*
* purpose: Take unsorted list of import ids and set up import lists.
*/
void mpi_fill_import_list(type,needs,count)
int type; /* of element */
element_id *needs;
int count;
{ int i,j,task;

  /* get rid of old */
  for ( task = 0 ; task < nprocs ; task++ )
  { if ( import_elements[task][type].id_list )
     myfree((char*)import_elements[task][type].id_list);
    if ( import_elements[task][type].buf )
     myfree((char*)import_elements[task][type].buf);
    memset(&import_elements[task][type],0,sizeof(struct import_list_t));
  }

  /* sort and uniquify */ 
  qsort(needs,count,sizeof(element_id),FCAST element_id_comp);
  for ( i = 1, j = 0 ; i < count ; i++ )
  { if ( element_id_comp(needs+j,needs+i) != 0 )
      needs[++j] = needs[i];
  }
  if ( count > 0 ) 
     count = j+1;
     
  if ( mpi_debug )
     printf("Task %d count %d\n",this_machine,count);
  /* count */
  for ( i = 0, task = 0, j = 0 ; i < count ; i++, j++ )
  { while ( id_machine(needs[i]) > task ) 
    { import_elements[task][type].count = j;
      task++;
      j = 0;
    }
  }
  import_elements[task][type].count = j;
  
  /* allocate */
  for ( task = 0 ; task < nprocs ; task++ )
  { int tcount = import_elements[task][type].count; 
    if ( tcount == 0 ) continue;
    import_elements[task][type].max = tcount;
    import_elements[task][type].id_list = 
      (element_id*)mycalloc(sizeof(element_id),tcount);
    import_elements[task][type].buf = 
      (struct element *)mycalloc(web.sizes[type],tcount);
    import_elements[task][type].bufsize = tcount*web.sizes[type]; 
  }

  /* fill */
  for ( task = 0, j = 0 ; task < nprocs ; task++ )
  { for ( i = 0 ; i < import_elements[task][type].count ; i++,j++ )
      import_elements[task][type].id_list[i] = needs[j];
    if ( mpi_debug )
      printf("Task %d wants %d elements of type %d from task %d\n",
        this_machine,import_elements[task][type].count,type,task);
  }
} /* end fill_import_list() */

/***************************************************************************
*
* function: mpi_add_import()
*
* purpose: add one element to import list.
*/
void mpi_add_import(id)
element_id id;
{ int type = id_type(id);
  int task = id_machine(id);
  struct import_list_t *t = &import_elements[task][type];

  if ( !valid_id(id) )
    kb_error(3432,"Trying to add null id to import list.\n",RECOVERABLE);

  if ( t->count >= t->max )
  { int newmax = (t->max < 100) ? 100 : 2*t->max;
    t->id_list = (element_id*)kb_realloc((char*)t->id_list,
        newmax*sizeof(element_id));
    t->max = newmax;
  }
  t->id_list[t->count++] = id;
  if ( t->bufsize < t->max*web.sizes[type] )
  { t->buf = (struct element*)kb_realloc((char*)t->buf,t->max*web.sizes[type]);
    t->bufsize = t->max*web.sizes[type];
  }
  
} /* end add_import() */

/***************************************************************************
*
* function: mpi_add_export()
*
* purpose: add one element to export list.
*/
void mpi_add_export(id,task)
element_id id;
{ int type = id_type(id);
  struct export_list_t *t = &export_elements[task][type];

  if ( !valid_id(id) )
    kb_error(3433,"Trying to add null id to export list.\n",RECOVERABLE);

  if ( t->count >= t->max )
  { int newmax = (t->max < 100) ? 100 : 2*t->max;
    t->id_list = (element_id*)kb_realloc((char*)t->id_list, 
                       newmax*sizeof(element_id));
    t->max = newmax;
  }
  t->id_list[t->count++] = id;
  if ( t->bufsize < t->max*web.sizes[type] )
  { t->buf = (struct element*)kb_realloc((char*)t->buf,t->max*web.sizes[type]);
    t->bufsize = t->max*web.sizes[type];
  }
  set_export_attr(id,task);
 
} /* end add_export() */
/*************************************************************************
*
* function: mpi_node_clean_imports()
*
* purpose: sort and uniquify import lists
*/

void mpi_node_clean_imports()
{
  int task,type;
  int i,keep;
  for ( task = 0 ; task < nprocs ; task++ )
    for ( type = 0 ; type < NUMELEMENTS ; type++ )
    { struct import_list_t *t = &import_elements[task][type];
      if ( t->count == 0 ) continue;
      qsort(t->id_list,t->count,sizeof(element_id),FCAST element_id_comp);
      for ( keep = 0, i = 1 ; i < t->count ; i++ )
        if ( !equal_element(t->id_list[keep],t->id_list[i]) )
          t->id_list[++keep] = t->id_list[i];
      t->count = keep+1;
    }
}

/*************************************************************************
*
* function: mpi_synch_corona()
*
* purpose: Make sure all home vertices have one layer of facets around,
*          so can compute things like normal.  Works by sending
*          facets, edges, facetedges, and vertices over to imported
*          vertices.   Root node part.
*/
int id_comp(element_id *a, element_id *b)
{
  if ( id_machine(*a) < id_machine(*b) ) return -1;
  if ( id_machine(*a) > id_machine(*b) ) return  1;
  if ( positive_id(*a) < positive_id(*b) ) return -1;
  if ( positive_id(*a) > positive_id(*b) ) return 1;
  return 0;
}

void mpi_synch_corona()
{ int incount;
  int *reducer,*reducee;
  int type;

  if ( mpi_corona_state == FAT_CORONA ) 
     return;

  message.cmd = mpi_SYNCH_CORONA;
  MPI_Bcast(&message,sizeof(message),MPI_BYTE, 0, MPI_COMM_WORLD);

  reducer = (int *)temp_calloc(nprocs,sizeof(int));
  reducee = (int *)temp_calloc(nprocs,sizeof(int));

  /* let everybody know how many messages to expect */
  MPI_Reduce(reducer,reducee,nprocs,MPI_INTEGER,MPI_SUM,0,MPI_COMM_WORLD);
  MPI_Scatter(reducee,1,MPI_INTEGER,&incount,1,MPI_INTEGER,0,MPI_COMM_WORLD);

  /* root part of fetching remotes */
  for  ( type = 0 ; type < NUMELEMENTS ; type++ )
    mpi_fetch_remote_elements(type);

  mpi_corona_state = FAT_CORONA;
  temp_free((char*)reducer);
  temp_free((char*)reducee);
}

/*************************************************************************
*
* function: mpi_node_synch_corona()
*
* purpose: Make sure all home vertices have one layer of facets around,
*          so can compute things like normal.  Works by sending
*          facets, edges, facetedges, and vertices over to imported
*          vertices.
*/

element_id **exlist; /* list for each task */
int *ex_alloc,*ex_count,*reducer,*reducee;

void add_to_exlist(id,task)
element_id id;
int task;
{
   if ( ex_count[task] >= ex_alloc[task] )
   { int newcount = ex_count[task] ? 2*ex_count[task] : 128;
     exlist[task] = (element_id*)temp_realloc((char*)exlist[task],
             newcount*sizeof(element_id));
     ex_alloc[task] = newcount;
   }
   exlist[task][ex_count[task]++] = id;
}

void mpi_node_synch_corona()
{ edge_id e_id;
  vertex_id v_id;
  facet_id f_id;
  int task;
  int i,keep,n;
  int incount;
  facetedge_id next,prev;
  MPI_Status status;
  MPI_Request request;
  int type;

  if ( mpi_debug )
    printf("Task %d in node_synch_corona\n",this_machine);

  exlist = (element_id**)temp_calloc(nprocs,sizeof(element_id*));
  ex_alloc = (int *)temp_calloc(nprocs,sizeof(int));
  ex_count = (int *)temp_calloc(nprocs,sizeof(int));
  reducer = (int *)temp_calloc(nprocs,sizeof(int));
  reducee = (int *)temp_calloc(nprocs,sizeof(int));

  /* Export all edges that have an endpoint that belongs to another
     task 
   */
  FOR_ALL_EDGES(e_id)
  { 
    v_id = get_edge_tailv(e_id);
    task = id_machine(v_id);
    if ( task != this_machine ) 
      add_to_exlist(e_id,task);
    else
    { unsigned short *p = mpi_export_vattr_ptr(v_id);
      int k;
      for ( k = 0 ; (p[k] != 0) && (k < MPI_EXPORT_MAX) ; k++ )
        add_to_exlist(e_id,p[k]);
    }
    v_id = get_edge_headv(e_id);
    task = id_machine(v_id);
    if ( task != this_machine ) 
      add_to_exlist(e_id,task);
    else
    { unsigned short *p = mpi_export_vattr_ptr(v_id);
      int k;
      for ( k = 0 ; (p[k] != 0) && (k < MPI_EXPORT_MAX) ; k++ )
        add_to_exlist(e_id,p[k]);
    }
   
  }

  if ( web.representation == SOAPFILM )
    FOR_ALL_FACETS(f_id)
    { facetedge_id fe = get_facet_fe(f_id);
      vertex_id vv_id;
      for ( i = 0 ; i < FACET_VERTS ; i++ )
      { v_id = get_fe_tailv(fe);
        task = id_machine(v_id);
        /* export facet if vertex imported */
        if ( id_machine(v_id) != this_machine )
        {
          add_to_exlist(f_id,task);
          add_to_exlist(fe,task);
          e_id = get_fe_edge(fe);
          if ( id_machine(e_id) != task ) 
             add_to_exlist(e_id,task);

          prev = get_prev_edge(fe);
          add_to_exlist(prev,task);
          e_id = get_fe_edge(prev);
          if ( id_machine(e_id) != task ) 
             add_to_exlist(e_id,task);
          vv_id = get_fe_tailv(prev);
          if ( id_machine(vv_id) != task ) 
             add_to_exlist(vv_id,task);

          next = get_next_edge(fe);
          add_to_exlist(next,task);
          e_id = get_fe_edge(next);
          if ( id_machine(e_id) != task ) 
             add_to_exlist(e_id,task);
          vv_id = get_fe_tailv(next);
          if ( id_machine(vv_id) != task ) 
             add_to_exlist(vv_id,task);
        }
        else /* export facet if vertex exported */
        { unsigned short *p = mpi_export_vattr_ptr(v_id);
          int k;
          for ( k = 0 ; (p[k] != 0) && (k < MPI_EXPORT_MAX) ; k++ )
          { task = p[k];
            add_to_exlist(f_id,task);
            add_to_exlist(fe,task);
            e_id = get_fe_edge(fe);
            if ( id_machine(e_id) != task ) 
               add_to_exlist(e_id,task);
  
            prev = get_prev_edge(fe);
            add_to_exlist(prev,task);
            e_id = get_fe_edge(prev);
            if ( id_machine(e_id) != task ) 
               add_to_exlist(e_id,task);
            vv_id = get_fe_tailv(prev);
            if ( id_machine(vv_id) != task ) 
               add_to_exlist(vv_id,task);
  
            next = get_next_edge(fe);
            add_to_exlist(next,task);
            e_id = get_fe_edge(next);
            if ( id_machine(e_id) != task ) 
               add_to_exlist(e_id,task);
            vv_id = get_fe_tailv(next);
            if ( id_machine(vv_id) != task ) 
               add_to_exlist(vv_id,task);
          }
        }
        fe = get_next_edge(fe);
      }   
    } 

  /* now sort task lists and uniquify */
  for ( task = 0 ; task < nprocs ; task++ )
  { if ( ex_count[task] == 0 ) continue;
    reducer[task] = 1;
    qsort(exlist[task],ex_count[task],sizeof(element_id),FCAST id_comp);
    for ( i = 1, keep = 0 ; i < ex_count[task] ; i++ )
    { if ( !equal_id(exlist[task][i],exlist[task][keep]) )
        exlist[task][++keep] = exlist[task][i];
    }
  }

  /* let everybody know how many messages to expect */
  MPI_Reduce(reducer,reducee,nprocs,MPI_INTEGER,MPI_SUM,0,MPI_COMM_WORLD);
  MPI_Scatter(reducee,1,MPI_INTEGER,&incount,1,MPI_INTEGER,0,MPI_COMM_WORLD);

  /* send element ids to other tasks */

  for ( task = 0 ; task < nprocs ; task++ )
  { if ( ex_count[task] == 0 ) continue;
    infos[task].type = mpi_SYNCH_CORONA;
    infos[task].count = ex_count[task];
    infos[task].size = ex_count[task]*sizeof(element_id);
    MPI_Isend(&infos[task],sizeof(struct message_info),MPI_BYTE,task,CORONA_TAG,
        MPI_COMM_WORLD,&request);
    MPI_Isend(exlist[task],infos[task].size,MPI_BYTE,task,CORONA_TAG,
        MPI_COMM_WORLD, &request);
  }
  
  /* receive from others */
  for( n = 0; n < incount ; n++)
  { char *spot;
    struct message_info in;
    MPI_Recv(&in,sizeof(in),MPI_BYTE,MPI_ANY_SOURCE,CORONA_TAG,MPI_COMM_WORLD,
       &status);
    check_bigbufsize(in.size);
    MPI_Recv(bigbuf,in.size,MPI_BYTE,status.MPI_SOURCE,CORONA_TAG, 
       MPI_COMM_WORLD,&status);
    for ( spot = bigbuf, i = 0 ; i < in.count ; i++ )
    {
      mpi_add_import(*((element_id*)spot));  
      spot += sizeof(element_id);
    }
  if ( mpi_debug )
    printf("Task %d got corona from task %d \n",this_machine,status.MPI_SOURCE);
  }


  /* clean up import lists */
  mpi_node_clean_imports();

  /* get needed elements */
  for  ( type = 0 ; type < NUMELEMENTS ; type++ )
    mpi_fetch_remote_elements(type);

  /* now fix up dangling links */
  mpi_node_fe_fixup();
  make_vedge_lists();  /* overkill to do all, but safe */

  /* deallocate */
  for ( task = 0 ; task < nprocs ; task++ )
    if ( exlist[task] )
       temp_free((char*)exlist[task]);
  temp_free((char*)exlist);
  temp_free((char*)ex_count);
  temp_free((char*)ex_alloc);
  temp_free((char*)reducer);
  temp_free((char*)reducee);
  top_timestamp = ++global_timestamp;
}

/*************************************************************************
*
* function: mpi_dissolve_corona()
*
* purpose: dissolve corona beyond 0th layer, in prep for refining or
*          whatever.   
*/
void mpi_dissolve_corona()
{
  message.cmd = mpi_DISSOLVE_CORONA;
  MPI_Bcast(&message,sizeof(message),MPI_BYTE, 0, MPI_COMM_WORLD);
  top_timestamp = ++global_timestamp;
  mpi_corona_state = THIN_CORONA;
  
}
/*************************************************************************
*
* function: mpi_node_dissolve_corona()
*
* purpose: dissolve corona beyond 0th layer, in prep for refining or
*          whatever.   Internal node function.
*/
void mpi_node_dissolve_corona()
{ MPI_Request request;
  int task;
  int etype;
  int i,j,n;
  int keep;
  int expected;


  if ( web.representation == SOAPFILM)
  { /* dissolve corona facets */
    for ( task = 1 ; task < nprocs ; task++ )
	{ struct import_list_t *il = &import_elements[task][FACET];
	  for ( i = 0 ; i < il->count ; i++ )
        dissolve_facet(il->id_list[i]);
	}
  }
  /* dissolve corona edges */
    for ( task = 1 ; task < nprocs ; task++ )
	{ struct import_list_t *il = &import_elements[task][EDGE];
	  for ( i = 0 ; i < il->count ; i++ )
        dissolve_edge(il->id_list[i]);
	}
  
  /* dissolve corona vertices */
     for ( task = 1 ; task < nprocs ; task++ )
	{ struct import_list_t *il = &import_elements[task][VERTEX];
	  for ( i = 0 ; i < il->count ; i++ )
        dissolve_vertex(il->id_list[i]);
	}
 
  /* Reformulate import/export lists, by reconstructing import lists
     from what is left, and sending import lists to other nodes as
     their export lists.
  */
  /* Discard dissolved elements */
  for ( task = 1 ; task < nprocs ; task++ )
    for ( etype = VERTEX ; etype < NUMELEMENTS ; etype++ )
    { struct import_list_t *ie = &import_elements[task][etype];
      for ( i = 0, keep = 0 ; i < ie->count ; i++ )
        if ( mpi_remote_present(ie->id_list[i]) ) 
          ie->id_list[keep++] = ie->id_list[i];
      ie->count = keep; 
    } 

  /* Clear own export lists */
  for ( task = 1 ; task < nprocs ; task++ )
    for ( etype = VERTEX ; etype < NUMELEMENTS ; etype++ )
    { struct export_list_t *ex = &export_elements[task][etype];
      int j;
      for ( i = 0 ; i < ex->count ; i++ )
      { unsigned short *nbr = mpi_export_attr_ptr(ex->id_list[i]);
        for ( j = 0 ; j < MPI_EXPORT_MAX ; j++ )
          nbr[j] = 0;
      }
      ex->count = 0;
    }

  /* Send */
  for ( task = 1 ; task < nprocs ; task++ )
    for ( etype = VERTEX ; etype < NUMELEMENTS ; etype++ )
    { struct import_list_t *ie = &import_elements[task][etype];
      if ( ie->max )
      {  struct message_info *info = &infos[task*NUMELEMENTS+etype];
         info->type = EXIM_INFO;
         info->count = ie->count;
         info->etype = etype;
         MPI_Isend(info,sizeof(struct message_info),MPI_BYTE,task,INFO_TAG,
           MPI_COMM_WORLD, &request);
         MPI_Isend(ie->id_list,ie->count*sizeof(element_id),MPI_BYTE,task,
           EXIM_TAG,MPI_COMM_WORLD,&request);
      }
    }
  
  /* Receive */
  /* first, count expected messages */
  expected = 0;
  for ( task = 1 ; task < nprocs ; task++ )
    for ( etype = VERTEX ; etype < NUMELEMENTS ; etype++ )
      if ( export_elements[task][etype].max > 0 ) 
        expected++;
  /* now listen for incoming */
  for ( n = 0 ; n < expected ; n++ )
  { struct message_info in;
    struct export_list_t *ex;
    MPI_Status status;
    MPI_Recv(&in,sizeof(in),MPI_BYTE,MPI_ANY_SOURCE,INFO_TAG,MPI_COMM_WORLD,
      &status);
    ex = &export_elements[status.MPI_SOURCE][in.etype];
    MPI_Recv(ex->id_list,in.count*sizeof(element_id),MPI_BYTE,status.MPI_SOURCE,
      EXIM_TAG,MPI_COMM_WORLD,&status);
    ex->count = in.count;
    /* fix up export attributes */
    for ( i = 0 ; i < ex->count ; i++ )
    { unsigned short *nbr = mpi_export_attr_ptr(ex->id_list[i]);
      for ( j = 0 ; j < MPI_EXPORT_MAX ; j++ )
	  { if ( nbr[j] == 0 )
        { nbr[j] = status.MPI_SOURCE;
          break;
        }
      }
    }
   
  }

  /* Patch up dangling links ??? already done by dissolve? */
  make_vedge_lists();

  top_timestamp = ++global_timestamp;
  mpi_corona_state = THIN_CORONA;
  
}  /* end mpi_node_dissolve_corona() */

/*************************************************************************
*
* function: mpi_add_remote_element()
*
* purpose: Add element to remote element lists.  Using simple linear
*    search through pointer list for now; maybe hash later.
*    Allocates space from regular element space, so remote element
*    can be accessed through MFOR_ALL_... .
*
*/

void mpi_add_remote_element(ep)
struct element *ep;  /* copied */
{ element_id id = ep->self_id;
  element_id new_id;
  int etype = id_type(id);
  /* int hash = (int)id_hash(id); */
  struct remote_list_t *r = remote_elements + etype;
  struct element *p,*space;
  int skip = 3*sizeof(element_id); /* for preserving list pointers */
  int ord,task;

  p = mpi_remote_elptr(id);
  if ( p ) 
  {
    if ( p->attr & ALLOCATED ) /* replace old, but keep local connectivity */
    { facetedge_id fe;
      edge_id e_id,next_vedge[2];
      switch ( etype )
      { case VERTEX: e_id = ((struct vertex*)p)->e_id;
             break;
        case EDGE: fe = ((struct edge*)p)->fe_id; 
               next_vedge[0] = ((struct edge*)p)->next_vedge[0];
               next_vedge[1] = ((struct edge*)p)->next_vedge[1];
            break;
        case FACET: fe = ((struct facet*)p)->fe_id;
            break;
      }
      memcpy((char*)p+skip,(char*)ep+skip,web.sizes[etype]-skip);
      switch ( etype )
      { case VERTEX: ((struct vertex*)p)->e_id = e_id;
            break;
        case EDGE: ((struct edge*)p)->fe_id = fe; 
              ((struct edge*)p)->next_vedge[0] = next_vedge[0];
              ((struct edge*)p)->next_vedge[1] = next_vedge[1];
            break;
        case FACET: ((struct facet*)p)->fe_id = fe;
            break;
      }
      
    }
    else /* fill in deleted element */
    { unfree_element(id);
      memcpy((char*)p+skip,(char*)ep+skip,web.sizes[etype]-skip); 
    }

    return;
  }

  /* new one */
  ord = ordinal(ep->self_id);
  task = id_machine(ep->self_id);

  while ( ord >= r->max[task] )
  { r->ibase[task] = (struct element**)kb_realloc((char*)(r->ibase[task]),
        (2*r->max[task]+100)*sizeof(struct element *));
    r->max[task] = 2*r->max[task] + 100;
  }
  
  new_id = new_element(etype,NULLID,NULLID);
  space = elptr(new_id);

  memcpy(&(space->attr),&(ep->attr),web.sizes[etype]-3*sizeof(element_id));
  r->ibase[task][ord] = space;
  r->count[task]++;
   
  /* fix up chain links */
  if ( valid_id(space->forechain) )
    elptr(space->forechain)->backchain = space->self_id;
  else web.skel[etype].last = space->self_id;
  if ( valid_id(space->backchain) )
    elptr(space->backchain)->forechain = space->self_id;
  else web.skel[etype].used = space->self_id;
  
}

/***************************************************************************
*
* function: mpi_remote_elptr()
*
* purpose: return pointer to structure of a remote element
*
*/

struct element *mpi_remote_elptr(id)
element_id id;
{
  int etype = id_type(id);
  int task = id_machine(id);

  if ( etype == BODY ) /* all tasks have all bodies for now */
    return web.skel[BODY].ibase[ordinal(id)];
  
  if ( remote_elements[etype].max[task] <= ordinal(id) ) 
    return NULL;
  return remote_elements[etype].ibase[task][ordinal(id)];

}

/***************************************************************************
*
* function: mpi_remove_remote(id)
*
* purpose: removes element from remote list index when element is deleted.
*          called from free_element(), so this is not a replacement for
*          free_element. 
*/
void mpi_remove_remote(id)
element_id id;
{
  int etype = id_type(id);
  int task = id_machine(id);

  if ( remote_elements[etype].max[task] <= ordinal(id) ) 
    kb_error(3546,
     "INTERNAL ERROR: mpi_remove_remote() on nonexistent element.\n", 
        RECOVERABLE);
  remote_elements[etype].count[task]--;
  remote_elements[etype].ibase[task][ordinal(id)] = NULLID; 
}

/***************************************************************************
*
* function: mpi_unfree_element(id)
*
* purpose: restores element to remote list index when element is deleted.
*          called from unfree_element(), so this is not a replacement for
*          unfree_element. 
*/
void mpi_unfree_element(id,ptr)
element_id id;
struct element *ptr;
{
  int etype = id_type(id);
  int task = id_machine(id);

  if ( remote_elements[etype].max[task] <= ordinal(id) ) 
    kb_error(3908,
     "INTERNAL ERROR: mpi_unfree_element() on nonexistent element.\n", 
        RECOVERABLE);
  remote_elements[etype].count[task]++;
  remote_elements[etype].ibase[task][ordinal(id)] = ptr; 
}

/****************************************************************************
*
* function: mpi_parallel_exec()
*
* purpose: sends command string to all nodes for execution.
*
*/
void mpi_parallel_exec(cmd)
char *cmd;
{
  message.cmd = mpi_PAR_EXEC;
  message.count = strlen(cmd)+1;
  MPI_Bcast(&message,sizeof(message),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Bcast(cmd,message.count,MPI_BYTE, 0, MPI_COMM_WORLD);
  /* command(cmd); just do it on remote nodes */
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_parallel_exec()
*
* purpose: Receives command string and executes it locally.
*
*/
void mpi_node_parallel_exec(count)
int count;
{ char *cmd = temp_calloc(count+1,1);

  if ( mpi_debug )
    printf("Task %d listening for command of length %d\n",this_machine,count);
  MPI_Bcast(cmd, count, MPI_BYTE, 0, MPI_COMM_WORLD);
  if ( mpi_debug )
    printf("Task %d got command: %s\n",this_machine,cmd);
  if ( !setjmp(jumpbuf) )  /* return here if error during execution */
  { command(cmd,NO_HISTORY);
  }
  temp_free(cmd);
  MPI_Barrier(MPI_COMM_WORLD);
}


/****************************************************************************
*
* function: mpi_node_exec()
*
* purpose: sends command string to all nodes for execution.
*
*/
void mpi_node_exec(task,cmd)
int task;
char *cmd;
{ MPI_Status status;

  if ( mpi_debug )
    printf("Task %d Doing node_exec: %s\n",this_machine,cmd);
  message.cmd = mpi_NODE_EXEC;
  message.task = task;
  message.count = strlen(cmd)+1;
  MPI_Bcast(&message,sizeof(message),MPI_BYTE,0,MPI_COMM_WORLD);
  MPI_Send(cmd,message.count,MPI_BYTE,task,EXEC_TAG,MPI_COMM_WORLD);
  /* command(cmd); just do it on remote nodes */
  MPI_Recv(&message,sizeof(message),MPI_BYTE,task,EXEC_TAG,MPI_COMM_WORLD,
    &status);
}

/****************************************************************************
*
* function: mpi_node_node_exec()
*
* purpose: Receives command string and executes it locally.
*
*/
void mpi_node_node_exec(count)
int count;
{ char *cmd = temp_calloc(count+1,1);
  MPI_Status status;

  if ( mpi_debug )
    printf("Task %d listening for command of length %d\n",this_machine,count);
  MPI_Recv(cmd, count, MPI_BYTE, 0, EXEC_TAG, MPI_COMM_WORLD,&status);
  if ( mpi_debug )
    printf("Task %d got command: %s\n",this_machine,cmd);
  if ( !setjmp(jumpbuf) )  /* return here if error during execution */
  { command(cmd,NO_HISTORY);  
    temp_free(cmd);
  }
  message.cmd = mpi_DONE;
  MPI_Send(&message,sizeof(message),MPI_BYTE,0,EXEC_TAG,MPI_COMM_WORLD);

}

/****************************************************************************
*
* function: mpi_calc_energy()
*
* purpose: called by root process to calculate total energy.
*
*/

void mpi_calc_energy()
{ REAL dummy[2];
  REAL results[2];
  dummy[0] = dummy[1] = 0.0;
  results[0] = results[1] = 0.0;
  message.cmd = mpi_CALC_ENERGY;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Reduce(dummy,results,2,MPI_REAL,MPI_SUM,0,MPI_COMM_WORLD);
  web.total_energy = results[0];
  web.total_area = results[1];
  if ( mpi_debug )
    printf("Task %d got total_energy %f\n",this_machine,web.total_energy);
}

/****************************************************************************
*
* function: mpi_node_calc_energy()
*
* purpose: node calculation of energy, and send back to root
*/

void mpi_node_calc_energy()
{ REAL dummy[2];
  REAL summands[2];
  local_calc_energy();
  summands[0] = web.total_energy;
  summands[1] = web.total_area;
  MPI_Reduce(summands,dummy,2,MPI_REAL,MPI_SUM,0,MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_calc_force()
*
* purpose: called by root process to calculate gradients of energy.
*
*/

void mpi_calc_force()
{ REAL dummy[2];
  REAL results[2];
  dummy[0] = dummy[1] = 0.0;
  results[0] = results[1] = 0.0;
  message.cmd = mpi_CALC_FORCE;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_calc_force()
*
* purpose: node calculation of energy gradient, and send back to root
*/

void mpi_node_calc_force()
{ int i,j,n,task;
  int nbr_task_count = 0;
  int nbrs_left;
  int maxn;
  
  local_calc_force();
  
  /* now scatter to home tasks (assuming remote elements properly ordered) */
  for ( task = 0, n = 0 ; task < nprocs ; task++ )
  {
    MPI_Request request;
    element_id *list;
    int count = import_elements[task][VERTEX].count;   
    REAL *buf = (REAL*)(import_elements[task][VERTEX].buf);
    
    if ( count == 0 ) continue;
    list = import_elements[task][VERTEX].id_list;
    for ( i = 0 ; i < count ; i++ )
    { REAL *f = get_force(list[i]);
      for ( j = 0 ; j < SDIM ; j++ )
        buf[SDIM*i+j] = f[j];
    }
    if ( mpi_debug )
      printf("Task %d sending %d forces to task %d\n",this_machine,count,task);
    MPI_Isend(buf,SDIM*count,MPI_REAL,task,FORCES_TAG,MPI_COMM_WORLD,
        &request);
  } 

  /* get incoming forces */
  for ( task = 0, nbr_task_count = 0, maxn = 0 ; task < nprocs ; task++ )
  { int n = export_elements[task][VERTEX].count;
    if ( n > 0 )
    { nbr_task_count++;
      if ( n > maxn ) maxn = n;
    }
  }
  check_bigbufsize(maxn*SDIM*sizeof(REAL));
         
  for ( nbrs_left = nbr_task_count ; nbrs_left > 0 ; nbrs_left-- )
  { MPI_Status status;
    MPI_Recv(bigbuf,bigbufsize,MPI_BYTE,MPI_ANY_SOURCE,FORCES_TAG,
       MPI_COMM_WORLD,&status);
    task = status.MPI_SOURCE;
    if ( mpi_debug )
      printf("Task %d got forces from task %d\n",this_machine,task);
    for ( n = 0 ; n < export_elements[task][VERTEX].count ; n++ )
    { REAL *f = get_force(export_elements[task][VERTEX].id_list[n]);
      for ( i = 0 ; i < SDIM ; i++ )
         f[i] += ((REAL*)bigbuf)[SDIM*n+i];
    }
  }  
  MPI_Barrier(MPI_COMM_WORLD);

}


/****************************************************************************
*
* function: mpi_move_vertices()
*
* purpose: called by root process to move vertices
*
*/

void mpi_move_vertices(mode,scale)
int mode;
REAL scale;
{
  message.cmd = mpi_MOVE_VERTICES;
  message.mode = mode;
  message.t    = scale;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_move_vertices()
*
* purpose: move local vertices
*/

void mpi_node_move_vertices(mode,scale)
int mode;
REAL scale;
{
  
  if ( mpi_debug )
    printf("Task %d doing local move vertices mode %d scale %f\n",
       this_machine,mode,scale);

  local_move_vertices(mode,scale);

  if ( mpi_debug )
    printf("Task %d done local move vertices mode %d scale %f\n",
        this_machine,mode,scale);
  
  mpi_node_synch_coord();  /* has own barrier */
}

/**************************************************************************
*
* function: mpi_node_synch_coord()
*
* purpose: Exports coordinates other nodes import.  To be called
*          by a node after it has done its local changes to coords.
*          Has barrier at end.
*/
void mpi_node_synch_coord()
{ int i,j,n,maxn;
  int task,nbr_task_count,nbrs_left;

  /* send updates to others */
  for ( task = 0, n = 0 ; task < nprocs ; task++ )
  {
    MPI_Request request;
    element_id *list;
    int count = export_elements[task][VERTEX].count;   
    REAL *buf = (REAL*)(export_elements[task][VERTEX].buf);
    
    if ( count == 0 ) continue;
    list = export_elements[task][VERTEX].id_list;
    for ( i = 0 ; i < count ; i++ )
    { REAL *x = get_coord(list[i]);
      for ( j = 0 ; j < SDIM ; j++ )
        buf[SDIM*i+j] = x[j];
    }
    if ( mpi_debug )
      printf("Task %d sending %d coords to task %d\n",this_machine,count,task);
    MPI_Isend(buf,SDIM*count,MPI_REAL,task,COORDS_TAG,MPI_COMM_WORLD,
        &request);
  } 

  /* get incoming coordinates */
  for ( task = 0, nbr_task_count = 0, maxn = 0 ; task < nprocs ; task++ )
  { int n = import_elements[task][VERTEX].count;
    if ( n > 0 )
    { nbr_task_count++;
      if ( n > maxn ) maxn = n;
    }
  }
  check_bigbufsize(maxn*SDIM*sizeof(REAL));

  for ( nbrs_left = nbr_task_count ; nbrs_left > 0 ; nbrs_left-- )
  { MPI_Status status;
    MPI_Recv(bigbuf,bigbufsize,MPI_BYTE,MPI_ANY_SOURCE,COORDS_TAG,
       MPI_COMM_WORLD,&status);
    task = status.MPI_SOURCE;
    if ( mpi_debug )
      printf("Task %d got coords from task %d\n",this_machine,task);
    for ( n = 0 ; n < import_elements[task][VERTEX].count ; n++ )
    { REAL *x = get_coord(import_elements[task][VERTEX].id_list[n]);
      for ( i = 0 ; i < SDIM ; i++ )
         x[i] = ((REAL*)bigbuf)[SDIM*n+i];
    }
  }  

  MPI_Barrier(MPI_COMM_WORLD);
}



/****************************************************************************
*
* function: mpi_convert_forms_to_vectors(mode)
*
* purpose: called by root process to convert forms on nodes
*
*/

void mpi_convert_forms_to_vectors(mode)
int mode;
{
  message.cmd = mpi_CONVERT_FORMS;
  message.mode = mode;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_convert_forms_to_vectors()
*
* purpose: local form conversion
*/

void mpi_node_convert_forms_to_vectors(mode)
int mode;
{
  local_convert_forms_to_vectors(mode);
  if ( mpi_debug )
    printf("Task %d done local convert forms\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD);
}


/****************************************************************************
*
* function: mpi_lagrange_adjust()
*
* purpose: called by root process to adjust velocities by Lagrange multipliers
*
*/

void mpi_lagrange_adjust(fixcount)
{
  message.cmd = mpi_LAGRANGE_ADJUST;
  message.count = fixcount;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Bcast(vpressures,fixcount*sizeof(REAL),MPI_BYTE,0,MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_lagrange_adjust()
*
* purpose: local adjustment to velocity with Lagrange multipliers
*/

void mpi_node_lagrange_adjust(fixcount)
int fixcount;
{ 
  vpressures = (REAL*)temp_calloc(fixcount,sizeof(REAL));
  MPI_Bcast(vpressures,fixcount*sizeof(REAL),MPI_BYTE,0,MPI_COMM_WORLD);
  pressure_set_flag = 1;
  local_lagrange_adjust();
  if ( mpi_debug )
    printf("Task %d done local lagrange_adjust\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD);
  temp_free((char*)vpressures); vpressures = NULL;
}


/****************************************************************************
*
* function: mpi_save_coords()
*
* purpose: called by root process to save coodinates on nodes
*
*/

void mpi_save_coords(mode)
int mode;
{
  message.cmd = mpi_SAVE_COORDS;
  message.mode = mode;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_save_coords()
*
* purpose: local coodinate saving
*/

struct oldcoord psaved;

void mpi_node_save_coords(mode)
int mode;
{
  local_save_coords((mode==SAVE_SEPARATE?&psaved:&saved),mode);
  if ( mpi_debug )
    printf("Task %d done local save_coords\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD);
}




/****************************************************************************
*
* function: mpi_restore_coords()
*
* purpose: called by root process to restore coodinates on nodes
*
*/

void mpi_restore_coords(mode)
int mode;
{
  message.cmd = mpi_RESTORE_COORDS;
  message.mode = mode;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_restore_coords()
*
* purpose: local coodinate saving
*/

void mpi_node_restore_coords(mode)
int mode;
{
  local_restore_coords((mode==SAVE_SEPARATE?&psaved:&saved),mode);
  if ( mpi_debug )
    printf("Task %d done local restore_coords\n",this_machine);
  mpi_node_synch_coord();  /* has own barrier */
  /* MPI_Barrier(MPI_COMM_WORLD); */
}




/****************************************************************************
*
* function: mpi_unsave_coords()
*
* purpose: called by root process to unsave coodinates on nodes
*
*/

void mpi_unsave_coords(mode)
int mode;
{
  message.cmd = mpi_UNSAVE_COORDS;
  message.mode = mode;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_unsave_coords()
*
* purpose: end local coodinate saving; free volgrads
*/

struct oldcoord psaved;

void mpi_node_unsave_coords(mode)
int mode;
{
  local_unsave_coords((mode==SAVE_SEPARATE?&psaved:&saved),mode);
  if ( mpi_debug )
    printf("Task %d done local unsave_coords\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD);
}


/****************************************************************************
*
* function: mpi_update_display()
*
* purpose: called by root process to tell nodes to refresh displays
*
*/

void mpi_update_display()
{
  message.cmd = mpi_UPDATE_DISPLAY;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}


/****************************************************************************
*
* function: mpi_temp_free_all()
*
* purpose: called by root process to tell nodes to refresh displays
*
*/

void mpi_temp_free_all()
{
  message.cmd = mpi_TEMP_FREE_ALL;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/*****************************************************************************
     MPI functions for 'r' global refining.
*/

struct mpi_ref {
    int alloc; /* triples */
    int count; /* of triples */
    char *buf; /* old id, newv data, newe data */
} *mpi_ref_data;

/***************************************************************************
*
* function: mpi_refine_edge()
*
* purpose: Collect data about refining one edge to be exported.
*          Called from edge_divide().
*
*/

void mpi_refine_edge(old_edge,new_v,new_e,task)
edge_id old_edge;
vertex_id new_v;
edge_id new_e;
int task; /* destination task */
{ 
  char *spot;
  int triple_size = web.sizes[VERTEX]+2*web.sizes[EDGE];

  if ( task == this_machine ) return;

  if ( mpi_debug )
    printf("Task %d mpi_refine_edge(%s,%s,%s,%d)\n",this_machine,
      ELNAME(old_edge),ELNAME1(new_v),ELNAME2(new_e),task);
  
  mpi_add_export(new_v,task);
  mpi_add_export(new_e,task);
  if ( mpi_ref_data[task].count*triple_size >= mpi_ref_data[task].alloc )
  { mpi_ref_data[task].buf = temp_realloc(mpi_ref_data[task].buf,
        (mpi_ref_data[task].count*2+1)*triple_size);
    mpi_ref_data[task].alloc = (mpi_ref_data[task].count*2+1)*triple_size;
  }
  spot = mpi_ref_data[task].buf + mpi_ref_data[task].count*triple_size;
  memcpy(spot,eptr(old_edge),web.sizes[EDGE]);
  spot += web.sizes[EDGE]; 
  memcpy(spot,vptr(new_v),web.sizes[VERTEX]);
  spot += web.sizes[VERTEX];
  memcpy(spot,eptr(new_e),web.sizes[EDGE]);
  mpi_ref_data[task].count++;
}

/***************************************************************************
*
* function: mpi_refine()
*
* purpose: Does 'r' refine across all tasks.
*
*/

void mpi_refine()
{ int done_count = 0;
  MPI_Request request;
  int task;
  
  if ( web.modeltype != LINEAR )
    kb_error(3334,"MPI refining only works in LINEAR model so far.\n",
      RECOVERABLE);

  mpi_dissolve_corona();

  message.cmd = mpi_EDGE_REFINE;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  while ( done_count < nprocs-1 )
  { MPI_Status status;
    
    MPI_Recv(&message,sizeof(message),MPI_BYTE,MPI_ANY_SOURCE,REFINE_TAG,
      MPI_COMM_WORLD, &status);

    if ( mpi_debug )
      printf("Task %d got done message from task %d\n",this_machine,
         status.MPI_SOURCE);
    done_count++;
  }
  message.cmd = mpi_DONE;
  if ( mpi_debug )
    printf("Task %d sending global done.\n",this_machine);
  for ( task = 1 ; task < nprocs ; task++ )
    MPI_Isend(&message,sizeof(message),MPI_BYTE,task,REFINE_TAG,MPI_COMM_WORLD,
       &request);

  MPI_Barrier(MPI_COMM_WORLD); /* edge sync barrier */
  if ( mpi_debug )
    printf("Task %d waiting at final barrier\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD); /* final barrier */

}

/***************************************************************************
*
* function: mpi_node_refine()
*
* purpose: Does 'r' refine on this node.
*
*/

void mpi_node_refine()
{ int task;
    
  mpi_ref_data = (struct mpi_ref*)temp_calloc(sizeof(struct mpi_ref),
                      nprocs);
  for ( task = 0 ; task < nprocs ; task++ )
    if ( export_elements[task][EDGE].count > 0 )
    { mpi_ref_data[task].alloc = export_elements[task][EDGE].count
                    *(web.sizes[VERTEX]+2*web.sizes[EDGE]);
      mpi_ref_data[task].buf = temp_calloc(1,mpi_ref_data[task].alloc);
    }

  if ( mpi_debug )
    printf("Task %d calling local_refine()\n",this_machine);
  local_refine();

  if ( mpi_debug )
    printf("Task %d waiting at final barrier.\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD);

  for ( task = 0 ; task < nprocs ; task++ )
    if ( mpi_ref_data[task].buf )
      temp_free((char*)mpi_ref_data[task].buf);
  temp_free((char*)mpi_ref_data); mpi_ref_data = NULL;
}

/**************************************************************************
*
* function: mpi_refine_edge_synch()
*
* purpose: synchronize refined edges among nodes in the middle of refine()
*/
void mpi_refine_edge_synch()
{ int task;
  int i;
  int done_flag;
  int mgs_sent = 0; /* for counting acknowledgements */
  MPI_Request request;
  int triple_size = web.sizes[VERTEX]+2*web.sizes[EDGE];
  struct mpi_command *messages = 
    (struct mpi_command*)temp_calloc(sizeof (struct mpi_command),nprocs);
  
  if ( mpi_debug )
    printf ("Task %d refine edge synch\n",this_machine);
  for ( task = 0 ; task < nprocs ; task++ )
  { int count = mpi_ref_data[task].count;
    if ( count == 0 ) continue;
    messages[task].cmd = mpi_EDGE_REFINE;
    messages[task].count = count;
  
    if ( mpi_debug )
       printf("Task %d sending %d edge refines to task %d\n",this_machine,
           count,task);

    MPI_Isend(&messages[task],sizeof(message),MPI_BYTE,task,REFINE_TAG,
        MPI_COMM_WORLD,&request);
    MPI_Isend(mpi_ref_data[task].buf,count*triple_size,
          MPI_BYTE,task,REFINE_TAG,MPI_COMM_WORLD, &request);
    mgs_sent++;
  }

  if ( mgs_sent == 0 )
  { struct mpi_command done;
    done.cmd = mpi_DONE;
    MPI_Isend(&done,sizeof(done),MPI_BYTE,0,REFINE_TAG,MPI_COMM_WORLD,
      &request);
  }

  /* get incoming data */
  done_flag = 0;
  while ( !done_flag )
  { MPI_Status status;
    MPI_Request request;
    char *buf;
    MPI_Recv(&message,sizeof(message),MPI_BYTE,MPI_ANY_SOURCE,REFINE_TAG,
       MPI_COMM_WORLD,&status);
    switch ( message.cmd )
    { case mpi_ACKNOWLEDGE: 
         mgs_sent--;
         if ( mgs_sent == 0 )
         { struct mpi_command done;
           done.cmd = mpi_DONE;
           MPI_Isend(&done,sizeof(done),MPI_BYTE,0,REFINE_TAG,MPI_COMM_WORLD,
              &request);
         }
         break;

      case mpi_DONE: /* from root, signalling all done */
         done_flag = 1;
         break;

      case mpi_EDGE_REFINE:
         task = status.MPI_SOURCE;
         buf = temp_calloc(triple_size,message.count);
         MPI_Recv(buf,triple_size*message.count,MPI_BYTE,
              task,REFINE_TAG,MPI_COMM_WORLD,&status);
         if ( mpi_debug )
           printf("Task %d got edge refines from task %d\n",this_machine,task);
         message.cmd = mpi_ACKNOWLEDGE;
         MPI_Isend(&message,sizeof(message),MPI_BYTE,task,REFINE_TAG,
            MPI_COMM_WORLD,&request);
         for ( i = 0 ; i < message.count ; i++ )
         { /* duplicate the refinement locally */
           char *spot = buf + i*triple_size; 
           struct edge *oldep = (struct edge *)(spot);
           struct vertex *new_vp = (struct vertex *)(spot+web.sizes[EDGE]);
           struct edge *new_ep = 
                      (struct edge *)(spot+web.sizes[EDGE]+web.sizes[VERTEX]);
           edge_id e_id = oldep->self_id;
           vertex_id divider = new_vp->self_id;
           edge_id new_e = new_ep->self_id;   
           vertex_id headv = get_edge_headv(e_id);
           vertex_id tailv = get_edge_tailv(e_id);
           facetedge_id fe,start_fe,prev;
 
           mpi_add_import(new_e);
           mpi_add_import(divider);
          
           mpi_add_remote_element((struct element*)oldep);
           mpi_add_remote_element((struct element*)new_vp);
           mpi_add_remote_element((struct element*)new_ep);
           
           set_vertex_edge(tailv,e_id);
           set_vertex_edge(divider,new_e);
           set_vertex_edge(headv,inverse_id(new_e));   

           /* fix up fe's */
           start_fe = get_edge_fe(e_id);
           fe = start_fe;
           prev = NULLID;
           if ( mpi_debug )
             printf(
               "Task %d importing edge refine olde %s divider %s new_e %s\n",
             this_machine,ELNAME(e_id),ELNAME1(divider),ELNAME2(new_e));
           do 
           { facetedge_id newfe = new_facetedge(get_fe_facet(fe),new_e);
             facetedge_id ffe = get_next_edge(fe);
             set_next_edge(fe,newfe);
             set_prev_edge(newfe,fe);
             set_next_edge(newfe,ffe);
             set_prev_edge(ffe,newfe);
             if ( valid_id(prev) )
             { ffe = get_next_facet(prev);
               set_prev_facet(newfe,prev);
               set_next_facet(prev,newfe);
               set_next_facet(newfe,ffe);
               set_prev_facet(ffe,newfe);
             }
             else 
             { set_prev_facet(newfe,newfe);
               set_next_facet(newfe,newfe);
             }
             prev = newfe;
             fe = get_next_facet(fe);
           } while ( !equal_id(start_fe,fe) );
           set_edge_fe(new_e,prev);
           /* prep for make_vedge_lists() */
           set_next_tail_edge(e_id,e_id);    
           set_next_tail_edge(divider,divider);    
           set_next_tail_edge(inverse_id(e_id),inverse_id(e_id));
           set_next_tail_edge(inverse_id(divider),inverse_id(divider));    
        }
        temp_free(buf); buf = NULL;
        break;
      
    }
  }  
  make_vedge_lists(); /* get connections all spiffed up */
  temp_free((char*)messages);
  MPI_Barrier(MPI_COMM_WORLD);
  
}

/*
    End 'r' functions.
***************************************************************************/

/**************************************************************************
  Content calculation.
*/

/**************************************************************************
*
* function: mpi_calc_content()
*
* purpose: root task procedure which controls other nodes.
*          Tells nodes to calculate local contribution to volumes
*          and quantities, then accumulates.
*
*/
char *content_buf;
int content_bufsize;

void mpi_calc_content(mode)
int mode;
{ int done_count = 0;
  int k;
  char *spot;
  int bufsize;
  
  message.cmd = mpi_CALC_CONTENT;
  message.mode = mode;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);

  for ( k = 0 ; k < gen_quant_count ; k++ )
  { struct gen_quant *q = GEN_QUANT(k);
    if ( q->flags & mode )
    {
      q->value = q->volconst;
      q->abstotal = 0.0;
      q->timestamp = global_timestamp;
    }
  }

  while ( done_count < nprocs-1 )
  { MPI_Status status;
    if ( mpi_debug )
       printf("Task %d waiting...\n",this_machine);   
    MPI_Recv(&message,sizeof(message),MPI_BYTE,MPI_ANY_SOURCE,CONTENT_TAG,
         MPI_COMM_WORLD, &status);
    if ( mpi_debug )
       printf("Task %d got %d content data from task %d\n",this_machine,
    message.count,status.MPI_SOURCE);
    /* Get contributions */
    bufsize = message.count*(3*sizeof(REAL));
    if ( bufsize > content_bufsize )
    { content_buf = kb_realloc(content_buf,bufsize);
      content_bufsize = bufsize;
    }
    MPI_Recv(content_buf,bufsize,MPI_BYTE,status.MPI_SOURCE,CONTENT_TAG,
         MPI_COMM_WORLD,&status);
    for ( k = 0, spot = content_buf ; k < message.count ; k++ )
    { body_id b_id = *(body_id*)spot;
      spot += sizeof(REAL);  /* for alignment sensitive architectures */
      if ( valid_id(b_id) )
      { add_body_volume_plain(b_id,*(REAL*)spot);
        add_body_abstotal(b_id,((REAL*)spot)[1]-fabs(*(REAL*)spot)); 
        spot += 2*sizeof(REAL);
      }
      else /* quantity */
      { struct gen_quant *q = GEN_QUANT(b_id);
        q->value += *(REAL*)spot;
        spot += sizeof(REAL);
        q->abstotal += *(REAL*)spot;
        spot += sizeof(REAL);
      }
    }
    done_count++;
  }

  if ( mpi_debug )
    printf("Task %d waiting at final barrier\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD); /* final barrier */
}


/***************************************************************************
*
* function: mpi_node_calc_content()
*
* purpose: Does content on this node and synchronizes with other nodes.
*
*/

void mpi_node_calc_content(mode)
int mode;
{ char *spot;
  int k;
  body_id b_id;
  int bufsize;
  MPI_Request request;
  
  FOR_ALL_BODIES(b_id)
    set_body_volume(b_id,0.0,NOSETSTAMP);

  if ( mpi_debug )
    printf("Task %d calling local_calc_content()\n",this_machine);
  local_calc_content(mode);

  /* now send to root task */
  message.count = 0;
  spot = content_buf;
  FOR_ALL_BODIES(b_id)
  { REAL vol = get_body_volume(b_id);
    REAL avol = get_body_abstotal(b_id);
    if ( vol == 0.0 && avol == 0.0 ) continue;

    message.count++;
    bufsize = message.count*(3*sizeof(REAL));
    if ( bufsize > content_bufsize )
    { int newsize = (bufsize < 1000) ? 1000 : 2*bufsize;
      int offset = spot-content_buf;
      content_buf = kb_realloc(content_buf,newsize);
      content_bufsize = newsize;
      spot = content_buf + offset;
    }
    *(body_id*)spot = b_id;
    spot += sizeof(REAL); /* for alignment sensitive architectures */
    *(REAL*)spot = vol;
    spot += sizeof(REAL);
    *(REAL*)spot = get_body_abstotal(b_id);
    spot += sizeof(REAL);
  }
  for ( k = 0 ; k < gen_quant_count ; k++ )
  { struct gen_quant *q = GEN_QUANT(k);
    if ( q->flags & mode )
    { if ( q->value == 0.0 && q->abstotal == 0.0 ) continue;
      message.count++;
      bufsize = message.count*(3*sizeof(REAL));
      if ( bufsize > content_bufsize )
      { int newsize = (bufsize < 1000) ? 1000 : 2*bufsize;
        int offset = spot-content_buf;
        content_buf = kb_realloc(content_buf,newsize);
        content_bufsize = newsize;
        spot = content_buf + offset;
      }
      *(body_id*)spot = k;
      spot += sizeof(REAL); /* for alignment sensitive architectures */
      *(REAL*)spot = q->value;
      spot += sizeof(REAL);
      *(REAL*)spot = q->abstotal;
      spot += sizeof(REAL);
    }
  }
  MPI_Isend(&message,sizeof(message),MPI_BYTE,0,
     CONTENT_TAG,MPI_COMM_WORLD,&request);
  MPI_Isend(content_buf,message.count*3*sizeof(REAL),MPI_BYTE,0,
     CONTENT_TAG,MPI_COMM_WORLD,&request);
 
  if ( mpi_debug )
    printf("Task %d waiting at final barrier.  Sent %d contents.\n",
      this_machine, message.count);
  MPI_Barrier(MPI_COMM_WORLD);

}




/**************************************************************************
   Volgrad synchronization
*/


/***************************************************************************
*
* function: mpi_calc_volgrads()
*
* purpose: Does calc_volgrads refine across all tasks.
*
*/

void mpi_calc_volgrads(mode)
int mode;
{ int done_count = 0;
  MPI_Request request;
  int task;
  
  message.cmd = mpi_CALC_VOLGRADS;
  message.mode = mode;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  while ( done_count < nprocs-1 )
  { MPI_Status status;
    
    MPI_Recv(&message,sizeof(message),MPI_BYTE,MPI_ANY_SOURCE,VOLGRAD_TAG,
       MPI_COMM_WORLD,&status);
    if ( mpi_debug )
      printf("Task %d got done message from task %d\n",this_machine,
         status.MPI_SOURCE);
    done_count++;
  }
  message.cmd = mpi_DONE;
  if ( mpi_debug )
    printf("Task %d sending global done.\n",this_machine);
  for ( task = 1 ; task < nprocs ; task++ )
    MPI_Isend(&message,sizeof(message),MPI_BYTE,task,VOLGRAD_TAG,MPI_COMM_WORLD,
       &request);

  if ( mpi_debug )
    printf("Task %d waiting at final barrier\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD); /* final barrier */
}

/***************************************************************************
*
* function: mpi_node_calc_volgrads()
*
* purpose: Does volgrads refine on this node and synchronizes with other nodes.
*
*/

void mpi_node_calc_volgrads(mode)
int mode;
{     
  if ( mpi_debug )
    printf("Task %d calling local_calc_volgrads()\n",this_machine);
  local_calc_volgrads(mode);
  mpi_synch_volgrads();

  if ( mpi_debug )
    printf("Task %d waiting at final barrier.\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD);

}

/**************************************************************************
*
* function: mpi_synch_volgrads()
*
* purpose: Synchronize volgrad grads among nodes.  
*          To be called in each task at end of grad calculation.
*/

void mpi_synch_volgrads()
{ int task;
  int i,k;
  int done_flag;
  int mgs_sent = 0; /* for counting acknowledgements */
  MPI_Request request;
  int vg_size = sizeof(vertex_id)+sizeof(int)+SDIM*sizeof(REAL);
  struct mpi_command *messages = 
    (struct mpi_command*)temp_calloc(sizeof (struct mpi_command),nprocs);
  char **vgrad_bufs = (char**)temp_calloc(sizeof(char *),nprocs);
  
  if ( mpi_debug )
    printf ("Task %d volgrad synch\n",this_machine);

  for ( task = 0 ; task < nprocs ; task++ )
  { int count;
    struct import_list_t *t = &import_elements[task][VERTEX];
    char *spot;
    
    /* count volgrads */
    for ( i = 0, count = 0 ; i < t->count ; i++ )
    { volgrad *vgptri;
  
      vgptri = get_vertex_vgrad(t->id_list[i]);
      while ( vgptri )
      { count++;
        vgptri = vgptri->chain;
      }
    }
    if ( count == 0 ) continue;
    vgrad_bufs[task] = temp_calloc(count,vg_size);

    /* gather volgrads for imported elements */
    for ( i = 0, spot = vgrad_bufs[task] ; i < t->count ; i++ )
    { volgrad *vgptri;
  
      vgptri = get_vertex_vgrad(t->id_list[i]);
      while ( vgptri )
      { *(vertex_id*)spot = t->id_list[i];
        spot += sizeof(vertex_id);
        *(int*)spot = vgptri->fixnum;
        spot += sizeof(int);
        for ( k = 0 ; k < SDIM ; k++ )
        { *(REAL*)spot = vgptri->grad[k];
          spot += sizeof(REAL);
        }
        vgptri = vgptri->chain;
      }
    }

    messages[task].cmd = mpi_VOLGRAD_SYNCH;
    messages[task].count = count;
  
    if ( mpi_debug )
     printf("Task %d sending %d volgrads to task %d\n",this_machine,count,task);

    MPI_Isend(&messages[task],sizeof(message),MPI_BYTE,task,VOLGRAD_TAG,
        MPI_COMM_WORLD,&request);
    MPI_Isend(vgrad_bufs[task],count*vg_size,
          MPI_BYTE,task,VOLGRAD_TAG,MPI_COMM_WORLD, &request);
    mgs_sent++;
  }

  if ( mgs_sent == 0 )
  { struct mpi_command done;
    done.cmd = mpi_DONE;
    MPI_Isend(&done,sizeof(done),MPI_BYTE,0,VOLGRAD_TAG,MPI_COMM_WORLD,
      &request);
  }

  /* get incoming data */
  done_flag = 0;
  while ( !done_flag )
  { MPI_Status status;
    MPI_Request request;
    char *buf;
    MPI_Recv(&message,sizeof(message),MPI_BYTE,MPI_ANY_SOURCE,VOLGRAD_TAG,
       MPI_COMM_WORLD,&status);
    switch ( message.cmd )
    { case mpi_ACKNOWLEDGE: 
         mgs_sent--;
         if ( mgs_sent == 0 )
         { struct mpi_command done;
           done.cmd = mpi_DONE;
           MPI_Isend(&done,sizeof(done),MPI_BYTE,0,VOLGRAD_TAG,MPI_COMM_WORLD,
              &request);
         }
         break;

      case mpi_DONE: /* from root, signalling all done */
         done_flag = 1;
         break;

      case mpi_VOLGRAD_SYNCH:
         task = status.MPI_SOURCE;
         buf = temp_calloc(vg_size,message.count);
         MPI_Recv(buf,vg_size*message.count,MPI_BYTE,
              task,VOLGRAD_TAG,MPI_COMM_WORLD,&status);
         if ( mpi_debug )
           printf("Task %d got edge refines from task %d\n",this_machine,task);
         message.cmd = mpi_ACKNOWLEDGE;
         MPI_Isend(&message,sizeof(message),MPI_BYTE,task,VOLGRAD_TAG,
             MPI_COMM_WORLD, &request);
         for ( i = 0 ; i < message.count ; i++ )
         { /* add to local volgrads */
           char *spot = buf + i*vg_size; 
           vertex_id v_id;
           int fixnum;
           REAL *grad;
           volgrad *vgptr;
           
           v_id = *(vertex_id*)spot;
           spot += sizeof(vertex_id);
           fixnum = *(int*)spot;
           spot += sizeof(int);
           grad = (REAL*)spot;
           vgptr = get_bv_new_vgrad(fixnum,v_id);
           for ( k = 0 ; k < SDIM ; k++ )
             vgptr->grad[k] += grad[k];
         }
         temp_free(buf); buf = NULL;
         break;
      
    }
  }  
  temp_free((char*)messages);
  for ( task = 0 ; task < nprocs ; task++ )
    temp_free((char*)vgrad_bufs[task]);
  temp_free((char*)vgrad_bufs);
  
}

/****************************************************************************
*
* function: mpi_vgrad_end()
*
* purpose: tell nodes to run vgrad_end()
*/

void mpi_vgrad_end()
{
  message.cmd = mpi_VGRAD_END;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
}

/*
    End volgrad functions.
***************************************************************************/

/**************************************************************************
*
* function: mpi_calc_leftside()
*
* purpose: calculate left side of constraint Gramm matrix.
*          Called in root task.
*/

void mpi_calc_leftside(fixcount)
{ int done_count = 0;
  int i,j;
  REAL **buf = dmatrix(0,fixcount,0,fixcount);
  int bufsize = sizeof(REAL)*(fixcount+1)*(fixcount+1);
 
  message.cmd = mpi_CALC_LEFTSIDE;
  message.count = fixcount;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);

  while ( done_count < nprocs-1 )
  { MPI_Status status;
    if ( mpi_debug )
      printf("Task %d waiting...\n",this_machine);   
    MPI_Recv(buf[0],bufsize,MPI_BYTE,MPI_ANY_SOURCE,LEFTSIDE_TAG,
         MPI_COMM_WORLD,&status);
    if ( mpi_debug )
      printf("Task %d got leftside data from task %d\n",this_machine,
        status.MPI_SOURCE);
    for ( i = 0 ; i < fixcount ; i++ )
      for ( j = 0 ; j < fixcount ; j++ )
        rleftside[i][j] += buf[i][j];
    done_count++;
  }
  free_matrix(buf);

  if ( mpi_debug )
    printf("Task %d waiting at final barrier\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD); /* final barrier */
}

/***************************************************************************
*
* function: mpi_node_calc_leftside()
*
* purpose: Does constraint grad Gramm matrix on node and sends
*          Gramm matrix back to root task.
*
*/

void mpi_node_calc_leftside()
{
  int bufsize;
  MPI_Request request;
  int fixcount;
  
  fixcount = find_fixed();
  rleftside = dmatrix(0,fixcount,0,fixcount);
  bufsize = sizeof(REAL)*(fixcount+1)*(fixcount+1);

  if ( mpi_debug )
    printf("Task %d calling local_calc_leftside()\n",this_machine);
  local_calc_leftside();

  /* now send to root task */
  MPI_Isend(rleftside[0],bufsize,MPI_BYTE,0,
     LEFTSIDE_TAG,MPI_COMM_WORLD,&request);

  if ( mpi_debug )
    printf("Task %d waiting at final barrier.  Sent %d contents.\n",
      this_machine, message.count);
  MPI_Barrier(MPI_COMM_WORLD);
  free_matrix(rleftside); rleftside = NULL;
}


/**************************************************************************
*
* function: mpi_calc_rightside()
*
* purpose: calculate right side for Lagrange multipliers.
*          Called in root task.
*/

void mpi_calc_rightside(fixcount)
{ int done_count = 0;
  int i;
  REAL *buf = (REAL*)temp_calloc(fixcount,sizeof(REAL));
  int bufsize = sizeof(REAL)*fixcount;
 
  message.cmd = mpi_CALC_RIGHTSIDE;
  message.count = fixcount;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);

  while ( done_count < nprocs-1 )
  { MPI_Status status;
    if ( mpi_debug )
       printf("Task %d waiting...\n",this_machine);   
    MPI_Recv(buf,bufsize,MPI_BYTE,MPI_ANY_SOURCE,RIGHTSIDE_TAG,
         MPI_COMM_WORLD,&status);
    if ( mpi_debug )
      printf("Task %d got leftside data from task %d\n",this_machine,
        status.MPI_SOURCE);
    for ( i = 0 ; i < fixcount ; i++ )
      rightside[i] += buf[i];
    done_count++;
  }
  temp_free((char*)buf);

  if ( mpi_debug )
    printf("Task %d waiting at final barrier\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD); /* final barrier */
}

/***************************************************************************
*
* function: mpi_node_calc_rightside()
*
* purpose: Does task part of Lagrange multiplier right side.
*          Sends results back to root task.
*
*/

void mpi_node_calc_rightside(fixcount)
int fixcount;
{
  int bufsize;
  MPI_Request request;
  
  rightside = (REAL*)temp_calloc(fixcount,sizeof(REAL));
  bufsize = sizeof(REAL)*fixcount;

  if ( mpi_debug )
    printf("Task %d calling local_calc_leftside()\n",this_machine);
  local_calc_rightside();

  /* now send to root task */
  MPI_Isend(rightside,bufsize,MPI_BYTE,0,
     RIGHTSIDE_TAG,MPI_COMM_WORLD,&request);

  if ( mpi_debug )
    printf("Task %d waiting at final barrier.  Sent %d contents.\n",
      this_machine, message.count);
  MPI_Barrier(MPI_COMM_WORLD);
  temp_free((char*)rightside); rightside = NULL;

}

/***************************************************************************
*
* function: mpi_volume_restore()
*
* purpose: Gets tasks to do volume restore motions of vertices.
*/
void mpi_volume_restore(maxquants,stepsize,mode)
int maxquants;
REAL stepsize;
int mode;
{
  message.cmd = mpi_VOLUME_RESTORE;
  message.count = maxquants;
  message.mode = mode;
  message.t = stepsize;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Bcast(vol_restore,maxquants*sizeof(REAL),MPI_BYTE,0,MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/***************************************************************************
*
* function: mpi_node_volume_restore()
*
* purpose: handles local volume restoration.
*/
void mpi_node_volume_restore(maxquants,stepsize,mode)
int maxquants;
REAL stepsize;
int mode;
{
  vol_restore = (REAL*)temp_calloc(maxquants,sizeof(REAL));
  MPI_Bcast(vol_restore,maxquants*sizeof(REAL),MPI_BYTE,0,MPI_COMM_WORLD);
  local_volume_restore(stepsize,mode);
  global_timestamp++; /* to force recalc of content */
  mpi_node_synch_coord();  /* has own barrier */
  temp_free((char*)vol_restore); vol_restore = NULL;
}


/****************************************************************************
*
* function: mpi_count_report()
*
* purpose: implement 'c' command in MPI version.
*/

struct count_report_t {
  int vertices;
  int edges;
  int facets;
  int bodies;
  int facetedges;
  double memory;

} count_report;

void mpi_count_report()
{ int n;
  struct count_report_t report;
  MPI_Status status;
  
  memset(&count_report,0,sizeof(count_report));
  message.cmd = mpi_COUNT_REPORT;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  for ( n = 0 ; n < nprocs-1 ; n++ )
  { MPI_Recv(&report,sizeof(report),MPI_BYTE,MPI_ANY_SOURCE,REPORT_TAG,
      MPI_COMM_WORLD,&status);
    count_report.vertices += report.vertices;
    count_report.edges    += report.edges   ;
    count_report.facets += report.facets;
    count_report.bodies += report.bodies;
    count_report.facetedges += report.facetedges;
    count_report.memory += report.memory;
  } 

  sprintf(msg,
    "Vertices: %ld  Edges: %ld  Facets: %ld  Bodies: %ld  Facetedges: %ld\n",
    count_report.vertices,count_report.edges,count_report.facets,
    web.skel[BODY].count,count_report.facetedges);
  outstring(msg);

  sprintf(msg,"Element memory: %15g  \n",count_report.memory);
  outstring(msg);
}

/****************************************************************************
*
* function: mpi_node_report()
*
* purpose: node-side part of 'c' command.
*/
void mpi_node_report()
{
    int k;

    count_report.vertices = web.skel[VERTEX].count;
    count_report.edges    = web.skel[EDGE].count;
    count_report.facets   = web.skel[FACET].count;
    count_report.bodies   = web.skel[BODY].count;
    count_report.facetedges = web.skel[FACETEDGE].count;
    count_report.memory = 0;
    for ( k = 0 ; k < NUMELEMENTS ; k++ )
      count_report.memory += web.skel[k].count*web.sizes[k];
    MPI_Send(&count_report,sizeof(count_report),MPI_BYTE,0,REPORT_TAG,
        MPI_COMM_WORLD);
}

/*****************************************************************************
   HESSIAN STUFF.  
*****************/

struct aij { int i,j; REAL a; };  /* hessian entry */

int aijcomp(a,b)
struct aij *a,*b;
{ if ( a->i < b->i ) return -1;
  if ( a->i > b->i ) return  1;
  if ( a->j < b->j ) return -1;
  if ( a->j > b->j ) return  1;
  return 0;
}

/*****************************************************************************
*
* function: mpi_hessian_fill(S,rhs)
*
* purpose: root function to get hessian entries back to root.
*/
int *task_hess_rows;
int *task_hess_displacements;
int local_rows;
int local_start,local_end;
   
void mpi_hessian_fill(S,rhsptr)
struct linsys *S;
REAL **rhsptr;
{
  int n,i,k,keep,aspot;
  MPI_Status status;
  int local_rows = 0,local_end;
  struct aij *aijlist;
  int na[2],atot[2]; /* [0] for number of rows, [1] for number of entries */
 
  mpi_synch_pressures();
  mpi_synch_elements(FAT_CORONA);  /* get corona for normals */
  message.cmd = mpi_HESSIAN_FILL;
  message.mode = (rhsptr && *rhsptr) ? 1 : 0;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Scan(&local_rows,&local_end,1,MPI_INTEGER,MPI_SUM,MPI_COMM_WORLD);

  /* let root know how many local rows are in each node */
  na[0] = na[1] = 0;
  if ( !task_hess_rows )
  { task_hess_rows = (int*)mycalloc(nprocs,sizeof(int));
    task_hess_displacements = (int*)mycalloc(nprocs,sizeof(int));
  }
  MPI_Gather(na,1,MPI_INTEGER,task_hess_rows,1,MPI_INTEGER,0,
     MPI_COMM_WORLD);
  for ( i = 0 ; i < nprocs-1 ; i++ )
   task_hess_displacements[i+1] = task_hess_displacements[i]+task_hess_rows[i];

  MPI_Allreduce(&na,&atot,2,MPI_INTEGER,MPI_SUM,MPI_COMM_WORLD);
  aijlist = (struct aij*)calloc(atot[1],sizeof(struct aij));
  if ( rhsptr )
  { if ( *rhsptr ) temp_free((char*)(*rhsptr));
    *rhsptr = (REAL *)temp_calloc(atot[0]+S->total_rows,sizeof(REAL));
  }

  aspot = 0;
  /* get entries and right hand side */
  for ( n = 0 ; n < nprocs-1 ; n++ )
  {
    MPI_Recv(&message,sizeof(message),MPI_BYTE,MPI_ANY_SOURCE,ROWNUMS_TAG,
      MPI_COMM_WORLD, &status);
    MPI_Recv(aijlist+aspot,message.size*sizeof(struct aij),MPI_BYTE,
        status.MPI_SOURCE,ROWNUMS_TAG,MPI_COMM_WORLD,&status);
    check_bigbufsize(message.count*sizeof(struct aij));
    if ( rhsptr )
    {
      if ( mpi_debug )
        printf("task %d awaiting rhs\n",this_machine);
      MPI_Recv(bigbuf,message.count*sizeof(struct aij),MPI_BYTE,
        status.MPI_SOURCE,ROWNUMS_TAG,MPI_COMM_WORLD,&status);
      for ( i = 0 ; i < message.count ; i++ )
      { struct aij *a = ((struct aij *)bigbuf) + i;
        (*rhsptr)[a->i] += a->a;
      }
    }
    aspot += message.size;
  }
  if ( mpi_debug )
    printf("task %d at barrier\n",this_machine);
  MPI_Barrier(MPI_COMM_WORLD);
    
  /* sort aijlist */
  qsort((char*)aijlist,atot[1],sizeof(struct aij),FCAST aijcomp);
  /* unify */
  for ( i = 1, keep = 0 ; i < atot[1] ; i++ )
  { if ( (aijlist[i].i==aijlist[keep].i) && (aijlist[i].j==aijlist[keep].j) )
      aijlist[keep].a += aijlist[i].a;
    else 
      aijlist[++keep] = aijlist[i];
  }
  atot[1] = keep+1;
 
  /* update root S structure */
  /* root S->total_rows etc start as just quantities etc */
  S->optparamrowstart += atot[0];
  S->N = S->A_rows = S->bodyrowstart += atot[0];
  S->quanrowstart += atot[0];
  S->total_rows += atot[0];
  S->maxN = S->total_rows;
  S->IA = (int*)temp_calloc(S->N+1,sizeof(int));
  S->JA = (int*)temp_calloc(atot[1],sizeof(int)); 
  S->A  = (REAL*)temp_calloc(atot[1],sizeof(REAL)); 
  
  /* fill in entries */
  S->IA[0] = A_OFF;
  for ( k = 0, i = 0 ; k < atot[1] ; k++ )
  { S->JA[k] = aijlist[k].j + A_OFF;
    S->A[k]  = aijlist[k].a;
    if ( aijlist[k].i != i )
      S->IA[++i] = k + A_OFF;   
  }
  S->IA[++i] = atot[1] + A_OFF;
  
  if ( S->P ) temp_free((char*)S->P);
  S->P = (int *)temp_calloc(S->total_rows,sizeof(int));
  if ( S->IP ) temp_free((char*)S->IP);
  S->IP = (int *)temp_calloc(S->total_rows,sizeof(int));
}

/*****************************************************************************
*
* function: mpi_node_hessian_fill()
*
* purpose: supply root with hessian entries.
*/

void mpi_node_hessian_fill(node_rhs_flag)
int node_rhs_flag;
{  int n,i,j;
   REAL *rhs = NULL;
   struct linsys S;

   int row;
   struct hess_verlist *vh;
   MPI_Request request;
   int nbrs_left, nbr_task_count=0;
   int na[2];
   int atot[2];  /* total vertex rows, total A entries */
   struct aij *aijlist=NULL;
   struct aij *rhslist=NULL;
   int *AItrans;
   int task;
   vertex_id v_id;

   /* get local hessian entries */
   hmode = hessian_normal_flag;
   hessian_cleanup(); /* cleanup from previous round */
   hessian_init(&S,&rhs);
   hess_flag = 1; rhs_flag = node_rhs_flag;
   hessian_fill(&S,&rhs);

   /* send back to root */
   
   /* set up global numbering for rows */
   /* Find out how many local rows */
   local_rows = 0;
   FOR_ALL_VERTICES(v_id)
    local_rows += get_vertex_vhead(v_id)->freedom;
   MPI_Scan(&local_rows,&local_end,1,MPI_INTEGER,MPI_SUM,MPI_COMM_WORLD);
   local_start = local_end - local_rows;
   row = local_start;
   FOR_ALL_VERTICES(v_id)
   { vh =  get_vertex_vhead(v_id);
     vh->global_rownum = row;
     row += vh->freedom;
   }
   
   /* let root know how many local rows are in each node */
   na[0] = local_rows;
   MPI_Gather(na,1,MPI_INTEGER,NULL,1,MPI_INTEGER,0,MPI_COMM_WORLD);

   /* let everybody know how many total entries to expect */
   na[1] = S.IA[S.N];
   MPI_Allreduce(&na,&atot,2,MPI_INTEGER,MPI_SUM,MPI_COMM_WORLD);
    
   /* send row numbers for exported vertices */  
   for ( task = 0 ; task < nprocs ; task++ )
   { struct export_list_t *et = &export_elements[task][VERTEX];
     for ( i = 0 ; i < et->count ; i++ )   
       ((int *)(et->buf))[i] = get_vertex_vhead(et->id_list[i])->global_rownum;
     if ( i == 0 ) continue;
     MPI_Isend(et->buf,i*sizeof(int),MPI_BYTE,task,ROWNUMS_TAG,MPI_COMM_WORLD,
       &request);
   }

  /* get imported row numbers */
  for ( task = 0, nbr_task_count = 0 ; task < nprocs ; task++ )
  { struct import_list_t *it = &import_elements[task][VERTEX];
    if ( it->count ) nbr_task_count++;
    check_bigbufsize(it->count*sizeof(int));
  }
  for ( nbrs_left = nbr_task_count ; nbrs_left > 0 ; nbrs_left-- )
  { MPI_Status status;
    struct import_list_t *it;
    MPI_Recv(bigbuf,bigbufsize,MPI_BYTE,MPI_ANY_SOURCE,ROWNUMS_TAG,
       MPI_COMM_WORLD,&status);
    task = status.MPI_SOURCE;
    it = &import_elements[task][VERTEX];
    memcpy(it->buf,bigbuf,it->count*sizeof(int));
    if ( mpi_debug )
      printf("Task %d got rownums from task %d\n",this_machine,task);
    for ( n = 0 ; n < import_elements[task][VERTEX].count ; n++ )
    { vh = get_vertex_vhead(it->id_list[n]);
      vh->global_rownum = ((int*)(it->buf))[n];
    }
  }  

  /* fill in global AI translation list */
  AItrans = (int*)temp_calloc(S.maxN+1,sizeof(int));
  for ( i = 0 ; i < vhead_count ; i++ )
  { for ( j = 0 ; j < vhead[i].freedom ; j++ )
      AItrans[vhead[i].rownum+j] = vhead[i].global_rownum+j;
  }
  /* and optimizing parameters and bodies and fixed quantities */
  for ( i = S.optparamrowstart ; i < S.maxN ; i++ )
      AItrans[i] = atot[0] + i - S.optparamrowstart;

  /* now send to root */
  message.count = S.N;  /* degrees of freedom in this node */
  message.size = S.IA[S.N]; /* elements of A */
  MPI_Isend(&message,sizeof(message),MPI_BYTE,0,ROWNUMS_TAG,MPI_COMM_WORLD,
     &request);
  /* assemble entry list */
  aijlist = (struct aij*)temp_calloc(S.IA[S.N],sizeof(struct aij));
  for ( i = 0 ; i < S.N ; i++ )
  { for ( j = S.IA[i]-A_OFF ; j < S.IA[i+1]-A_OFF ; j++ )
    { struct aij *a = aijlist+j;
      a->i = AItrans[i];
      a->j = AItrans[S.JA[j]-A_OFF];
      if ( a->i > a->j )
      { int tmp = a->i; a->i = a->j; a->j = tmp; }
      a->a = S.A[j];
    }
  }
  if ( mpi_debug )
    printf("Task %d sending A\n",this_machine);
  MPI_Isend(aijlist,S.IA[S.N]*sizeof(struct aij),MPI_BYTE,0,ROWNUMS_TAG,
     MPI_COMM_WORLD,&request);
  if (rhs_flag )
  { /* send rhs */
    rhslist = (struct aij*)temp_calloc(S.N,sizeof(struct aij));
    for ( i = 0 ; i < S.N ; i++ )
    { rhslist[i].i = AItrans[i];
      rhslist[i].a = rhs[i];
    }
    if ( mpi_debug )
      printf("Task %d Sending rhslist\n",this_machine);
    MPI_Isend(rhslist,S.N*sizeof(struct aij),MPI_BYTE,0,ROWNUMS_TAG,
       MPI_COMM_WORLD,&request);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  temp_free((char*)aijlist);
  temp_free((char*)AItrans);
  if ( rhslist ) temp_free((char*)rhslist);
  if ( rhs ) temp_free((char*)rhs);
  /* hessian_cleanup(); don't do yet; need vhead */
  free_system(&S);
}

/*****************************************************************************
*
* function: mpi_hessian_move()
*
* purpose: Handle root part of moving vertices in tasks for hessian commands.
*/
void mpi_hessian_move(stepsize,mode,X)
REAL stepsize;
int mode;
REAL *X;  /* direction */
{
  message.cmd = mpi_HESSIAN_MOVE;
  message.mode = mode;
  message.t = stepsize;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  if ( mpi_debug )
    printf("task %d scatter X\n",this_machine);
  MPI_Scatterv(X,task_hess_rows,task_hess_displacements,MPI_REAL,bigbuf,0,
     MPI_REAL,0,MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/*****************************************************************************
* 
* function: mpi_node_hessian_move()
*
* purpose: Handle task moving vertices in tasks for hessian commands.
*/
void mpi_node_hessian_move(stepsize,mode)
REAL stepsize;
int mode;
{ REAL *X;
  vertex_id v_id;
  
  /* get proper part of X */
  check_bigbufsize(local_rows*sizeof(REAL));
  if ( mpi_debug )
    printf("Task %d awaiting X\n",this_machine);
  MPI_Scatterv(NULL,NULL,NULL,MPI_REAL,bigbuf,local_rows,
     MPI_REAL,0,MPI_COMM_WORLD);
  if ( mpi_debug )
    printf("Task %d got X\n",this_machine);
  /* translate between global_rownum and rownum */
  X = (REAL*)temp_calloc(vhead_count,sizeof(REAL));
  FOR_ALL_VERTICES(v_id)
  { struct hess_verlist *vh = get_vertex_vhead(v_id);
    X[vh->rownum] = ((REAL*)bigbuf)[vh->global_rownum-local_start];
  }
  hessian_move(stepsize,mode,X);
  temp_free((char*)X);
  mpi_node_synch_coord(); /* has own barrier */ 
}

/**************************************************************************
*
* function: mpi_synch_pressures()
*
* purpose: Root tells nodes what body and quantity pressures are.
*          Nodes need to know in order to calculate constrained hessian.
*/

void mpi_synch_pressures()
{
  REAL *spot;
  int i;
  body_id b_id;

  message.cmd = mpi_SYNCH_PRESSURES;
  message.count = web.skel[BODY].count + gen_quant_count;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  check_bigbufsize(message.count*sizeof(REAL));
  spot = (REAL*)bigbuf;
  FOR_ALL_BODIES(b_id)
    *(spot++) = get_body_pressure(b_id);
  for ( i = 0 ; i < gen_quant_count ; i++ )
    *(spot++) = GEN_QUANT(i)->pressure;
  MPI_Bcast(bigbuf,message.count*sizeof(REAL),MPI_BYTE, 0, MPI_COMM_WORLD);
}

/**************************************************************************
*
* function: mpi_node_synch_pressures()
*
* purpose: Nodes find out what body and quantity pressures are.
*          Nodes need to know in order to calculate constrained hessian.
*/

void mpi_node_synch_pressures()
{
  REAL *spot;
  int i;
  body_id b_id;

  if ( message.count != web.skel[BODY].count + gen_quant_count )
    kb_error(3911,"mpi_node_synch_pressure - disagreement on number of bodies",
       RECOVERABLE);
  check_bigbufsize(message.count*sizeof(REAL));
  MPI_Bcast(bigbuf,message.count*sizeof(REAL),MPI_BYTE, 0, MPI_COMM_WORLD);
  spot = (REAL*)bigbuf;
  FOR_ALL_BODIES(b_id)
  { set_body_pressure(b_id,*spot);
    spot++;
  }
  for ( i = 0 ; i < gen_quant_count ; i++ )
    GEN_QUANT(i)->pressure = *(spot++); 
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/****************************************************************************
* 
* function: mpi_synch_mpi_debug()
*
* purpose: sychronize mpi_debug among nodes.
*/

void mpi_synch_mpi_debug()
{
  message.cmd = mpi_DEBUG;
  message.mode = mpi_debug;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
}

/***************************************************************************
* 
* function: mpi_node_checks()
*
* purpose: run checks on integrity of mpi version data structures.
*          called from run_check() from 'C' command.
*/

int mpi_node_checks()
{
  int type,task;
  int numerr = 0;

  if ( this_machine > 0 )
   for ( type = VERTEX ; type < NUMELEMENTS ; type++ )
    for ( task = 0 ; task < nprocs ; task++ )
      if ( remote_elements[type].count[task] !=
             import_elements[task][type].count )
      { printf("Task %d %s from %d remote %d import %d\n", this_machine,
           typenames[type],task,remote_elements[type].count[task],
             import_elements[task][type].count);
        numerr++;
      }
  return numerr;
}
            

/****************************************************************************
*
* function: mpi_vertex_average()
*
* purpose: called by root process to do vertex averaging on nodes
*          and resynch coords.
*
*/

void mpi_vertex_average(mode)
int mode;
{
  message.cmd = mpi_VERTEX_AVERAGE;
  message.mode = mode;
  MPI_Bcast(&message,sizeof(struct mpi_command),MPI_BYTE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
}

/****************************************************************************
*
* function: mpi_node_vertex_average()
*
* purpose: node process to do vertex averaging on nodes
*          and resynch coords.
*
*/

void mpi_node_vertex_average(mode)
int mode;
{
  vertex_average(mode);
  mpi_node_synch_coord();  /* has own barrier */

}
