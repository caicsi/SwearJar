//returns whether or not word was changed
//param: whether or not this is the first initialization of the system (aka the first word)
bool loopChangeWord(bool initial)
{
  bool updated;

  char activeLetter = ' ';
  int selectedLetter = 0;  //character currently selected, and also the size of the array
  char newWord[] = "              ";
  char newLetter;

  char letters[] = {' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
  const int letterSize = 27;

  printChangeWord(initial, selectedLetter, activeLetter, newWord);

  //need this delay, otherwise since this was called from a button,
  // it will still think that button is high and add one letter immediately.
  delay(500);
  
  while (true) {

    updated = false;
    
    //if they've reached the max length, don't let them input more letters
    if (selectedLetter == SWEAR_MAX_LENGTH)
      newLetter = ' ';
    else 
      newLetter = getLetter(letters, letterSize);
    
    if (activeLetter != newLetter)
    {
      activeLetter = newLetter;
      updated = true;
    }
  
    button1state = digitalRead(button1pin);
    if (button1state == HIGH)
    {
      //exit button
      if (selectedLetter == 0) {
        if (!initial) {
          return false;
        }
      }
  
      //backspace button
      else {
        //doesn't really set active letter to this, because activeLetter
        // is still based on the potentiometer.
        activeLetter = backspace(selectedLetter, newWord);
        selectedLetter--;
        updated = true;
        delay(500);
      }
    }

    button2state = digitalRead(button2pin);
    if (button2state == HIGH)
    {
      //done button
      if (activeLetter == ' ' || selectedLetter == SWEAR_MAX_LENGTH) {
        if (selectedLetter != 0) {
          swear = done(selectedLetter, newWord);
          storeWord(newWord, selectedLetter);
          return true;
        }
      }
  
      //next button
      else {
        activeLetter = next(selectedLetter, newWord, activeLetter);
        selectedLetter++;
        delay(500);
        updated = true;
      }
    }
  
    if (updated) {
      printChangeWord(initial, selectedLetter, activeLetter, newWord);
      updated = false;
    }
  }
}

char getLetter(char letters[], int arraySize)
{
  //use a little more than the maximum in order to cover rounding
 int range = (POT_MAX + 6) / arraySize;
 int index = analogRead(potPin) / range;
 return letters[index];
}

char backspace(int selectedLetter, char newWord[])
{
  char temp = newWord[selectedLetter - 1];
  newWord[selectedLetter - 1] = 0;
  return temp;
}

char next(int selectedLetter, char newWord[], char letter)
{
  newWord[selectedLetter] = letter;
  return ' ';
}

String done(int index, char newWord[])
{
  //first line
  lcd.clear();
  lcd.print("Saving word...");

  for (int i = 0; i < SWEAR_MAX_LENGTH + 1; i++) {
    if (newWord[i] == ' ') {
      newWord[i] = 0; 
      break;
    }
  }

  String newSwear = String(newWord);
 
  //show word still
  lcd.setCursor(0,2);
  lcd.print(newSwear);
  delay(500);
  
  return newSwear;
}

void printChangeWord(bool initial, int selectedLetter, char activeLetter, char newWord[])
{
  String label1, label2;

  if (selectedLetter == 0) 
    if (initial)
      label1 = "    ";
    else
      label1 = "EXIT";
  else {
    label1 = "BACK";
  }
  
  if (selectedLetter == SWEAR_MAX_LENGTH || activeLetter == ' ') { 
    if (selectedLetter != 0)
      label2 = "DONE";
    else
      label2 = "    ";
  }
  else 
    label2 = "NEXT";

  //first line
  lcd.clear();
  lcd.setCursor(11, 0);
  lcd.print(label1); 
  lcd.print(" "); 
  lcd.print(label2);

  //second line
  lcd.setCursor(0, 1);
  lcd.print("ENTER WORD:");

  //third line
  lcd.setCursor(0,2);
  for (int i = 0; i < selectedLetter; i++) {
    lcd.print(newWord[i]);
  }
  lcd.print(activeLetter);

  //fourth line
  lcd.setCursor(selectedLetter, 3);
  lcd.print("^");

  if (selectedLetter == SWEAR_MAX_LENGTH){
    lcd.setCursor(17, 3);
    lcd.print("MAX");
  }
}

//separate function to store the word in long-term memory
void storeWord(char swear[], int selectedLetter)
{
  int index = NUM_PLAYERS * 2 + 1; //index in EEPROM to start at
  int swearLen = selectedLetter;
  EEPROM.write(index, swearLen);
  for (int i = 0; i < swearLen; i++) {
    EEPROM.write((index + 1 + i), swear[i]);
  }
}

String recallWord()
{ 
  int index = NUM_PLAYERS * 2 + 1;
  int swearLen = EEPROM.read(index);
  char temp[swearLen];
  int i = 0;
  for (i = 0; i < swearLen; i++) {
    temp[i] = EEPROM.read(index + 1 + i);
  }
  temp[i] = 0;

  String sw = String(temp);
  return sw;
}


