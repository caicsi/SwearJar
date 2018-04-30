uint16_t getNameAsm()
{
  uint16_t potReading = analogRead(potPin);
  uint16_t index = 0;
  
  asm volatile (
    //check if pot reading is < 5, as that leads to the menu
      
      // set exit value in case of menu
      "ldi r16, -2 \n\t"
      //load potReading
      "mov XL, %A2 \n\t"
      "mov XH, %B2 \n\t"
      "cpi XH, 0 \n\t"
      "brne notMenu \n\t" // if the top byte != 0, then it isn't the menu
      "cpi XL, 6 \n\t"
      "brlo menuCheckpoint \n\t" 

      "notMenu: "
    
      "mov r16, 0 \n\t"               //set counter to zero
      "ldi XH, 4 \n\t"       //get max reading for poteniometer
      "ldi XL, 0 \n\t"
      "mov r17, %A1 \n\t"
      "mov r16, %B1 \n\t"
 
      "divLoop: "                        //loop to find index of player based on pot
        
        "eor r20, r20 \n\t" // reset counter
        "eor r19, r19 \n\t" // reset great8 flag
        "eor r21, r21 \n\t" // reset quotient bit

        // branch if x < r17:r16 to finish line
        "cp XL, r16 \n\t"
        "cpc XH, r17 \n\t" // compare with carry
        "brlo divComplete \n\t"
        
        "shiftLoop: "  // loop to shift and cmp divisor by dividend
          // left shift divisor (r17:r16)
          "lsl r17 \n\t"
          "lsl r16 \n\t"
          "brcc LSYnoCarry \n\t" 
          "sbr r17, 1 \n\t" // set the bit that was carried from r16
          "LSYnoCarry: "
          
          // inc counter (r20)
          "inc r20 \n\t"
          
          // branch if x < y to subSection:
          "cp XL, r16 \n\t"
          "cpc XH, r17 \n\t" // compare with carry
          "brlo subSection\n\t"
          "breq  RSYOnceNoCarry \n\t" // skip decrementing if they are equal
          
          // jump to shiftLoop:
          "jmp shiftLoop \n\t"
          
// menu checkpoint
"menuCheckpoint: jmp end \n\t"

        "subSection: " // haha get it like SUBtract SUBsection i am hilarious
          // dec counter (r20)
          "dec r20 \n\t"
          
          // right shift r17:r16
          "lsr r16 \n\t"
          "lsr r17 \n\t"
          "brcc RSYOnceNoCarry \n\t"
          "sbr r16, 128 \n\t" // set the bit that was carried from r16
          "RSYOnceNoCarry: "
          
          // sub dividend, divisor (storing result in dividend)
          "sub XL, r16 \n\t"
          "sbc XH, r17 \n\t"
            
          // branch if counter (r20) is greater than 8 to great8:
          "cpi r20, 8 \n\t"
          "brlo notGreat8 \n\t"
          "subi r20, 8 \n\t" // ignore bottom byte
          "sbr r19, 1 \n\t" // flag as greater than 8
          "notGreat8:"

          "ldi r21, 1 \n\t"
          "mov r18, r20 \n\t" // using r18 as a temp counter

          "cpi r18, 0 \n\t" // The bit is already in place if the counter is 0
          "breq outputSet\n\t"

          //shift the bit into the right position
          "setOutputBit: "
            "lsl r21 \n\t"
            "dec r18 \n\t"
            "brne setOutputBit \n\t" // loop if r18 != 0
          "outputSet: "
          
          // check if great8 is set (aka ZH vs ZL)
          "cpi r19, 0 \n\t"
          "brne setZH \n\t" 
            // place bit in quotient (ZL version)
            "or ZL, r21  \n\t"
            "cpi r20, 0 \n\t" // if counter == 0, we don't need to shift r17:r16 back
            "breq divLoop \n\t"
            "jmp shiftYBack  \n\t"
            
          "setZH: "
          // place bit in quotient (ZH version) \n\t"
            "or ZH, r21  \n\t"
            "subi r20, -8 \n\t"  // re add the 8 so the shiftYBack works correctly
            "jmp shiftYBack \n\t"  // this is just here for consistancy (it isn't really needed)
            
          // right shift r17:r16, (r20) times to reset the divisor
          "shiftYBack: "
            "lsr r16  \n\t"
            "lsr r17  \n\t"
            "brcc RSYLoopNoCarry  \n\t" 
            "sbr r16, 128 \n\t"  // set the bit that was carried from r16
            "RSYLoopNoCarry: "
            "dec r20 \n\t" 
            "brne shiftYBack \n\t"  // loop if r20 != 0

          "jmp divLoop \n\t" 



          
      "divComplete: "

      "mov %A0, ZL \n\t"
      "mov %B0, ZH \n\t"
      "jmp end \n\t"

      "mov r21, XL \n\t" // store remainder (not used)

      // will give range in the Z register

      //load potReading
      "mov XL, %B2 \n\t"
      "mov XH, %A2 \n\t"

      //move number of players to r19
      "mov r19, r16 \n\t"
      "dec r19 \n\t" // dec to match zero indexing
      
      "eor r16, r16 \n\t"
 
      "aloop: "                        //loop to find index of player based on potentiometer
          "cp XL, ZL \n\t"         //compare pot reading to range
          "cpc XH, ZH \n\t"
          "brlo end \n\t"             // if pot < range, jump to end
          "inc r16 \n\t"              // if not, then increase counter
          "cp r16, r19 \n\t"
          "brsh end \n\t"       // if the last index is reached, the index was found
          "sub XL, ZL \n\t"   // subtract range from pot reading
          "sbc XH, ZH \n\t"
          "jmp aloop \n\t"             // and loop back

      "end: "
          //"ldi r16, 5 \n\t"
          "eor %B2, %B2 \n\t"
          "eor %A2, %A2 \n\t"
          "eor %[result], %[result] \n\t"
          "mov %[result], r16 \n\t"    //return counter
     : [result]"=&l" (index)
     : "l" (NUM_PLAYERS),
       "l" (potReading)
     : "r16", "r17", "r18", "r19", "r20", "r21", "r27", "r26", "r31", "r30"  //clobbers
  );
  lcd.print(potReading);
  lcd.print(" ");
  lcd.print(index);
  lcd.print(" ");
  
  return index;
}

