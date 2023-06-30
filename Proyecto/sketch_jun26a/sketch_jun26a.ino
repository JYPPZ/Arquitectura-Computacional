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
#define photocellPin A7 // Pin al que está conectado el sensor de luz
#define buzzerPin 6 // Pin al que está conectado el buzzer
#define trackerPin 4 // Pin al que está conectado el tracker
#define hallPin A4 // Pin al que está conectado el hall

DHT dht(DHTPIN, DHTTYPE); // Objeto DHT

/* Display */
LiquidCrystal lcd(30, 31, 32, 33, 34, 35);
/* Keypad setup */
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
byte rowPins[KEYPAD_ROWS] = {47, 45, 43, 41};
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
int attempts = 0;//intentos
const unsigned long maxInputTime = 10000; // Tiempo máximo de espera para ingresar la contraseña (en milisegundos)
/* Variables de cambio de estado */
unsigned long state3StartTime = 0; // Almacena el tiempo en milisegundos cuando se entra al State_4
const unsigned long state3Duration = 20000; // Duración en milisegundos para permanecer en el State_4
unsigned long state2StartTime = 0; // Almacena el tiempo en milisegundos cuando se entra al State_2
const unsigned long state2Duration = 5000; // Duración en milisegundos para permanecer en el State_2
const unsigned long state2Duration2 = 20000; // Duración en milisegundos para permanecer en el State_2
unsigned long state4StartTime = 0; // Almacena el tiempo en milisegundos cuando se entra al State_5

/* Variables humedad temperatura y luz */
float humedad = 0.0f;
float temp = 0.0f;
int luz = 0;
/* tracker variables */
const int threshold = 800; // Umbral de detección (ajusta este valor según tu sensor)
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
  State_4
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
  /* Congiguracion de las entradas digitales de los diferentes sensores */
  pinMode(buzzerPin, OUTPUT);
  pinMode(trackerPin, INPUT);
  pinMode(hallPin, INPUT);
  pinMode(DHTPIN, INPUT);
  //inicializar dht
  dht.begin();
}

void loop() {
  updateCursor();
  Estado siguienteEstado = estadoActual;
  /* Alternar entre estados */
  Serial.println(estadoActual);
  switch (estadoActual) {
    case InicitialState:
      siguienteEstado = estado1();
    break;
    case State_2:
      siguienteEstado = estado2();
    break;
    case State_3:
      siguienteEstado = estado3();
    break;
    case State_4:
      siguienteEstado = estado4();
    break;
    default: break;
  }
  // Actualizar el estado actual si ha cambiado
  if (siguienteEstado != estadoActual) {
    estadoActual = siguienteEstado;
    state2StartTime = millis(); // inicializar conteo para esatdo 2
    state3StartTime = millis(); // inicializar conteo para esatdo 3
    state4StartTime = millis(); // inicializar conteo para esatdo 4
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
*@param key digito ingresado en keypad
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
*@brief imprime en lcd los datos obtenidos en obtenerHumTempLuz(), lcd -> 16x2
**/
void displayDht(){
  humedad = dht.readHumidity();
  temp = dht.readTemperature();
  luz = analogRead(photocellPin);
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
   Serial.println(temp);
}
/**
**/
int valorHall(){
  int sensorValue = analogRead(hallPin);  // Leer el valor analógico del sensor
  //Serial.println(sensorValue);
  return sensorValue;
}
/**
**/
bool trackerProximidad() {
  bool sensorValue = digitalRead(trackerPin);
  return sensorValue;
}

/**
**/
int estado1(){
//estado inicial, validar contraseña contraseña 
  handleKeypadInput();
  //Serial.println(attempts);
  if (passwordCorrect == true){
    Serial.println("true");
    attempts = 1; // reiniciar intentos
    return State_2;
  }
  else if(attempts >= maxAttempts){
    //Serial.println("dentro");
    return State_4;
  }
  return InicitialState;
}
/**
**/
int estado2(){
//lectura de humedad y luz
  Serial.println("estado 2");
  lcd.clear();
  displayDht();
  if (temp > 30) {
    tone(buzzerPin, 1000, 200); // Encender el buzzer
    lcd.clear();
    lcd.print("Temperatura > 32.0fC");
    //cambiar de estado si pasa de 20 segundos
    if (millis() - state2StartTime > state2Duration2) {
      state2StartTime = 0; // Reinicia el tiempo para el próximo ciclo en el State_2
      return State_3;
    }
  }
  if (millis() - state2StartTime >= 2500) {
    Serial.println("pasa a 3");
    return State_4;
  }
  delay(100);
  return State_2;
}
/**
**/
int estado3(){
// Acciones cuando la máquina está bloqueada
  Serial.println("estado 3");
    lcd.clear();
    lcd.print("Sistema bloqueado");
    // Comprueba si ha transcurrido el tiempo necesario para la transición al State_4
    if (millis() - state3StartTime >= state3Duration) {
      return State_4;
      attempts = 0; // Reinicia el contador de intentos
    }
  delay(100);
  return State_3;
}
/**
**/
int estado4() {
// Estado sensores hall y tracker
  lcd.clear();
  Serial.println("estado 4");
  lcd.print("Sensor activo...");
  //Serial.println(valorHall());
  // Verificar si se ha alcanzado el tiempo para cambiar al Estado 3
  if (valorHall() < threshold) {
    lcd.clear();
    lcd.print("Objeto detectado");  // Imprimir un mensaje si el objeto es detectado
    Serial.println("pasa a 2");
    return State_3;
  }
  // Verificar si se ha alcanzado el tiempo para cambiar al Estado 2
  if (millis() - state4StartTime >= 1500) {
    //state4StartTime = 0; // Reiniciar el tiempo para el Estado 4
    Serial.println("pasa a 1");
    return State_2;
  }
  // Permanecer en el Estado 4
  delay(100);
  return State_4;
}