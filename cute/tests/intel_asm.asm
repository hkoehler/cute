	    BITS	32
	    GLOBAL	retcpuid
	    GLOBAL	rdtscret
	    extern	stringcpu,AxoneEDX,CacheEAX,CacheEDX,XthreeB
	    extern	Time1a,Maxcpuid,AxoneEAX,XthreeC,XthreeD,AxoneEBX
	    
	    SECTION	.text

; prototype: void retcpuid(void)
retcpuid:
	    push	esi
	    xor		eax,eax
	    lea		esi,[stringcpu]
	    cpuid
	    mov		[esi],ebx
	    mov		[esi+4],edx
	    mov		[esi+8],ecx	
	    mov		dword [esi+12],0x0d0a	
	    mov		[Maxcpuid],eax
	    mov		eax,1
	    cpuid
	    mov		[AxoneEAX],eax
	    mov		[AxoneEBX],ebx
	    mov		eax,2
	    mov		[AxoneEDX],edx
	    cmp		eax,[Maxcpuid]
	    jb		nopro
	    cpuid	
	    mov		[CacheEAX],eax
	    mov		[CacheEDX],edx
	    mov		eax,3
	    cmp		eax,[Maxcpuid]
	    jb		nopro
	    cpuid
	    mov		[XthreeB],ebx
	    mov		[XthreeC],ecx
	    mov		[XthreeD],edx
	    
nopro:	    
	    pop		esi
	    ret
	    
	    
	    	
rdtscret:
	    rdtsc
	    cmp		byte [esp+4],0
	    jz		firstime
	    sub		eax,[Time1a]
	    sbb		edx,[Time1a+4]
	    mov		ecx,1998000
	    div		ecx
	    cmp		edx,999900
	    cmc
	    adc		eax,0
firstime:	    
	    mov		[Time1a],eax
	    mov		[Time1a+4],edx
	    ret

