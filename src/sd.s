; ATA read sectors (LBA mode)
; Based on OSDev Wiki ATA PIO Mode article

    [BITS 32]
    global ata_lba_read

ata_lba_read:
    push ebp
    mov ebp,esp
    push eax
    push ebx
    push ecx
    push edx
    push edi

    mov edx, 0x03F6      ; Digital output register
    mov al,2             ; Disable interrupts
    out dx,al

    mov eax,[8+ebp]      ; Get LBA in EAX
    mov edi,[12+ebp]     ; Get buffer in EDI
    mov ecx,[16+ebp]     ; Get sector count in ECX
    and eax, 0x0FFFFFFF
    mov ebx, eax         ; Save LBA in EBX

    mov edx, 0x01F6      ; Port to send drive and bit 24 - 27 of LBA
    shr eax, 24          ; Get bit 24 - 27 in al
    or al, 11100000b     ; Set bit 6 in al for LBA mode
    out dx, al

    mov edx, 0x01F2      ; Port to send number of sectors
    mov al, cl           ; Get number of sectors from CL
    out dx, al

    mov edx, 0x1F3       ; Port to send bit 0 - 7 of LBA
    mov eax, ebx         ; Get LBA from EBX
    out dx, al

    mov edx, 0x1F4       ; Port to send bit 8 - 15 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 8           ; Get bit 8 - 15 in AL
    out dx, al

    mov edx, 0x1F5       ; Port to send bit 16 - 23 of LBA
    mov eax, ebx         ; Get LBA from EBX
    shr eax, 16          ; Get bit 16 - 23 in AL
    out dx, al

    mov edx, 0x1F7       ; Command port
    mov al, 0x20         ; Read with retry.
    out dx, al

    mov ecx, 4
.lp1:
    in al, dx
    test al, 0x80
    jne short .retry
    test al, 8
    jne short .data_rdy
.retry:
    dec ecx
    jg short .lp1

.pior_l:
    in al, dx
    test al, 0x80
    jne short .pior_l
    test al, 0x21
    jne short .fail
    
.data_rdy:
    mov edx, 0x1F0
    mov cx, 256
    rep insw

    mov edx,0x1f7
    in al, dx
    in al, dx
    in al, dx
    in al, dx

    dec dword [16+ebp]
    jne short .pior_l

    mov edx,0x1f0
    xor eax,eax
    test al, 0x21
    je short .done

.fail:
    mov eax,-1

.done:
    pop edi
    pop edx
    pop ecx
    pop ebx
    leave
    ret
