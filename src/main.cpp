/*######################################################################
  //#  G0B1T: uC EXAMPLES. 2024.
  //######################################################################
  //# Copyright (C) 2024. F.E.Segura-Quijano (FES) fsegura@uniandes.edu.co
  //#
  //# This program is free software: you can redistribute it and/or modify
  //# it under the terms of the GNU General Public License as published by
  //# the Free Software Foundation, version 3 of the License.
  //#
  //# This program is distributed in the hope that it will be useful,
  //# but WITHOUT ANY WARRANTY; without even the implied warranty of
  //# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  //# GNU General Public License for more details.
  //#
  //# You should have received a copy of the GNU General Public License
  //# along with this program.  If not, see <http://www.gnu.org/licenses/>
  //####################################################################*/

//=======================================================
//  LIBRARY Definition
//=======================================================
#include <Arduino.h>
#define BUTTON_LEFT 22
#define BUTTON_RIGHT 19
#define REALMATRIX // Variable to use real matrix. Comment to not use it.


#ifdef REALMATRIX
#include "LedControl.h"
/* Pin definition for MAX72XX.
 ARDUINO pin 12 is connected to the DataIn  - In ESP32 pin 23
 ARDUINO pin 11 is connected to the CLK     - In ESP32 pin 18
 ARDUINO pin 10 is connected to LOAD        - In ES32 pin 5
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(23,18,5,1);
#endif
bool readLeftButton();
bool readRightButton();
bool lastStateLeft = true; // Los botones están en pull-up, así que el estado "no presionado" es HIGH
bool lastStateRight = true;

//=======================================================
//  IF PROBLEMS
//=======================================================
// In LedControl.h  change the following line:
// #include <avr/pgmspace.h>
// by 
/* #if (defined(__AVR__))
 #include <avr\pgmspace.h>
 #else
 #include <pgmspace.h>
 #endif
*/

//=======================================================
//  REGISTERS (VARIABLES) to control the game
//=======================================================
/* Registers to background cars.*/
byte RegBACKGTYPE_dataRANDOM;
byte RegBACKGTYPE_dataZEROS = B00000000;

/* Time delays. */
unsigned long delaytime = 1000;
unsigned long previousMillis = 0;
unsigned long interval = 1000;

/* Global Variables */
int i = 0;
int count = 0; 

/* States ans signals to change state*/
enum State_enum {STATERESET, STATESTART, STATECLEAR, STATECHECK, STATELEFT, STATERIGHT, STATELOST, STATENEXTLEVEL, STATELEVELPASS, STATEPAUSE};
uint8_t state = STATERESET;

enum State_enumC {STATEMOVE, STATENOMOVE};
uint8_t stateC = STATENOMOVE;

enum Keys_enum {RESET_KEY, START_KEY, LEFT_KEY, RIGHT_KEY, NO_KEY, PAUSE_KEY};
uint8_t keys = RESET_KEY;

enum Status_enum {LOST, CONTINUE};
uint8_t Status = CONTINUE;

/* Key to control game by serial input. */
  int incomingByte;

/* Pointer and Matrix to Control Driver. */
  byte RegMatrix[8];
  byte *pointerRegMatrix;

/* Pointer and Register to control bottom car. */
  byte RegCar[1];
  byte *pointerRegCar;

/* Pointer and Register (Variable) to move bottom car */
  byte ShiftDir[1];
  byte *pointerShiftDir;

//=======================================================
//  SETUP Arduino function
//=======================================================
/* Setup function initialization */
void setup()
{
#ifdef REALMATRIX
  /* The MAX72XX is in power-saving mode on startup, we have to do a wakeup call. */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values. */
  lc.setIntensity(0, 1); //? NORMALMENTE ES 8 LA INTENSIDAD
  /* Clear the display. */
  lc.clearDisplay(0);
#endif

  /* Serial port initialization. */
  Serial.begin(9600);

  /* Pointer to use Matrix between functions. */
  pointerRegMatrix = &RegMatrix[0];

  /* Pointer to use VectorCar between functions. */
  pointerRegCar = &RegCar[0];

  /* Pointer to use shift dir between functions */
  pointerShiftDir = &ShiftDir[0];
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
}
bool readLeftButton() {
  return digitalRead(BUTTON_LEFT) == LOW;
}
bool readRightButton() {
  return digitalRead(BUTTON_RIGHT) == LOW;
}

//=======================================================
//  FUNCTION: writeResetMatrix
//=======================================================
/* Data matrix when reset*/
void writeResetMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to reset matrix */
  pointerRegMatrix[7] = B11111111;
  pointerRegMatrix[6] = B11111111;
  pointerRegMatrix[5] = B11111111;
  pointerRegMatrix[4] = B11111111;
  pointerRegMatrix[3] = B11111111;
  pointerRegMatrix[2] = B11111111;
  pointerRegMatrix[1] = B11111111;
  pointerRegMatrix[0] = B11111111;
  /* Here is the data to reset bottomCar */
  pointerRegCar[0] = B00000000;
  i=0;
}
//=======================================================
//  FUNCTION: writeStartMatrix
//=======================================================
void writeStartMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to start matrix */
  pointerRegMatrix[7] = B01111110;
  pointerRegMatrix[6] = B10000001;
  pointerRegMatrix[5] = B10011001;
  pointerRegMatrix[4] = B10000001;
  pointerRegMatrix[3] = B10100101;
  pointerRegMatrix[2] = B10011001;
  pointerRegMatrix[1] = B10000001;
  pointerRegMatrix[0] = B01111110;
/* Here is the data to start bottomCar */
  pointerRegCar[0] = B00000000;
}
//=======================================================
//  FUNCTION: writeClearMatrix
//=======================================================
void writeClearMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to clear matrix */
  pointerRegMatrix[7] = B00000000;
  pointerRegMatrix[6] = B00000000;
  pointerRegMatrix[5] = B00000000;
  pointerRegMatrix[4] = B00000000;
  pointerRegMatrix[3] = B00000000;
  pointerRegMatrix[2] = B00000000;
  pointerRegMatrix[1] = B00000000;
  pointerRegMatrix[0] = B00000000;
  /* Here is the data to clear bottomCar */
  pointerRegCar[0] = B00010000;
}
//=======================================================
//  FUNCTION: writeLostMatrix
//=======================================================
void writeLostMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to lost matrix */
  pointerRegMatrix[7] = B01111110;
  pointerRegMatrix[6] = B10000001;
  pointerRegMatrix[5] = B10011001;
  pointerRegMatrix[4] = B10000001;
  pointerRegMatrix[3] = B10011001;
  pointerRegMatrix[2] = B10100101;
  pointerRegMatrix[1] = B10000001;
  pointerRegMatrix[0] = B01111110;
  /* Here is the data to lost matrix */
  //pointerRegCar[0] = B00000000;
}
//=======================================================
//  FUNCTION: writePassLevel2
//=======================================================
void writePassLevel2(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to start matrix */
  pointerRegMatrix[7] = B00000000;
  pointerRegMatrix[6] = B11101001;
  pointerRegMatrix[5] = B10001001;
  pointerRegMatrix[4] = B11101011;
  pointerRegMatrix[3] = B00101101;
  pointerRegMatrix[2] = B11101001;
  pointerRegMatrix[1] = B00000000;
  pointerRegMatrix[0] = B00000000;
/* Here is the data to start bottomCar */
  pointerRegCar[0] = B00010000;
}
//=======================================================
//  FUNCTION: writePassLevel3
//=======================================================
void writePassLevel3(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to start matrix */
  pointerRegMatrix[7] = B00000000;
  pointerRegMatrix[6] = B11101001;
  pointerRegMatrix[5] = B10001001;
  pointerRegMatrix[4] = B11101011;
  pointerRegMatrix[3] = B10001101;
  pointerRegMatrix[2] = B11101001;
  pointerRegMatrix[1] = B00000000;
  pointerRegMatrix[0] = B00000000;
/* Here is the data to start bottomCar */
  pointerRegCar[0] = B00010000;
}
//=======================================================
//  FUNCTION: writeGameWon
//=======================================================
void writeGameWon(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */

  /* Here is the data to start matrix */
  pointerRegMatrix[7] = B00000110;
  pointerRegMatrix[6] = B00000001;
  pointerRegMatrix[5] = B00001101;
  pointerRegMatrix[4] = B01101001;
  pointerRegMatrix[3] = B00010110;
  pointerRegMatrix[2] = B11010000;
  pointerRegMatrix[1] = B10010000;
  pointerRegMatrix[0] = B01100000;
/* Here is the data to start bottomCar */
  pointerRegCar[0] = B00000000;
}
//=======================================================
//  FUNCTION: writeGoCarsMatrix
//=======================================================
void writeGoCarsMatrix(byte *pointerRegMatrix)
{
  /* Global variables. */
  int m;

  i = i + 1;
  /* Here is the data to start matrix */
  unsigned int numeros[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 28, 32, 33,
   34, 35, 36, 37, 38, 40, 41, 42, 44, 48, 49, 50, 52, 56, 64, 65, 66, 67, 68, 69, 70, 72, 73, 74, 76, 80, 81, 82, 84, 88,
   96, 97, 98, 100, 104, 112, 128, 129, 130, 131, 132, 133, 134, 136, 137, 138, 140, 144, 145, 146, 148, 152, 160, 161, 162,
   164, 168, 176, 192, 193, 194, 196, 200, 208, 224};

  unsigned int numeros2[] = {15, 23, 27, 29, 30, 31, 39, 43, 45, 46, 47, 51, 53, 54, 55, 57, 58, 59, 60, 61, 62,
  71, 75, 77, 78, 79, 83, 85, 86, 87, 89, 90, 91, 92, 93, 94, 99, 101, 102, 103,
  105, 106, 107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 118, 120, 121, 122,
  124, 135, 139, 141, 142, 143, 147, 149, 150, 151, 153, 154, 155, 156, 157, 158,
  163, 165, 166, 167, 169, 170, 171, 172, 173, 174, 177, 178, 179, 180, 181, 182,
  184, 185, 186, 188, 195, 197, 198, 199, 201, 202, 203, 204, 205, 206, 209, 210,
  211, 212, 213, 214, 216, 217, 218, 220, 225, 226, 227, 228, 229, 230, 232, 233,
  234, 236, 240, 241, 242, 244, 248};

  unsigned int numeros_combined[] = {
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
  20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37,
  38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61, 62, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
  75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92,
  93, 94, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 112,
  113, 114, 115, 116, 117, 118, 120, 121, 122, 124, 128, 129, 130, 131,
  132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145,
  146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 160,
  161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
  176, 177, 178, 179, 180, 181, 182, 184, 185, 186, 188, 192, 193, 194,
  195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 208, 209,
  210, 211, 212, 213, 214, 216, 217, 218, 220, 224, 225, 226, 227, 228,
  229, 230, 232, 233, 234, 236, 240, 241, 242, 244, 248};

  unsigned int numeros3[] = {63, 95, 111, 119, 123, 125, 126, 127, 159, 175, 183, 187, 189, 190, 191,
  207, 215, 219, 221, 222, 223, 231, 235, 237, 238, 239, 243, 245, 246, 247, 249, 250, 251,
  252, 253, 254, 255};

  size_t size = sizeof(numeros) / sizeof(numeros[0]);
  size_t size2 = sizeof(numeros_combined) / sizeof(numeros_combined[0]);
  
  if (delaytime == 1000){
    RegBACKGTYPE_dataRANDOM = numeros[random(0,size)];
    Serial.println(size);
  } else if (delaytime == 666){
    RegBACKGTYPE_dataRANDOM = numeros_combined[random(0,size2)];
    Serial.println(size2);
  } else {
    RegBACKGTYPE_dataRANDOM = random(1, 255);
  }

  for (m = 0; m < 7; m++)
  {
    pointerRegMatrix[m] = pointerRegMatrix[m + 1];
  }
  if (i % 2 == 0)
    pointerRegMatrix[7] = RegBACKGTYPE_dataRANDOM;
  else
    pointerRegMatrix[7] = RegBACKGTYPE_dataZEROS;
}
//=======================================================
//  FUNCTION: writeCarBase
//=======================================================
void writeCarBase(byte *pointerRegCar, byte *pointerShiftDir)
{
  /* Global variables. */
  int m;
  
  /* Here is the data to start matrix */
  if (pointerShiftDir[0] == B00000010)
  {
    if (pointerRegCar[0] == B00000001)
      pointerRegCar[0] = pointerRegCar[0];
    else
      pointerRegCar[0] = pointerRegCar[0] >> 1;
  }
  else if (pointerShiftDir[0] == B00000001)
  {
    if (pointerRegCar[0] == B10000000)
      pointerRegCar[0] = pointerRegCar[0];
    else
      pointerRegCar[0] = pointerRegCar[0] << 1;
  }
  else
    pointerRegCar[0] = pointerRegCar[0];
}
//=======================================================
//  FUNCTION: checkLostMatrix (leds and console)
//=======================================================
void checkLostMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */
  byte check1, check2;

  check1 = pointerRegCar[0] ^ pointerRegMatrix[0];
  check2 = pointerRegCar[0] | pointerRegMatrix[0];

  if (pointerRegCar[0] == pointerRegMatrix[0])
    Status = LOST;
  else if (check1 != check2)
    Status = LOST;
  else
    Status = CONTINUE;
}
//=======================================================
//  FUNCTION: printBits (by console all bits)
//=======================================================
void printBits(byte myByte)
{
  for (byte mask = 0x80; mask; mask >>= 1) {
    if (mask  & myByte)
      Serial.print('1');
    else
      Serial.print('0');
  }
}
//=======================================================
//  FUNCTION: PrintMatrix (Console)
//=======================================================
void PrintMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */
  int m;

  for (m = 7; m >= 1; m--)
  {
    printBits(pointerRegMatrix[m]);
    Serial.println();
  }
  printBits(pointerRegMatrix[0] | pointerRegCar[0]);
  Serial.println();
}
//=======================================================
//  FUNCTION: PrintALLMatrix (leds and console)
//=======================================================
void PrintALLMatrix(byte *pointerRegMatrix, byte *pointerRegCar)
{
  /* Global variables. */
  int m;

#ifdef REALMATRIX
  /* Display data one by one in matrix. */
  for (m = 7; m >= 1; m--)
  {
    lc.setRow(0, m, pointerRegMatrix[m]);
  }
  lc.setRow(0, m, (pointerRegMatrix[0] | pointerRegCar[0]));
#endif
  /* Display data one by one in console. */
  Serial.println("########");
  Serial.println("########");
  Serial.println("########");
  Serial.println("########");
  PrintMatrix(pointerRegMatrix, pointerRegCar);
  Serial.println("########");
}
//=======================================================
//  FUNCTION: read_KEY
//=======================================================
byte read_KEY(void)
{
  // Lectura de botones físicos
  bool leftButtonPressed = readLeftButton();
  bool rightButtonPressed = readRightButton();

  // Primero, verificar los botones físicos antes de leer el Serial
  if (leftButtonPressed) {
    keys = LEFT_KEY;
  } else if (rightButtonPressed) {
    keys = RIGHT_KEY;
  } else {
    // Si no hay pulsación de botón físico, entonces leer el Serial
    if (Serial.available() > 0) {
      incomingByte = Serial.read();
      delay(10); // Pequeña pausa para estabilizar la lectura
    }

    switch (incomingByte) {
      case 'R':
        keys = RESET_KEY;
        break;
      case 'S':
        keys = START_KEY;
        break;
      case 'A':
        keys = LEFT_KEY;
        break;
      case 'D':
        keys = RIGHT_KEY;
        break;
      default:
        keys = NO_KEY;
        break;
      case 'P':
        keys = PAUSE_KEY; 
        break;
    }
    incomingByte = 'N'; // Restablecer la entrada para evitar repeticiones
  }
  return keys;
}


//=======================================================
//  FUNCTION: state_machine_run_cars
//=======================================================
void state_machine_run_cars(byte *pointerRegMatrix, byte *pointerRegCar, byte *pointerShiftDir)
{

  PrintALLMatrix(pointerRegMatrix, pointerRegCar);

  switch (state)
  {
    case STATERESET:
      writeResetMatrix(pointerRegMatrix,pointerRegCar);
      delay(delaytime);
      if (keys == RESET_KEY)
        state = STATERESET;
      else if (keys == START_KEY)
        state = STATESTART;
      break;

    case STATESTART:
      writeStartMatrix(pointerRegMatrix,pointerRegCar);
      delay(delaytime);
      state = STATECLEAR;
      count=0;
      delaytime=1000;
      interval=1000;
      break;

    case STATECLEAR:
      writeClearMatrix(pointerRegMatrix,pointerRegCar);
      delay(delaytime);
      state = STATECHECK;
      Serial.println("CLEAR");
      break;

    case STATECHECK:
      pointerShiftDir[0] = B00000000;
      writeCarBase(pointerRegCar, pointerShiftDir);
      writeGoCarsMatrix(pointerRegMatrix);
      //delay(delaytime);
      checkLostMatrix(pointerRegMatrix, pointerRegCar);
      count++;
      Serial.println(count);
      if (Status == LOST)
        state = STATELOST;
      else if (count == 29 /*20+9*/ | count == 68 /*50+18*/ | count == 117 /*90+27*/)
        {state = STATELEVELPASS;
        Serial.println(count);
        Serial.println("                 Paso nivel");}
      else if (keys == PAUSE_KEY) // Añade esta línea para verificar la tecla de pausa
        state  = STATEPAUSE;      // Cambia al estado de pausa
      else if (keys == RESET_KEY)
        state = STATERESET;
      else if (keys != NO_KEY)
        {stateC = STATEMOVE;
        Serial.println("INPUT READ");}
      else
        state = STATECHECK;
      break;

    case STATELOST:
      writeLostMatrix(pointerRegMatrix,pointerRegCar);
      delay(delaytime);
      if (keys == START_KEY)
        state = STATESTART;
      else
        state = STATELOST;
      break;
    
    case STATELEVELPASS:
      state = STATENEXTLEVEL;
      break;

    case STATENEXTLEVEL:
      if (delaytime == 1000) //* el 2000 toca cambiarlo si cambia en la línea 58
        {
        delaytime=666; //* delay para N2
        interval=666;
        writePassLevel2(pointerRegMatrix,pointerRegCar);
        state=STATECLEAR;
        //delay(delaytime/2);
        }
      else if (delaytime == 666) //* el 1500 toca cambiarlo si cambia en la anterior linea
        {
        delaytime=333; //* delay para N3
        interval=333;
        writePassLevel3(pointerRegMatrix,pointerRegCar);
        state=STATECLEAR;
        //delay(delaytime/2);
        }
      else if (delaytime == 333) //* el 1000 toca cambiarlo si cambia en la anterior linea
      {
        writeGameWon(pointerRegMatrix,pointerRegCar);
        state=STATEPAUSE;
        break;
      }
      else
        delaytime=delaytime; //* se mantiene delay actual
        interval = interval;
      state = STATECHECK;
      break;
    case STATEPAUSE:
      // Aquí, simplemente espera por otra presión de tecla para reanudar.
      if (keys == PAUSE_KEY || keys == START_KEY) { // Usar START_KEY o cualquier otra tecla para reanudar.
        state = STATECHECK; // O el estado desde el cual la pausa fue activada.
      }
      break;
    default:
      state = STATERESET;
      break;



    
  }
}
//=======================================================
//  FUNCTION: state_machine_move_car
//=======================================================
void state_machine_move_car(byte *pointerRegMatrix, byte *pointerRegCar, byte *pointerShiftDir) {
  PrintALLMatrix(pointerRegMatrix, pointerRegCar);

  switch (stateC) {
    case STATENOMOVE:
      // Si 'keys' indica una acción, cambia al estado STATEMOVE para procesar esa acción.
      if (keys != NO_KEY) {
        stateC = STATEMOVE;
      }
      break;
    case STATEMOVE:
      // Procesa el movimiento basado en la tecla presionada.
      if (keys == LEFT_KEY) {
        pointerShiftDir[0] = B00000001; // Corrige la dirección para que coincida con la expectativa.
        writeCarBase(pointerRegCar, pointerShiftDir);
      } else if (keys == RIGHT_KEY) {
        pointerShiftDir[0] = B00000010; // Corrige la dirección para que coincida con la expectativa.
        writeCarBase(pointerRegCar, pointerShiftDir);
      }
      // Vuelve al estado de no movimiento después de procesar la acción.
      stateC = STATENOMOVE;
      break;
    default:
      stateC = STATENOMOVE;
      break;
  }
  // Resetea 'keys' al final para asegurarse de que cada acción se procese una sola vez.
  //keys = NO_KEY;
}



//=======================================================
//  FUNCTION: Arduino loop
//=======================================================
void loop()
{
  // Llama a read_KEY() para actualizar el estado de los botones.
  unsigned long currentMillis = millis();
  read_KEY();
  // Ejecuta la lógica principal de movimiento y estado del juego.
  if (currentMillis - previousMillis >= interval){
    previousMillis=currentMillis;
    state_machine_run_cars(pointerRegMatrix,pointerRegCar,pointerShiftDir);
    //state_machine_move_car(pointerRegMatrix,pointerRegCar,pointerShiftDir);
    /*while (keys != NO_KEY){
      state_machine_move_car(pointerRegMatrix,pointerRegCar,pointerShiftDir);
      read_KEY();
      }*/
  }
  while (keys != NO_KEY){
      state_machine_move_car(pointerRegMatrix,pointerRegCar,pointerShiftDir);
      read_KEY();
      }
  // Ahora read_KEY() ya maneja la lógica de flanco ascendente, por lo que solo necesitas llamar a
  // state_machine_move_car() una vez por ciclo de loop, no dentro de un while.
  // Esto asegura que cada pulsación de botón se maneje de forma individual.
  //state_machine_move_car(pointerRegMatrix,pointerRegCar,pointerShiftDir);
  /*
  while (keys != NO_KEY){
  state_machine_move_car(pointerRegMatrix,pointerRegCar,pointerShiftDir);
  read_KEY();
  }*/
  // Un breve delay para estabilizar la lectura de los botones y no sobrecargar el loop.
  delay(0.1);
}
