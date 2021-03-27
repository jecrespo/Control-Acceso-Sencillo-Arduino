#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>

#define SS_PIN 10
#define RST_PIN 9

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = { //número ASCII no caracteres, no son imprimibles. Solo como indices
  {1, 2, 3, 4},
  {5, 6, 7, 8},
  {9, 10, 11, 12},
  {13, 14, 15, 16}
};
String teclas [] = {"S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9", "S10", "S11", "S12", "S13", "S14", "S15", "S16"};
byte rowPins[ROWS] = {A1, A2, A3, A4}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {A0, 2, 3, 4}; //connect to the column pinouts of the keypad
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad

// Init array that will store new NUID 
byte nuidPICC[4] = {0x96,0x86,0xED,0x75}; //tarjeta conocida
String pin1[4] = {"S2","S4","S5","S7"};

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
  lcd.init(); // initialize the lcd
  lcd.backlight();
  mensajeInicio();
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    rfid.PICC_HaltA();
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    Serial.println(F("Tarjeta Desconocida"));

    //error por LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tarjeta desconocida");
    delay(2000);
    mensajeInicio();

    rfid.PICC_HaltA();
    return;
  }
  else {
    Serial.println(F("Tarjeta Conocida"));
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tarjeta Leida");
    lcd.setCursor(0, 1);
    lcd.print("Introduce PIN");
  }

  //lee PIN
  String pin_leido[4];
  int i = 0;
  unsigned long tiempo = millis();
  while (true){
    char customKey = customKeypad.getKey();
    if (customKey) {
      pin_leido[i] = teclas[(int)customKey-1];
      Serial.println(teclas[(int)customKey-1]);
      i++;
    }
    if (i == 4)
      break;

    if ((millis()-tiempo) > 15000){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Timeout");
      Serial.println("Timeout");
      delay(2000);
      mensajeInicio();
      rfid.PICC_HaltA();
      return;
    }
  }

  //Comprobar PIN
  for (int j = 0; j<=3; j++){
    if (pin_leido[j] != pin1[j]){
      //error por LCD
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("PIN erroneo");
      Serial.println("PIN erroneo");
      delay(2000);
      mensajeInicio();

      rfid.PICC_HaltA();
      return;
    }   
  }

  Serial.println("Abrir Puerta");
  digitalWrite(8,HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Puerta Abierta");
  delay(5000);
  digitalWrite(8,LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Puerta Cerrada");
  delay(2000);

  // Halt PICC
  rfid.PICC_HaltA();

  delay(2000);
  mensajeInicio();
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void mensajeInicio(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Control Acceso");
  lcd.setCursor(0, 1);
  lcd.print("Espero Tarjeta");
}