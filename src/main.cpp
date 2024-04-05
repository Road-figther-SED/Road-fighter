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
unsigned long delaytime = 2000;

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
  pointerRegMatrix[7] = B00001001;
  pointerRegMatrix[6] = B00001011;
  pointerRegMatrix[5] = B00001101;
  pointerRegMatrix[4] = B11101001;
  pointerRegMatrix[3] = B10000000;
  pointerRegMatrix[2] = B11100000;
  pointerRegMatrix[1] = B00100000;
  pointerRegMatrix[0] = B11100000;
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
  pointerRegMatrix[7] = B00001001;
  pointerRegMatrix[6] = B00001011;
  pointerRegMatrix[5] = B00001101;
  pointerRegMatrix[4] = B11101001;
  pointerRegMatrix[3] = B10000000;
  pointerRegMatrix[2] = B11100000;
  pointerRegMatrix[1] = B10000000;
  pointerRegMatrix[0] = B11100000;
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
  RegBACKGTYPE_dataRANDOM = random(1, 255);

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

int DELAY = delaytime;
//=======================================================
//  FUNCTION: read_KEY
//=======================================================
byte read_KEY(void) {
  // Lectura de botones físicos
  bool leftButtonPressed = readLeftButton();
  bool rightButtonPressed = readRightButton();

  // Restablece keys a NO_KEY para evitar acciones continuas
  keys = NO_KEY;

  // Detección de flanco descendente para botón izquierdo
  if (leftButtonPressed == LOW && lastStateLeft == HIGH) {
    keys = LEFT_KEY; // Ajusta según la lógica correcta de tu botón
  }

  // Detección de flanco descendente para botón derecho
  if (rightButtonPressed == LOW && lastStateRight == HIGH) {
    keys = RIGHT_KEY; // Ajusta según la lógica correcta de tu botón
  }

  // Actualización de los estados anteriores al final
  lastStateLeft = leftButtonPressed;
  lastStateRight = rightButtonPressed;

  // Manejo de entrada serial se mantiene igual
  if (keys == NO_KEY && Serial.available() > 0) { // Si no se ha detectado pulsación de botón físico
    incomingByte = Serial.read();
    delay(10); // Pequeña pausa para estabilizar la lectura

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
      case 'P':
        keys = PAUSE_KEY;
        break;
      default:
        keys = NO_KEY;
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
      delaytime=2000;
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
      delay(delaytime);
      checkLostMatrix(pointerRegMatrix, pointerRegCar);
      count++;
      Serial.println(count);
      if (Status == LOST)
        state = STATELOST;
      else if (count == 18 /*10+8*/ | count == 36 /*20+16*/ | count == 54 /*30+24*/)
        state = STATELEVELPASS;
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
      if (delaytime == 2000) //* el 2000 toca cambiarlo si cambia en la línea 58
        {
        delaytime=1500; //* delay para N2
        writePassLevel2(pointerRegMatrix,pointerRegCar);
        state=STATECLEAR;
        //delay(delaytime/2);
        }
      else if (delaytime == 1500) //* el 1500 toca cambiarlo si cambia en la anterior linea
        {
        delaytime=1000; //* delay para N3
        writePassLevel3(pointerRegMatrix,pointerRegCar);
        state=STATECLEAR;
        //delay(delaytime/2);
        }
      else if (delaytime == 1000) //* el 1000 toca cambiarlo si cambia en la anterior linea
      {
        writeGameWon(pointerRegMatrix,pointerRegCar);
        state=STATEPAUSE;
        break;
      }
      else
        delaytime=delaytime; //* se mantiene delay actual
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
  read_KEY();

  // Ejecuta la lógica principal de movimiento y estado del juego.
  state_machine_run_cars(pointerRegMatrix,pointerRegCar,pointerShiftDir);

  // Ahora read_KEY() ya maneja la lógica de flanco ascendente, por lo que solo necesitas llamar a
  // state_machine_move_car() una vez por ciclo de loop, no dentro de un while.
  // Esto asegura que cada pulsación de botón se maneje de forma individual.
  //state_machine_move_car(pointerRegMatrix,pointerRegCar,pointerShiftDir);
  while (keys != NO_KEY){
  state_machine_move_car(pointerRegMatrix,pointerRegCar,pointerShiftDir);
  read_KEY();
  }
  // Un breve delay para estabilizar la lectura de los botones y no sobrecargar el loop.
  delay(1);
}
