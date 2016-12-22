// array for the pins for all the comlumns.
int Column[] = {12, 11, 10, 9, 8, 7, 6, 5, 2, 23, 14, 15, 16, 4 };
// array for the pins for all the rows.
int Row[] = {18, 19, 20, 21, 22};

bool functionPressed = false;     // set to true if function key is pressed.
bool pressedKeys[70];             // keep track of what keys are pressed.
int pressedCount = 0;             // keep track of how many keys are pressed.

// all the key when function key is not pressed.
int keys[] = {'`', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', KEY_BACKSPACE,
              KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
              KEY_CAPS_LOCK, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', 'J', KEY_RETURN,
              KEY_LEFT_SHIFT, 'A', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 'I', KEY_RIGHT_SHIFT,
              KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, 'B', 'C', 'D', ' ', 'E', 'G', 'H', KEY_RIGHT_ALT, 'F', KEY_RIGHT_GUI, KEY_LEFT_CTRL
             };

// array of all the keys when function key has been pressed.
int funkeys[] = {KEY_F1, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_DELETE,
                 KEY_TAB, 'q', KEY_UP_ARROW, 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\\',
                 KEY_CAPS_LOCK, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_RIGHT_ARROW, 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', ' ', KEY_RETURN,
                 KEY_LEFT_SHIFT, ' ', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ', KEY_RIGHT_SHIFT,
                 KEY_LEFT_CTRL, KEY_LEFT_GUI, KEY_LEFT_ALT, ' ', ' ', ' ', ' ', ' ', ' ', ' ', KEY_RIGHT_ALT, 'F', KEY_RIGHT_GUI, KEY_LEFT_CTRL
                };

//array that points to the function keys
//(this should stary the same on both the keys[] and the funkeys marked with F char.
int functionKey[] = {67};
int numverOfFunKeys = 1; // set to the size of the array does not like the sizeof.
bool funcKeyPressed = false; // set to true buy function checkFunctionKeyPressed if the F key is pressed.

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
}

/* loop to check for any keys pressed or released */
void loop() {
  for (int c = 0; c < 14; c++) {
    // turn the current column output on.
    digitalWrite(Column[c], HIGH);
    //delay(10);
    for (int r = 0; r < 5 ; r++) {
      //read each row for a key press.
      if (digitalRead(Row[r]) == HIGH) {
        checkPressKey(r, c);
      } else {
        checkReleaseKey(r, c);
      }
    }
    digitalWrite(Column[c], LOW);
  }
}

/* presses the key for the given row and column */
void checkPressKey(int r, int c) {
  if (pressedCount < 6 && pressedKeys[r * 14 + c] != true) {
      if (keys[r * 14 + c] == 'F') { // if the key being pressed is a function key
        Keyboard.releaseAll();
        clearAllPressed();
        pressedCount = 0;
      }
      checkFunctionKeyPressed();
      if (funcKeyPressed) {
        Keyboard.press(funkeys[r * 14 + c]);
      } else {
        Keyboard.press(keys[r * 14 + c]);
      }
      addKey(r, c);
  }
}

/* Releases the key for the given row and column */
void checkReleaseKey(int r, int c) {
  if (pressedKeys[r * 14 + c] == true) {
    checkFunctionKeyPressed();
    if (funcKeyPressed) {
      Keyboard.release(funkeys[r * 14 + c]);
    } else {
      Keyboard.release(keys[r * 14 + c]);
    }
    removeKey(r, c);
    if (keys[r * 14 + c] == 'F') { // if the key being released is a function key
      Keyboard.releaseAll();
      clearAllPressed();
      pressedCount = 0;
    }
  }
}

/* set the givin key to true in the pressed array. pressedKeys[] */
void addKey(int r, int c) {
  pressedKeys[r * 14 + c] = true;
  pressedCount++;
}

/*  set the givin key to false in the pressed array. pressedKeys[] */
void removeKey(int r, int c) {
  pressedKeys[r * 14 + c] = false;
  pressedCount--;
  if (pressedCount < 0) { // just ot make sure we dont go to -.
    pressedCount = 0;
  }
}

/* resets funcKeyPressed then sets it to true if any function key is pressed. */
void checkFunctionKeyPressed() {
  funcKeyPressed = false;
  for (int i = 0; i < numverOfFunKeys; i++) {
    if (pressedKeys[functionKey[i]]) {
      funcKeyPressed = true;
    }
  }
}

/* sets all the set all the pressedKeys to false. */
void clearAllPressed() {
  for (int i = 0; i < 70; i++) {
    pressedKeys[i] = false;
  }
}

