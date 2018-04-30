
int getName() 
{
  uint16_t input = analogRead(potPin);
  return getNameAsm(NUM_PLAYERS, input);
//  if (input <= 5) {
//     return -2;
//  }
//  for (int i = 0; i < NUM_PLAYERS; i++)
//  {
//    if (input <= playerRange[i])
//    {
//      return i;
//    }
//  }
//  //if potentiometer reads out of range for some reason
//  return 0;
}

//print:
//        OWE+ DEPOSIT+
// Kelsea O:15 D:80
// WORD: WHATEVER
void printAll(int index) 
{
  //int index = getPlayerIndex(player);
  //first line
  lcd.clear();
  lcd.setCursor(7, 0);
  lcd.print("OWE+ DEPOSIT+");

  //second line
  lcd.setCursor(0, 1);
  lcd.print(players[index]);
  lcd.setCursor(7, 1);
  lcd.print("O:");
  lcd.print(owed[index]);
  lcd.setCursor(12, 1);
  lcd.print("D:");
  lcd.print(deposited[index]);

  //third line

  //fourth line
  lcd.setCursor(0, 3);
  lcd.print("WORD: " + swear);
}

//print:
//      STATS  CHANGE
// MENU         WORD
// WORD: YES
void printMenu()
{
  //first line
  lcd.clear();
  lcd.setCursor(8, 0);
  lcd.print("RESET CHANGE");

  //second line
  lcd.setCursor(16, 1);
  lcd.print("WORD");

  //third line

  //fourth line
  lcd.setCursor(0, 3);
  lcd.print("WORD: " + swear);
}

int addOwe(int player) 
{
  owed[player]++;
  lcd.clear();
  //first line
  lcd.print("Owed +1 coin.");
  //third line
  lcd.setCursor(0, 2);
  lcd.print("New amount owed");
  //fourth line
  lcd.setCursor(0, 3);
  lcd.print("for " + players[player] + ": ");
  lcd.print(owed[player]);

  //store new value in memory
  EEPROM.write(player + 1, owed[player]);
  return owed[player];
}

int addDeposit(int player) 
{
  deposited[player]++;

  //decrement amount owed for this player if they owe anything
  if (owed[player] > 0) {
    owed[player]--;
    EEPROM.write(player + 1, owed[player]);
  }

  lcd.clear();
  //first line
  lcd.print("Deposited +1 coin!");
  //third line
  lcd.setCursor(0, 2);
  lcd.print("New amount deposited");
  //fourth line
  lcd.setCursor(0, 3);
  lcd.print("for " + players[player] + ": ");
  lcd.print(deposited[player]);

  //store new value in memory
  EEPROM.write(player + OFFSET, deposited[player]);
  return deposited[player];
}


void resetLoop()
{
  //print confirmation screen

  //first line
  lcd.clear();
  lcd.print("RESET");
  lcd.setCursor(14, 0);
  lcd.print("NO YES");

  //second line
  lcd.setCursor(0, 1);
  lcd.print("This will clear all");

  //third line
  lcd.setCursor(0, 2);
  lcd.print("long-term storage.");

  //fourth line
  lcd.setCursor(0, 3);
  lcd.print("Are you sure?");

  delay(1000);

  //wait for user response
  while (true) {

    //no
    button1state = digitalRead(button1pin);
    if (button1state == HIGH) {
      lcd.clear();
      lcd.print("Thank goodness.");
      return false;
    }

    //yes
    button2state = digitalRead(button2pin);
    if (button2state == HIGH) {
      reset();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Memory cleared.");
      lcd.setCursor(0, 1);
      lcd.print("Please hit reset");
      lcd.setCursor(0, 2);
      lcd.print(" button on Arduino.");
      while (true) {
          //enter infinite loop to force user to hit reset on arduino.
      }
    } 
  }
}

void reset()
{
  //clear EEPROM

  //index of size of word
  int index = NUM_PLAYERS * 2 + 1;

  //only need to clear memory for the word, because the other EEPROM memory is cleared in initialization
  // clears all 13 possible bytes, in case past words have been longest possible
  for (int i = index; i < index + SWEAR_MAX_LENGTH; i++) {
    EEPROM.write(i, 0);
  }

  //set first byte to 0 to indicate that data needs to be re-initialized.
  EEPROM.write(0, 0);
}



