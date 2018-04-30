
; uint8_t getNameAsm(uint8_t players, uint16_t potReading)
; players = r24
; potReading = r22, r23

.global getNameAsm
    ;check if pot reading is < 5, as that leads to the menu
   getNameAsm:
      ; set exit value in case of menu
      ;load potReading
      mov XL, r22
      mov XH, r23
      cpi XH, 0 
      brne notMenu  ; if the top byte != 0, then it isn't the menu
      cpi XL, 6 
      brlo menuCheckpoint  

      notMenu: 
    
      ldi XH,  4       ;get max reading for poteniometer
      ldi XL, 0 
      ldi r25, 0 
      mov r24, r24     ; it is already in the register
 
      divLoop:                         ;loop to find index of player based on pot
        
        eor r20, r20  ; reset counter
        eor r19, r19  ; reset great8 flag
        eor r21, r21  ; reset quotient bit

        ; branch if x < r25:r24 to finish line
        cp XL, r24 
        cpc XH, r25  ; compare with carry
        brlo divComplete 
        
        shiftLoop:   ; loop to shift and cmp divisor by dividend
          ; left shift divisor (r25:r24)
          lsl r25 
          lsl r24 
          brcc LSYnoCarry  
          sbr r25, 1  ; set the bit that was carried from r24
          LSYnoCarry: 
          
          ; inc counter (r20)
          inc r20 
          
          ; branch if x < y to subSection:
          cp XL, r24 
          cpc XH, r25  ; compare with carry
          brlo subSection
          breq  RSYOnceNoCarry  ; skip decrementing if they are equal
          
          ; jump to shiftLoop:
          jmp shiftLoop 
          
; menu checkpoint
menuCheckpoint: 
ldi r24, -2 ; set the return value
jmp end 

        subSection:  ; haha get it like SUBtract SUBsection i am hilarious
          ; dec counter (r20)
          dec r20 
          
          ; right shift r25:r24
          lsr r24 
          lsr r25 
          brcc RSYOnceNoCarry 
          sbr r24, 128  ; set the bit that was carried from r24
          RSYOnceNoCarry: 
          
          ; sub dividend, divisor (storing result in dividend)
          sub XL, r24 
          sbc XH, r25 
            
          ; branch if counter (r20) is greater than 8 to great8:
          cpi r20, 8 
          brlo notGreat8 
          subi r20, 8  ; ignore bottom byte
          sbr r19, 1  ; flag as greater than 8
          notGreat8:

          ldi r21, 1 
          mov r18, r20  ; using r18 as a temp counter

          cpi r18, 0  ; The bit is already in place if the counter is 0
          breq outputSet

          ;shift the bit into the right position
          setOutputBit: 
            lsl r21 
            dec r18 
            brne setOutputBit  ; loop if r18 != 0
          outputSet: 
          
          ; check if great8 is set (aka ZH vs ZL)
          cpi r19, 0 
          brne setZH  
            ; place bit in quotient (ZL version)
            or ZL, r21  
            cpi r20, 0  ; if counter == 0, we don't need to shift r25:r24 back
            breq divLoop 
            jmp shiftYBack  
            
          setZH: 
          ; place bit in quotient (ZH version) 
            or ZH, r21  
            subi r20, -8   ; re add the 8 so the shiftYBack works correctly
            jmp shiftYBack   ; this is just here for consistancy (it isn't really needed)
            
          ; right shift r25:r24, (r20) times to reset the divisor
          shiftYBack: 
            lsr r24  
            lsr r25  
            brcc RSYLoopNoCarry   
            sbr r24, 128   ; set the bit that was carried from r24
            RSYLoopNoCarry: 
            dec r20  
            brne shiftYBack   ; loop if r20 != 0

          jmp divLoop  



          
      divComplete: 


      mov r21, XL  ; store remainder (not used)

      ; will give range in the Z register

      ;load potReading
      mov XL, r22
      mov XH, r23

      ;move number of players to r19
      mov r19, r24 
      dec r19  ; dec to match zero indexing
      
      eor r24, r24 
 
      aloop:                         ;loop to find index of player based on potentiometer
          cp XL, ZL          ;compare pot reading to range
          cpc XH, ZH 
          brlo end              ; if pot < range, jump to end
          inc r24               ; if not, then increase counter
          cp r24, r19 
          brsh end        ; if the last index is reached, the index was found
          sub XL, ZL    ; subtract range from pot reading
          sbc XH, ZH 
          jmp aloop              ; and loop back

      end: 

	  ;The return register (r24) has the index already
    ret