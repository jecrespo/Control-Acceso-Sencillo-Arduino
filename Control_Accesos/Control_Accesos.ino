#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define SS_PIN 10
#define RST_PIN 9
 
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

// Init array that will store new NUID 
byte nuidPICC[4] = {0x96,0x86,0xED,0x75}; //tarjeta conocida

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