#include <Keypad.h>
#include <LiquidCrystal.h>

const int ROW_NUM = 4; // Número de filas del teclado
const int COLUMN_NUM = 4; // Número de columnas del teclado
char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pin_rows[ROW_NUM] = {5, 4, 3, 2}; // Pines de las filas del teclado
byte pin_column[COLUMN_NUM] = {A3, A2, A1, A0}; // Pines de las columnas del teclado

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);
LiquidCrystal lcd(12, 11, 10, 9, 8, 7); // Pines de conexión al LCD

const char* correctPasscode = "1234"; // Clave correcta
const int maxAttempts = 3; // Número máximo de intentos
const unsigned long maxInputTime = 10000; // Tiempo máximo de espera para ingresar la contraseña (en milisegundos)

void setup() {
  lcd.begin(16, 2);
  lcd.print("Ingrese clave:");
}

void loop() {
  static String passcodeInput;
  static unsigned long inputStartTime = 0;
  unsigned long currentTime = millis();
  char key = keypad.getKey();
  
  if (key) {
    inputStartTime = currentTime; // Reinicia el tiempo de espera al presionar cualquier tecla
    if (key != '#') {
      passcodeInput += key;
      lcd.setCursor(passcodeInput.length() - 1, 1);
      lcd.print("*");
    } else {
      if (passcodeInput.length() >= 4 && passcodeInput.length() <= 8) {
        if (passcodeInput == correctPasscode) {
          lcd.clear();
          lcd.print("Clave Correcta");
          delay(2000);
          lcd.clear();
          lcd.print("Ingrese clave:");
        } else {
          static int attempts = 1;
          if (attempts < maxAttempts) {
            lcd.clear();
            lcd.print("Clave Incorrecta");
            delay(2000);
            lcd.clear();
            lcd.print("Ingrese clave:");
            attempts++;
          } else {
            lcd.clear();
            lcd.print("Sistema bloqueado");
            while (true); // Bucle infinito para bloquear el sistema
          }
        }
      } else {
        lcd.clear();
        lcd.print("Clave Incorrecta");
        delay(2000);
        lcd.clear();
        lcd.print("Ingrese clave:");
        static int attempts = 1;
        if (attempts < maxAttempts) {
          attempts++;
        } else {
          lcd.clear();
          lcd.print("Sistema bloqueado");
          while (true); // Bucle infinito para bloquear el sistema
        }
      }
      passcodeInput = "";
      lcd.setCursor(0, 1);
      lcd.print("                ");
    }
  } else if (currentTime - inputStartTime > maxInputTime) {
    lcd.clear();
    lcd.print("Tiempo Excedido");
    delay(2000);
    lcd.clear();
    lcd.print("Ingrese clave:");
    static int attempts = 1;
    if (attempts < maxAttempts) {
      attempts++;
    } else {
      lcd.clear();
      lcd.print("Sistema bloqueado");
      while (true); // Bucle infinito para bloquear el sistema
    }
    passcodeInput = "";
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
}