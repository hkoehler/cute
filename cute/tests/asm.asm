; This is just a test file,
; I used it to check the compiler,
; it does nothing specific.

#make_COM#

; COM file is loaded at CS:0100h
ORG 100h


jmp start

arr db 1, 2, 3, 4, 5

start:

MOV BX, 1
MOV SI, 1

mov arr[bx][si], 1

add [bx][si] arr, 1




mov al, 95h
add al, 95h
daa		; AL=90h , CF=1, AF=1

mov al, 05h
add al, 15h
daa		; AL=20h , CF=0, AF=1


		
mov al, 0
sub al, 95h
das		; AL=05h , CF=1, AF=1

mov al, 50h
sub al, 15h
das             ; AL=35h , CF=0, AF=1


MOV AX, [BP]
MOV AX, [BP+0]
MOV AX, [BP+3]

MOV DL, 12h
MOV DH, 34h

XCHG DL, DH

XCHG D1, DL
XCHG DH, D2


LEA BX, v1
MOV b. [BX], 5

XOR CX, CX

MOV CL, 1
OR  CL, 2
XOR CL, 1
NOT CL
NEG CL
NEG CL

MOV CX, 0ABCDh
OR  CX, 10b
XOR CX, 00B00h
NOT CX
NEG CX
NEG CX


MOV AL, 5
STC		; set Carry Flag.
SBB AL, 0       ; AL = AL - 0 - CF = 4

STC		; set Carry Flag.
ADC AL, 0       ; AL = AL + 0 + CF = 5


MOV AL, 5
STC		; set Carry Flag.
SBB AL, 2       ; AL = AL - 2 - CF = 2

STC		; set Carry Flag.
ADC AL, 2       ; AL = AL + 2 + CF = 5



STC
; just clear CF:
ADD AL, 0

STC
; clear CF, set ZF:
SUB AL, AL     ; AL = 0.


; AX = 10 * 2 = 20 (14h)
MOV AX, 10
SHL AX, 1

; AX = 10 * 2 = 20 (14h)
MOV BL, 2
MOV AX, 10
MUL BL



MOV AX, 1234h
MOV BX, 5678h

XCHG AX, BX

; the same as XCHG:
XOR AX, BX
XOR BX, AX
XOR AX, BX


LEA BX, dat
MOV AL, 2
XLATB     ; AL = 33h

JMP skip_data

dat DB 11h, 22h, 33h, 44h, 55h

skip_data:


MOV AL, 00000111b
XOR AL, 00000010b    ; AL = 00000101b


MOV AL, 00000101b
TEST AL, 1         ; ZF = 0.
TEST AL, 10b       ; ZF = 1.


MOV AX, 0
MOV CX, 5
m1: INC AX
LOOP m1

MOV AL, 2
MOV CX, 5
m2: CMP AL, 2
LOOPE m2

MOV AL, 2
MOV CX, 5
m3: CMP AL, 2
LOOPNE m3


MOV AL, 1
MOV AH, 1
CMP AL, AH


MOV CL, 0
MOV AX, 5
SHL AX, CL

MOV CL, 1
MOV AX, 0FFFFh
SHL AX, CL
SAL AX, CL
SHR AX, CL
SAR AX, CL
ROL AX, CL
ROR AX, CL
RCL AX, CL
RCR AX, CL

MOV CL, 1
MOV AL, 0FFh
SHL AL, CL
SAL AL, CL
SHR AL, CL
SAR AL, CL
ROL AL, CL
ROR AL, CL
RCL AL, CL
RCR AL, CL

LEA BX, a
SHL [BX], CL
SHL [BX], 2
SHL word ptr [BX], CL
SHL word ptr [BX], 3


; the same as MOV AX, 12
MOV     AX, 7 * 2 - 2
MOV     AX, -2 + 7 * 2

; the same as MOV AX, 10
MOV     AX, (-2 + 7) * 2

; the same as MOV AX, -10
MOV     AX, (2 - 7) * 2
MOV     AX, (- 7 + 2) * 2

; hexadecimal input:
MOV	AX, 10h	    ; 16
MOV	AX, 10h - 1 ; 15
MOV	AX, 10h / 2 ; 8

; octal input:
MOV	AX, 11o		; 9

; binary input:
MOV	AX, 1010b	; 10
; 100:
MOV	AX, 1010b * 0Ah

; modify variables using memory
; pointers:
MOV SI, OFFSET a
CS:
MOV [SI], 44h
CS:
MOV [SI+1], 55h

; treat two byte variables
; as a single word variable:
MOV CS:w.[SI], 0

; low byte is stored at
; lower address:
CS:
MOV w.[SI], 1234h

MOV w. CS: [SI], 5678h

; same thing, using different
; syntax:

MOV word ptr a, 5
MOV word ptr a, 5678h
MOV byte ptr a, 1
MOV a, 2

; compiler should report errors:
; MOV a, 5678h  
; MOV  BX, a

MOV BX, w.a
MOV CX, word ptr a


; unconditional jump forward:
; skip over next 2 bytes,
JMP $2
a DB 3    ; 1 byte.
b DB 4    ; 1 byte.

; should be compiled:
; just a check of different
; memory access syntax:

MOV AL, [BX + SI]
MOV AL, [BX + DI]
MOV AL, [BP + SI]
MOV AL, [BP + DI]
MOV AL, [BP + DI]

MOV AL, [SI]
MOV AL, [DI]
MOV AL, a ; d16 (variable offset only)
MOV AL, [BX]

; + d8
MOV AL, [BX + SI] + 5
MOV AL, [BX + DI] + 6
MOV AL, [BP + SI] + 7
MOV AL, [BP + DI] + 8

; + d8
MOV AL, [SI] + 9
MOV AL, [DI] + 1
MOV AL, [BP] + 2
MOV AL, [BX] + 3

; + d16
MOV AL, [BX + SI] + 1234h
MOV AL, [BX + DI] + 1234h
MOV AL, [BP + SI] + 1234h
MOV AL, [BP + DI] + 1234h

; + d16
MOV AL, [SI] + 1234h
MOV AL, [DI] + 1234h
MOV AL, [BP] + 1234h
MOV AL, [BX] + 1234h



; JCC jump back 7 bytes:
; (JMP takes 2 bytes itself)
MOV BL,9
DEC BL      ; 2 bytes.
CMP BL, 0   ; 3 bytes.
JNZ $-7

; modify only high byte
; of a variable
MOV BX, 1
MOV b.V1[BX], 50h

; modify two byte
; variables in once:
XOR BX, BX  ; BX = 0
MOV w.D1[BX], 1234h

;==========================

; Signed Byte
;  multiplication:
; AX = AL * BL = 10
;               (000Ah)
MOV AL, -5
MOV BL, -2
IMUL BL

NOP

; Unsigned Byte
;  multiplication:
; AX = AL * BL = 63754
;               (0F90Ah)
MOV AL, 251
MOV BL, 254
MUL BL

NOP

; Signed Word
;  multiplication:
; (DX AX) = AX * BX = 10
;            (0000 000Ah)
MOV AX, -5
MOV BX, -2
IMUL BX

NOP

; Unsigned Word
;  multiplication:
; (DX AX) = AX * BX = 4294508554
;            (0FFF9 000Ah)
MOV AX, 65531
MOV BX, 65534
MUL BX

NOP

;==========================

; Signed Word
;  division:
; AL = AX / BL = 2
; AH = -3 (remainder) (0FDh)
MOV AX, -13
MOV BL, -5
IDIV BL

NOP

; Unsigned Word
;  division:
; AL = AX / BL = 254 (0FEh)
; AH = 230 (remainder) (0E6h)
MOV AX, 65000
MOV BL, 255
DIV BL

NOP


; Signed Double-Word
;  division:
; AX = (DX AX) / BX = 2
; DX = -3 (remainder) (0FFFDh)
MOV AX, -13
; Convert Word to Double-Word
;         (AX  to DX AX)
CWD  
MOV BX, -5
IDIV BX

NOP


; Unsigned Double-Word
;  division:
; AX = (DX AX) / BX = 
;    1048575 / 255 = 4112
;                   (1010h)
; DX = 15 (remainder)
;    (000Fh)
MOV DX, 0Fh
MOV AX, 0FFFFh
MOV BX, 255
DIV BX

NOP

;==========================


MOV CL, 2
MOV AX, 255
DIV CL

MOV AX, 255
SHR AX, 1



RET  ; return to operating system.


; execution never gets here,
; so we may define variable
; here:

V1 DW 1234h

D1 DB 33h
D2 DB 44h
