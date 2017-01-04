#include <EEPROM.h> //used for storing things in the EEPROM memorry. 
// The Teensy 3.2 has 2048 bytes.

///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// user defined /////////////////////////////////////
// pin set up.
// Array for the pins for all the comlumns.
int Column[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
// Array for the pins for all the rows.
int Row[] = {16, 17, 18, 19, 20};

// caplocks led pin.
const int capsLedPin = 21;
bool useCapsLed = false; // set to true if using led when caplocks is on. 

// keys set up.
int maxKeysPressed = 6; // Set to the maximum number of keys that can be pressed at a time.

// Sets number of times to loop and read a pin to decreases the number of false positives. 
int keyPressDebounce = 5; // I could not set this to high or it would always read false at some point.
int keyReleaseDebounce = 5;

/*****************************************************************
*                       to set a macro.                          *
*  - hold down the 'M' key.                                      *
*  - then hit r key.                                             *
*  - release both keys.                                          *
*  - then hit a number key to set the macro to.                  *
*  - then hit all the key you would like to recored when         *
*    done hit the 'M' to finish and save. or if you hit the      *
*    100 key limit it will auto save.                            *
*                                                                *
*                      to play a macro.                          *
*  - hold down the 'M' key.                                      *
*  - then hit the number key of the macro you would like to      *
*    play.                                                       *
*                                                                *
*****************************************************************/
/*****************************************************************
*                    Keyboard modifiers                          * 
* KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI,     *
* KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, KEY_RIGHT_ALT, KEY_RIGHT_GUI, *
* KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, *
* KEY_BACKSPACE, KEY_TAB, KEY_RETURN, KEY_ESC, KEY_INSERT,       *
* KEY_DELETE, KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END,     *
* KEY_CAPS_LOCK, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,         *
* KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12              *
*                                                                *
*****************************************************************/
// All the keys when function key is not pressed.
// 'F' for function key
// 'M' Macro key 
int keys[] = {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', KEY_BACKSPACE,
              KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
              KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 'J', KEY_RETURN,
              KEY_LEFT_SHIFT, 'A', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 'I', KEY_RIGHT_SHIFT,
              KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', ' ', ' ', ' ', ' ', ' ', ' ', KEY_RIGHT_ALT, 'F', 'M', KEY_LEFT_CTRL
             };

// Array of all the keys when function key has been pressed.
int funkeys[] = {KEY_F1, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_DELETE,
                 KEY_TAB, 'q', KEY_UP_ARROW, 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
                 KEY_CAPS_LOCK, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', ' ', KEY_RETURN,
                 KEY_LEFT_SHIFT, ' ', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ', KEY_RIGHT_SHIFT,
                 KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', ' ', ' ', ' ', ' ', ' ', ' ', KEY_RIGHT_ALT, 'F', 'M', KEY_LEFT_CTRL
                };
 
/////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Global vars - dont change //////////////////////////////////

bool pressedKeys[70];             // Keep track of what keys are pressed.
bool pressedFunMacroKey[70];
bool pressedMacroKey[70];
int pressedCount = 0;             // Keep track of how many keys are pressed.

// Array that points to the function keys
// (this should stary the same on both the keys[] and the funkeys marked with F char.
int functionKey[] = {67};
int numberOfFunKeys = 1; // Set to the size of the array does not like the sizeof.
int macroKey[] = {68};
int numberOfMacKeys = 1; // Set to the size of the array does not like the sizeof.

bool macroRecordMode = false;     // Set to true when in macro record mode is on.
int activeMacro = 11;             // hold the active macro being recorded.
int tempMacro[200];               // holds the temp macro being recorded.
int tempMacroSize = -1;            // holds the number of keys recorded.
bool activeMacroReless = false;   // set to true when active macro num key gets released.

#define LED_CAPSLOCK   (1 << 1)

/////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------- setup() ---------------------------------
/* Set all the pins for input andout put. */
void setup() {
  //set all the columns to outpur and set them low to start.
  for (int i = 0; i < 15; i++) {
    pinMode(Column[i], OUTPUT);
    digitalWrite(Column[i], LOW);
  }
  //set all the rows to input.
  for (int i = 0; i < 6; i++) {
    pinMode(Row[i], INPUT);
    digitalWrite(Row[i], LOW);
  }
  //set all the pressedKeys to false to start.
  clearAllPressed();
  if (useCapsLed) {
    pinMode(capsLedPin, OUTPUT);
  }
}

//-------------------------------- loop() -------------------------------------
/* loop to check for any keys pressed or released */
void loop() {
  for (int c = 0; c < 14; c++) {
    // turn the current column output on.
    digitalWrite(Column[c], HIGH);
    //delay(10);
    for (int r = 0; r < 5 ; r++) {
      //read each row for a key press.
      if (digitalRead(Row[r]) == HIGH && debouncePress(r)) {
        checkPressKey(r, c);
      } else {
        checkReleaseKey(r, c);
      }
    }
    digitalWrite(Column[c], LOW);
  }
  capsCheck(); // check and turn on led for capslock.
}

//----------------------- checkPressKey(int, int) -----------------------------
/* presses the key for the given row and column */
void checkPressKey(int r, int c) {
  // check to make sure we have not pessed to many keys and key is not pessed yet.
  if (pressedCount < maxKeysPressed && pressedKeys[r * 14 + c] != true) { 
    if (keys[r * 14 + c] == 'F') { // if the key being pressed is a function key.
      Keyboard.releaseAll();
      clearAllPressed();
      pressedCount = 0;
      addKey(r, c);
    } else if (keys[r * 14 + c] == 'M') {  // if the key being pressed is the macro key.
      Keyboard.releaseAll();
      clearAllPressed();
      pressedCount = 0;
      addKey(r, c);
      if (macroRecordMode) {
        endRecording();
      }
      macroRecordMode = false;
    } else {
      if (checkFunctionKeyPressed()) {
        if (!macroRecordMode && checkMacroKeyPressed() && isNumKey(funkeys[r * 14 + c])) { 
          playMacro(keys[r * 14 + c]);
        } else if (!macroRecordMode && checkMacroKeyPressed() && funkeys[r * 14 + c] == 'r') {
          macroRecordMode = true;
        } else {
          Keyboard.press(funkeys[r * 14 + c]);
          if (macroRecordMode) {
            recoredKeystroke(r * 14 + c, true);
          }
        }
      } else {
        if (!macroRecordMode && checkMacroKeyPressed() && isNumKey(keys[r * 14 + c])) { 
          playMacro(keys[r * 14 + c]);
        } else if (!macroRecordMode && checkMacroKeyPressed() && keys[r * 14 + c] == 'r') {
          macroRecordMode = true;
        } else {
          Keyboard.press(keys[r * 14 + c]);
          if (macroRecordMode) {
            recoredKeystroke(r * 14 + c, false);
          }
        }
      }
      addKey(r, c);
    }
  }
}

//-------------------- checkReleaseKey(int, int) --------------------------------
/* Releases the key for the given row and column */
void checkReleaseKey(int r, int c) {
  if (pressedKeys[r * 14 + c] == true && debounceRelease(r)) {
    if (keys[r * 14 + c] == 'F') { // if the key being released is a function key
      Keyboard.releaseAll();
      clearAllPressed();
      pressedCount = 0;
      removeKey(r, c);
    } else if (keys[r * 14 + c] == 'M') {
      Keyboard.releaseAll();
      clearAllPressed();
      pressedCount = 0;
      removeKey(r, c);
    } else {
      if (checkFunctionKeyPressed()) { // function key is pressed so release from funkeys.
        Keyboard.release(funkeys[r * 14 + c]);
        if (macroRecordMode && tempMacroSize >= 0) {
          if (!activeMacroReless && activeMacro == funkeys[r * 14 + c]) { // ignore first release
            activeMacroReless = false;
          } else {
            recoredKeystroke(r * 14 + c, true);
          }
        }
      } else { // relsease from keys becouse function key is NOT pressed.
        Keyboard.release(keys[r * 14 + c]);
        if (macroRecordMode && tempMacroSize >= 0) {
          if (!activeMacroReless && activeMacro == keys[r * 14 + c]) { // ignore first release
            activeMacroReless = false;
          } else {
            recoredKeystroke(r * 14 + c, false);
          }
        }
      }
      removeKey(r, c);
    }
  }
}

//----------------------------- capsCheck() -----------------------------------
/* turn the caplocks led on or off if useCapsLed is true. */
void capsCheck() {
  if (useCapsLed) {
    if (keyboard_leds & LED_CAPSLOCK) {
      digitalWrite(capsLedPin, HIGH);
    } else {
      digitalWrite(capsLedPin, LOW);
    }
  }
}

//------------------------- debouncePress(int) --------------------------------
/*  return true if for all the loops it reads high.
 *  The numbers of loops is determined by
 *  keyPressDebounce.
 */
bool debouncePress(int r) {
  for(int i = 0; i < keyPressDebounce; i++) {
    delay(1);
    if (digitalRead(Row[r]) != HIGH) {
      return false;
    }
  }
  return true;
}

//------------------------ debounceRelease(int) -------------------------------
/*  return true if for all the loops it reads low.
 *  The numbers of loops is determined by
 *  keyReleaseDebounce.
 */
bool debounceRelease(int r) {
  for(int i = 0; i < keyReleaseDebounce; i++) {
    delay(1);
    if (digitalRead(Row[r]) != LOW) {
      return false;
    }
  }
  return true;
}

//-------------------------- addKey(int, int) --------------------------------
/* set the givin key to true in the pressed array. pressedKeys[] */
void addKey(int r, int c) {
  pressedKeys[r * 14 + c] = true;
  pressedCount++;
}

//--------------------------- removeKey(int, int) ----------------------------
/*  set the givin key to false in the pressed array. pressedKeys[] */
void removeKey(int r, int c) {
  pressedKeys[r * 14 + c] = false;
  pressedCount--;
  if (pressedCount < 0) { // just ot make sure we dont go to -.
    pressedCount = 0;
  }
}

//------------------------ checkFunctionKeyPressed() --------------------------
/* resets funcKeyPressed then sets it to true if any function key is pressed. */
bool checkFunctionKeyPressed() {
  for (int i = 0; i < numberOfFunKeys; i++) {
    if (pressedKeys[functionKey[i]]) {
      return true;
    }
  }
  return false;
}

//-------------------------- checkMacroKeyPressed() ---------------------------
/* resets funcKeyPressed then sets it to true if any function key is pressed. */
bool checkMacroKeyPressed() {
  for (int i = 0; i < numberOfMacKeys; i++) {
    if (pressedKeys[macroKey[i]]) {
      return true;
    }
  }
  return false;
}

//--------------------------- clearAllPressed() -------------------------------
/* sets all the set all the pressedKeys to false. */
void clearAllPressed() {
  for (int i = 0; i < 70; i++) {
    pressedKeys[i] = false;
  }
}

//-------------------------- clearAllPressedMacro() ---------------------------
/* clear all the macropressed keys */
void clearAllPressedMacro() {
  for (int i = 0; i < 70; i++) {
    pressedFunMacroKey[i] = false;
    pressedMacroKey[i] = false;
  }
}

//-----------------------------isNumKey(int) ----------------------------------
/* returns true if char passed in is a number from 0-9. a number key. */
bool isNumKey(int key) {
  return (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9');
}

//-------------------------- charNumToInt(int) --------------------------------
/* returns and int of the char */
int charNumToInt(int key) {
  if (key == '0') {
    return 0;
  }
  if (key == '1') {
    return 1;
  }
  if (key == '2') {
    return 2;
  }
  if (key == '3') {
    return 3;
  }
  if (key == '4') {
    return 4;
  }
  if (key == '5') {
    return 5;
  }
  if (key == '6') {
    return 6;
  }
  if (key == '7') {
    return 7;
  }
  if (key == '8') {
    return 8;
  }
  if (key == '9') {
    return 9;
  }
  return 0;
}

//----------------------- recoredKeystroke(int, bool) -------------------------
/* recordes the keystrocks */
void recoredKeystroke(int key, bool isfunkey) {
  if (tempMacroSize < 0) {  // check to start the macro.
    int mykey = keys[key];
    if (isfunkey) {
     mykey = funkeys[key];
    }
    if (isNumKey(mykey)) {  // set active macro and start recordering.
      setActiveMacro(mykey);
      tempMacroSize = 0;
    } else {                // need to hit a num key befor recorde.
      macroRecordMode = false;
    }
  } else {
    if (isfunkey) {
      key += 100;
    }
    tempMacro[tempMacroSize] = key;
    tempMacroSize++;
    if (tempMacroSize == 200) {
      endRecording();
    }
  }
}

//------------------------- setActiveMacro(int) -------------------------------
/* set the target active macro 
  takes a int in as the macro number.
*/
void setActiveMacro(int mac) {
  activeMacro = mac;
}

//--------------------------- playMacro(int) ----------------------------------
/* play the macro saved for the num key passed in. */
void playMacro(int macroNum) {
  int macN = charNumToInt(macroNum);
  int starAdress = macN + (macN * 200);
  int lengthOfMacro = EEPROM.read(starAdress);
  if (lengthOfMacro <= 200) {
    for (int i = 0; i < lengthOfMacro; i++) { // for each key saved go through press and release it.
      int tempkey = EEPROM.read(i + 1 + starAdress);
      bool isfkey = false;
      if (tempkey > 100) {
        tempkey -= 100;
        isfkey = true;
      }
      if (tempkey >= 0 && tempkey < 70) { // make sure we are a key on the keyboard.
        if (isfkey) {
          if (pressedFunMacroKey[tempkey]) { 
            Keyboard.release(funkeys[tempkey]);
            pressedFunMacroKey[tempkey] = false;
          } else {
            Keyboard.press(funkeys[tempkey]);
            pressedFunMacroKey[tempkey] = true;
          }
        } else {
          if (pressedMacroKey[tempkey]) { 
            Keyboard.release(keys[tempkey]);
            pressedMacroKey[tempkey] = false;
          } else {
            Keyboard.press(keys[tempkey]);
            pressedMacroKey[tempkey] = true;
          }
        }
        delay(10);
      }
    }
  }
  Keyboard.releaseAll();  // just incase a key is still pressed.
  clearAllPressedMacro();
}

//--------------------------- endRecording() ----------------------------------
/* called at the end of recording a macro. */
void endRecording() {
  int actMac = charNumToInt(activeMacro);
  int starAdress = actMac + (actMac * 200);
  EEPROM.write(starAdress, tempMacroSize);
  for (int i = 0; i < tempMacroSize; i++) {
    EEPROM.write(i+1+starAdress, tempMacro[i]);
  }
  tempMacroSize = -1; //reset the size.
  activeMacroReless = false;
}

