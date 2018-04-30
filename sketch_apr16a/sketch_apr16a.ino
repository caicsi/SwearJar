

/*
  LiquidCrystal Library - display() and noDisplay()

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe
  modified 7 Nov 2016
  by Arturo Guadalupi

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay

*/
#define POT_MAX 1024
#define NUM_PLAYERS 3

// include the library code:
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "./getNameAsmLib.h"

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7,
          button1pin = 12, button2pin = 13, potPin = 0;
const int OFFSET = NUM_PLAYERS + 1;
const String players[] = {"Cai", "Kelsea", "Nick"};
const int SWEAR_MAX_LENGTH = 13;

String swear;

int playerRange[NUM_PLAYERS];
int owed[NUM_PLAYERS]; //array of how many times each player still has to deposit change
int deposited[NUM_PLAYERS]; //array of how many times each player has deposited change

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int button1state = 0, button2state = 0;
int activePlayer = -1;

// EEPROM storage:
// first byte says whether or not it has been initialized
// followed by: owed (length: NUM_PLAYERS), deposited (length: NUM_PLAYERS),
//              word length (length 1), word (length word length)

void setup() {
  
   // set up the LCD's number of columns and rows:
  lcd.begin(20, 4);
  // Print a message to the LCD.
  lcd.display();

  //check if it has been initialized
  if (EEPROM.read(0) != 1) {
    lcd.print("Initializing...");

    //initialize player data
    for (int i = 1; i <= NUM_PLAYERS; i++) {
      EEPROM.write(i, 0);
      EEPROM.write(i + NUM_PLAYERS, 0);
    }
    delay(1000);

    //initialize word
    loopChangeWord(true);
    delay(1000);

    //set initialization as complete
    EEPROM.write(0, 1);
  }
  else {
    lcd.print("Booting up...");
    //load word from storage
    swear = recallWord();
  }
  
  delay(500);

  //load info from storage into arrays for quick access
  for (int i = 0; i < NUM_PLAYERS; i++)
  {
   playerRange[i] =  (POT_MAX / 3) * (i+1);
   owed[i] = EEPROM.read(i + 1);
   deposited[i] = EEPROM.read(i + OFFSET);
  }
  
  //set up buttons
  pinMode(button1pin, INPUT);
  pinMode(button2pin, INPUT);
  
  //uncomment this if you want to see the Eyes of God. (aka uninitialized data) 
  // best bug ever tho
  //printAll(activePlayer);

  lcd.clear();

  
}

void loop() {
  bool updated = false;
  int newPlayer = getName();
  if (activePlayer != newPlayer)
  {
    activePlayer = newPlayer;
    updated = true;
  }
  
  //menu
  if (activePlayer == 254) {
    //reset button
    button1state = digitalRead(button1pin);
    if (button1state == HIGH) {
      resetLoop();
      delay(500);
      updated = true;
    }
    
    //change word button
    button2state = digitalRead(button2pin);
    if (button2state == HIGH) 
    {
      updated = loopChangeWord(false);
      delay(500);
    }
  }
  
  //player screen
  else {
    //get button input
    button1state = digitalRead(button1pin);
    if (button1state == HIGH) 
    {
      addOwe(activePlayer);
      delay(750);
      updated = true;
    }
  
    button2state = digitalRead(button2pin);
    if (button2state == HIGH) 
    {
      addDeposit(activePlayer);
      delay(750);
      updated = true;
    }
  }

  if (updated) {
    if (activePlayer == 254) {
      printMenu();
    }
    else {
      printAll(activePlayer);
    }
  }
}

