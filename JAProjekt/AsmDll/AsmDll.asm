
.DATA

countWidth DWORD 0

reminder DWORD 0

increment DWORD 0

.code

;//BYTE* data, BYTE* output, int width, int padding, int start, int count
filterASM proc data: QWORD, output: QWORD, wid: SDWORD, padding: SDWORD, start: SDWORD, count: SDWORD

pop rax

push RBX
push rbp
push rsi
push rdi
push rsp
push r12
push r13
push r14
push r15

mov data, rcx
mov output, rdx
mov wid, r8d
mov padding, r9d

xor r8, r8 ; I
xor r9, r9 ; J

xor rax, rax
mov eax, wid
mov edx, 3
mul edx
add eax, padding
mov increment, eax

mov ebx, start
dec ebx
mul ebx
add rax, data
mov r12, rax

mov ecx, increment
add rax, rcx
mov r13, rax

mov ecx, increment
add rax, rcx
mov r14, rax

xor rax, rax
mov eax, increment
inc ebx
mul ebx
add rax, output
mov r15, rax

mov eax, wid
sub eax , 2
mov ecx, 10
xor edx, edx
div ecx
mov countWidth, eax
mov reminder, edx
dec count

;Pêtla rzêdy
loopHeight:
	mov eax, count
	cmp eax, r8d
	JL exit

	xor r9, r9

	;Pêtla szerokoœæ
	loopWidth:

		mov eax, countWidth
		cmp eax, r9d
		JG mainLoop
		JL widthExit
		JE pixRest

		pixRest:
		mov eax, 10
		sub eax, reminder
		mov ecx, 3
		mul ecx
		sub r12, rax
		sub r13, rax
		sub r14, rax
		sub r15, rax

		mainLoop:
		;vzeroall ; zerowanie rejstrów
		vxorps ymm0, ymm0, ymm0 

		;g³ówne instrukcje

		;pobranie danych
		movdqu xmm1, [r12 - 3]
		movdqu xmm3, [r13 - 3]
		movdqu xmm6, [r14 - 3]

		vinserti128 ymm1, ymm1, xmm1, 1
		vinserti128 ymm3, ymm3, xmm3, 1
		vinserti128 ymm6, ymm6, xmm6, 1

		movdqu xmm1, [r12 + 13]
		movdqu xmm3, [r13 + 13]
		movdqu xmm6, [r13 + 13]

		movdqu xmm4, [r13]
		vinserti128 ymm4, ymm4 , xmm4, 1
		movdqu xmm4, [r13 + 16]

		movdqu xmm2, [r12 + 3]
		movdqu xmm5, [r13 + 3]
		movdqu xmm7, [r14 + 3]

		vinserti128 ymm2, ymm2, xmm2, 1
		vinserti128 ymm5, ymm5, xmm5, 1
		vinserti128 ymm7, ymm7, xmm7, 1

		movdqu xmm2, [r12 + 19]
		movdqu xmm5, [r13 + 19]
		movdqu xmm7, [r14 + 19]

		;rozpakowanie danych
		vpunpcklbw ymm8, ymm1, ymm0
		vpunpcklbw ymm9, ymm2, ymm0
		vpunpcklbw ymm10, ymm3, ymm0
		vpunpcklbw ymm11, ymm4, ymm0
		vpunpcklbw ymm12, ymm5, ymm0
		vpunpcklbw ymm13, ymm6, ymm0
		vpunpcklbw ymm14, ymm7, ymm0

		vpunpckhbw ymm1, ymm1, ymm0
		vpunpckhbw ymm2, ymm2, ymm0
		vpunpckhbw ymm3, ymm3, ymm0
		vpunpckhbw ymm4, ymm4, ymm0
		vpunpckhbw ymm5, ymm5, ymm0
		vpunpckhbw ymm6, ymm6, ymm0
		vpunpckhbw ymm7, ymm7, ymm0

		;operacje arytmetyczne
		vpaddsw ymm4, ymm4, ymm1
		vpaddsw ymm4, ymm4, ymm3
		vpaddsw ymm4, ymm4, ymm6
		vpsubsw ymm4, ymm4, ymm2
		vpsubsw ymm4, ymm4, ymm5
		vpsubsw ymm4, ymm4, ymm7

		vpaddsw ymm11, ymm11, ymm8
		vpaddsw ymm11, ymm11, ymm10
		vpaddsw ymm11, ymm11, ymm13
		vpsubsw ymm11, ymm11, ymm9
		vpsubsw ymm11, ymm11, ymm12
		vpsubsw ymm11, ymm11, ymm14

		;pakowanie danych
		vpackuswb ymm0, ymm11, ymm4

		;zapis danych
		vextracti128 xmm15, ymm0, 1
		vmovups [r15], xmm15
		vmovups [r15 + 16], xmm0
		
		;przesuniêcie wskaŸników o 10 pikseli
		add r12, 30
		add r13, 30
		add r14, 30
		add r15, 30

		inc r9
		jmp loopWidth

	widthExit:

	inc r8
	;przesuniêcie wskaŸników do pocz¹tku nastêpnej linii
	mov eax, increment
	mov r10d, start
	add r8,r10
	mul r8
	sub r8, r10

	mov ecx, increment
	
	mov r12, data
	sub r12, rcx
	add r12, rax
	
	mov r14, data
	add r14, rcx
	add r14, rax

	mov r13, data
	add r13, rax
	mov r15, output
	add r15, rax
	
	jmp loopHeight

exit:

vzeroall
pop r15
pop r14
pop r13
pop r12
pop rsp
pop rdi
pop rsi
pop rbp
pop rbx

ret

filterASM endp
end