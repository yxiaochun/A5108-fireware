
    .section .entry

.code 16
	B	hook_proto_pkt_header_pull			// + 0x0
	NOP
	B	hook_wlc_recv					// + 0x4
	B	hook_show_string				// + 0x6
	B	hook_dngl_sendpkt				// + 0x8
	B	hook_wlc_set_chanspec				// + 0xA
	B	hook_wlc_down			 		// + 0xC
	NOP
	NOP
	NOP
	NOP


.code 16


//ROM:0019C7D4                 BL              x_proto_pkt_header_pull_sub_19C5C0

hook_proto_pkt_header_pull:
	PUSH    {R1-R7,LR}
.align 2	
	BLX	ex_proto_pkt_header_pull
	POP     {R1-R7,PC}


//ROM:001BFC7E                 BL              x_wlc_recv_sub_1B6B30
hook_wlc_recv:
	PUSH    {R0-R7,LR}
	
	mov	r2, r4 
.align 2	
	BLX	ex_hook_wlc_recv
	POP     {R0-R7,PC}

//ROM:0019B8E0                 BL              x_print_some_console_stuff_sub_8C74
hook_show_string:
	PUSH    {R0-R7,LR}
.align 2	
	BLX	ex_hook_show_string
	POP     {R0-R7,PC}

//ROM:0019C71E                 MOV             R4, R0
//ROM:0019C720                 MOV             R6, R1
hook_dngl_sendpkt:
 	MOV             R4, R0
 	MOV             R6, R1
	PUSH    {R0-R7,LR}
.align 2	
	BLX	ex_dngl_sendpkt
	POP     {R0-R7,PC}

//ROM:001AE338                 LDR.W           R0, [R0,#0x18C]
//001AE338  D0 F8 8C 01
hook_wlc_set_chanspec:
	.byte 0xD0,0xF8,0x8C,0x01
	LDR	R1, =0x1001 
	//LDR	R1, =0xD897 
	PUSH    {R0-R7,LR}
	mov	r0, r4
.align 2	
	BLX	ex_wlc_set_chanspec
	POP     {R0-R7,PC}

//ROM:001A8680                 MOV             R4, R0
//ROM:001A8682                 LDRB            R3, [R3,#0x18]
hook_wlc_down:
	
	MOV     R4, R0 
	LDRB    R3, [R3,#0x18]
	PUSH    {R0-R7,LR}
.align 2	
	BLX	ex_wlc_down
	POP     {R0-R7,PC}
