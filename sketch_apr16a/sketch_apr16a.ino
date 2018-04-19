/*
  LiquidCrystal Library - display() and noDisplay()

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD and uses the
  display() and noDisplay() functions to turn on and off
  the display.

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

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

// include the library code:
#include <LiquidCrystal.h>
#include <EEPROM.h>
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2,
          potMax = 1024, owePin = 7, depositPin = 8;
const int numPlayers = 3;
int offset = numPlayers + 1;
const String players[] = {"Cai", "Kelsea", "Nick"};
int playerRange[numPlayers];
int owed[numPlayers]; //array of how many times each player still has to deposit change
int deposited[numPlayers]; //array of how many times each player has deposited change
//int storage[numPlayers * 2]; //array of # of change owed and deposited
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int potPin = 0;
int oweState = 0, depState = 0;

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
      for (int i = 1; i < numPlayers; i++) {
        EEPROM.write(i, 0);
        EEPROM.write(i + numPlayers, 0);
      }
      EEPROM.write(0, 1);
  }
  else {
    lcd.print("Booting up...");
    //EEPROM.write(0, 0);
  }
  delay(500);
  
  for (int i = 0; i < numPlayers; i++)
  {
   playerRange[i] =  (potMax / 3) * (i+1);
   owed[i] = EEPROM.read(i + 1);
   deposited[i] = EEPROM.read(i + offset);
  }
  //set up buttons
  pinMode(owePin, INPUT);
  pinMode(depositPin, INPUT);

}

void loop() {
  // Turn off the display:
  int player = getName();
  //lcd.clear();
  //lcd.print(players[player]);

  //Print to display
  printAll(player);

  //get button input
  oweState = digitalRead(owePin);
  if (oweState == HIGH) {
    addOwe(player);
    delay(500);
  }

  depState = digitalRead(depositPin);
  if (depState == HIGH) {
    addDeposit(player);
    delay(500);
  }
  
  delay(250);
  // Turn on the display:
}

int getName()
{
  int input = analogRead(potPin);

  for (int i = 0; i < numPlayers; i++)
  {
    if (input <= playerRange[i])
    {
      //return players[i] + " " + input;
      return i;
    }
  }
  //if potentiometer reads out of range for some reason
  //return players[0] + " " + input; 
  return 0;
}

//print:
//        OWE+  DEPOSIT+
// Kelsea  O:15 D:80
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
  EEPROM.write(player + offset, deposited[player]);
  return deposited[player];
}

//int getPlayerIndex(String player)
//{
//  for (int i = 0; i < numPlayers; i++) {
//    if (players[i] == player) {
//      return i;
//    }
//  }
//  return -1;
//}


