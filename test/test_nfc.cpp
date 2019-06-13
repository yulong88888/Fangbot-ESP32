
#include <Wire.h>

#include "MFRC522_I2C.h"

#define SDA_PIN 17
#define SCL_PIN 16
#define RST_PIN 2

MFRC522 mfrc522(0x28, RST_PIN);  // Create MFRC522 instance.

void ShowReaderDetails();

void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC
  Wire.begin();          // Initialize I2C
  mfrc522.PCD_Init();    // Init MFRC522
  ShowReaderDetails();   // Show details of PCD - MFRC522 Card Reader details
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
}

void loop() {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  byte len;
  MFRC522::StatusCode status;

  // Look for new cards
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  byte buffer1[18];

  block = 4;
  len = 18;

  // Dump debug info about the card; PICC_HaltA() is automatically called
  //   mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
    return;
  }

  // PRINT FIRST NAME
//   for (uint8_t i = 0; i < 16; i++) {
//     if (buffer1[i] != 32) {
//       Serial.write(buffer1[i]);
//     }
//   }
//   Serial.print(" ");

  String str = String((char *)buffer1);
  if (str.length() != 0) {
    String audioName = str + ".mp3";
    Serial.println(audioName);
    // readSuccess = true;
  }

  Serial.println(F("\n**End Reading**\n"));

  delay(1);

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void ShowReaderDetails() {
  // Get the MFRC522 software version
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("MFRC522 Software Version: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (unknown)"));
  Serial.println("");
  // When 0x00 or 0xFF is returned, communication probably failed
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F(
        "WARNING: Communication failure, is the MFRC522 properly connected?"));
  }
}
