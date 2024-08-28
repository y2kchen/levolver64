/*************************************************************
*  This file is part of the Surface Evolver source code.     *
*  Programmer:  Ken Brakke, brakke@susqu.edu                 *
*************************************************************/

/**************************************************************
*
* file: mpi_kb.h
*
* contents: My own definitions in regard to MPI (copied from pvm_kb.h)
*/

extern int count;  /* array size */
struct mpi_command { 
                 int cmd;  /* see defines */ 
                 int task; /* task meant for, if just one */
                 int count;  /* total elements */
                 int type ;  /* type of element */
                 int size;   /* size of element structure */
                 int mode;   /* type of quantity */
                 int i;      /* misc. data */
                 int j;      /* misc. data */
                 REAL t;     /* misc. data */
     };
/* commands */
#define mpi_QUIT       9
#define mpi_PAR_EXEC      10
#define mpi_CALC_ENERGY 11
#define mpi_CALC_FORCE 12
#define mpi_MOVE_VERTICES 13
#define mpi_CONVERT_FORMS 14
#define mpi_SAVE_COORDS 15
#define mpi_RESTORE_COORDS 16
#define mpi_UNSAVE_COORDS 17
#define mpi_UPDATE_DISPLAY 18
#define mpi_EDGE_REFINE 19
#define mpi_ACKNOWLEDGE 20
#define mpi_DONE 21
#define mpi_NODE_EXEC 22
#define mpi_VOLGRAD_SYNCH 23
#define mpi_CALC_VOLGRADS 24
#define mpi_CALC_CONTENT  25
#define mpi_CALC_LEFTSIDE 26
#define mpi_VOLUME_RESTORE 27
#define mpi_CALC_RIGHTSIDE 28
#define mpi_LAGRANGE_ADJUST 29
#define mpi_TEMP_FREE_ALL 30
#define mpi_COUNT_REPORT 31
#define mpi_HESSIAN_FILL 32
#define mpi_HESSIAN_MOVE 33
#define mpi_SYNCH_CORONA 34
#define mpi_DISSOLVE_CORONA 35
#define mpi_SYNCH_ELEMENTS 36
#define mpi_VGRAD_END      37
#define mpi_SYNCH_PRESSURES 38
#define mpi_DEBUG  39
#define mpi_VERTEX_AVERAGE 40

/* tag values for messages */
#define INFO_TAG     0x200
#define IDLIST_TAG   0x201
#define RESPONSE_TAG 0x202
#define FORCES_TAG   0x203
#define COORDS_TAG   0x204
#define REFINE_TAG   0x205
#define EXEC_TAG     0x206
#define VOLGRAD_TAG  0x207
#define CONTENT_TAG  0x208
#define LEFTSIDE_TAG 0x209
#define RIGHTSIDE_TAG 0x209
#define REPORT_TAG   0x210
#define ROWNUMS_TAG  0x211
#define CORONA_TAG   0x212
#define EXIM_TAG     0x213
#define PRESSURES_TAG 0x214

/* a smaller info structure */
struct message_info {
     int type; /* see defines below */
     int count;   /* number of elements in list */
     int size;   /* to verify element structure size */
     int etype;  /* element type */
};

#define ID_REQUEST    0x1000
#define ID_RESPONSE   0x1001
#define DONE_REQUESTS 0x1002
#define EXIM_INFO     0x1003
extern char *infotypes[];
struct message_info *infos; /* enough to use simultaneously in Isends */



/* Accessing export nbr attribute */
extern int mpi_export_voffset;
extern int mpi_export_eoffset;
extern int mpi_export_foffset;
extern int mpi_export_boffset;
extern int mpi_export_feoffset;
#define mpi_export_attr_ptr(id)  (unsigned short int*)((char*)elptr(id) + \
   EXTRAS(id_type(id))[web.mpi_export_attr[id_type(id)]].offset)
#define mpi_export_vattr_ptr(id)  (unsigned short int*)((char*)vptr(id) + \
   mpi_export_voffset)
#define mpi_export_eattr_ptr(id)  (unsigned short int*)((char*)eptr(id) + \
   mpi_export_eoffset)
#define mpi_export_fattr_ptr(id)  (unsigned short int*)((char*)fptr(id) + \
   mpi_export_foffset)
#define mpi_export_battr_ptr(id)  (unsigned short int*)((char*)bptr(id) + \
   mpi_export_boffset)
#define mpi_export_feattr_ptr(id)  (unsigned short int*)((char*)feptr(id) + \
   mpi_export_feoffset)

