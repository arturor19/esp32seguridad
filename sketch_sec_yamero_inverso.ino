#include <MD5.h>
#include "BluetoothSerial.h"
#define MAX_UID 8 
BluetoothSerial SerialBT;

const int ledPin = 2; 
#define LED_ON LOW  // Definir que el LED se enciende con LOW
#define LED_OFF HIGH // Definir que el LED se apaga con HIGH
const char* SecDevName = "SEC_AG_A";
char lastSix[7];
char lastSixC[7];

const char * generateUID() {
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
  char buffer[128];
  strncpy(buffer, input, sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';
  unsigned char* hash = MD5::make_hash(buffer);
  char* md5str = MD5::make_digest(hash, 16);
  int len = strlen(md5str);
  char lastSix[7];
  strcpy(lastSix, md5str + (len - 6));
  Serial.println("Last 6 Characters:");
  Serial.println(lastSix);
  return md5str;
}

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LED_OFF); // Asegurarse de que el LED esté apagado inicialmente
  Serial.begin(115200);
  SerialBT.begin(SecDevName); 
  const char* inputString = "hello world";
  char* md5str = calculateMD5(inputString);
  Serial.println("The device started, now you can pair it with Bluetooth!");
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
    switch (command) {
      case 'm':
        SerialBT.println("Utiliza las siguientes opciones:");
        SerialBT.println("m Para menu");
        SerialBT.println("o Para obtener la clave");
        SerialBT.println("v Para validar la clave");
        break;
      case 'o':
        uidVariable = generateUID();
        md5str = calculateMD5(uidVariable);
        len = strlen(md5str);
        strcpy(lastSix, md5str + (len - 6));
        md5strClave = calculateMD5(lastSix);
        lenc = strlen(md5strClave);
        strcpy(lastSixC, md5strClave + (lenc - 6));
        SerialBT.println("Clave de unico acceso");
        SerialBT.println(lastSix);
        break;
      case 'v':
        randomNumber = random(1000);
        SerialBT.println("Inserte el codigo de validacion");
        clave = SerialBT.readStringUntil('\n');
        clave.trim();
        lastSixClave = clave.substring(clave.length() - 6);
        SerialBT.println(clave);
        SerialBT.println(lastSixC);
        if (strcmp(lastSixC, lastSixClave.c_str()) == 0) {
          SerialBT.println("Codigo valido");
          digitalWrite(ledPin, LED_ON); // Encender el LED
        } else {
          SerialBT.println("Codigo Invalido");
        }
        break;
      case '0':
        digitalWrite(ledPin, LED_OFF); // Apagar el LED
        SerialBT.println("Desactivado");
        break;
      default:
        break;
    }
  }
}
