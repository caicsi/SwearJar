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
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 2, en = 3, d4 = 4, d5 = 5, d6 = 6, d7 = 7,
          owePin = 12, depositPin = 13, potPin = 0;
const int OFFSET = NUM_PLAYERS + 1;
const String players[] = {"Cai", "Kelsea", "Nick"};

int playerRange[NUM_PLAYERS];
int owed[NUM_PLAYERS]; //array of how many times each player still has to deposit change
int deposited[NUM_PLAYERS]; //array of how many times each player has deposited change

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int oweState = 0, depState = 0;
int activePlayer;

// EEPROM storage:
// first byte says whether or not it has been initialized
// followed by: owed, deposited. 

void setup() {

   // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a mess age to the LCD.
  lcd.display();

  //check if it has been initialized
  if (EEPROM.read(0) != 1) {
    lcd.print("Initializing...");
      for (int i = 1; i < NUM_PLAYERS; i++) {
        EEPROM.write(i, 0);
        EEPROM.write(i + NUM_PLAYERS, 0);
      }
      EEPROM.write(0, 1);
  }
  else {
    lcd.print("Booting up...");
    //EEPROM.write(0, 0);
  }

  activePlayer = getName();
  
  delay(500);
  
  for (int i = 0; i < NUM_PLAYERS; i++)
  {
   playerRange[i] =  (POT_MAX / 3) * (i+1);
   owed[i] = EEPROM.read(i + 1);
   deposited[i] = EEPROM.read(i + OFFSET);
  }
  //set up buttons
  pinMode(owePin, INPUT);
  pinMode(depositPin, INPUT);

  printAll(activePlayer);

}

void loop() {
  bool updated = false;
  int newPlayer = getName();
  if (activePlayer != newPlayer)
  {
    activePlayer = newPlayer;
    updated = true;
  }
  
  //get button input
  oweState = digitalRead(owePin);
  if (oweState == HIGH) 
  {
    addOwe(activePlayer);
    delay(750);
    updated = true;
  }

  depState = digitalRead(depositPin);
  if (depState == HIGH) 
  {
    addDeposit(activePlayer);
    delay(750);
    updated = true;
  }

  if (updated)
    printAll(activePlayer);
}

int getName() 
{
  int input = analogRead(potPin);
  for (int i = 0; i < NUM_PLAYERS; i++)
  {
    if (input <= playerRange[i])
    {
      return i;
    }
  }
  //if potentiometer reads out of range for some reason
  return 0;
}

int getNameAsm()
{
  //we should really store the ranges in stack or something, or else this will be a pain.
  int range = POT_MAX / NUM_PLAYERS; //done not in assembly because asm has no division

  int potReading = analogRead(potPin);
  int index;
  asm volatile (
      "mov r16, 0 \n\t"               //set counter to zero
      ";mov r17, 0 \n\t"          //get potentiometer reading
      ";mov XH, HIGH(potMax) \n\t"       //get max reading for poteniometer
      ";mov XL, LOW(potMax) \n\t"
 
      "divLoop: "                        //loop to find index of player based on pot
        " \n\t"
        " \n\t"
        " \n\t"
        " \n\t"
        " \n\t"

      // will give range in the X register
 
      "loop: "                        //loop to find index of player based on pot
          "cp r17, r18 \n\t"         //compare pot reading to range
          "brlt end \n\t"             // if pot < range, jump to end
          "inc r16 \n\t"              // if not, then increase counter
          "subi r18, 0 \n\t"   // subtract range from pot reading
          "jmp loop \n\t"             // and loop back

      "end: "
          "mov %0, r18 \n\t"    //return counter
     : "=r" (index)
     : [players] "n" (NUM_PLAYERS),
       [potMax] "n" (POT_MAX)
     : "r16", "r17", "r18" //clobbers
  );
  return index;
}

//print:
//        OWE+  DEPOSIT+
// Kelsea O:15 D:80
// WORD: WHATEVER
void printAll(int index) 
{
  //int index = getPlayerIndex(player);
  //first line
  lcd.clear();
  lcd.print("       OWE+ DEPOSIT+");

  //second line
  lcd.setCursor(0, 1);
  lcd.print(players[index]);
  lcd.setCursor(7, 1);
  lcd.print("O:");
  lcd.print(owed[index]);
  lcd.setCursor(12, 1);
  lcd.print("D:");
  lcd.print(deposited[index]);
  //lcd.print("     ");

  //third line
  lcd.setCursor(19, 1);
  lcd.print(" WORD: STEVEN");
  lcd.print(getNameAsm());
}


int addOwe(int player) 
{
  owed[player]++;
  lcd.clear();
  lcd.print("Owed +1 coin.");
  lcd.setCursor(0, 1);
  lcd.print("New amount owed for " + players[player] + ":");
  lcd.print(owed[player]);
  EEPROM.write(player + 1, owed[player]);
  return owed[player];
}

int addDeposit(int player) 
{
  deposited[player]++;
  if (owed[player] > 0) {
    owed[player]--;
    EEPROM.write(player + 1, owed[player]);
  }
  lcd.clear();
  lcd.print("Deposited +1 coin!");
  lcd.setCursor(0, 1);
  lcd.print("New amount deposited for " + players[player] + ":");
  lcd.print(deposited[player]);
  EEPROM.write(player + OFFSET, deposited[player]);
  return deposited[player];
}

//int getPlayerIndex(String player)
//{
//  for (int i = 0; i < NUM_PLAYERS; i++) {
//    if (players[i] == player) {
//      return i;
//    }
//  }
//  return -1;
//}


