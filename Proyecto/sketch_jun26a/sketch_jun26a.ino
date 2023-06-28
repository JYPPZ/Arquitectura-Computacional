/**
   Proyecto Final Arquitectura

   Copyright (C) 2023, Juanes Yepez, Manuel Paz, Daniel Solarte.
   Released under the MIT License.
*/

#include <LiquidCrystal.h>
#include <Keypad.h>
#include <Servo.h>
#include <DHT.h>
#define DHTPIN 7 // Pin al que está conectado el sensor DHT
#define DHTTYPE DHT11 // Tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE); // Objeto DHT

/* Display */
LiquidCrystal lcd(30, 31, 32, 33, 34, 35);
/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {41, 43, 45, 47};
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
int attempts = 1;//intentos
const unsigned long maxInputTime = 10000; // Tiempo máximo de espera para ingresar la contraseña (en milisegundos)
/* Variables de cambio de estado */
unsigned long state4StartTime = 0; // Almacena el tiempo en milisegundos cuando se entra al State_4
const unsigned long state4Duration = 20000; // Duración en milisegundos para permanecer en el State_4
unsigned long state2StartTime = 0; // Almacena el tiempo en milisegundos cuando se entra al State_2
const unsigned long state2Duration = 5000; // Duración en milisegundos para permanecer en el State_2
const unsigned long state2Duration2 = 20000; // Duración en milisegundos para permanecer en el State_2
/* Variables humedad temperatura y luz */
const int photocellPin = A7;
float humedad;
float temp;
int luz;
/* variablesa de buzzer */
const int buzzerPin = 6;// Definir el pin del buzzer
/**************************/

/**
*@brief lanza mensaje de bienvenida 
*@return imprime en lcd mensaje de bienvenida
*/
void showSplashScreen() {
  String message = "Bienvenido";
  for (byte i = 0; i < message.length(); i++) {
    lcd.print(message[i]);
    delay(50);
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
  lcd.setCursor(0, 1);
  //ingresar al estado inicial
  estadoActual = InicitialState;
  //Configurar el pin del buzzer como salida
  pinMode(buzzerPin, OUTPUT);
  //inicializar dht
  dht.begin();
  
}

void loop() {
  updateCursor();
  Serial.println("<--");
  Serial.println(estadoActual);
  /* Alternar entre estados */
  switch (estadoActual) {
    case InicitialState:
      estado1();
    break;
    case State_2:
      estado2();
    break;
    case State_3:
      estado3();//vacio, @Todo Borrar y cambiar nombre de estados
    break;
    case State_4:
      estado4();
    break;
    case State_5:
      estado5();
    break;
    default: break;
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
      }passwordCorrect = false;
    }
  }
}
/**
*@brief obtiene valores de humedad y temperatura del ambiente en tiempo real
*@return humedad y temperatura
**/
void obtenerHumTempLuz(){
  humedad = dht.readHumidity();
  temp = dht.readTemperature();
  luz = analogRead(photocellPin);
}
/**
*@brief imprime en lcd los datos obtenidos en obtenerHumTempLuz(), lcd -> 16x2
*
**/
void displayDht(){
   obtenerHumTempLuz();
   lcd.setCursor(0, 0);
   lcd.print("Hum:");
   lcd.setCursor(4, 0);
   lcd.print(humedad,1);
   lcd.setCursor(0, 1);
   lcd.print("Temp:");
   lcd.setCursor(5, 1);
   lcd.print(temp);
   lcd.setCursor(9, 1);
   lcd.print("C");
   lcd.setCursor(9, 0);
   lcd.print("luz:");
   lcd.setCursor(13, 0);
   lcd.print(luz);
}
/**
**/
void estado1(){
//estado inicial, validar contraseña contraseña 
      handleKeypadInput();
      if (passwordCorrect == true){
        estadoActual = State_2;
      }
      if(attempts >= maxAttempts){
        estadoActual = State_4;
      }
}
/**
**/
void estado2(){
//lectura de humedad y luz
    delay(2000);
    lcd.clear();
    displayDht();
    if (temp > 30) {
      tone(buzzerPin, 1000, 200); // Encender el buzzer
      lcd.clear();
      lcd.print("Temperatura > 30C");
      //cambiar de estado si pasa de 20 segundos
      if (state2StartTime == 0) {
        state2StartTime = millis();
      }
      if (millis() - state2StartTime > state2Duration2) {
        estadoActual = State_4;
        state2StartTime = 0; // Reinicia el tiempo para el próximo ciclo en el State_2
      }
    }
}
/**
**/
void estado3(){
//estado activador de buzzer
      
}
/**
**/
void estado4(){
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
      attempts = 1; // Reinicia el contador de intentos
      }
    }
//@Todo agregar funcionalidades
}
/**
**/
void estado5(){
  lcd.clear();
  lcd.print("estado 5");
  delay(2000);
}