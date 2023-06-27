/**
   Proyecto Final Arquitectura

   Copyright (C) 2023, Juanes Yepez.
   Released under the MIT License.
*/

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>

/* Display */
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {5, 4, 3, 2};
byte colPins[KEYPAD_COLS] = {A3, A2, A1, A0};
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'.', '0', '=', '/'}
};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);
/* Password variables */
const String correctPasscode = "1234"; // Clave correcta
const int maxAttempts = 3; // Número máximo de intentos
bool passwordCorrect = false; // true si la contraseña es correcta
static int attempts = 0;//intentos
const unsigned long maxInputTime = 10000; // Tiempo máximo de espera para ingresar la contraseña (en milisegundos)
/* Variables de cambio de estado */
unsigned long state4StartTime = 0; // Almacena el tiempo en milisegundos cuando se entra al State_4
const unsigned long state4Duration = 20000; // Duración en milisegundos para permanecer en el State_4

/**************************/

uint64_t value = 0;
/**
*@brief lanza mensaje de bienvenida 
*@return imprime en lcd mensaje de bienvenida
*/
void showSplashScreen() {
  String message = "Bienvenido";
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(70);
  }
  delay(500);
}

/* Definición de los estados posibles */
enum Estado {
  InicitialState,
  State_2,
  State_3,
  State_4,
  State_5
};
// Variable que guarda el estado actual
Estado estadoActual;

void updateCursor() {
  if (millis() / 250 % 2 == 0 ) {
    lcd.cursor();
  } else {
    lcd.noCursor();
  }
}

void setup() {
  Serial.begin(9600);
  //bienvenida
  showSplashScreen();
  //ingreso de clave
  lcd.begin(16, 2);
  lcd.print("Ingrese la clave");
  //ingresar al estado inicial
  estadoActual = InicitialState;

  lcd.setCursor(0, 1);
}

void loop() {
  updateCursor();
  /* Alternar entre estados */
  switch (estadoActual) {
    case InicitialState:
      //estado inicial, validar contraseña contraseña 
      handleKeypadInput();
      if (passwordCorrect == true){
        estadoActual = State_2;
      }
      if(attempts > maxAttempts){
        estadoActual = State_4;
      }

    break;
    case State_2:
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0); 
      lcd.print("estado 2");
    break;
    case State_3: 
    break;
    case State_4:
      // Acciones cuando la máquina está bloqueada
      if (state4StartTime == 0) {
      // Guarda el tiempo actual al entrar al State_4
      state4StartTime = millis();
      lcd.clear();
      lcd.print("Sistema bloqueado");
      } else {
        // Comprueba si ha transcurrido el tiempo necesario para la transición al State_5
        if (millis() - state4StartTime >= state4Duration) {
          estadoActual = State_5;
          state4StartTime = 0; // Reinicia el tiempo para el próximo ciclo en el State_4
          attempts = 0; // Reinicia el contador de intentos
        }
      }
      //@Todo agregar funcionalidades
      break;
    case State_5:
      lcd.clear();
      lcd.print("estado 5");
      delay(2000);
  break;

    break;
  }
}

/**
*@brief coloca * por cada digito ingresado
*@return envia caracteres a validatePasscode para su correcta validacion
**/
void handleKeypadInput() {
  char key = keypad.getKey();
  if (key) {
    if (key != '=') {
      lcd.print("*");
      validatePasscode(key);
    }
  }
}

/**
*@brief metodo para validar la contraseña ingresada por el usuario
*@param digito ingresado en keypad
*@return verdadero si la clave es verdadera
*/
void validatePasscode(char key) {
  static String passcodeInput;
  passcodeInput += key;

  if (passcodeInput.length() == correctPasscode.length()) {
    if (passcodeInput == correctPasscode) {
      lcd.clear();
      lcd.print("Clave Correcta");
      attempts = 1; // Reiniciar los intentos después de una clave correcta
      passwordCorrect = true;
    } else {
      if (attempts < maxAttempts) {
        lcd.clear();
        lcd.print("Clave Incorrecta");
        delay(2000);
        lcd.clear();
        lcd.print("Ingrese clave:");
        lcd.setCursor(0, 1);
        passcodeInput = ""; // Reiniciar la entrada de la contraseña incorrecta
        attempts++;
      }
      passwordCorrect = false;
    }
  }
}

