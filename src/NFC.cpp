#include "NFC.h"
#include <MFRC522_I2C.h>
#include <Wire.h>

MFRC522 *mfrc522;

String audioName = "";

void NFC::setup() {
  Wire.begin();
  mfrc522 = new MFRC522(0x28, NULL);
  mfrc522->PCD_Init();
}

void NFC::loop() {
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  byte len;
  MFRC522::StatusCode status;

  if (!mfrc522->PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522->PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));

  byte buffer[18];

  block = 4;
  len = 18;

  status = (MFRC522::StatusCode)mfrc522->PCD_Authenticate(
      MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522->uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522->GetStatusCodeName(status));
    mfrc522->PICC_HaltA();
    mfrc522->PCD_StopCrypto1();
    return;
  }

  status = (MFRC522::StatusCode)mfrc522->MIFARE_Read(block, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522->GetStatusCodeName(status));
    mfrc522->PICC_HaltA();
    mfrc522->PCD_StopCrypto1();
    return;
  }

  String str = String((char *)buffer);
  if (str.length() != 0) {
    audioName = str + ".mp3";
    readSuccess = true;
  }

  Serial.println(F("\n**End Reading**\n"));

  delay(1);

  mfrc522->PICC_HaltA();
  mfrc522->PCD_StopCrypto1();
}

bool NFC::checkReady() { return readSuccess; }

String NFC::getResult() {
  if (!audioName.equals("")) {
    readSuccess = false;
    return audioName;
  }
}

int NFC::getBytesLength(byte *buffer) {
  int len = 0;
  for (byte i = 0; i < 16; i++) {
    if (buffer[i] != 0x00) {
      len++;
    }
  }
  return len;
}

void NFC::dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
