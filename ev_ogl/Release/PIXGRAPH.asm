; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\levolver\yonkang-sefit\levolver\ev_ogl\PIXGRAPH.C
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

_DATA	SEGMENT
COMM	_pix_file_name:BYTE:03e8H
_DATA	ENDS
_BSS	SEGMENT
_pfd	DD	01H DUP (?)
_BSS	ENDS
PUBLIC	??_C@_0L@JLMBHOKF@PIXGRAPH?4C?$AA@		; `string'
PUBLIC	??_C@_01EEMJAFIK@?6?$AA@			; `string'
PUBLIC	??_C@_05COGMDDIH@CPOLY?$AA@			; `string'
PUBLIC	??_C@_06MLLPDBGA@CNPOLY?$AA@			; `string'
PUBLIC	??_C@_0P@IHHAEANP@Bad?5filename?4?6?$AA@	; `string'
PUBLIC	??_C@_01NOFIACDB@w?$AA@				; `string'
PUBLIC	??_C@_05KDEMKMHP@?4quad?$AA@			; `string'
PUBLIC	??_C@_06IPOIOIJ@?5?7?$DL?3?0?6?$AA@		; `string'
PUBLIC	??_C@_06HKCJHNLM@?5?7?0?$DL?3?6?$AA@		; `string'
PUBLIC	??_C@_0CB@GHEDCGIA@Thicken?$CIn?5?$HM?5y?5?$FLthickness?$CI?$CFg?$CJ?$FN?$CJ?$DP?5@ ; `string'
PUBLIC	__real@408f400000000000
PUBLIC	__real@0000000000000000
PUBLIC	??_C@_0BD@DAIMKNPL@No?5colormap?5used?4?6?$AA@	; `string'
PUBLIC	??_C@_0BO@ONMFPAEA@Enter?5name?5of?5colormap?5file?3?5?$AA@ ; `string'
PUBLIC	??_C@_0BB@BLNMEKNO@Do?5body?5colors?$DP?5?$AA@	; `string'
PUBLIC	??_C@_0CF@FNPONDP@Unintelligible?5response?4?5Try?5aga@ ; `string'
PUBLIC	??_C@_0BF@GLPJGIPO@Pixar?5file?5aborted?4?6?$AA@ ; `string'
PUBLIC	??_C@_0CK@CFPLAEFF@Do?5inner?0?5outer?0?5or?5all?5surfaces@ ; `string'
PUBLIC	??_C@_0BK@LHFCGKCJ@Do?5normal?5interpolation?$DP?5?$AA@ ; `string'
PUBLIC	??_C@_0BN@PCMGKOBJ@Enter?5name?5of?5picture?5file?3?5?$AA@ ; `string'
PUBLIC	__$ArrayPad$
PUBLIC	_pix_start
EXTRN	_kb_dmatrix:PROC
EXTRN	_fprintf:PROC
EXTRN	_erroutstring:PROC
EXTRN	_perror:PROC
EXTRN	_l_evolver_mode:DWORD
EXTRN	_l_redirect:DWORD
EXTRN	_fopen:PROC
EXTRN	_atof:PROC
EXTRN	_strtok:PROC
EXTRN	_thickenflag:DWORD
EXTRN	_sprintf:PROC
EXTRN	_thickness:QWORD
EXTRN	_msg:DWORD
EXTRN	_resize:PROC
EXTRN	_overall_size:QWORD
EXTRN	_colorflag:DWORD
EXTRN	_cmapname:BYTE
EXTRN	_outstring:PROC
EXTRN	_kb_error:PROC
EXTRN	_innerflag:DWORD
EXTRN	_outerflag:DWORD
EXTRN	_kb_upper_array:BYTE
EXTRN	_normflag:DWORD
EXTRN	_commandfd:DWORD
EXTRN	___iob_func:PROC
EXTRN	_prompt:PROC
EXTRN	___security_cookie:DWORD
EXTRN	__fltused:DWORD
EXTRN	@__security_check_cookie@4:PROC
_BSS	SEGMENT
_verts	DD	01H DUP (?)
_BSS	ENDS
;	COMDAT ??_C@_0L@JLMBHOKF@PIXGRAPH?4C?$AA@
; File c:\levolver\yonkang-sefit\levolver\ev_ogl\pixgraph.c
CONST	SEGMENT
??_C@_0L@JLMBHOKF@PIXGRAPH?4C?$AA@ DB 'PIXGRAPH.C', 00H	; `string'
CONST	ENDS
;	COMDAT ??_C@_01EEMJAFIK@?6?$AA@
CONST	SEGMENT
??_C@_01EEMJAFIK@?6?$AA@ DB 0aH, 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_05COGMDDIH@CPOLY?$AA@
CONST	SEGMENT
??_C@_05COGMDDIH@CPOLY?$AA@ DB 'CPOLY', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_06MLLPDBGA@CNPOLY?$AA@
CONST	SEGMENT
??_C@_06MLLPDBGA@CNPOLY?$AA@ DB 'CNPOLY', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_0P@IHHAEANP@Bad?5filename?4?6?$AA@
CONST	SEGMENT
??_C@_0P@IHHAEANP@Bad?5filename?4?6?$AA@ DB 'Bad filename.', 0aH, 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_01NOFIACDB@w?$AA@
CONST	SEGMENT
??_C@_01NOFIACDB@w?$AA@ DB 'w', 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_05KDEMKMHP@?4quad?$AA@
CONST	SEGMENT
??_C@_05KDEMKMHP@?4quad?$AA@ DB '.quad', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_06IPOIOIJ@?5?7?$DL?3?0?6?$AA@
CONST	SEGMENT
??_C@_06IPOIOIJ@?5?7?$DL?3?0?6?$AA@ DB ' ', 09H, ';:,', 0aH, 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_06HKCJHNLM@?5?7?0?$DL?3?6?$AA@
CONST	SEGMENT
??_C@_06HKCJHNLM@?5?7?0?$DL?3?6?$AA@ DB ' ', 09H, ',;:', 0aH, 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_0CB@GHEDCGIA@Thicken?$CIn?5?$HM?5y?5?$FLthickness?$CI?$CFg?$CJ?$FN?$CJ?$DP?5@
CONST	SEGMENT
??_C@_0CB@GHEDCGIA@Thicken?$CIn?5?$HM?5y?5?$FLthickness?$CI?$CFg?$CJ?$FN?$CJ?$DP?5@ DB 'T'
	DB	'hicken(n | y [thickness(%g)])? ', 00H	; `string'
CONST	ENDS
;	COMDAT __real@408f400000000000
CONST	SEGMENT
__real@408f400000000000 DQ 0408f400000000000r	; 1000
CONST	ENDS
;	COMDAT __real@0000000000000000
CONST	SEGMENT
__real@0000000000000000 DQ 00000000000000000r	; 0
CONST	ENDS
;	COMDAT ??_C@_0BD@DAIMKNPL@No?5colormap?5used?4?6?$AA@
CONST	SEGMENT
??_C@_0BD@DAIMKNPL@No?5colormap?5used?4?6?$AA@ DB 'No colormap used.', 0aH
	DB	00H						; `string'
CONST	ENDS
;	COMDAT ??_C@_0BO@ONMFPAEA@Enter?5name?5of?5colormap?5file?3?5?$AA@
CONST	SEGMENT
??_C@_0BO@ONMFPAEA@Enter?5name?5of?5colormap?5file?3?5?$AA@ DB 'Enter nam'
	DB	'e of colormap file: ', 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_0BB@BLNMEKNO@Do?5body?5colors?$DP?5?$AA@
CONST	SEGMENT
??_C@_0BB@BLNMEKNO@Do?5body?5colors?$DP?5?$AA@ DB 'Do body colors? ', 00H ; `string'
CONST	ENDS
;	COMDAT ??_C@_0CF@FNPONDP@Unintelligible?5response?4?5Try?5aga@
CONST	SEGMENT
??_C@_0CF@FNPONDP@Unintelligible?5response?4?5Try?5aga@ DB 'Unintelligibl'
	DB	'e response. Try again.', 0aH, 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_0BF@GLPJGIPO@Pixar?5file?5aborted?4?6?$AA@
CONST	SEGMENT
??_C@_0BF@GLPJGIPO@Pixar?5file?5aborted?4?6?$AA@ DB 'Pixar file aborted.', 0aH
	DB	00H						; `string'
CONST	ENDS
;	COMDAT ??_C@_0CK@CFPLAEFF@Do?5inner?0?5outer?0?5or?5all?5surfaces@
CONST	SEGMENT
??_C@_0CK@CFPLAEFF@Do?5inner?0?5outer?0?5or?5all?5surfaces@ DB 'Do inner,'
	DB	' outer, or all surfaces? (i,o,a)', 00H	; `string'
CONST	ENDS
;	COMDAT ??_C@_0BK@LHFCGKCJ@Do?5normal?5interpolation?$DP?5?$AA@
CONST	SEGMENT
??_C@_0BK@LHFCGKCJ@Do?5normal?5interpolation?$DP?5?$AA@ DB 'Do normal int'
	DB	'erpolation? ', 00H				; `string'
CONST	ENDS
;	COMDAT ??_C@_0BN@PCMGKOBJ@Enter?5name?5of?5picture?5file?3?5?$AA@
CONST	SEGMENT
??_C@_0BN@PCMGKOBJ@Enter?5name?5of?5picture?5file?3?5?$AA@ DB 'Enter name'
	DB	' of picture file: ', 00H			; `string'
; Function compile flags: /Ogtp
CONST	ENDS
;	COMDAT _pix_start
_TEXT	SEGMENT
tv271 = -288						; size = 4
_quadname$ = -284					; size = 160
_response$ = -124					; size = 120
__$ArrayPad$ = -4					; size = 4
_pix_start PROC						; COMDAT

; 38   : {

	push	ebp
	mov	ebp, esp
	sub	esp, 288				; 00000120H
	mov	eax, DWORD PTR ___security_cookie
	xor	eax, ebp
	mov	DWORD PTR __$ArrayPad$[ebp], eax

; 39   :   char quadname[160];    /* quadrilateral file */
; 40   :   char response[120];
; 41   :   char *tptr;
; 42   : 
; 43   :   if ( pix_file_name[0] == 0 )

	cmp	BYTE PTR _pix_file_name, 0
	push	esi
	push	edi
	mov	edi, 1
	jne	$LN41@pix_start

; 44   :   {  prompt("Enter name of picture file: ",pix_file_name,
; 45   :        sizeof(pix_file_name)-10);

	push	990					; 000003deH
	push	OFFSET _pix_file_name
	push	OFFSET ??_C@_0BN@PCMGKOBJ@Enter?5name?5of?5picture?5file?3?5?$AA@
	call	_prompt
	add	esp, 12					; 0000000cH

; 46   : 
; 47   :   if ( commandfd == stdin )

	call	___iob_func
	cmp	DWORD PTR _commandfd, eax
	jne	$LN41@pix_start

; 48   :   {
; 49   :     for ( normflag = -1 ; normflag == -1 ; )

	or	esi, -1
	mov	DWORD PTR _normflag, esi
$LL27@pix_start:

; 50   :     {
; 51   :       prompt("Do normal interpolation? ",response,sizeof(response));

	push	120					; 00000078H
	lea	eax, DWORD PTR _response$[ebp]
	push	eax
	push	OFFSET ??_C@_0BK@LHFCGKCJ@Do?5normal?5interpolation?$DP?5?$AA@
	call	_prompt

; 52   :       switch(toupper(response[0]))

	movsx	ecx, BYTE PTR _response$[ebp]
	mov	al, BYTE PTR _kb_upper_array[ecx]
	add	esp, 12					; 0000000cH
	cmp	al, 48					; 00000030H
	je	SHORT $LN21@pix_start
	cmp	al, 78					; 0000004eH
	je	SHORT $LN22@pix_start
	cmp	al, 89					; 00000059H
	jne	SHORT $LN20@pix_start

; 53   :          { case 'Y': normflag = 1; break;

	mov	DWORD PTR _normflag, edi
$LN40@pix_start:
	xor	esi, esi
$LN35@pix_start:

; 58   :          }
; 59   :     }
; 60   : 
; 61   :     prompt("Do inner, outer, or all surfaces? (i,o,a)",response,sizeof(response));

	push	120					; 00000078H
	lea	edx, DWORD PTR _response$[ebp]
	push	edx
	push	OFFSET ??_C@_0CK@CFPLAEFF@Do?5inner?0?5outer?0?5or?5all?5surfaces@
	call	_prompt

; 62   :     switch ( response[0] )

	mov	al, BYTE PTR _response$[ebp]
	add	esp, 12					; 0000000cH
	cmp	al, 105					; 00000069H
	je	SHORT $LN17@pix_start

; 65   :       default  : innerflag = 1; outerflag = 1; break;

	mov	DWORD PTR _outerflag, edi
	cmp	al, 111					; 0000006fH
	jne	SHORT $LN42@pix_start

; 64   :       case 'o' : innerflag = 0; outerflag = 1; break;

	mov	DWORD PTR _innerflag, esi
	jmp	SHORT $LN18@pix_start
$LN21@pix_start:

; 55   :            case '0': 
; 56   :               kb_error(3223,"Pixar file aborted.\n",RECOVERABLE);

	push	edi
	push	OFFSET ??_C@_0BF@GLPJGIPO@Pixar?5file?5aborted?4?6?$AA@
	push	3223					; 00000c97H
	call	_kb_error
	add	esp, 12					; 0000000cH
$LN20@pix_start:

; 57   :            default: outstring("Unintelligible response. Try again.\n");

	push	OFFSET ??_C@_0CF@FNPONDP@Unintelligible?5response?4?5Try?5aga@
	call	_outstring
	add	esp, 4
	cmp	DWORD PTR _normflag, esi
	je	$LL27@pix_start

; 48   :   {
; 49   :     for ( normflag = -1 ; normflag == -1 ; )

	jmp	SHORT $LN40@pix_start
$LN22@pix_start:

; 54   :            case 'N': normflag = 0; break;

	xor	esi, esi
	mov	DWORD PTR _normflag, esi
	jmp	SHORT $LN35@pix_start
$LN17@pix_start:

; 63   :     { case 'i' : innerflag = 1; outerflag = 0; break;

	mov	DWORD PTR _outerflag, esi
$LN42@pix_start:
	mov	DWORD PTR _innerflag, edi
$LN18@pix_start:

; 66   :     }
; 67   :     prompt("Do body colors? ",response,sizeof(response));

	push	120					; 00000078H
	lea	eax, DWORD PTR _response$[ebp]
	push	eax
	push	OFFSET ??_C@_0BB@BLNMEKNO@Do?5body?5colors?$DP?5?$AA@
	call	_prompt

; 68   :     if ( toupper(response[0]) == 'Y' )

	movsx	ecx, BYTE PTR _response$[ebp]
	add	esp, 12					; 0000000cH
	cmp	BYTE PTR _kb_upper_array[ecx], 89	; 00000059H
	jne	SHORT $LN14@pix_start

; 69   :       colorflag = 1;
; 70   :     else colorflag = 0;
; 71   :     if ( colorflag && ( strlen(cmapname) == 0 ) )

	mov	eax, OFFSET _cmapname
	mov	DWORD PTR _colorflag, edi
	lea	edx, DWORD PTR [eax+1]
$LL38@pix_start:
	mov	cl, BYTE PTR [eax]
	inc	eax
	test	cl, cl
	jne	SHORT $LL38@pix_start
	sub	eax, edx
	jne	SHORT $LN11@pix_start

; 72   :     { prompt("Enter name of colormap file: ",cmapname,sizeof(cmapname));

	push	1000					; 000003e8H
	push	OFFSET _cmapname
	push	OFFSET ??_C@_0BO@ONMFPAEA@Enter?5name?5of?5colormap?5file?3?5?$AA@
	call	_prompt
	add	esp, 12					; 0000000cH

; 73   :       if ( cmapname[0] == 0 )

	cmp	BYTE PTR _cmapname, 0
	jne	SHORT $LN11@pix_start

; 74   :       { outstring("No colormap used.\n"); colorflag = 0; }

	push	OFFSET ??_C@_0BD@DAIMKNPL@No?5colormap?5used?4?6?$AA@
	call	_outstring
	add	esp, 4
$LN14@pix_start:
	mov	DWORD PTR _colorflag, esi
$LN11@pix_start:

; 75   :     }
; 76   : 
; 77   :     if ( overall_size <= 0 ) resize();

	fldz
	fcomp	QWORD PTR _overall_size
	fnstsw	ax
	test	ah, 1
	jne	SHORT $LN10@pix_start
	call	_resize
$LN10@pix_start:

; 78   :     thickness = overall_size/1000;

	fld	QWORD PTR _overall_size

; 79   :     sprintf(msg,"Thicken(n | y [thickness(%g)])? ",(DOUBLE)thickness);

	mov	edx, DWORD PTR _msg
	fdiv	QWORD PTR __real@408f400000000000
	sub	esp, 8
	fst	QWORD PTR _thickness
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0CB@GHEDCGIA@Thicken?$CIn?5?$HM?5y?5?$FLthickness?$CI?$CFg?$CJ?$FN?$CJ?$DP?5@
	push	edx
	call	_sprintf

; 80   :     prompt(msg,response,sizeof(response));

	mov	ecx, DWORD PTR _msg
	push	120					; 00000078H
	lea	eax, DWORD PTR _response$[ebp]
	push	eax
	push	ecx
	call	_prompt

; 81   :     if ( toupper(response[0]) == 'Y' )

	movsx	edx, BYTE PTR _response$[ebp]
	add	esp, 28					; 0000001cH
	cmp	BYTE PTR _kb_upper_array[edx], 89	; 00000059H
	jne	SHORT $LN9@pix_start

; 82   :     { thickenflag = 1;
; 83   :       strtok(response," \t,;:\n");

	lea	eax, DWORD PTR _response$[ebp]
	push	OFFSET ??_C@_06HKCJHNLM@?5?7?0?$DL?3?6?$AA@
	push	eax
	mov	DWORD PTR _thickenflag, edi
	call	_strtok

; 84   :       tptr = strtok(NULL," \t;:,\n");

	push	OFFSET ??_C@_06IPOIOIJ@?5?7?$DL?3?0?6?$AA@
	push	esi
	call	_strtok
	add	esp, 16					; 00000010H

; 85   :       if ( tptr )

	cmp	eax, esi
	je	SHORT $LN7@pix_start

; 86   :         thickness = atof(tptr);

	push	eax
	call	_atof
	fstp	QWORD PTR _thickness
	add	esp, 4

; 87   :     }
; 88   :     else thickenflag = 0;

	jmp	SHORT $LN7@pix_start
$LN9@pix_start:
	mov	DWORD PTR _thickenflag, esi
	jmp	SHORT $LN7@pix_start
$LN41@pix_start:
	xor	esi, esi
$LN7@pix_start:

; 89   :   }
; 90   :   }
; 91   :   
; 92   :   strcpy(quadname,pix_file_name);

	xor	eax, eax
	npad	9
$LL32@pix_start:
	mov	cl, BYTE PTR _pix_file_name[eax]
	mov	BYTE PTR _quadname$[ebp+eax], cl
	add	eax, edi
	test	cl, cl
	jne	SHORT $LL32@pix_start

; 93   :   strcat(quadname,".quad");

	lea	eax, DWORD PTR _quadname$[ebp]
	dec	eax
	npad	6
$LL39@pix_start:
	mov	cl, BYTE PTR [eax+1]
	inc	eax
	test	cl, cl
	jne	SHORT $LL39@pix_start
	mov	ecx, DWORD PTR ??_C@_05KDEMKMHP@?4quad?$AA@
	mov	dx, WORD PTR ??_C@_05KDEMKMHP@?4quad?$AA@+4
	mov	DWORD PTR [eax], ecx
	mov	WORD PTR [eax+4], dx

; 94   :   pix_file_name[0] = 0;
; 95   :   pfd = fopen(quadname,"w"); 

	lea	eax, DWORD PTR _quadname$[ebp]
	push	OFFSET ??_C@_01NOFIACDB@w?$AA@
	push	eax
	mov	BYTE PTR _pix_file_name, 0
	call	_fopen
	add	esp, 8
	mov	DWORD PTR _pfd, eax

; 96   :   if (pfd == NULL)

	cmp	eax, esi
	jne	SHORT $LN6@pix_start

; 97   :   {
; 98   : 	                       
; 99   : #ifdef __L_EVOLVER__		 
; 100  : 		   		       if (l_redirect||l_evolver_mode)

	cmp	DWORD PTR _l_redirect, esi
	jne	SHORT $LN4@pix_start
	cmp	DWORD PTR _l_evolver_mode, esi
	jne	SHORT $LN4@pix_start

; 102  : 		   		       else
; 103  : 					     perror(quadname); 

	lea	ecx, DWORD PTR _quadname$[ebp]
	push	ecx
	call	_perror
	jmp	SHORT $LN43@pix_start
$LN4@pix_start:

; 101  : 			   		     erroutstring(quadname);

	lea	edx, DWORD PTR _quadname$[ebp]
	push	edx
	call	_erroutstring
$LN43@pix_start:
	add	esp, 4

; 104  : #else
; 105  :                        perror(quadname); 
; 106  : #endif
; 107  : 
; 108  : 					
; 109  :     kb_error(3373,"Bad filename.\n",RECOVERABLE);

	push	edi
	push	OFFSET ??_C@_0P@IHHAEANP@Bad?5filename?4?6?$AA@
	push	3373					; 00000d2dH
	call	_kb_error
	mov	eax, DWORD PTR _pfd
	add	esp, 12					; 0000000cH
$LN6@pix_start:

; 110  :   } 
; 111  : 
; 112  :   if ( normflag ) 

	cmp	DWORD PTR _normflag, esi
	je	SHORT $LN2@pix_start

; 113  :   { fprintf(pfd,"CNPOLY");

	push	OFFSET ??_C@_06MLLPDBGA@CNPOLY?$AA@

; 114  :   }
; 115  :   else 

	jmp	SHORT $LN44@pix_start
$LN2@pix_start:

; 116  :   { fprintf(pfd,"CPOLY");

	push	OFFSET ??_C@_05COGMDDIH@CPOLY?$AA@
$LN44@pix_start:
	push	eax
	call	_fprintf

; 117  :   }
; 118  :   fprintf(pfd,"\n");

	mov	eax, DWORD PTR _pfd
	add	esp, 8
	push	OFFSET ??_C@_01EEMJAFIK@?6?$AA@
	push	eax
	call	_fprintf

; 119  :   verts = dmatrix(0,2,0,2);

	push	119					; 00000077H
	push	OFFSET ??_C@_0L@JLMBHOKF@PIXGRAPH?4C?$AA@
	push	2
	push	esi
	push	2
	push	esi
	call	_kb_dmatrix

; 120  : } // end pix_start()

	mov	ecx, DWORD PTR __$ArrayPad$[ebp]
	add	esp, 32					; 00000020H
	pop	edi
	xor	ecx, ebp
	mov	DWORD PTR _verts, eax
	pop	esi
	call	@__security_check_cookie@4
	mov	esp, ebp
	pop	ebp
	ret	0
_pix_start ENDP
_TEXT	ENDS
PUBLIC	__real@4000000000000000
PUBLIC	??_C@_0BM@PEHEPGLC@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?$AA@ ; `string'
PUBLIC	??_C@_0BH@IKAMCDBO@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?5?$AA@ ; `string'
PUBLIC	??_C@_0BK@HMDGBGJO@?$CF17?415f?5?$CF17?415f?5?$CF17?415f?5?5?$AA@ ; `string'
PUBLIC	__$ArrayPad$
PUBLIC	_pix_facet
EXTRN	_facet_alpha:QWORD
EXTRN	_rgb_colors:BYTE
EXTRN	_colormap:DWORD
EXTRN	_web:BYTE
;	COMDAT __real@4000000000000000
CONST	SEGMENT
__real@4000000000000000 DQ 04000000000000000r	; 2
CONST	ENDS
;	COMDAT ??_C@_0BM@PEHEPGLC@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?$AA@
CONST	SEGMENT
??_C@_0BM@PEHEPGLC@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?$AA@ DB '%'
	DB	'5.3f %5.3f %5.3f %5.3f    ', 00H		; `string'
CONST	ENDS
;	COMDAT ??_C@_0BH@IKAMCDBO@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?5?$AA@
CONST	SEGMENT
??_C@_0BH@IKAMCDBO@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?5?$AA@ DB '%5.'
	DB	'3f %5.3f %5.3f     ', 00H			; `string'
CONST	ENDS
;	COMDAT ??_C@_0BK@HMDGBGJO@?$CF17?415f?5?$CF17?415f?5?$CF17?415f?5?5?$AA@
CONST	SEGMENT
??_C@_0BK@HMDGBGJO@?$CF17?415f?5?$CF17?415f?5?$CF17?415f?5?5?$AA@ DB '%17'
	DB	'.15f %17.15f %17.15f  ', 00H		; `string'
; Function compile flags: /Ogtp
CONST	ENDS
;	COMDAT _pix_facet
_TEXT	SEGMENT
_map$ = -36						; size = 32
__$ArrayPad$ = -4					; size = 4
_gdata$ = 8						; size = 4
_f_id$ = 12						; size = 4
_pix_facet PROC						; COMDAT

; 133  : {

	push	ebp
	mov	ebp, esp
	sub	esp, 36					; 00000024H
	mov	eax, DWORD PTR ___security_cookie
	xor	eax, ebp
	mov	DWORD PTR __$ArrayPad$[ebp], eax

; 134  :   int i,j;
; 135  :   REAL map[4];  /* color of this vertex */
; 136  :   int color = get_facet_color(f_id);

	mov	eax, DWORD PTR _f_id$[ebp]
	mov	ecx, DWORD PTR _web+236
	and	eax, 134217727				; 07ffffffH
	mov	edx, DWORD PTR [ecx+eax*4]
	movsx	eax, WORD PTR [edx+48]
	push	ebx
	mov	ebx, DWORD PTR _gdata$[ebp]

; 137  : 
; 138  :   if ( color == CLEAR ) return;

	cmp	eax, -1
	je	$LN34@pix_facet

; 139  :   if ( colormap )

	mov	ecx, DWORD PTR _colormap
	test	ecx, ecx
	je	SHORT $LN32@pix_facet

; 140  :       for ( i = 0 ; i < 4 ; i++ ) map[i] = colormap[gdata[0].color][i];

	mov	eax, DWORD PTR [ebx+104]
	add	eax, eax
	add	eax, eax
	fld	QWORD PTR [ecx+eax*8]
	fstp	QWORD PTR _map$[ebp]
	fld	QWORD PTR [ecx+eax*8+8]
	fstp	QWORD PTR _map$[ebp+8]
	fld	QWORD PTR [ecx+eax*8+16]
	fstp	QWORD PTR _map$[ebp+16]
	fld	QWORD PTR [ecx+eax*8+24]

; 141  :   else  {

	jmp	SHORT $LN69@pix_facet
$LN32@pix_facet:

; 142  :              for ( j = 0 ; j < 3 ; j++ )
; 143  :                 map[j] = rgb_colors[color][j];

	shl	eax, 5
	fld	QWORD PTR _rgb_colors[eax]
	fstp	QWORD PTR _map$[ebp]
	fld	QWORD PTR _rgb_colors[eax+8]
	fstp	QWORD PTR _map$[ebp+8]
	fld	QWORD PTR _rgb_colors[eax+16]
	fstp	QWORD PTR _map$[ebp+16]

; 144  :              map[3] = facet_alpha;

	fld	QWORD PTR _facet_alpha
$LN69@pix_facet:

; 145  :           }
; 146  :              
; 147  :   if ( thickenflag )

	cmp	DWORD PTR _thickenflag, 0
	fstp	QWORD PTR _map$[ebp+24]
	push	esi
	push	edi
	je	SHORT $LN61@pix_facet

; 148  :      for ( i = 0 ; i < FACET_VERTS ; i++ )

	mov	edx, DWORD PTR _web+616
	mov	esi, ebx
	mov	edi, 3
	npad	4
$LL23@pix_facet:

; 149  :         for ( j = 0 ; j < SDIM ; j++ )

	xor	ecx, ecx
	test	edx, edx
	jle	SHORT $LN22@pix_facet
	mov	eax, esi
	npad	8
$LL20@pix_facet:

; 150  :           gdata[i].x[j] += gdata[i].norm[j]*thickness;

	fld	QWORD PTR [eax+56]
	inc	ecx
	fmul	QWORD PTR _thickness
	add	eax, 8
	fadd	QWORD PTR [eax-8]
	fstp	QWORD PTR [eax-8]
	mov	edx, DWORD PTR _web+616
	cmp	ecx, edx
	jl	SHORT $LL20@pix_facet
$LN22@pix_facet:

; 148  :      for ( i = 0 ; i < FACET_VERTS ; i++ )

	add	esi, 144				; 00000090H
	dec	edi
	jne	SHORT $LL23@pix_facet
$LN61@pix_facet:

; 151  : 
; 152  :   for ( i = 0 ; i < FACET_VERTS ; i++ )

	lea	esi, DWORD PTR [ebx+8]
	mov	edi, 3
	npad	2
$LL17@pix_facet:

; 153  :      { 
; 154  :         fprintf(pfd,"%17.15f %17.15f %17.15f  ",(DOUBLE)gdata[i].x[0],
; 155  :             (DOUBLE)gdata[i].x[1],(DOUBLE)gdata[i].x[2]); 

	fld	QWORD PTR [esi+8]
	mov	eax, DWORD PTR _pfd
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [esi]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [esi-8]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BK@HMDGBGJO@?$CF17?415f?5?$CF17?415f?5?$CF17?415f?5?5?$AA@
	push	eax
	call	_fprintf
	add	esp, 32					; 00000020H

; 156  :         if ( normflag )

	cmp	DWORD PTR _normflag, 0
	je	SHORT $LN14@pix_facet

; 157  :           { fprintf(pfd,"%5.3f %5.3f %5.3f     ",(DOUBLE)gdata[i].norm[0],
; 158  :                       (DOUBLE)gdata[i].norm[1],(DOUBLE)gdata[i].norm[2]);

	fld	QWORD PTR [esi+64]
	mov	ecx, DWORD PTR _pfd
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [esi+56]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [esi+48]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BH@IKAMCDBO@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?5?$AA@
	push	ecx
	call	_fprintf
	add	esp, 32					; 00000020H
$LN14@pix_facet:

; 159  :           }
; 160  :          fprintf(pfd,"%5.3f %5.3f %5.3f %5.3f    ",
; 161  :                     (DOUBLE)map[0],(DOUBLE)map[1],(DOUBLE)map[2],(DOUBLE)map[3]);

	fld	QWORD PTR _map$[ebp+24]
	mov	edx, DWORD PTR _pfd
	sub	esp, 32					; 00000020H
	fstp	QWORD PTR [esp+24]
	fld	QWORD PTR _map$[ebp+16]
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR _map$[ebp+8]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR _map$[ebp]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BM@PEHEPGLC@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?$AA@
	push	edx
	call	_fprintf
	add	esp, 40					; 00000028H
	add	esi, 144				; 00000090H
	dec	edi
	jne	$LL17@pix_facet

; 162  :      }
; 163  : 
; 164  :   i = FACET_VERTS-1;  /* repeat last point */ 
; 165  :   fprintf(pfd,"%17.15f %17.15f %17.15f  ",(DOUBLE)gdata[i].x[0],
; 166  :       (DOUBLE)gdata[i].x[1],(DOUBLE)gdata[i].x[2]); 

	fld	QWORD PTR [ebx+304]
	mov	eax, DWORD PTR _pfd
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [ebx+296]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [ebx+288]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BK@HMDGBGJO@?$CF17?415f?5?$CF17?415f?5?$CF17?415f?5?5?$AA@
	push	eax
	call	_fprintf
	add	esp, 32					; 00000020H

; 167  :   if ( normflag )

	cmp	DWORD PTR _normflag, edi
	je	SHORT $LN13@pix_facet

; 168  :      { fprintf(pfd,"%5.3f %5.3f %5.3f     ",(DOUBLE)gdata[i].norm[0],
; 169  :                  (DOUBLE)gdata[i].norm[1],(DOUBLE)gdata[i].norm[2]);

	fld	QWORD PTR [ebx+360]
	mov	ecx, DWORD PTR _pfd
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [ebx+352]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [ebx+344]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BH@IKAMCDBO@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?5?$AA@
	push	ecx
	call	_fprintf
	add	esp, 32					; 00000020H
$LN13@pix_facet:

; 170  :      }
; 171  :   fprintf(pfd,"%5.3f %5.3f %5.3f %5.3f    ",
; 172  :               (DOUBLE)map[0],(DOUBLE)map[1],(DOUBLE)map[2],(DOUBLE)map[3]);

	fld	QWORD PTR _map$[ebp+24]
	mov	edx, DWORD PTR _pfd
	sub	esp, 32					; 00000020H
	fstp	QWORD PTR [esp+24]
	fld	QWORD PTR _map$[ebp+16]
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR _map$[ebp+8]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR _map$[ebp]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BM@PEHEPGLC@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?$AA@
	push	edx
	call	_fprintf

; 173  : 
; 174  :   fprintf(pfd,"\n");

	mov	eax, DWORD PTR _pfd
	push	OFFSET ??_C@_01EEMJAFIK@?6?$AA@
	push	eax
	call	_fprintf
	add	esp, 48					; 00000030H

; 175  : 
; 176  :   if ( thickenflag )

	cmp	DWORD PTR _thickenflag, 0
	je	$LN12@pix_facet

; 177  :     { /* do other orientation */
; 178  :      for ( i = 0 ; i < FACET_VERTS ; i++ )

	fld	QWORD PTR __real@4000000000000000
	mov	edx, DWORD PTR _web+616
	mov	esi, ebx
	mov	edi, 3
$LN11@pix_facet:

; 179  :         for ( j = 0 ; j < SDIM ; j++ )

	xor	ecx, ecx
	test	edx, edx
	jle	SHORT $LN10@pix_facet
	mov	eax, esi
$LN8@pix_facet:

; 180  :           gdata[i].x[j] -= gdata[i].norm[j]*2*thickness;

	fld	QWORD PTR [eax+56]
	inc	ecx
	fmul	ST(0), ST(1)
	add	eax, 8
	fmul	QWORD PTR _thickness
	fsubr	QWORD PTR [eax-8]
	fstp	QWORD PTR [eax-8]
	mov	edx, DWORD PTR _web+616
	cmp	ecx, edx
	jl	SHORT $LN8@pix_facet
$LN10@pix_facet:

; 177  :     { /* do other orientation */
; 178  :      for ( i = 0 ; i < FACET_VERTS ; i++ )

	add	esi, 144				; 00000090H
	dec	edi
	jne	SHORT $LN11@pix_facet
	fstp	ST(0)

; 181  : 
; 182  :      for ( i = FACET_VERTS-1 ; i >=  0 ; i-- )

	mov	edi, 2
	lea	esi, DWORD PTR [ebx+296]
	npad	6
$LL5@pix_facet:

; 183  :       { 
; 184  :         fprintf(pfd,"%17.15f %17.15f %17.15f  ",(DOUBLE)gdata[i].x[0],
; 185  :             (DOUBLE)gdata[i].x[1],(DOUBLE)gdata[i].x[2]); 

	fld	QWORD PTR [esi+8]
	mov	ecx, DWORD PTR _pfd
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [esi]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [esi-8]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BK@HMDGBGJO@?$CF17?415f?5?$CF17?415f?5?$CF17?415f?5?5?$AA@
	push	ecx
	call	_fprintf
	add	esp, 32					; 00000020H

; 186  :         if ( normflag )

	cmp	DWORD PTR _normflag, 0
	je	SHORT $LN2@pix_facet

; 187  :           { fprintf(pfd,"%5.3f %5.3f %5.3f     ",-(DOUBLE)gdata[i].norm[0],
; 188  :                       -(DOUBLE)gdata[i].norm[1],-(DOUBLE)gdata[i].norm[2]);

	fld	QWORD PTR [esi+64]
	mov	edx, DWORD PTR _pfd
	fchs
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [esi+56]
	fchs
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [esi+48]
	fchs
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BH@IKAMCDBO@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?5?$AA@
	push	edx
	call	_fprintf
	add	esp, 32					; 00000020H
$LN2@pix_facet:

; 189  :           }
; 190  :         fprintf(pfd,"%5.3f %5.3f %5.3f %5.3f    ",(DOUBLE)map[0],(DOUBLE)map[1],
; 191  :             (DOUBLE)map[2],(DOUBLE)map[3]);

	fld	QWORD PTR _map$[ebp+24]
	mov	eax, DWORD PTR _pfd
	sub	esp, 32					; 00000020H
	fstp	QWORD PTR [esp+24]
	fld	QWORD PTR _map$[ebp+16]
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR _map$[ebp+8]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR _map$[ebp]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BM@PEHEPGLC@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?$AA@
	push	eax
	call	_fprintf
	add	esp, 40					; 00000028H
	sub	esi, 144				; 00000090H
	dec	edi
	jns	$LL5@pix_facet

; 192  :       }
; 193  : 
; 194  :      i = 0;  /* repeat last point */ 
; 195  :      fprintf(pfd,"%17.15f %17.15f %17.15f  ",(DOUBLE)gdata[i].x[0],
; 196  :          (DOUBLE)gdata[i].x[1],(DOUBLE)gdata[i].x[2]); 

	fld	QWORD PTR [ebx+16]
	mov	ecx, DWORD PTR _pfd
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [ebx+8]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [ebx]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BK@HMDGBGJO@?$CF17?415f?5?$CF17?415f?5?$CF17?415f?5?5?$AA@
	push	ecx
	call	_fprintf
	add	esp, 32					; 00000020H

; 197  :      if ( normflag )

	cmp	DWORD PTR _normflag, 0
	je	SHORT $LN1@pix_facet

; 198  :         { fprintf(pfd,"%5.3f %5.3f %5.3f     ",-(DOUBLE)gdata[i].norm[0],
; 199  :                  -(DOUBLE)gdata[i].norm[1],-(DOUBLE)gdata[i].norm[2]);

	fld	QWORD PTR [ebx+72]
	mov	edx, DWORD PTR _pfd
	fchs
	sub	esp, 24					; 00000018H
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR [ebx+64]
	fchs
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR [ebx+56]
	fchs
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BH@IKAMCDBO@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?5?$AA@
	push	edx
	call	_fprintf
	add	esp, 32					; 00000020H
$LN1@pix_facet:

; 200  :         }
; 201  :      fprintf(pfd,"%5.3f %5.3f %5.3f %5.3f    ",(DOUBLE)map[0],(DOUBLE)map[1],(DOUBLE)map[2],(DOUBLE)map[3]);

	fld	QWORD PTR _map$[ebp+24]
	mov	eax, DWORD PTR _pfd
	sub	esp, 32					; 00000020H
	fstp	QWORD PTR [esp+24]
	fld	QWORD PTR _map$[ebp+16]
	fstp	QWORD PTR [esp+16]
	fld	QWORD PTR _map$[ebp+8]
	fstp	QWORD PTR [esp+8]
	fld	QWORD PTR _map$[ebp]
	fstp	QWORD PTR [esp]
	push	OFFSET ??_C@_0BM@PEHEPGLC@?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?$CF5?43f?5?5?5?5?$AA@
	push	eax
	call	_fprintf
	add	esp, 40					; 00000028H
$LN12@pix_facet:

; 202  :     } /* end thickenflag */
; 203  : 
; 204  :   fprintf(pfd,"\n");

	mov	ecx, DWORD PTR _pfd
	push	OFFSET ??_C@_01EEMJAFIK@?6?$AA@
	push	ecx
	call	_fprintf
	add	esp, 8
	pop	edi
	pop	esi
$LN34@pix_facet:

; 205  : } // end pix_facet()

	mov	ecx, DWORD PTR __$ArrayPad$[ebp]
	xor	ecx, ebp
	pop	ebx
	call	@__security_check_cookie@4
	mov	esp, ebp
	pop	ebp
	ret	0
_pix_facet ENDP
_TEXT	ENDS
PUBLIC	_pix_end
EXTRN	_free_matrix:PROC
EXTRN	_fclose:PROC
; Function compile flags: /Ogtp
;	COMDAT _pix_end
_TEXT	SEGMENT
_pix_end PROC						; COMDAT

; 217  :   fclose(pfd);

	mov	eax, DWORD PTR _pfd
	push	eax
	call	_fclose

; 218  :   free_matrix(verts);

	mov	ecx, DWORD PTR _verts
	push	ecx
	call	_free_matrix
	add	esp, 8

; 219  : } // end pix_end()

	ret	0
_pix_end ENDP
_TEXT	ENDS
END
