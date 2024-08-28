; Listing generated by Microsoft (R) Optimizing Compiler Version 16.00.40219.01 

	TITLE	C:\levolver\yonkang-sefit\levolver\ev_ogl\MACHINE.C
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	_set_ctypes
EXTRN	_kb_lower_array:BYTE
EXTRN	_kb_upper_array:BYTE
; Function compile flags: /Ogtp
; File c:\levolver\yonkang-sefit\levolver\ev_ogl\machine.c
;	COMDAT _set_ctypes
_TEXT	SEGMENT
_set_ctypes PROC					; COMDAT

; 21   :   int c;
; 22   :   for ( c = 0xFF ; c > 0 ; c-- )

	xor	eax, eax
	npad	14
$LL9@set_ctypes:

; 23   :   { kb_upper_array[c] = (char)c;

	lea	ecx, DWORD PTR [eax-1]
	mov	BYTE PTR _kb_upper_array[eax+255], cl

; 24   :     kb_lower_array[c] = (char)c;

	mov	BYTE PTR _kb_lower_array[eax+255], cl
	dec	eax
	lea	ecx, DWORD PTR [eax+255]
	test	ecx, ecx
	jg	SHORT $LL9@set_ctypes

; 25   :   }
; 26   :   for ( c = 'a' ; c <= 'z' ; c++ )

	mov	eax, 97					; 00000061H
	npad	1
$LL6@set_ctypes:

; 27   :     kb_upper_array[c] = (char)(c + 'A' - 'a');

	lea	edx, DWORD PTR [eax-32]
	mov	BYTE PTR _kb_upper_array[eax], dl
	inc	eax
	cmp	eax, 122				; 0000007aH
	jle	SHORT $LL6@set_ctypes

; 28   :   for ( c = 'A' ; c <= 'Z' ; c++ )

	mov	eax, 65					; 00000041H
$LL3@set_ctypes:

; 29   :     kb_lower_array[c] = (char)(c - 'A' + 'a');

	lea	ecx, DWORD PTR [eax+32]
	mov	BYTE PTR _kb_lower_array[eax], cl
	inc	eax
	cmp	eax, 90					; 0000005aH
	jle	SHORT $LL3@set_ctypes

; 30   : 
; 31   : } // end set_ctypes()

	ret	0
_set_ctypes ENDP
_TEXT	ENDS
PUBLIC	_kb_stricmp
; Function compile flags: /Ogtp
;	COMDAT _kb_stricmp
_TEXT	SEGMENT
_a$ = 8							; size = 4
_b$ = 12						; size = 4
_kb_stricmp PROC					; COMDAT

; 41   : { register char aa,bb;  /* lower case versions of characters */

	push	ebp
	mov	ebp, esp

; 42   :   for(;;a++,b++)
; 43   :   { aa = tolower(*a); 
; 44   :     bb = tolower(*b);

	mov	ecx, DWORD PTR _b$[ebp]
	movsx	edx, BYTE PTR [ecx]
	mov	dl, BYTE PTR _kb_lower_array[edx]
	push	esi
	mov	esi, DWORD PTR _a$[ebp]
	movsx	eax, BYTE PTR [esi]
	mov	al, BYTE PTR _kb_lower_array[eax]

; 45   :     if ( aa < bb ) return -1;

	cmp	al, dl
	jl	SHORT $LN11@kb_stricmp
	sub	esi, ecx

; 46   :     if ( aa > bb ) return 1;

	cmp	al, dl
$LN15@kb_stricmp:
	jg	SHORT $LN12@kb_stricmp

; 47   :     if ( !aa ) break;  /* have reached both nulls */

	test	al, al
	je	SHORT $LN13@kb_stricmp

; 42   :   for(;;a++,b++)
; 43   :   { aa = tolower(*a); 
; 44   :     bb = tolower(*b);

	movsx	eax, BYTE PTR [esi+ecx+1]
	movsx	edx, BYTE PTR [ecx+1]
	mov	al, BYTE PTR _kb_lower_array[eax]
	mov	dl, BYTE PTR _kb_lower_array[edx]
	inc	ecx

; 45   :     if ( aa < bb ) return -1;

	cmp	al, dl
	jge	SHORT $LN15@kb_stricmp
$LN11@kb_stricmp:
	or	eax, -1
	pop	esi

; 50   : } // end kb_stricmp()

	pop	ebp
	ret	0
$LN12@kb_stricmp:

; 46   :     if ( aa > bb ) return 1;

	mov	eax, 1
	pop	esi

; 50   : } // end kb_stricmp()

	pop	ebp
	ret	0
$LN13@kb_stricmp:

; 48   :   }
; 49   :   return 0;  /* equal strings */

	xor	eax, eax
	pop	esi

; 50   : } // end kb_stricmp()

	pop	ebp
	ret	0
_kb_stricmp ENDP
_TEXT	ENDS
PUBLIC	_kb_strnicmp
; Function compile flags: /Ogtp
;	COMDAT _kb_strnicmp
_TEXT	SEGMENT
_a$ = 8							; size = 4
_b$ = 12						; size = 4
_n$ = 16						; size = 4
_kb_strnicmp PROC					; COMDAT

; 64   : { register char aa,bb;  /* lower case versions of characters */

	push	ebp
	mov	ebp, esp
	push	esi

; 65   :   for(;n;n--,a++,b++)

	mov	esi, DWORD PTR _n$[ebp]
	push	edi
	test	esi, esi
	je	SHORT $LN12@kb_strnicm
	mov	edi, DWORD PTR _a$[ebp]
	mov	ecx, DWORD PTR _b$[ebp]
	sub	edi, ecx
	npad	12
$LL6@kb_strnicm:

; 66   :   { aa = tolower(*a); 

	movsx	eax, BYTE PTR [edi+ecx]

; 67   :     bb = tolower(*b);

	movsx	edx, BYTE PTR [ecx]
	mov	al, BYTE PTR _kb_lower_array[eax]
	mov	dl, BYTE PTR _kb_lower_array[edx]

; 68   :     if ( aa < bb ) return -1;

	cmp	al, dl
	jl	SHORT $LN10@kb_strnicm

; 69   :     if ( aa > bb ) return 1;

	jg	SHORT $LN11@kb_strnicm

; 70   :     if ( !aa ) break;  /* have reached both nulls */

	test	al, al
	je	SHORT $LN12@kb_strnicm

; 65   :   for(;n;n--,a++,b++)

	inc	ecx
	dec	esi
	jne	SHORT $LL6@kb_strnicm
$LN12@kb_strnicm:
	pop	edi

; 71   :   }
; 72   :   return 0;  /* equal strings */

	xor	eax, eax
	pop	esi

; 73   : } // end kb_strnicmp()

	pop	ebp
	ret	0
$LN10@kb_strnicm:
	pop	edi

; 68   :     if ( aa < bb ) return -1;

	or	eax, -1
	pop	esi

; 73   : } // end kb_strnicmp()

	pop	ebp
	ret	0
$LN11@kb_strnicm:
	pop	edi

; 69   :     if ( aa > bb ) return 1;

	mov	eax, 1
	pop	esi

; 73   : } // end kb_strnicmp()

	pop	ebp
	ret	0
_kb_strnicmp ENDP
_TEXT	ENDS
PUBLIC	_kb_strupr
; Function compile flags: /Ogtp
;	COMDAT _kb_strupr
_TEXT	SEGMENT
_s$ = 8							; size = 4
_kb_strupr PROC						; COMDAT

; 83   : {

	push	ebp
	mov	ebp, esp

; 84   :   while ( *s )

	mov	eax, DWORD PTR _s$[ebp]
	cmp	BYTE PTR [eax], 0
	je	SHORT $LN1@kb_strupr
	npad	5
$LL2@kb_strupr:

; 85   :      { *s = (char)toupper(*s);

	movsx	ecx, BYTE PTR [eax]
	mov	dl, BYTE PTR _kb_upper_array[ecx]
	mov	BYTE PTR [eax], dl

; 86   :         s++;

	inc	eax
	cmp	BYTE PTR [eax], 0
	jne	SHORT $LL2@kb_strupr
$LN1@kb_strupr:

; 87   :      }
; 88   : } // end kb_strupr()

	pop	ebp
	ret	0
_kb_strupr ENDP
_TEXT	ENDS
PUBLIC	_kb_strstr
; Function compile flags: /Ogtp
;	COMDAT _kb_strstr
_TEXT	SEGMENT
_a$ = 8							; size = 4
_b$ = 12						; size = 4
_kb_strstr PROC						; COMDAT

; 98   : {

	push	ebp
	mov	ebp, esp

; 99   :   char *ptr,*ch;
; 100  : 
; 101  :   for ( ; *a ; a++ )

	mov	eax, DWORD PTR _a$[ebp]
	cmp	BYTE PTR [eax], 0
	push	ebx
	push	esi
	push	edi
	je	SHORT $LN5@kb_strstr
	mov	edi, DWORD PTR _b$[ebp]
	mov	bl, BYTE PTR [edi]
	mov	esi, eax
	sub	esi, edi
$LL7@kb_strstr:

; 102  :      { for ( ptr = a, ch = b; *ch && (*ptr == *ch) ; ptr++,ch++ ) 

	mov	ecx, edi
	test	bl, bl
	je	SHORT $LN2@kb_strstr
	mov	dl, bl
	npad	1
$LL4@kb_strstr:
	cmp	BYTE PTR [esi+ecx], dl
	jne	SHORT $LN2@kb_strstr
	mov	dl, BYTE PTR [ecx+1]
	inc	ecx
	test	dl, dl
	jne	SHORT $LL4@kb_strstr
	pop	edi
	pop	esi
	pop	ebx

; 107  : } // end kb_strstr()

	pop	ebp
	ret	0
$LN2@kb_strstr:

; 103  :           ;
; 104  :        if ( *ch == '\0' ) return a;

	cmp	BYTE PTR [ecx], 0
	je	SHORT $LN8@kb_strstr

; 99   :   char *ptr,*ch;
; 100  : 
; 101  :   for ( ; *a ; a++ )

	inc	eax
	inc	esi
	cmp	BYTE PTR [eax], 0
	jne	SHORT $LL7@kb_strstr
$LN5@kb_strstr:

; 105  :      }
; 106  :   return NULL;

	xor	eax, eax
$LN8@kb_strstr:
	pop	edi
	pop	esi
	pop	ebx

; 107  : } // end kb_strstr()

	pop	ebp
	ret	0
_kb_strstr ENDP
_TEXT	ENDS
PUBLIC	_kb_memmove
; Function compile flags: /Ogtp
;	COMDAT _kb_memmove
_TEXT	SEGMENT
_dest$ = 8						; size = 4
_src$ = 12						; size = 4
_n$ = 16						; size = 4
_kb_memmove PROC					; COMDAT

; 120  : {

	push	ebp
	mov	ebp, esp

; 121  :   /* crude bytewise move */
; 122  :   if ( (dest - src) > 0 )  /* move from top down */

	mov	eax, DWORD PTR _dest$[ebp]
	mov	ecx, DWORD PTR _src$[ebp]
	mov	edx, eax
	sub	edx, ecx
	push	esi

; 123  :      { 
; 124  :         src += n; dest += n; 

	mov	esi, DWORD PTR _n$[ebp]
	test	edx, edx
	jle	SHORT $LN8@kb_memmove
	add	ecx, esi
	add	eax, esi

; 125  :         for ( ; n ; n-- ) *(--dest) = *(--src);

	test	esi, esi
	je	SHORT $LN1@kb_memmove
	npad	3
$LL7@kb_memmove:
	mov	dl, BYTE PTR [ecx-1]
	dec	ecx
	dec	eax
	dec	esi
	mov	BYTE PTR [eax], dl
	jne	SHORT $LL7@kb_memmove
	pop	esi

; 130  :      }
; 131  : } // end kb_memmove()

	pop	ebp
	ret	0
$LN8@kb_memmove:

; 126  :      }
; 127  :   else  /* move from bottom up */
; 128  :      { 
; 129  :         for ( ; n ; n-- ) *(dest++) = *(src++);

	test	esi, esi
	je	SHORT $LN1@kb_memmove
$LL3@kb_memmove:
	mov	dl, BYTE PTR [ecx]
	mov	BYTE PTR [eax], dl
	inc	eax
	inc	ecx
	dec	esi
	jne	SHORT $LL3@kb_memmove
$LN1@kb_memmove:
	pop	esi

; 130  :      }
; 131  : } // end kb_memmove()

	pop	ebp
	ret	0
_kb_memmove ENDP
_TEXT	ENDS
END
