#include <Keypad.h>
#include <LiquidCrystal.h>
#include "DHT.h"


/**
* Declaración de variables auxiliares
*/
bool band = false;
char password[5];
char clave[5] = {'0', '1', '2', '3', '4'};
int cont = 0;
int intentos = 3; 
bool bandera = true;
bool bloqueado = false;
bool claveCorrecta = false;
char opcionSeleccionada = ' ';

#define beta 4090 //the beta of the thermistor

unsigned long tiempo;
float temperatura;
int luz;

int estado = 0;

/**
* Declaración de variables Keypad
*/
const byte ROWS = 4; 
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {32, 34, 36, 38}; 
byte colPins[COLS] = {40, 42, 44, 46}; 

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

/**
 Declaración de variables de umbral para Temperatura y intencidad de Luz.
*/
#define DEFAULT_TEMPHIGH 25
#define DEFAULT_TEMPLOW 10
#define DEFAULT_LUZHIGH 15
#define DEFAULT_LUZLOW 100

int umbrTempHigh = DEFAULT_TEMPHIGH, umbrTempLow = DEFAULT_TEMPLOW;
int umbrLuzHigh = DEFAULT_LUZHIGH, umbrLuzLow = DEFAULT_LUZLOW;

/**
* Declaración de variables para el control del buzzer analogico
*/
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
int duraciones[] = { 8, 8, 4, 4 };    // array con la duracion de cada nota
int melodia[] = { NOTE_B6, NOTE_C7, NOTE_CS7, NOTE_D7 };//// array con las notasa a reproducir

/**
* Declarión de pines de buzzer, sensor de luz, sensor de temperatura analogico
*/
#define DHTTYPE DHT22

#define BUZZER_PIN 22
#define DHT22_PIN A0
#define photo A1
#define PIR_SENSOR_PIN 27

DHT dht(DHT22_PIN, DHTTYPE);

/**
* Declaración de pines de rgb
*/
#define ledRedPin 10 
#define ledGreenPin 9
#define ledBluePin 8 

/**
* Declaración de pines de lcd
*/
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 

/**
* Declaración Variales Tiempo de espera ingreso clave de seguridad 
*/
unsigned long lastKeyPressTime = 0;
const unsigned long maxKeyDelay = 7000;

/**
 NAME: setup
----------------------------------------------------------------------------
 PARAMS: ninguno
----------------------------------------------------------------------------
 RETURN: nada
----------------------------------------------------------------------------
 PURPOSE: Configura los pines para controlar dispositivos externos como LEDs, inicializa la comunicación serial para enviar o recibir datos y prepara librerías para sensores u otros componentes electrónicos.
----------------------------------------------------------------------------
*/

void setup() {
  pinMode(ledRedPin, OUTPUT);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledBluePin, OUTPUT);

  Serial.begin(9600);
  lcd.begin(16, 2);

  dht.begin();

  lcd.print("Bienvenido");
  delay(500);
  lcd.clear();
}

/**
 NAME: loop
----------------------------------------------------------------------------
 PARAMS: ninguno
----------------------------------------------------------------------------
 RETURN: nada
----------------------------------------------------------------------------
 PURPOSE: La función loop() es el núcleo del programa. Se ejecuta repetidamente, llamando a la función Estados() para gestionar el estado actual del sistema y ejecutar las acciones correspondientes.
----------------------------------------------------------------------------
*/

void loop() {
  Estados();
}

/**
 NAME: Estados
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Permite gestionar los diferentes estados del sistema ('0', '1', '2' y '3'), ejecutando las acciones correspondientes a cada uno, como mostrar el menú principal, monitorear sensores, indicar alarmas o solicitar la clave de acceso.
----------------------------------------------------------------------------
*/

void Estados(){
  switch(estado){
    case 0:
      if (bloqueado) {
        sonar_buzz();
        mostrarMensaje("Bloqueado");
        delay(10000);
        bloqueado = false;
        claveCorrecta = false;
      }
      if (!claveCorrecta) {
        solicitarClave();
        verificarClave();
      } else {
        estado = 1;
      }
    break;
    case 1:
      opcionSeleccionada == ' ';
      mostrarMenu();
    break;
    case 2:
      lcd.clear();
      Serial.println("Estado de monitoreo");
      Monitoreo();
    break;
    case 3:
      lcd.setCursor(0, 0); // Establecer la posición del cursor en la primera fila
      lcd.print("Alarma.......");
      Serial.println("Estado de alarma");
      sonar_buzz();
      color(255, 0, 0, 10000);
      color(0, 0, 0, 100);
      lcd.clear();
      estado = 2;
      Estados();
    break;
    default:
      estado=0;
    break;
  }
}

/**
 NAME: solicitarClave
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE:  Proporciona un tiempo límite de espera por tecla presionada para evitar entradas erróneas. Almacena la clave ingresada caracter a caracter en un array y oculta los caracteres reales mostrando asteriscos en la pantalla LCD.
----------------------------------------------------------------------------
*/

void solicitarClave(){
  lcd.clear();
  mostrarMensaje("Ingrese Clave:");
  lastKeyPressTime = millis();

  while (cont < 5)
  {
    if (millis() - lastKeyPressTime > maxKeyDelay)
    {
      return;
    }

    char key = keypad.getKey();

    if (key)
    {
      Serial.println(key);
      password[cont] = key;
      lcd.setCursor(cont, 1);
      lcd.print('*');
      cont++;
      lastKeyPressTime = millis();
    }
  }
}

/**
 NAME: verificarClave
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: La función permite el ingreso de una clave por parte del usuario y la compara con una clave predefinida. Si la clave es correcta, habilita el acceso. Si la clave es incorrecta y se superan los intentos permitidos, bloquea el acceso.
----------------------------------------------------------------------------
*/

void verificarClave() {
  bandera = true;
  for (int i = 0; i < 5; ++i) {
    if (password[i] != clave[i]) {
      bandera = false;
    }
  }
  if (bandera) {
    color(LOW,HIGH,LOW, 3000);
    lcd.clear(); 
    lcd.print("Clave correcta");
    delay(1000);
    claveCorrecta = true;
  } else {
    color(HIGH, LOW, HIGH, 1000);
    lcd.clear();
    lcd.print("Clave incorrecta");
    delay(1000);
    intentos--; 
    lcd.clear(); 
    lcd.print("Intentos restantes:");
    lcd.setCursor(0, 1);
    lcd.print(intentos);
    delay(1000);
    if (intentos == 0) {
      color(HIGH, LOW, LOW, 10000);
      intentos = 3;
      bloqueado = true;
    }
  }
  color(LOW, LOW, LOW, 100);
  cont = 0;
}

/**
* NAME: mostrarMenu
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE:  Muestra el menú principal en la pantalla LCD, permite el desplazamiento automático para visualizar todas las opciones y ofrece dos formas de selección: por tecla de navegación ('A' y 'B') o directamente por el número de la opción (del 1 al 6). Al seleccionar una opción válida, se llama a la función ejecutarOpcion() para procesarla.
----------------------------------------------------------------------------
*/

void mostrarMenu() {
  char key = ' ';
  int opcion = 0;
  int scrollDelay = 1000; 

  unsigned long lastScrollTime = 0; 

  while (key != 'D') { 
    if (millis() - lastScrollTime >= scrollDelay) { 
      lcd.clear();
      lcd.setCursor(0, 0); 
      for (int i = 0; i < 2; i++) {
        int currentOption = opcion + i;
        lcd.setCursor(0, i);
        if (currentOption == 0) {
          lcd.print("1. UmbralTempHigh");
        } else if (currentOption == 1) {
          lcd.print("2. UmbralTempLow ");
        } else if (currentOption == 2) {
          lcd.print("3. UmbralLuzHigh ");
        } else if (currentOption == 3) {
          lcd.print("4. UmbralLuzLow  ");
        } else if (currentOption == 4) {
          lcd.print("5. Monitoreo"); 
        } else if (currentOption == 5) {
          lcd.print("6. Reset"); 
        }
      }

      lastScrollTime = millis(); 
    }

    key = keypad.getKey(); 

    if (key == 'A') { 
      opcion = (opcion - 1 + 6) % 6;
    } else if (key == 'B') { 
      opcion = (opcion + 1) % 6;
    } else if (key >= '1' && key <= '6') { 
      ejecutarOpcion(key);
      key = 'D';
    }
  }
}  

/**
 NAME: esperarSeleccion
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE:  Proporciona un tiempo de espera para que el usuario seleccione una opción en el teclado.  Si se selecciona una opción válida dentro del tiempo límite, se procesa la opción llamando a la función ejecutarOpcion(). Si no se selecciona ninguna opción o se excede el tiempo, la función finaliza sin procesar ninguna opción.
----------------------------------------------------------------------------
*/

void esperarSeleccion() {
  char opcion = ' ';
  tiempo = millis() + 4000;

  while(opcion == ' ' && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' && key != 'B' && key != 'C' && key != 'D'){
        opcion = key;
      }
    }
  }
  if (opcion != ' '){
    tiempo = 0;
    opcionSeleccionada = opcion;
    ejecutarOpcion(opcionSeleccionada);
  }
}

/**
 NAME: ejecutarOpcion
----------------------------------------------------------------------------
 PARAMS:	char
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Permite al usuario interactuar con el menú principal, seleccionando opciones para configurar umbrales o cambiar el modo de funcionamiento llamando a las funciones correspondientes. Si se introduce una opción no válida, simplemente se limpia la selección y se vuelve a mostrar el menú.
----------------------------------------------------------------------------
*/

void ejecutarOpcion(char opcion) {
  switch (opcion) {
    case '1':
      umbralTempHigh();
      break;
    case '2':
      umbralTempLow();
      break;
    case '3':
      umbralLuzHigh();
      break;
    case '4':
      umbralLuzLow();
      break;
    case '5':
      estado = 2;
      Estados();
      break;
    case '6':
      reset();
      break;
    default:
      opcionSeleccionada = ' ';
      mostrarMenu();
      break;
  }
}

/**
 NAME: umbralTempHigh
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Modifica el valor de umbrTempHigh con el valor ingresado en el keypad
----------------------------------------------------------------------------
 NOTE: El valor ingresado debe ser mayor a 30 y menor a 60
*/
void umbralTempHigh() {
  mostrarMensaje("TEMP_HIGH");
  delay(1000);
  String temp="";
  int i = 0;
  tiempo = millis() + 4000;
  while(temp.length() < 2 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' &&key != 'B' && key != 'C' && key != 'D'){
        temp = temp + key;
        lcd.setCursor(i, 1); 
        lcd.print(key);
        i= i+1;
      }
    }
  }
  tiempo = 0;
  if(temp.length() != 0){
    delay(1000);
    verificarTempHigh(temp.toInt());
  }
}

/**
 NAME: umbralTempLow
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Modifica el valor de umbrTempLow con el valor ingresado en el keypad
----------------------------------------------------------------------------
 NOTE: El valor ingresado debe ser menor a 26 y mayor a 0
*/
void umbralTempLow() {
  mostrarMensaje("TEMP_LOW");
  delay(1000);
  String temp="";
  int i = 0;
  tiempo = millis() + 4000;
  while(temp.length() < 2 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' &&key != 'B' && key != 'C' && key != 'D'){
        temp = temp + key;
        lcd.setCursor(i, 1); 
        lcd.print(key);
        i= i+1;
      }
    }
  }
  tiempo = 0;
  if(temp.length() != 0){
    delay(1000);
    verificarTempLow(temp.toInt());
  }
}

/**
 NAME: umbralLuzHigh
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Modifica el valor de umbrLuzHigh con el valor ingresado en el keypad
----------------------------------------------------------------------------
 NOTE: El valor ingresado debe ser mayor a 190 y menor a 1023
*/
void umbralLuzHigh() {
  mostrarMensaje("LUZ_HIGH");
  delay(1000);
  String luz = "";
  int i = 0;
  tiempo = millis() + 4000;
  while(luz.length() < 3 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' && key != 'B' && key != 'C' && key != 'D'){
        luz = luz + key;
        lcd.setCursor(i, 1); 
        lcd.print(key);
        i= i+1;
      }
    }
  }
  if(luz.length() != 0){
    delay(1000);
    verificarLuzHigh(luz.toInt());
  }
}

/**
 NAME: umbralLuzLow
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Modifica el valor de umbrLuzLow con el valor ingresado en el keypad
----------------------------------------------------------------------------
 NOTE: El valor ingresado debe ser menor a 150 y mayor a 0
*/
void umbralLuzLow() {
  mostrarMensaje("LUZ_LOW");
  delay(1000);
  String luz = "";
  int i = 0;
  tiempo = millis() + 4000;
  while(luz.length() < 3 && millis() < tiempo){
    char key = keypad.getKey();
    if (key){
      if(key != '#' && key != '*' && key != 'A' && key != 'B' && key != 'C' && key != 'D'){
        luz = luz + key;
        lcd.setCursor(i, 1); 
        lcd.print(key);
        i= i+1;
      }
    }
  }
  if(luz.length() != 0){
    delay(1000);
    verificarLuzLow(luz.toInt());
  }
}

/**
 NAME: Monitoreo
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: La función Monitoreo() funciona como un bucle principal que se ejecuta continuamente, monitoreando la temperatura, la luminosidad, y respondiendo a la pulsación de una tecla específica (*)
----------------------------------------------------------------------------
*/

void Monitoreo() {
  unsigned long tiempo_inicial = millis();
  while (true) {
    unsigned long tiempo_actual = millis();
    luz = analogRead(photo);
    temperatura = dht.readTemperature(); //Leemos la temperatura en grados Celsius  
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Monitoreo ");
    
    lcd.setCursor(0, 1);
    lcd.print("T: ");
    lcd.print(temperatura);
    lcd.print(" L: ");
    lcd.print(luz); 

    if(temperatura > umbrTempHigh && luz < umbrLuzLow){
      estado = 3;
      Estados();
    }else if (temperatura > umbrTempLow && temperatura < umbrTempHigh && luz < umbrLuzHigh && luz > umbrLuzLow){
      estado = 3;
      Estados();
    }
    if (tiempo_actual - tiempo_inicial > 7000) {
      MonitoreoMovimiento();
      tiempo_inicial = millis();
    }

    char key = keypad.getKey();

    if (key == '*') { 
      estado = 1;
      Estados();
      break; 
    }

    delay(500);
  }
}

/**
 NAME: MonitoreoMovimiento
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: La función MonitoreoMovimiento() funciona como un detector de movimiento durante un período de 3 segundos,  actualizando la pantalla LCD con la información de movimiento detectado y  respondiendo en caso de que se detecte movimiento llamando a la función Estados().
----------------------------------------------------------------------------
*/

void MonitoreoMovimiento() {
  unsigned long tiempo_inicial = millis();
  while (millis() - tiempo_inicial <= 3000) {
    unsigned long tiempo_actual = millis();
    int movimiento = digitalRead(PIR_SENSOR_PIN);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Monitoreo ");
    lcd.setCursor(0, 1);
    lcd.print("Movimiento: ");
    lcd.print(movimiento);

    if(movimiento == 1){
      estado = 3;
      Estados();
    }
    delay(500);
  }
}

/**
 NAME: mostrarMensaje
----------------------------------------------------------------------------
 PARAMS:	String
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: La función mostrarMensaje() es una función simple y útil para mostrar mensajes informativos o de alerta. Se encarga de limpiar la pantalla, imprimir el mensaje recibido y mantenerlo visible durante un segundo antes de regresar al código que la llamó.
----------------------------------------------------------------------------
*/

void mostrarMensaje(String mensaje) {
  lcd.clear(); 
  lcd.print(mensaje);
  delay(1000);
}

/**
 NAME: sonar_buzz
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Permite reproducir tres notas con duraciones predeterminadas y pausas entre ellas.
----------------------------------------------------------------------------
*/

void sonar_buzz() { 
  for (int i = 0; i < 3; i++) {      
    int duracion = 1000 / duraciones[i]; 
    tone(BUZZER_PIN, melodia[i], duracion); 
    int pausa = duracion * 4.30;   
    delay(pausa);      
    noTone(BUZZER_PIN);  
  }
}

/**
 NAME: color
----------------------------------------------------------------------------
 PARAMS: char, char, char, int
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Permite ajustar la intensidad de cada color por separado (rojo, verde, azul) y especificar el tiempo de visualización del color seleccionado.
----------------------------------------------------------------------------
*/

void color (unsigned char red, unsigned char green, unsigned char blue, unsigned int tiempo) {
  digitalWrite(ledRedPin, red);
  digitalWrite(ledGreenPin, green);
  digitalWrite(ledBluePin, blue);
  delay(tiempo);
}

/** 
NAME: verificarTempHigh
----------------------------------------------------------------------------
 PARAMS:	int
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Verifica si se modifico el valor en umbrTempHigh
----------------------------------------------------------------------------
 NOTE:
----------------------------------------------------------------------------
*/
void verificarTempHigh(int temp){
  if(temp < 60 && temp > umbrTempLow && temp > 30){
    umbrTempHigh = temp;
    mostrarMensaje("Cambio exitoso");
  }else{
    mostrarMensaje("Cambio fallido");
  }
  opcionSeleccionada = ' ';
  mostrarMenu();
}

/**
 NAME: verificarTempLow
----------------------------------------------------------------------------
 PARAMS:	int
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Verifica si se modifico el valor en umbrTempLow
----------------------------------------------------------------------------
 NOTE:
----------------------------------------------------------------------------
 
*/
void verificarTempLow(int temp){
  if(temp < 15 && temp < umbrTempHigh && temp > 0){
    umbrTempLow = temp;
    mostrarMensaje("Cambio exitoso");
  }else{
    mostrarMensaje("Cambio fallido");
  }
  opcionSeleccionada = ' ';
  mostrarMenu();
}

/**
 NAME: verificarLuzHigh
----------------------------------------------------------------------------
 PARAMS:	int luz
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Verifica si se modifico el valor en umbrLuzHigh
----------------------------------------------------------------------------
 NOTE: 
----------------------------------------------------------------------------

*/
void verificarLuzHigh(int luz){
  if(luz < 1023 && luz > umbrLuzLow && luz > 150){
    umbrLuzHigh = luz;
    mostrarMensaje("Cambio exitoso");
  }else{
    mostrarMensaje("Cambio fallido");
  }
  opcionSeleccionada = ' ';
  mostrarMenu();
}

/**
 NAME: verificarLuzLow
----------------------------------------------------------------------------
 PARAMS:	int luz
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: Verifica si se modifico el valor en umbrLuzLow
----------------------------------------------------------------------------
 NOTE: 
----------------------------------------------------------------------------

*/
void verificarLuzLow(int luz){
  if(luz < 200 && luz < umbrLuzHigh&&luz >= 89){
    umbrLuzLow = luz;
    mostrarMensaje("Cambio exitoso");
  }else{
    mostrarMensaje("Cambio fallido");
  }
  opcionSeleccionada = ' ';
  mostrarMenu();
}

/**
 NAME: reset
----------------------------------------------------------------------------
 PARAMS:	ninguno
----------------------------------------------------------------------------
 RETURN:   nada
----------------------------------------------------------------------------
 PURPOSE: La función reset() solicita confirmación para realizar un reinicio de los umbrales, restaurándolos a sus valores por defecto. Si se confirma, actualiza los valores y vuelve al menú principal. Si se cancela, regresa al menú principal sin realizar cambios.
----------------------------------------------------------------------------
*/
void reset(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print("\"*\" Confirmar.  ");
  lcd.setCursor(0, 1);
  lcd.print("\"#\" Cancelar.   ");
  opcionSeleccionada = ' ';
  char pressedKey;
  while ((pressedKey = keypad.getKey()) != '*' && pressedKey == NO_KEY && pressedKey != '#') {
  }
  if (pressedKey == '#') {
    lcd.clear();
    mostrarMenu();
    return;
  }
  umbrTempHigh = DEFAULT_TEMPHIGH;
  umbrTempLow = DEFAULT_TEMPLOW;
  umbrLuzHigh = DEFAULT_LUZHIGH;
  umbrLuzLow = DEFAULT_LUZLOW;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reseteando...  ");
  delay(500);
  lcd.clear();
  mostrarMenu();
}
