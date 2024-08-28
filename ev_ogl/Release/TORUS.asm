; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\levolver\yonkang-sefit\levolver\ev_ogl\TORUS.C
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_get_edge_wrap
EXTRN	_sym_inverse:DWORD
EXTRN	_dymem:DWORD
EXTRN	_web:BYTE
; Function compile flags: /Ogtp
; File c:\levolver\yonkang-sefit\levolver\ev_ogl\inline.h
;	COMDAT _get_edge_wrap
_TEXT	SEGMENT
_e_id$ = 8						; size = 4
_get_edge_wrap PROC					; COMDAT

; 163  : {

	push	ebp
	mov	ebp, esp

; 164  :   WRAPTYPE wrap = *EINT(e_id,E_WRAP_ATTR) ;

	mov	ecx, DWORD PTR _e_id$[ebp]
	mov	edx, DWORD PTR _web+124
	mov	eax, ecx
	and	eax, 134217727				; 07ffffffH
	mov	eax, DWORD PTR [edx+eax*4]
	mov	edx, DWORD PTR _web+216
	push	esi
	mov	esi, DWORD PTR _dymem
	mov	edx, DWORD PTR [edx+esi+784]
	mov	eax, DWORD PTR [eax+edx]
	pop	esi

; 165  :   return    ( inverted(e_id) ? (*sym_inverse)(wrap) : wrap );

	test	ecx, 134217728				; 08000000H
	je	SHORT $LN4@get_edge_w
	push	eax
	call	DWORD PTR _sym_inverse
	add	esp, 4
$LN4@get_edge_w:

; 166  : }

	pop	ebp
	ret	0
_get_edge_wrap ENDP
_TEXT	ENDS
PUBLIC	_get_edge_tailv
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
PUBLIC	_torus_wrap
EXTRN	__fltused:DWORD
; Function compile flags: /Ogtp
; File c:\levolver\yonkang-sefit\levolver\ev_ogl\torus.c
;	COMDAT _torus_wrap
_TEXT	SEGMENT
_wrapnum$ = 8						; size = 4
_x$ = 8							; size = 4
_y$ = 12						; size = 4
_wrap$ = 16						; size = 4
_torus_wrap PROC					; COMDAT

; 56   : {

	push	ebp
	mov	ebp, esp

; 57   :   int i,j;
; 58   :   int wrapnum;
; 59   : 
; 60   :   if ( x != y )
; 61   :      for ( i = 0 ; i < SDIM ; i++ ) y[i] = x[i];

	mov	edx, DWORD PTR _web+616
	push	esi
	mov	esi, DWORD PTR _y$[ebp]
	push	edi
	mov	edi, DWORD PTR _x$[ebp]
	cmp	edi, esi
	je	SHORT $LN8@torus_wrap
	xor	ecx, ecx
	test	edx, edx
	jle	SHORT $LN8@torus_wrap
	mov	eax, esi
	sub	edi, esi
	npad	1
$LL10@torus_wrap:
	fld	QWORD PTR [edi+eax]
	inc	ecx
	fstp	QWORD PTR [eax]
	mov	edx, DWORD PTR _web+616
	add	eax, 8
	cmp	ecx, edx
	jl	SHORT $LL10@torus_wrap
$LN8@torus_wrap:

; 62   :   for ( i = 0 ; wrap != 0 ; i++, wrap >>= TWRAPBITS )

	mov	edi, DWORD PTR _wrap$[ebp]
	test	edi, edi
	je	SHORT $LN5@torus_wrap
	xor	ecx, ecx
	npad	4
$LL7@torus_wrap:

; 63   :   { wrapnum =  WRAPNUM(wrap & WRAPMASK);

	mov	eax, edi
	and	eax, 31					; 0000001fH
	cmp	eax, 16					; 00000010H
	jle	SHORT $LN14@torus_wrap
	add	eax, -32				; ffffffe0H
$LN14@torus_wrap:
	mov	DWORD PTR _wrapnum$[ebp], eax

; 64   :     if ( wrapnum )

	test	eax, eax
	je	SHORT $LN6@torus_wrap

; 65   :        for ( j = 0 ; j < SDIM ; j++ )

	xor	eax, eax
	test	edx, edx
	jle	SHORT $LN6@torus_wrap
	fild	DWORD PTR _wrapnum$[ebp]
$LN3@torus_wrap:

; 66   :           y[j] += wrapnum*web.torus_period[i][j];

	mov	edx, DWORD PTR _web+1608
	mov	edx, DWORD PTR [ecx+edx]
	fld	QWORD PTR [edx+eax*8]
	inc	eax
	fmul	ST(0), ST(1)
	fadd	QWORD PTR [esi+eax*8-8]
	fstp	QWORD PTR [esi+eax*8-8]
	mov	edx, DWORD PTR _web+616
	cmp	eax, edx
	jl	SHORT $LN3@torus_wrap

; 65   :        for ( j = 0 ; j < SDIM ; j++ )

	fstp	ST(0)
$LN6@torus_wrap:

; 62   :   for ( i = 0 ; wrap != 0 ; i++, wrap >>= TWRAPBITS )

	sar	edi, 6
	add	ecx, 4
	test	edi, edi
	jne	SHORT $LL7@torus_wrap
$LN5@torus_wrap:
	pop	edi
	pop	esi

; 67   :   }
; 68   : } // end torus_wrap()

	pop	ebp
	ret	0
_torus_wrap ENDP
_TEXT	ENDS
PUBLIC	_torus_form_pullback
EXTRN	_memcpy:PROC
; Function compile flags: /Ogtp
;	COMDAT _torus_form_pullback
_TEXT	SEGMENT
_x$ = 8							; size = 4
_xform$ = 12						; size = 4
_yform$ = 16						; size = 4
_wrap$ = 20						; size = 4
_torus_form_pullback PROC				; COMDAT

; 88   : {

	push	ebp
	mov	ebp, esp

; 89   :   memcpy((char*)xform,(char*)yform,SDIM*sizeof(REAL));

	mov	eax, DWORD PTR _web+616
	mov	edx, DWORD PTR _yform$[ebp]
	lea	ecx, DWORD PTR [eax*8]
	mov	eax, DWORD PTR _xform$[ebp]
	push	ecx
	push	edx
	push	eax
	call	_memcpy
	add	esp, 12					; 0000000cH

; 90   : } // end torus_form_pullback()

	pop	ebp
	ret	0
_torus_form_pullback ENDP
_TEXT	ENDS
PUBLIC	??_C@_0DA@COINFANF@Wrap?5out?5of?5bounds?5as?5result?5of?5@ ; `string'
PUBLIC	_torus_compose
EXTRN	_kb_error:PROC
;	COMDAT ??_C@_0DA@COINFANF@Wrap?5out?5of?5bounds?5as?5result?5of?5@
CONST	SEGMENT
??_C@_0DA@COINFANF@Wrap?5out?5of?5bounds?5as?5result?5of?5@ DB 'Wrap out '
	DB	'of bounds as result of torus_compose.', 0aH, 00H ; `string'
; Function compile flags: /Ogtp
CONST	ENDS
;	COMDAT _torus_compose
_TEXT	SEGMENT
_wrap1$ = 8						; size = 4
_wrap2$ = 12						; size = 4
_torus_compose PROC					; COMDAT

; 104  : { int w = (wrap1 + wrap2) & ALLWRAPMASK;

	push	ebp
	mov	ebp, esp
	mov	eax, DWORD PTR _wrap1$[ebp]
	mov	ecx, DWORD PTR _wrap2$[ebp]
	push	esi
	lea	esi, DWORD PTR [eax+ecx]

; 105  : #ifdef _DEBUG
; 106  :   int i;
; 107  :   int ww = w;
; 108  : 
; 109  :   if ( verbose_flag )
; 110  :   for ( i = 0, ww=w ; i < SDIM ; i++ )
; 111  :   { switch(ww&WRAPMASK)
; 112  :     { case 0: case POSWRAP: case NEGWRAP: break;
; 113  :       default: kb_error(3917,"Bad wrap as result of torus_compose.\n",
; 114  :          WARNING);
; 115  :     }
; 116  :     ww >>= TWRAPBITS; 
; 117  :   }
; 118  :  #endif
; 119  :   if ( ~(wrap1|wrap2) & TERRDETECT & w )

	or	eax, ecx
	and	esi, 528349151				; 1f7df7dfH
	not	eax
	and	eax, esi
	test	eax, 818089008				; 30c30c30H
	je	SHORT $LN4@torus_comp

; 120  :     kb_error(4553,"Wrap out of bounds as result of torus_compose.\n",
; 121  :          WARNING);

	push	2
	push	OFFSET ??_C@_0DA@COINFANF@Wrap?5out?5of?5bounds?5as?5result?5of?5@
	push	4553					; 000011c9H
	call	_kb_error
	add	esp, 12					; 0000000cH
$LN4@torus_comp:

; 122  :   return w;

	mov	eax, esi
	pop	esi

; 123  : } // end torus_compose()

	pop	ebp
	ret	0
_torus_compose ENDP
_TEXT	ENDS
PUBLIC	_torus_inverse
; Function compile flags: /Ogtp
;	COMDAT _torus_inverse
_TEXT	SEGMENT
_wrap$ = 8						; size = 4
_torus_inverse PROC					; COMDAT

; 135  : {

	push	ebp
	mov	ebp, esp

; 136  :   return ((~ALLWRAPMASK)-wrap) & ALLWRAPMASK;

	mov	eax, 8521760				; 00820820H
	sub	eax, DWORD PTR _wrap$[ebp]
	and	eax, 528349151				; 1f7df7dfH

; 137  : }

	pop	ebp
	ret	0
_torus_inverse ENDP
_TEXT	ENDS
PUBLIC	__$ArrayPad$
PUBLIC	_torus_unwrap_edge
EXTRN	_wrap_vertex:PROC
EXTRN	_matvec_mul:PROC
EXTRN	___security_cookie:DWORD
EXTRN	@__security_check_cookie@4:PROC
EXTRN	__ftol2_sse:PROC
EXTRN	_floor:PROC
; Function compile flags: /Ogtp
;	COMDAT _torus_unwrap_edge
_TEXT	SEGMENT
_headv$ = -60						; size = 4
_wrap$ = -56						; size = 4
_u$ = -52						; size = 48
__$ArrayPad$ = -4					; size = 4
_e_id$ = 8						; size = 4
_torus_unwrap_edge PROC					; COMDAT

; 149  : { edge_id pos_e = positive_id(e_id);

	push	ebp
	mov	ebp, esp
	sub	esp, 60					; 0000003cH
	mov	eax, DWORD PTR ___security_cookie
	xor	eax, ebp
	mov	DWORD PTR __$ArrayPad$[ebp], eax

; 150  :   vertex_id tailv,headv;
; 151  :   REAL *x;
; 152  :   WRAPTYPE wrap = get_edge_wrap(pos_e);

	mov	ecx, DWORD PTR _web+124
	push	esi
	mov	esi, DWORD PTR _e_id$[ebp]
	and	esi, -134217729				; f7ffffffH
	mov	eax, esi
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	mov	eax, DWORD PTR _web+216
	mov	ecx, DWORD PTR _dymem
	mov	eax, DWORD PTR [eax+ecx+784]
	mov	eax, DWORD PTR [edx+eax]
	test	esi, 134217728				; 08000000H
	je	SHORT $LN9@torus_unwr
	push	eax
	call	DWORD PTR _sym_inverse
	add	esp, 4
$LN9@torus_unwr:
	mov	DWORD PTR _wrap$[ebp], eax

; 153  :   WRAPTYPE tailwrap,headwrap;
; 154  :   REAL u[MAXCOORD]; /* tail unit cell coords */
; 155  :   int i;
; 156  :   
; 157  :   if ( wrap == 0 ) return;

	test	eax, eax
	je	$LN5@torus_unwr

; 158  :   tailv = get_edge_tailv(pos_e);

	push	ebx
	push	edi
	push	esi
	call	_get_edge_tailv

; 159  :   headv = get_edge_headv(pos_e);

	push	esi
	mov	ebx, eax
	call	_get_edge_headv
	mov	DWORD PTR _headv$[ebp], eax

; 160  :   x = get_coord(tailv);
; 161  :   matvec_mul(web.inverse_periods,x,u,SDIM,SDIM);

	mov	eax, DWORD PTR _web+616
	push	eax
	push	eax
	mov	eax, DWORD PTR _web+12
	lea	ecx, DWORD PTR _u$[ebp]
	push	ecx
	mov	edx, ebx
	and	edx, 134217727				; 07ffffffH
	mov	ecx, DWORD PTR [eax+edx*4]
	mov	edx, DWORD PTR _web+104
	mov	eax, DWORD PTR _dymem
	add	ecx, DWORD PTR [edx+eax+64]
	push	ecx
	mov	ecx, DWORD PTR _web+1612
	push	ecx
	call	_matvec_mul

; 162  :   tailwrap = 0;
; 163  :   for ( i = SDIM-1 ; i >= 0 ; i-- )

	mov	esi, DWORD PTR _web+616
	add	esp, 28					; 0000001cH
	xor	edi, edi
	dec	esi
	js	SHORT $LN1@torus_unwr
$LL3@torus_unwr:

; 164  :   { tailwrap <<= TWRAPBITS;
; 165  :     tailwrap |= ((int)(floor(u[i]))) & WRAPMASK;

	fld	QWORD PTR _u$[ebp+esi*8]
	sub	esp, 8
	fstp	QWORD PTR [esp]
	call	_floor
	add	esp, 8
	call	__ftol2_sse
	and	eax, 31					; 0000001fH
	shl	edi, 6
	or	edi, eax
	dec	esi
	jns	SHORT $LL3@torus_unwr
$LN1@torus_unwr:

; 166  :   }   
; 167  :   tailwrap = torus_inverse(tailwrap);

	push	edi
	call	_torus_inverse

; 168  :   headwrap = torus_compose(tailwrap,wrap);

	mov	edx, DWORD PTR _wrap$[ebp]
	mov	esi, eax
	push	edx
	push	esi
	call	_torus_compose

; 169  :   wrap_vertex(tailv,tailwrap);

	push	esi
	push	ebx
	mov	edi, eax
	call	_wrap_vertex

; 170  :   wrap_vertex(headv,headwrap);

	mov	eax, DWORD PTR _headv$[ebp]
	push	edi
	push	eax
	call	_wrap_vertex
	add	esp, 28					; 0000001cH
	pop	edi
	pop	ebx
$LN5@torus_unwr:

; 171  : } // end torus_unwrap_edge()

	mov	ecx, DWORD PTR __$ArrayPad$[ebp]
	xor	ecx, ebp
	pop	esi
	call	@__security_check_cookie@4
	mov	esp, ebp
	pop	ebp
	ret	0
_torus_unwrap_edge ENDP
_TEXT	ENDS
PUBLIC	_identity_wrap
; Function compile flags: /Ogtp
;	COMDAT _identity_wrap
_TEXT	SEGMENT
_x$ = 8							; size = 4
_y$ = 12						; size = 4
_wrap$ = 16						; size = 4
_identity_wrap PROC					; COMDAT

; 182  : {

	push	ebp
	mov	ebp, esp

; 183  :   int i;
; 184  : 
; 185  :   if ( x != y )

	mov	edx, DWORD PTR _x$[ebp]
	mov	eax, DWORD PTR _y$[ebp]
	cmp	edx, eax
	je	SHORT $LN1@identity_w

; 186  :      for ( i = 0 ; i < SDIM ; i++ ) 

	xor	ecx, ecx
	cmp	DWORD PTR _web+616, ecx
	jle	SHORT $LN1@identity_w
	sub	edx, eax
	npad	7
$LL3@identity_w:

; 187  :        y[i] = x[i];

	fld	QWORD PTR [edx+eax]
	inc	ecx
	fstp	QWORD PTR [eax]
	add	eax, 8
	cmp	ecx, DWORD PTR _web+616
	jl	SHORT $LL3@identity_w
$LN1@identity_w:

; 188  : } // end identity_wrap()

	pop	ebp
	ret	0
_identity_wrap ENDP
_TEXT	ENDS
PUBLIC	_identity_form_pullback
; Function compile flags: /Ogtp
;	COMDAT _identity_form_pullback
_TEXT	SEGMENT
_x$ = 8							; size = 4
_xform$ = 12						; size = 4
_yform$ = 16						; size = 4
_wrap$ = 20						; size = 4
_identity_form_pullback PROC				; COMDAT

; 195  : {

	push	ebp
	mov	ebp, esp

; 196  :   memcpy((char*)xform,(char*)yform,SDIM*sizeof(REAL));

	mov	eax, DWORD PTR _web+616
	mov	edx, DWORD PTR _yform$[ebp]
	lea	ecx, DWORD PTR [eax*8]
	mov	eax, DWORD PTR _xform$[ebp]
	push	ecx
	push	edx
	push	eax
	call	_memcpy
	add	esp, 12					; 0000000cH

; 197  : }

	pop	ebp
	ret	0
_identity_form_pullback ENDP
_TEXT	ENDS
PUBLIC	_identity_compose
; Function compile flags: /Ogtp
;	COMDAT _identity_compose
_TEXT	SEGMENT
_wrap1$ = 8						; size = 4
_wrap2$ = 12						; size = 4
_identity_compose PROC					; COMDAT

; 201  : { return 0;

	xor	eax, eax

; 202  : }

	ret	0
_identity_compose ENDP
_TEXT	ENDS
PUBLIC	_identity_inverse
; Function compile flags: /Ogtp
;	COMDAT _identity_inverse
_TEXT	SEGMENT
_wrap$ = 8						; size = 4
_identity_inverse PROC					; COMDAT

; 205  : {  return 0;

	xor	eax, eax

; 206  : }

	ret	0
_identity_inverse ENDP
_TEXT	ENDS
PUBLIC	_identity_unwrap_edge
; Function compile flags: /Ogtp
;	COMDAT _identity_unwrap_edge
_TEXT	SEGMENT
_e_id$ = 8						; size = 4
_identity_unwrap_edge PROC				; COMDAT

; 210  : {}

	ret	0
_identity_unwrap_edge ENDP
_TEXT	ENDS
END
