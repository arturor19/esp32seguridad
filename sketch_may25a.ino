#include <MD5.h>
#include "BluetoothSerial.h"
#define MAX_UID 8 
BluetoothSerial SerialBT;

const int ledPin = 2; 
const char* SecDevName = "Deco_SEC_AG";
char lastSix[7];
char lastSixC[7];

const char * generateUID() {
  /* Change to allowable characters */
  const char possible[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  static char uid[MAX_UID + 1];
  for (int p = 0, i = 0; i < MAX_UID; i++) {
    int r = random(0, strlen(possible));
    uid[p++] = possible[r];
  }
  uid[MAX_UID] = '\0';
  return uid;
}

char* calculateMD5(const char* input) {
  char buffer[128]; // Define un buffer de caracteres suficientemente grande
  strncpy(buffer, input, sizeof(buffer)); // Copia el string constante en el buffer
  buffer[sizeof(buffer) - 1] = '\0'; // Asegúrate de que el buffer tenga un terminador nulo
  unsigned char* hash = MD5::make_hash(buffer);
  char* md5str = MD5::make_digest(hash, 16);
  return md5str;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  SerialBT.begin(SecDevName); // Bluetooth device name
  const char* inputString = "hello world";
  char* md5str = calculateMD5(inputString);
  Serial.println("The device started, now you can pair it with bluetooth!");
  Serial.println(md5str);
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();
    String clave;
    String lastSixClave;
    int randomNumber;
    const char* uidVariable;
    char* md5str;
    int len;
    char* md5strClave;
    int lenc;
    String script; // Declarar la variable aquí

    switch (command) {
      case 'm':
        SerialBT.println("Utiliza las siguientes opciones:");
        SerialBT.println("m Para menu");
        SerialBT.println("s Para enviar codigo de validacion");
        break;
      case 's':
        SerialBT.println("Inserte codigo:");
        script = SerialBT.readStringUntil('\n'); // Inicializar la variable aquí
        script.trim(); // Eliminar espacios en blanco al principio y al final
        md5str = calculateMD5(script.c_str());
        len = strlen(md5str);
        strncpy(lastSix, md5str + (len - 6), 6);
        lastSix[6] = '\0'; // Agregar el terminador nulo
        SerialBT.print("Clave de apertura: ");
        SerialBT.println(lastSix);
        break;
      case '0':
        digitalWrite(ledPin, LOW);
        SerialBT.println("Desactivado");
        break;
      default:
        break;
    }
  }
}
