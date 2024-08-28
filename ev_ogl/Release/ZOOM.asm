; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\levolver\yonkang-sefit\levolver\ev_ogl\ZOOM.C
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_set_attr
EXTRN	_web:BYTE
; Function compile flags: /Ogtp
; File c:\levolver\yonkang-sefit\levolver\ev_ogl\inline.h
;	COMDAT _set_attr
_TEXT	SEGMENT
_id$ = 8						; size = 4
_attrib$ = 12						; size = 8
_set_attr PROC						; COMDAT

; 21   : {

	push	ebp
	mov	ebp, esp

; 22   :   elptr(id)->attr |= attrib;

	mov	eax, DWORD PTR _id$[ebp]
	mov	ecx, eax
	shr	ecx, 29					; 0000001dH
	imul	ecx, 112				; 00000070H
	mov	edx, DWORD PTR _web[ecx+12]
	mov	ecx, DWORD PTR _attrib$[ebp]
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edx+eax*4]
	mov	edx, DWORD PTR _attrib$[ebp+4]
	or	DWORD PTR [eax+8], ecx
	or	DWORD PTR [eax+12], edx

; 23   : }

	pop	ebp
	ret	0
_set_attr ENDP
_TEXT	ENDS
PUBLIC	_get_fe_edge
; Function compile flags: /Ogtp
;	COMDAT _get_fe_edge
_TEXT	SEGMENT
_fe_id$ = 8						; size = 4
_get_fe_edge PROC					; COMDAT

; 43   : {

	push	ebp
	mov	ebp, esp

; 44   :   edge_id e_id;
; 45   :     
; 46   :   e_id = feptr(fe_id)->fe_edge_id;

	mov	eax, DWORD PTR _fe_id$[ebp]

; 47   : 
; 48   :   /*
; 49   :   if ( inverted(fe_id) ) invert(e_id);
; 50   :   return e_id;
; 51   :   */
; 52   :   
; 53   :   return same_sign(e_id,fe_id);

	mov	edx, DWORD PTR _web+460
	mov	ecx, eax
	and	ecx, 134217727				; 07ffffffH
	mov	ecx, DWORD PTR [edx+ecx*4]
	and	eax, 134217728				; 08000000H
	xor	eax, DWORD PTR [ecx+20]

; 54   : }

	pop	ebp
	ret	0
_get_fe_edge ENDP
_TEXT	ENDS
PUBLIC	_get_fe_facet
EXTRN	_NULLFACET:DWORD
; Function compile flags: /Ogtp
;	COMDAT _get_fe_facet
_TEXT	SEGMENT
_fe_id$ = 8						; size = 4
_get_fe_facet PROC					; COMDAT

; 58   : {

	push	ebp
	mov	ebp, esp

; 59   :   facet_id f_id;
; 60   :   
; 61   :   if ( !valid_id(fe_id) ) return NULLFACET;

	mov	ecx, DWORD PTR _fe_id$[ebp]
	test	ecx, 268435456				; 10000000H
	jne	SHORT $LN2@get_fe_fac
	mov	eax, DWORD PTR _NULLFACET

; 64   :   return f_id;
; 65   : }

	pop	ebp
	ret	0
$LN2@get_fe_fac:

; 62   :   f_id = feptr(fe_id)->fe_facet_id;

	mov	edx, DWORD PTR _web+460
	mov	eax, ecx
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edx+eax*4]
	mov	eax, DWORD PTR [eax+24]

; 63   :   if ( inverted(fe_id) ) invert(f_id);

	test	ecx, 134217728				; 08000000H
	je	SHORT $LN3@get_fe_fac
	xor	eax, 134217728				; 08000000H
$LN3@get_fe_fac:

; 64   :   return f_id;
; 65   : }

	pop	ebp
	ret	0
_get_fe_facet ENDP
_TEXT	ENDS
PUBLIC	_get_next_edge
; Function compile flags: /Ogtp
;	COMDAT _get_next_edge
_TEXT	SEGMENT
_fe_id$ = 8						; size = 4
_get_next_edge PROC					; COMDAT

; 76   : {

	push	ebp
	mov	ebp, esp

; 77   :   if ( inverted(fe_id) ) return inverse_id(feptr(fe_id)->nextedge[0]);

	mov	eax, DWORD PTR _fe_id$[ebp]
	mov	ecx, DWORD PTR _web+460
	test	eax, 134217728				; 08000000H
	je	SHORT $LN2@get_next_e
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR [edx+28]
	xor	eax, 134217728				; 08000000H

; 79   : }

	pop	ebp
	ret	0
$LN2@get_next_e:

; 78   :   else return feptr(fe_id)->nextedge[1];

	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR [edx+32]

; 79   : }

	pop	ebp
	ret	0
_get_next_edge ENDP
_TEXT	ENDS
PUBLIC	_get_prev_facet
; Function compile flags: /Ogtp
;	COMDAT _get_prev_facet
_TEXT	SEGMENT
_fe_id$ = 8						; size = 4
_get_prev_facet PROC					; COMDAT

; 83   : {

	push	ebp
	mov	ebp, esp

; 84   :   if ( inverted(fe_id) ) 

	mov	eax, DWORD PTR _fe_id$[ebp]

; 85   :     return inverse_id(feptr(fe_id)->nextfacet[1]);

	mov	ecx, DWORD PTR _web+460
	test	eax, 134217728				; 08000000H
	je	SHORT $LN2@get_prev_f
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR [edx+40]
	xor	eax, 134217728				; 08000000H

; 88   : }

	pop	ebp
	ret	0
$LN2@get_prev_f:

; 86   :   else 
; 87   :     return feptr(fe_id)->nextfacet[0];

	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR [edx+36]

; 88   : }

	pop	ebp
	ret	0
_get_prev_facet ENDP
_TEXT	ENDS
PUBLIC	_get_next_facet
; Function compile flags: /Ogtp
;	COMDAT _get_next_facet
_TEXT	SEGMENT
_fe_id$ = 8						; size = 4
_get_next_facet PROC					; COMDAT

; 92   : {

	push	ebp
	mov	ebp, esp

; 93   :   if ( inverted(fe_id) ) 

	mov	eax, DWORD PTR _fe_id$[ebp]

; 94   :     return inverse_id(feptr(fe_id)->nextfacet[0]);

	mov	ecx, DWORD PTR _web+460
	test	eax, 134217728				; 08000000H
	je	SHORT $LN2@get_next_f
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR [edx+36]
	xor	eax, 134217728				; 08000000H

; 97   : }

	pop	ebp
	ret	0
$LN2@get_next_f:

; 95   :   else 
; 96   :     return feptr(fe_id)->nextfacet[1];

	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR [edx+40]

; 97   : }

	pop	ebp
	ret	0
_get_next_facet ENDP
_TEXT	ENDS
PUBLIC	_set_prev_facet
EXTRN	_top_timestamp:DWORD
EXTRN	_global_timestamp:DWORD
; Function compile flags: /Ogtp
;	COMDAT _set_prev_facet
_TEXT	SEGMENT
_fe_id$ = 8						; size = 4
_fe$ = 12						; size = 4
_set_prev_facet PROC					; COMDAT

; 129  : {

	push	ebp
	mov	ebp, esp

; 130  :   if ( !valid_id(fe_id) ) return;

	mov	eax, DWORD PTR _fe_id$[ebp]
	test	eax, 268435456				; 10000000H
	je	SHORT $LN4@set_prev_f

; 131  :   if ( inverted(fe_id) )

	test	eax, 134217728				; 08000000H
	je	SHORT $LN2@set_prev_f

; 132  :   { invert(fe);
; 133  :     feptr(fe_id)->nextfacet[1] = fe;

	mov	edx, DWORD PTR _web+460
	mov	ecx, DWORD PTR _fe$[ebp]
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edx+eax*4]
	xor	ecx, 134217728				; 08000000H
	mov	DWORD PTR [eax+40], ecx

; 137  :   top_timestamp = ++global_timestamp;

	mov	eax, DWORD PTR _global_timestamp
	inc	eax
	mov	DWORD PTR _global_timestamp, eax
	mov	DWORD PTR _top_timestamp, eax

; 138  : }

	pop	ebp
	ret	0
$LN2@set_prev_f:

; 134  :   }
; 135  :   else
; 136  :     feptr(fe_id)->nextfacet[0] = fe;

	mov	ecx, DWORD PTR _web+460
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _fe$[ebp]
	mov	DWORD PTR [edx+36], eax

; 137  :   top_timestamp = ++global_timestamp;

	mov	eax, DWORD PTR _global_timestamp
	inc	eax
	mov	DWORD PTR _global_timestamp, eax
	mov	DWORD PTR _top_timestamp, eax
$LN4@set_prev_f:

; 138  : }

	pop	ebp
	ret	0
_set_prev_facet ENDP
_TEXT	ENDS
PUBLIC	_set_next_facet
; Function compile flags: /Ogtp
;	COMDAT _set_next_facet
_TEXT	SEGMENT
_fe_id$ = 8						; size = 4
_fe$ = 12						; size = 4
_set_next_facet PROC					; COMDAT

; 143  : {

	push	ebp
	mov	ebp, esp

; 144  :   if ( !valid_id(fe_id) ) return;

	mov	eax, DWORD PTR _fe_id$[ebp]
	test	eax, 268435456				; 10000000H
	je	SHORT $LN1@set_next_f

; 145  :   if ( inverted(fe_id) )

	test	eax, 134217728				; 08000000H
	je	SHORT $LN2@set_next_f

; 146  :   { invert(fe);

	mov	ecx, DWORD PTR _fe$[ebp]

; 147  :     feptr(fe_id)->nextfacet[0] = fe;

	mov	edx, DWORD PTR _web+460
	xor	ecx, 134217728				; 08000000H
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edx+eax*4]
	mov	DWORD PTR [eax+36], ecx

; 151  : }

	pop	ebp
	ret	0
$LN2@set_next_f:

; 148  :   }
; 149  :   else
; 150  :     feptr(fe_id)->nextfacet[1] = fe;

	mov	ecx, DWORD PTR _web+460
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _fe$[ebp]
	mov	DWORD PTR [edx+40], eax
$LN1@set_next_f:

; 151  : }

	pop	ebp
	ret	0
_set_next_facet ENDP
_TEXT	ENDS
PUBLIC	_set_edge_fe
; Function compile flags: /Ogtp
;	COMDAT _set_edge_fe
_TEXT	SEGMENT
_e_id$ = 8						; size = 4
_fe$ = 12						; size = 4
_set_edge_fe PROC					; COMDAT

; 171  : {

	push	ebp
	mov	ebp, esp

; 172  :   if ( inverted(e_id) ) invert(fe);

	mov	eax, DWORD PTR _e_id$[ebp]
	test	eax, 134217728				; 08000000H
	je	SHORT $LN1@set_edge_f
	xor	DWORD PTR _fe$[ebp], 134217728		; 08000000H
$LN1@set_edge_f:

; 173  :   eptr(e_id)->fe_id = fe;

	mov	ecx, DWORD PTR _web+124
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _fe$[ebp]
	mov	DWORD PTR [edx+28], eax

; 174  :   top_timestamp = ++global_timestamp;

	mov	eax, DWORD PTR _global_timestamp
	inc	eax
	mov	DWORD PTR _global_timestamp, eax
	mov	DWORD PTR _top_timestamp, eax

; 175  : }

	pop	ebp
	ret	0
_set_edge_fe ENDP
_TEXT	ENDS
PUBLIC	_get_edge_tailv
EXTRN	_dymem:DWORD
; Function compile flags: /Ogtp
;	COMDAT _get_edge_tailv
_TEXT	SEGMENT
_e_id$ = 8						; size = 4
_get_edge_tailv PROC					; COMDAT

; 191  : {

	push	ebp
	mov	ebp, esp

; 192  :   if ( inverted(e_id) )

	mov	eax, DWORD PTR _e_id$[ebp]

; 193  :      return get_edge_vertices(e_id)[web.headvnum];

	mov	ecx, DWORD PTR _web+124
	test	eax, 134217728				; 08000000H
	je	SHORT $LN2@get_edge_t
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _web+216
	mov	ecx, DWORD PTR _dymem
	add	edx, DWORD PTR [eax+ecx+304]
	mov	eax, DWORD PTR _web+636
	mov	eax, DWORD PTR [edx+eax*4]

; 196  : }

	pop	ebp
	ret	0
$LN2@get_edge_t:

; 194  :   else
; 195  :      return get_edge_vertices(e_id)[0];

	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _web+216
	mov	ecx, DWORD PTR _dymem
	mov	eax, DWORD PTR [eax+ecx+304]
	mov	eax, DWORD PTR [edx+eax]

; 196  : }

	pop	ebp
	ret	0
_get_edge_tailv ENDP
_TEXT	ENDS
PUBLIC	_get_edge_headv
; Function compile flags: /Ogtp
;	COMDAT _get_edge_headv
_TEXT	SEGMENT
_e_id$ = 8						; size = 4
_get_edge_headv PROC					; COMDAT

; 200  : {

	push	ebp
	mov	ebp, esp

; 201  :   if ( inverted(e_id) )

	mov	eax, DWORD PTR _e_id$[ebp]

; 202  :      return get_edge_vertices(e_id)[0];

	mov	ecx, DWORD PTR _web+124
	test	eax, 134217728				; 08000000H
	je	SHORT $LN2@get_edge_h
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _web+216
	mov	ecx, DWORD PTR _dymem
	mov	eax, DWORD PTR [eax+ecx+304]
	mov	eax, DWORD PTR [edx+eax]

; 205  : }

	pop	ebp
	ret	0
$LN2@get_edge_h:

; 203  :   else
; 204  :      return get_edge_vertices(e_id)[web.headvnum];

	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _web+216
	mov	ecx, DWORD PTR _dymem
	add	edx, DWORD PTR [eax+ecx+304]
	mov	eax, DWORD PTR _web+636
	mov	eax, DWORD PTR [edx+eax*4]

; 205  : }

	pop	ebp
	ret	0
_get_edge_headv ENDP
_TEXT	ENDS
PUBLIC	_get_facet_fe
; Function compile flags: /Ogtp
;	COMDAT _get_facet_fe
_TEXT	SEGMENT
_f_id$ = 8						; size = 4
_get_facet_fe PROC					; COMDAT

; 260  : {

	push	ebp
	mov	ebp, esp

; 261  :   facetedge_id fe;
; 262  :   
; 263  :   if ( !valid_id(f_id) ) return NULLID;

	mov	ecx, DWORD PTR _f_id$[ebp]
	test	ecx, 268435456				; 10000000H
	jne	SHORT $LN2@get_facet_
	xor	eax, eax

; 266  :   return fe;
; 267  : }

	pop	ebp
	ret	0
$LN2@get_facet_:

; 264  :   fe = fptr(f_id)->fe_id;

	mov	edx, DWORD PTR _web+236
	mov	eax, ecx
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edx+eax*4]
	mov	eax, DWORD PTR [eax+28]

; 265  :   if ( inverted(f_id) ) invert(fe);

	test	ecx, 134217728				; 08000000H
	je	SHORT $LN3@get_facet_
	xor	eax, 134217728				; 08000000H
$LN3@get_facet_:

; 266  :   return fe;
; 267  : }

	pop	ebp
	ret	0
_get_facet_fe ENDP
_TEXT	ENDS
PUBLIC	??_C@_0BJ@EPDMMGFA@Vertex?5?$CFs?5is?5not?5valid?4?6?$AA@ ; `string'
PUBLIC	??_C@_00CNPNBAHC@?$AA@				; `string'
PUBLIC	??_C@_02DPKJAMEF@?$CFd?$AA@			; `string'
PUBLIC	??_C@_0CE@MPMHOJEC@Must?5have?5positive?5cut?9off?5radiu@ ; `string'
PUBLIC	__real@0000000000000000
PUBLIC	??_C@_0CF@BEFGGPEA@Zoom?5is?5not?5implemented?5for?5bodi@ ; `string'
PUBLIC	_zoom_vertex
EXTRN	_free_element:PROC
EXTRN	_NULLFACETEDGE:DWORD
EXTRN	_distance:PROC
EXTRN	_errmsg:BYTE
EXTRN	_sprintf:PROC
EXTRN	_elnames:BYTE
EXTRN	_valid_element:PROC
EXTRN	_kb_error:PROC
EXTRN	__fltused:DWORD
;	COMDAT ??_C@_0BJ@EPDMMGFA@Vertex?5?$CFs?5is?5not?5valid?4?6?$AA@
CONST	SEGMENT
??_C@_0BJ@EPDMMGFA@Vertex?5?$CFs?5is?5not?5valid?4?6?$AA@ DB 'Vertex %s i'
	DB	's not valid.', 0aH, 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_00CNPNBAHC@?$AA@
CONST	SEGMENT
??_C@_00CNPNBAHC@?$AA@ DB 00H				; `string'
CONST	ENDS
;	COMDAT ??_C@_02DPKJAMEF@?$CFd?$AA@
CONST	SEGMENT
??_C@_02DPKJAMEF@?$CFd?$AA@ DB '%d', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_0CE@MPMHOJEC@Must?5have?5positive?5cut?9off?5radiu@
CONST	SEGMENT
??_C@_0CE@MPMHOJEC@Must?5have?5positive?5cut?9off?5radiu@ DB 'Must have p'
	DB	'ositive cut-off radius.', 0aH, 00H		; `string'
CONST	ENDS
;	COMDAT __real@0000000000000000
CONST	SEGMENT
__real@0000000000000000 DQ 00000000000000000r	; 0
CONST	ENDS
;	COMDAT ??_C@_0CF@BEFGGPEA@Zoom?5is?5not?5implemented?5for?5bodi@
CONST	SEGMENT
??_C@_0CF@BEFGGPEA@Zoom?5is?5not?5implemented?5for?5bodi@ DB 'Zoom is not'
	DB	' implemented for bodies.', 0aH, 00H		; `string'
; Function compile flags: /Ogtp
; File c:\levolver\yonkang-sefit\levolver\ev_ogl\zoom.c
CONST	ENDS
;	COMDAT _zoom_vertex
_TEXT	SEGMENT
tv1722 = -12						; size = 4
tv1613 = -8						; size = 4
tv1766 = -4						; size = 4
tv1725 = 8						; size = 4
tv1620 = 8						; size = 4
_prev$ = 8						; size = 4
_i$ = 8							; size = 4
_v_id$ = 8						; size = 4
tv1890 = 12						; size = 8
tv1709 = 12						; size = 8
tv1616 = 12						; size = 8
_radius$ = 12						; size = 8
tv1610 = 16						; size = 4
_zoom_vertex PROC					; COMDAT

; 28   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 12					; 0000000cH

; 29   :   vertex_id vv_id;
; 30   :   edge_id e_id;
; 31   :   facet_id f_id;
; 32   :   facetedge_id fe_id,next,prev;
; 33   :   int i; 
; 34   : 
; 35   :   if ( (web.bodycount != 0) && (web.representation == SOAPFILM) )

	cmp	DWORD PTR _web+1500, 0
	je	SHORT $LN62@zoom_verte
	cmp	DWORD PTR _web+624, 2
	jne	SHORT $LN62@zoom_verte

; 36   :      kb_error(1540,"Zoom is not implemented for bodies.\n",RECOVERABLE);

	push	1
	push	OFFSET ??_C@_0CF@BEFGGPEA@Zoom?5is?5not?5implemented?5for?5bodi@
	push	1540					; 00000604H
	call	_kb_error
	add	esp, 12					; 0000000cH
$LN62@zoom_verte:

; 37   : 
; 38   :   if ( radius <= 0.0 )

	fldz
	fcomp	QWORD PTR _radius$[ebp]
	fnstsw	ax
	test	ah, 1
	jne	SHORT $LN61@zoom_verte

; 39   :      kb_error(1541,"Must have positive cut-off radius.\n",RECOVERABLE);

	push	1
	push	OFFSET ??_C@_0CE@MPMHOJEC@Must?5have?5positive?5cut?9off?5radiu@
	push	1541					; 00000605H
	call	_kb_error
	add	esp, 12					; 0000000cH
$LN61@zoom_verte:

; 40   : 
; 41   :   if ( !valid_id(v_id) || !valid_element(v_id) )

	push	ebx
	mov	ebx, DWORD PTR _v_id$[ebp]
	push	esi
	mov	esi, ebx
	shr	esi, 28					; 0000001cH
	and	esi, 1
	push	edi
	je	SHORT $LN59@zoom_verte
	push	ebx
	call	_valid_element
	add	esp, 4
	test	eax, eax
	jne	SHORT $LN60@zoom_verte
$LN59@zoom_verte:

; 42   :      { sprintf(errmsg,"Vertex %s is not valid.\n",ELNAME(v_id));

	test	esi, esi
	je	SHORT $LN65@zoom_verte
	mov	eax, ebx
	and	eax, 134217727				; 07ffffffH
	inc	eax
	push	eax
	push	OFFSET ??_C@_02DPKJAMEF@?$CFd?$AA@
	push	OFFSET _elnames
	call	_sprintf
	add	esp, 12					; 0000000cH
	mov	eax, OFFSET _elnames
	jmp	SHORT $LN66@zoom_verte
$LN65@zoom_verte:
	mov	eax, OFFSET ??_C@_00CNPNBAHC@?$AA@
$LN66@zoom_verte:
	push	eax
	push	OFFSET ??_C@_0BJ@EPDMMGFA@Vertex?5?$CFs?5is?5not?5valid?4?6?$AA@
	push	OFFSET _errmsg
	call	_sprintf

; 43   :        kb_error(1542,errmsg,RECOVERABLE);

	push	1
	push	OFFSET _errmsg
	push	1542					; 00000606H
	call	_kb_error
	add	esp, 24					; 00000018H
$LN60@zoom_verte:

; 44   :      }
; 45   : 
; 46   :   /* eliminate all vertices beyond cutoff */
; 47   :   FOR_ALL_VERTICES(vv_id)

	mov	edi, DWORD PTR _web+48
	test	edi, 268435456				; 10000000H
	je	SHORT $LN177@zoom_verte
	npad	10
$LL58@zoom_verte:
	mov	ecx, DWORD PTR _web+12
	mov	esi, edi
	and	esi, 134217727				; 07ffffffH
	add	esi, esi
	add	esi, esi
	mov	eax, DWORD PTR [esi+ecx]
	mov	eax, DWORD PTR [eax+8]
	and	eax, 1
	xor	ecx, ecx
	or	eax, ecx
	je	SHORT $LN57@zoom_verte

; 48   :      { 
; 49   :         if ( distance(v_id,vv_id) > radius )

	push	edi
	push	ebx
	call	_distance
	fcomp	QWORD PTR _radius$[ebp]
	add	esp, 8
	fnstsw	ax
	test	ah, 65					; 00000041H
	jne	SHORT $LN57@zoom_verte

; 50   :           set_attr(vv_id,DISSOLVED);

	shr	edi, 29					; 0000001dH
	imul	edi, 112				; 00000070H
	mov	edx, DWORD PTR _web[edi+12]
	mov	eax, DWORD PTR [edx+esi]
	mov	ecx, DWORD PTR [eax+12]
	or	DWORD PTR [eax+8], 33554432		; 02000000H
	mov	DWORD PTR [eax+12], ecx
$LN57@zoom_verte:

; 44   :      }
; 45   : 
; 46   :   /* eliminate all vertices beyond cutoff */
; 47   :   FOR_ALL_VERTICES(vv_id)

	mov	edx, DWORD PTR _web+12
	mov	eax, DWORD PTR [esi+edx]
	mov	edi, DWORD PTR [eax]
	test	edi, 268435456				; 10000000H
	jne	SHORT $LL58@zoom_verte
$LN177@zoom_verte:

; 51   :      }
; 52   : 
; 53   :   /* eliminate all edges connected to gone vertices */
; 54   :   FOR_ALL_EDGES(e_id)

	mov	ecx, DWORD PTR _web+160
	test	ecx, 268435456				; 10000000H
	je	$LN178@zoom_verte
	mov	eax, DWORD PTR _web+124
	npad	5
$LL186@zoom_verte:
	mov	ebx, DWORD PTR _web+216
	mov	esi, ecx
	and	esi, 134217727				; 07ffffffH
	add	esi, esi
	add	esi, esi
	mov	edx, DWORD PTR [esi+eax]
	mov	eax, DWORD PTR [edx+8]
	and	eax, 1
	xor	edi, edi
	or	eax, edi
	je	$LN51@zoom_verte

; 55   :      { int val_head,val_tail;
; 56   :         
; 57   :         val_tail = !(get_vattr(get_edge_tailv(e_id)) & DISSOLVED);

	mov	eax, ecx
	shr	eax, 27					; 0000001bH
	and	eax, 1
	mov	DWORD PTR tv1620[ebp], eax
	mov	eax, DWORD PTR _dymem
	mov	eax, DWORD PTR [ebx+eax+304]
	je	SHORT $LN74@zoom_verte
	add	edx, eax
	mov	eax, DWORD PTR _web+636
	mov	eax, DWORD PTR [edx+eax*4]
	jmp	SHORT $LN73@zoom_verte
$LN74@zoom_verte:
	add	edx, eax
	mov	eax, DWORD PTR [edx]
$LN73@zoom_verte:
	mov	edi, DWORD PTR _web+12
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edi+eax*4]
	mov	eax, DWORD PTR [eax+8]
	and	eax, 33554432				; 02000000H
	xor	edi, edi
	or	eax, edi
	jne	SHORT $LN67@zoom_verte
	lea	ebx, DWORD PTR [edi+1]
	jmp	SHORT $LN68@zoom_verte
$LN67@zoom_verte:
	xor	ebx, ebx
$LN68@zoom_verte:

; 58   :         val_head = !(get_vattr(get_edge_headv(e_id)) & DISSOLVED);

	cmp	DWORD PTR tv1620[ebp], edi
	je	SHORT $LN78@zoom_verte
	mov	eax, DWORD PTR [edx]
	jmp	SHORT $LN77@zoom_verte
$LN78@zoom_verte:
	mov	eax, DWORD PTR _web+636
	mov	eax, DWORD PTR [edx+eax*4]
$LN77@zoom_verte:
	mov	edi, DWORD PTR _web+12
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edi+eax*4]
	mov	eax, DWORD PTR [eax+8]
	and	eax, 33554432				; 02000000H
	xor	edi, edi
	or	eax, edi

; 59   :         if ( !val_head || !val_tail )

	jne	$LN85@zoom_verte
	test	ebx, ebx
	jne	SHORT $LN51@zoom_verte

; 60   :           { if (val_head) 
; 61   :                 { set_attr(get_edge_headv(e_id),FIXED);

	cmp	DWORD PTR tv1620[ebp], edi
	je	$LN82@zoom_verte

; 64   :                 { set_attr(get_edge_tailv(e_id),FIXED);

$LN88@zoom_verte:
	mov	edx, DWORD PTR [edx]
$LN87@zoom_verte:
	mov	eax, edx
	shr	eax, 29					; 0000001dH
	imul	eax, 112				; 00000070H
	mov	eax, DWORD PTR _web[eax+12]
	and	edx, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [eax+edx*4]
	mov	eax, DWORD PTR [edx+12]
	mov	DWORD PTR [edx+12], eax
	or	DWORD PTR [edx+8], 64			; 00000040H
$LN91@zoom_verte:

; 65   :                 }
; 66   :              set_attr(e_id,DISSOLVED);

	shr	ecx, 29					; 0000001dH
	imul	ecx, 112				; 00000070H
	mov	ecx, DWORD PTR _web[ecx+12]
	mov	eax, DWORD PTR [ecx+esi]
	mov	edx, DWORD PTR [eax+12]
	or	DWORD PTR [eax+8], 33554432		; 02000000H
	mov	DWORD PTR [eax+12], edx
$LN51@zoom_verte:

; 51   :      }
; 52   : 
; 53   :   /* eliminate all edges connected to gone vertices */
; 54   :   FOR_ALL_EDGES(e_id)

	mov	eax, DWORD PTR _web+124
	mov	ecx, DWORD PTR [esi+eax]
	mov	ecx, DWORD PTR [ecx]
	test	ecx, 268435456				; 10000000H
	jne	$LL186@zoom_verte
$LN178@zoom_verte:

; 67   :           }
; 68   :      }
; 69   : 
; 70   :   /* eliminate all facets verging on gone edges */
; 71   :   FOR_ALL_FACETS(f_id)

	mov	edi, DWORD PTR _web+272
	mov	ecx, edi
	shr	ecx, 28					; 0000001cH
	and	ecx, 1
	je	$LN179@zoom_verte
	npad	8
$LL43@zoom_verte:
	mov	edx, DWORD PTR _web+236
	mov	ebx, edi
	and	ebx, 134217727				; 07ffffffH
	add	ebx, ebx
	add	ebx, ebx
	mov	edx, DWORD PTR [ebx+edx]
	mov	eax, DWORD PTR [edx+8]
	and	eax, 1
	xor	esi, esi
	or	eax, esi
	je	$LN42@zoom_verte

; 72   :      { 
; 73   :         fe_id = get_facet_fe(f_id);

	test	ecx, ecx
	jne	SHORT $LN96@zoom_verte
	xor	edx, edx
	jmp	SHORT $LN95@zoom_verte

; 60   :           { if (val_head) 
; 61   :                 { set_attr(get_edge_headv(e_id),FIXED);

$LN85@zoom_verte:

; 62   :                 }
; 63   :             if (val_tail) 

	test	ebx, ebx
	je	SHORT $LN91@zoom_verte

; 64   :                 { set_attr(get_edge_tailv(e_id),FIXED);

	cmp	DWORD PTR tv1620[ebp], edi
	je	$LN88@zoom_verte

; 60   :           { if (val_head) 
; 61   :                 { set_attr(get_edge_headv(e_id),FIXED);

$LN82@zoom_verte:

; 64   :                 { set_attr(get_edge_tailv(e_id),FIXED);

	mov	eax, DWORD PTR _web+636
	mov	edx, DWORD PTR [edx+eax*4]
	jmp	$LN87@zoom_verte

; 72   :      { 
; 73   :         fe_id = get_facet_fe(f_id);

$LN96@zoom_verte:
	mov	edx, DWORD PTR [edx+28]
	test	edi, 134217728				; 08000000H
	je	SHORT $LN95@zoom_verte
	xor	edx, 134217728				; 08000000H
$LN95@zoom_verte:

; 74   :         for ( i = 0 ; i < FACET_EDGES ; i++ )

	mov	DWORD PTR _i$[ebp], esi
	npad	5
$LL38@zoom_verte:

; 75   :           { if ( get_eattr(get_fe_edge(fe_id)) & DISSOLVED )

	mov	ecx, DWORD PTR _web+460
	mov	esi, DWORD PTR _web+124
	mov	eax, edx
	and	eax, 134217727				; 07ffffffH
	mov	ecx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR [ecx+20]
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [esi+eax*4]
	mov	eax, DWORD PTR [eax+8]
	and	eax, 33554432				; 02000000H
	xor	esi, esi
	or	eax, esi
	jne	SHORT $LN164@zoom_verte

; 77   :                   break;
; 78   :                 }
; 79   :              fe_id = get_next_edge(fe_id);

	test	edx, 134217728				; 08000000H
	je	SHORT $LN104@zoom_verte
	mov	edx, DWORD PTR [ecx+28]
	xor	edx, 134217728				; 08000000H
	jmp	SHORT $LN37@zoom_verte
$LN104@zoom_verte:
	mov	edx, DWORD PTR [ecx+32]
$LN37@zoom_verte:

; 74   :         for ( i = 0 ; i < FACET_EDGES ; i++ )

	mov	eax, DWORD PTR _i$[ebp]
	inc	eax
	mov	DWORD PTR _i$[ebp], eax
	cmp	eax, 3
	jl	SHORT $LL38@zoom_verte

; 75   :           { if ( get_eattr(get_fe_edge(fe_id)) & DISSOLVED )

	jmp	SHORT $LN42@zoom_verte
$LN164@zoom_verte:

; 76   :                 { set_attr(f_id,DISSOLVED);

	shr	edi, 29					; 0000001dH
	imul	edi, 112				; 00000070H
	mov	ecx, DWORD PTR _web[edi+12]
	mov	eax, DWORD PTR [ecx+ebx]
	mov	edx, DWORD PTR [eax+12]
	or	DWORD PTR [eax+8], 33554432		; 02000000H
	mov	DWORD PTR [eax+12], edx
$LN42@zoom_verte:

; 67   :           }
; 68   :      }
; 69   : 
; 70   :   /* eliminate all facets verging on gone edges */
; 71   :   FOR_ALL_FACETS(f_id)

	mov	eax, DWORD PTR _web+236
	mov	ecx, DWORD PTR [ebx+eax]
	mov	edi, DWORD PTR [ecx]
	mov	ecx, edi
	shr	ecx, 28					; 0000001cH
	and	ecx, 1
	jne	$LL43@zoom_verte
$LN179@zoom_verte:

; 80   :           }
; 81   :      }
; 82   :      
; 83   :   /* eliminate all facet-edges on gone facets */
; 84   :   FOR_ALL_FACETEDGES(fe_id)

	mov	ecx, DWORD PTR _web+496
	mov	eax, ecx
	shr	eax, 28					; 0000001cH
	and	eax, 1
	mov	DWORD PTR tv1766[ebp], eax
	je	$LN187@zoom_verte
	npad	2
$LL34@zoom_verte:
	mov	edx, DWORD PTR _web+460
	mov	esi, ecx
	and	esi, 134217727				; 07ffffffH
	add	esi, esi
	add	esi, esi
	mov	edi, DWORD PTR [esi+edx]
	mov	eax, DWORD PTR [edi+8]
	and	eax, 1
	xor	edx, edx
	or	eax, edx
	mov	DWORD PTR tv1722[ebp], esi
	mov	DWORD PTR tv1725[ebp], edi
	je	$LN33@zoom_verte

; 85   :      { 
; 86   :         e_id = get_fe_edge(fe_id);
; 87   :         if ( get_eattr(e_id) & DISSOLVED ) 

	mov	eax, DWORD PTR _web+124
	mov	edx, ecx
	and	edx, 134217728				; 08000000H
	xor	edx, DWORD PTR [edi+20]
	xor	ebx, ebx
	mov	edi, edx
	and	edi, 134217727				; 07ffffffH
	add	edi, edi
	add	edi, edi
	mov	eax, DWORD PTR [edi+eax]
	mov	DWORD PTR tv1613[ebp], eax
	mov	eax, DWORD PTR [eax+8]
	and	eax, 33554432				; 02000000H
	or	eax, ebx
	mov	DWORD PTR tv1610[ebp], edi
	je	SHORT $LN29@zoom_verte

; 88   :          { set_attr(fe_id,DISSOLVED); continue; }

	shr	ecx, 29					; 0000001dH
	imul	ecx, 112				; 00000070H
	mov	ecx, DWORD PTR _web[ecx+12]
	mov	eax, DWORD PTR [ecx+esi]
	mov	edx, DWORD PTR [eax+12]
	mov	DWORD PTR [eax+12], edx
	jmp	$LN190@zoom_verte
$LN29@zoom_verte:

; 89   :         f_id = get_fe_facet(fe_id);

	cmp	DWORD PTR tv1766[ebp], ebx
	jne	SHORT $LN112@zoom_verte
	mov	eax, DWORD PTR _NULLFACET
	jmp	SHORT $LN111@zoom_verte
$LN112@zoom_verte:
	mov	eax, DWORD PTR tv1725[ebp]
	mov	eax, DWORD PTR [eax+24]
	test	ecx, 134217728				; 08000000H
	je	SHORT $LN111@zoom_verte
	xor	eax, 134217728				; 08000000H
$LN111@zoom_verte:

; 90   :         if ( valid_id(f_id) && (get_fattr(f_id) & DISSOLVED) )

	test	eax, 268435456				; 10000000H
	je	$LN33@zoom_verte
	mov	ebx, DWORD PTR _web+236
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [ebx+eax*4]
	mov	eax, DWORD PTR [eax+8]
	and	eax, 33554432				; 02000000H
	xor	ebx, ebx
	or	eax, ebx
	je	$LN33@zoom_verte

; 91   :           {    /* have kept edge on removed facet */
; 92   :                  /* patch up ragged edges */
; 93   :                  next = get_next_facet(fe_id);

	mov	ebx, DWORD PTR tv1725[ebp]
	mov	esi, ecx
	shr	esi, 27					; 0000001bH
	and	esi, 1
	je	SHORT $LN116@zoom_verte
	mov	eax, DWORD PTR [ebx+36]
	xor	eax, 134217728				; 08000000H
	jmp	SHORT $LN115@zoom_verte
$LN116@zoom_verte:
	mov	eax, DWORD PTR [ebx+40]
$LN115@zoom_verte:

; 94   :                  prev = get_prev_facet(fe_id);

	test	esi, esi
	je	SHORT $LN120@zoom_verte
	mov	esi, DWORD PTR [ebx+40]
	xor	esi, 134217728				; 08000000H
	jmp	SHORT $LN191@zoom_verte
$LN120@zoom_verte:
	mov	esi, DWORD PTR [ebx+36]
$LN191@zoom_verte:
	mov	DWORD PTR _prev$[ebp], esi

; 95   :                  if ( equal_id(next,fe_id) )

	cmp	eax, ecx
	jne	SHORT $LN27@zoom_verte

; 96   :                   { /* was only facet on edge */
; 97   :                      set_edge_fe(e_id,NULLFACETEDGE);

	mov	eax, DWORD PTR _NULLFACETEDGE
	mov	esi, edx
	shr	esi, 27					; 0000001bH
	and	esi, 1
	je	SHORT $LN123@zoom_verte
	xor	eax, 134217728				; 08000000H
$LN123@zoom_verte:
	mov	ebx, DWORD PTR tv1613[ebp]

; 98   :                   }
; 99   :                  else

	jmp	$LN192@zoom_verte
$LN27@zoom_verte:

; 100  :                   { /* close ranks */
; 101  :                      set_next_facet(prev,next);

	test	esi, 268435456				; 10000000H
	je	SHORT $LN181@zoom_verte
	mov	ebx, DWORD PTR _web+460
	test	esi, 134217728				; 08000000H
	je	SHORT $LN127@zoom_verte
	mov	edi, eax
	xor	edi, 134217728				; 08000000H
	and	esi, 134217727				; 07ffffffH
	mov	esi, DWORD PTR [ebx+esi*4]
	mov	DWORD PTR [esi+36], edi
	mov	esi, DWORD PTR _prev$[ebp]
	jmp	SHORT $LN193@zoom_verte
$LN127@zoom_verte:
	mov	edi, esi
	and	edi, 134217727				; 07ffffffH
	mov	edi, DWORD PTR [ebx+edi*4]
	mov	DWORD PTR [edi+40], eax
$LN193@zoom_verte:
	mov	edi, DWORD PTR tv1610[ebp]
$LN181@zoom_verte:

; 102  :                      set_prev_facet(next,prev);

	test	eax, 268435456				; 10000000H
	je	SHORT $LN134@zoom_verte
	mov	ebx, DWORD PTR _web+460
	mov	edi, eax
	test	eax, 134217728				; 08000000H
	je	SHORT $LN132@zoom_verte
	xor	esi, 134217728				; 08000000H
	and	edi, 134217727				; 07ffffffH
	mov	edi, DWORD PTR [ebx+edi*4]
	mov	DWORD PTR [edi+40], esi
	jmp	SHORT $LN131@zoom_verte
$LN132@zoom_verte:
	and	edi, 134217727				; 07ffffffH
	mov	edi, DWORD PTR [ebx+edi*4]
	mov	DWORD PTR [edi+36], esi
$LN131@zoom_verte:
	mov	esi, DWORD PTR _global_timestamp
	mov	edi, DWORD PTR tv1610[ebp]
	inc	esi
	mov	DWORD PTR _global_timestamp, esi
	mov	DWORD PTR _top_timestamp, esi
$LN134@zoom_verte:

; 103  :                      set_edge_fe(e_id,next);

	mov	esi, edx
	shr	esi, 27					; 0000001bH
	and	esi, 1
	je	SHORT $LN136@zoom_verte
	xor	eax, 134217728				; 08000000H
$LN136@zoom_verte:
	mov	ebx, DWORD PTR _web+124
	mov	ebx, DWORD PTR [edi+ebx]
$LN192@zoom_verte:
	mov	DWORD PTR [ebx+28], eax
	mov	eax, DWORD PTR _global_timestamp
	inc	eax

; 104  :                   }
; 105  :             
; 106  :                  if ( web.representation == SOAPFILM)

	cmp	DWORD PTR _web+624, 2
	mov	DWORD PTR _top_timestamp, eax
	mov	DWORD PTR _global_timestamp, eax
	jne	$LN180@zoom_verte

; 107  :                   {
; 108  :                      /* fix edge in place */
; 109  :                      set_attr(e_id,FIXED);

	shr	edx, 29					; 0000001dH
	imul	edx, 112				; 00000070H
	mov	edx, DWORD PTR _web[edx+12]
	mov	eax, DWORD PTR [edx+edi]
	or	DWORD PTR [eax+8], 64			; 00000040H
	mov	edx, DWORD PTR [eax+12]
	mov	DWORD PTR [eax+12], edx

; 110  :                      set_attr(get_edge_tailv(e_id),FIXED);

	mov	ebx, DWORD PTR _dymem
	test	esi, esi
	je	SHORT $LN142@zoom_verte
	mov	eax, DWORD PTR _web+124
	mov	edx, DWORD PTR [edi+eax]
	mov	eax, DWORD PTR _web+216
	add	edx, DWORD PTR [eax+ebx+304]
	mov	eax, DWORD PTR _web+636
	mov	eax, DWORD PTR [edx+eax*4]
	jmp	SHORT $LN141@zoom_verte
$LN142@zoom_verte:
	mov	edx, DWORD PTR _web+124
	mov	eax, DWORD PTR [edi+edx]
	mov	edx, DWORD PTR _web+216
	mov	edx, DWORD PTR [edx+ebx+304]
	mov	eax, DWORD PTR [eax+edx]
$LN141@zoom_verte:
	mov	edx, eax
	shr	edx, 29					; 0000001dH
	imul	edx, 112				; 00000070H
	mov	edx, DWORD PTR _web[edx+12]
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edx+eax*4]
	mov	ebx, DWORD PTR [eax+12]
	mov	DWORD PTR [eax+12], ebx
	mov	edx, 64					; 00000040H
	or	DWORD PTR [eax+8], edx

; 111  :                      set_attr(get_edge_headv(e_id),FIXED);

	mov	eax, DWORD PTR _web+124
	mov	eax, DWORD PTR [edi+eax]
	mov	edi, DWORD PTR _dymem
	test	esi, esi
	mov	esi, DWORD PTR _web+216
	je	SHORT $LN148@zoom_verte
	mov	esi, DWORD PTR [esi+edi+304]
	mov	eax, DWORD PTR [eax+esi]
	jmp	SHORT $LN147@zoom_verte
$LN148@zoom_verte:
	add	eax, DWORD PTR [esi+edi+304]
	mov	esi, DWORD PTR _web+636
	mov	eax, DWORD PTR [eax+esi*4]
$LN147@zoom_verte:
	mov	esi, eax
	shr	esi, 29					; 0000001dH
	imul	esi, 112				; 00000070H
	mov	esi, DWORD PTR _web[esi+12]
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [esi+eax*4]
	or	DWORD PTR [eax+8], edx
	mov	edx, DWORD PTR [eax+12]
	mov	DWORD PTR [eax+12], edx
$LN180@zoom_verte:

; 112  :                   }
; 113  :                  set_attr(fe_id,DISSOLVED);

	mov	esi, DWORD PTR tv1722[ebp]
	shr	ecx, 29					; 0000001dH
	imul	ecx, 112				; 00000070H
	mov	eax, DWORD PTR _web[ecx+12]
	mov	eax, DWORD PTR [eax+esi]
	mov	ecx, DWORD PTR [eax+12]
	mov	DWORD PTR [eax+12], ecx
$LN190@zoom_verte:
	or	DWORD PTR [eax+8], 33554432		; 02000000H
$LN33@zoom_verte:

; 80   :           }
; 81   :      }
; 82   :      
; 83   :   /* eliminate all facet-edges on gone facets */
; 84   :   FOR_ALL_FACETEDGES(fe_id)

	mov	edx, DWORD PTR _web+460
	mov	eax, DWORD PTR [esi+edx]
	mov	ecx, DWORD PTR [eax]
	mov	eax, ecx
	shr	eax, 28					; 0000001cH
	and	eax, 1
	mov	DWORD PTR tv1766[ebp], eax
	jne	$LL34@zoom_verte
$LN187@zoom_verte:

; 114  :           }
; 115  :      }
; 116  :      FOR_ALL_VERTICES(v_id)

	mov	edx, DWORD PTR _web+48
	test	edx, 268435456				; 10000000H
	je	SHORT $LN184@zoom_verte
	mov	eax, DWORD PTR _web+12
$LL24@zoom_verte:
	mov	esi, edx
	and	esi, 134217727				; 07ffffffH
	add	esi, esi
	add	esi, esi
	mov	eax, DWORD PTR [esi+eax]
	mov	ecx, DWORD PTR [eax+8]
	mov	eax, DWORD PTR [eax+12]
	mov	DWORD PTR tv1890[ebp+4], eax
	mov	eax, ecx
	and	eax, 1
	xor	edi, edi
	or	eax, edi
	je	SHORT $LN23@zoom_verte

; 117  :         if ( get_vattr(v_id) & DISSOLVED ) free_element(v_id);

	and	ecx, 33554432				; 02000000H
	xor	eax, eax
	or	ecx, eax
	je	SHORT $LN23@zoom_verte
	push	edx
	call	_free_element
	add	esp, 4
$LN23@zoom_verte:

; 114  :           }
; 115  :      }
; 116  :      FOR_ALL_VERTICES(v_id)

	mov	eax, DWORD PTR _web+12
	mov	ecx, DWORD PTR [esi+eax]
	mov	edx, DWORD PTR [ecx]
	test	edx, 268435456				; 10000000H
	jne	SHORT $LL24@zoom_verte
$LN184@zoom_verte:

; 118  :      FOR_ALL_EDGES(e_id)

	mov	edx, DWORD PTR _web+160
	test	edx, 268435456				; 10000000H
	je	SHORT $LN188@zoom_verte
	mov	eax, DWORD PTR _web+124
$LL18@zoom_verte:
	mov	esi, edx
	and	esi, 134217727				; 07ffffffH
	add	esi, esi
	add	esi, esi
	mov	eax, DWORD PTR [esi+eax]
	mov	ecx, DWORD PTR [eax+8]
	mov	eax, DWORD PTR [eax+12]
	mov	DWORD PTR tv1616[ebp+4], eax
	mov	eax, ecx
	and	eax, 1
	xor	edi, edi
	or	eax, edi
	je	SHORT $LN17@zoom_verte

; 119  :         if ( get_eattr(e_id) & DISSOLVED ) free_element(e_id);

	and	ecx, 33554432				; 02000000H
	xor	eax, eax
	or	ecx, eax
	je	SHORT $LN17@zoom_verte
	push	edx
	call	_free_element
	add	esp, 4
$LN17@zoom_verte:

; 118  :      FOR_ALL_EDGES(e_id)

	mov	eax, DWORD PTR _web+124
	mov	ecx, DWORD PTR [esi+eax]
	mov	edx, DWORD PTR [ecx]
	test	edx, 268435456				; 10000000H
	jne	SHORT $LL18@zoom_verte
$LN188@zoom_verte:

; 120  :      FOR_ALL_FACETS(f_id)

	mov	edx, DWORD PTR _web+272
	test	edx, 268435456				; 10000000H
	je	SHORT $LN189@zoom_verte
	mov	eax, DWORD PTR _web+236
	npad	2
$LL12@zoom_verte:
	mov	esi, edx
	and	esi, 134217727				; 07ffffffH
	add	esi, esi
	add	esi, esi
	mov	eax, DWORD PTR [esi+eax]
	mov	ecx, DWORD PTR [eax+8]
	mov	eax, DWORD PTR [eax+12]
	mov	DWORD PTR tv1709[ebp+4], eax
	mov	eax, ecx
	and	eax, 1
	xor	edi, edi
	or	eax, edi
	je	SHORT $LN11@zoom_verte

; 121  :         if ( get_fattr(f_id) & DISSOLVED ) free_element(f_id);

	and	ecx, 33554432				; 02000000H
	xor	eax, eax
	or	ecx, eax
	je	SHORT $LN11@zoom_verte
	push	edx
	call	_free_element
	add	esp, 4
$LN11@zoom_verte:

; 120  :      FOR_ALL_FACETS(f_id)

	mov	eax, DWORD PTR _web+236
	mov	ecx, DWORD PTR [esi+eax]
	mov	edx, DWORD PTR [ecx]
	test	edx, 268435456				; 10000000H
	jne	SHORT $LL12@zoom_verte
$LN189@zoom_verte:

; 122  :      FOR_ALL_FACETEDGES(fe_id)

	mov	ecx, DWORD PTR _web+496
	test	ecx, 268435456				; 10000000H
	je	SHORT $LN4@zoom_verte
	mov	eax, DWORD PTR _web+460
	npad	3
$LL6@zoom_verte:
	mov	esi, ecx
	and	esi, 134217727				; 07ffffffH
	add	esi, esi
	add	esi, esi
	mov	eax, DWORD PTR [esi+eax]
	mov	eax, DWORD PTR [eax+8]
	and	eax, 1
	xor	edx, edx
	or	eax, edx
	je	SHORT $LN5@zoom_verte

; 123  :         if ( get_attr(fe_id) & DISSOLVED ) free_element(fe_id);

	mov	edx, ecx
	shr	edx, 29					; 0000001dH
	imul	edx, 112				; 00000070H
	mov	eax, DWORD PTR _web[edx+12]
	mov	eax, DWORD PTR [eax+esi]
	mov	eax, DWORD PTR [eax+8]
	and	eax, 33554432				; 02000000H
	xor	edx, edx
	or	eax, edx
	je	SHORT $LN5@zoom_verte
	push	ecx
	call	_free_element
	add	esp, 4
$LN5@zoom_verte:

; 122  :      FOR_ALL_FACETEDGES(fe_id)

	mov	eax, DWORD PTR _web+460
	mov	ecx, DWORD PTR [esi+eax]
	mov	ecx, DWORD PTR [ecx]
	test	ecx, 268435456				; 10000000H
	jne	SHORT $LL6@zoom_verte
$LN4@zoom_verte:
	pop	edi
	pop	esi
	pop	ebx

; 124  : 
; 125  : } // end zoom_vertex()

	mov	esp, ebp
	pop	ebp
	ret	0
_zoom_vertex ENDP
_TEXT	ENDS
END
