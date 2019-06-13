#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include "SPIFFS.h"
#else
#include <ESP8266WiFi.h>
#endif
#include "AudioFileSourceID3.h"
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "SD.h"

// To run, set your ESP8266 build to 160MHz, and include a SPIFFS of 512KB or
// greater. Use the "Tools->ESP8266/ESP32 Sketch Data Upload" menu to write the
// MP3 to SPIFFS Then upload the sketch normally.

// pno_cs from https://ccrma.stanford.edu/~jos/pasp/Sound_Examples.html

AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

// Called when a metadata event occurs (i.e. an ID3 tag, an ICY block, etc.
void MDCallback(void *cbData, const char *type, bool isUnicode,
                const char *string) {
  (void)cbData;
  Serial.printf("ID3 callback for: %s = '", type);

  if (isUnicode) {
    string += 2;
  }

  while (*string) {
    char a = *(string++);
    if (isUnicode) {
      string++;
    }
    Serial.printf("%c", a);
  }
  Serial.printf("'\n");
  Serial.flush();
}

void setup() {
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  delay(1000);

  // SD卡初始化
  boolean sd_success = false;
  do {
    sd_success = SD.begin();
    Serial.println("init sdcard");
    // delay(500);
  } while (!sd_success);

  pinMode(16, OUTPUT);
  digitalWrite(16, HIGH);

  Serial.printf("Sample MP3 playback begins...\n");
  file = new AudioFileSourceSD("/1.mp3");
  id3 = new AudioFileSourceID3(file);
  id3->RegisterMetadataCB(MDCallback, (void *)"ID3TAG");
  out = new AudioOutputI2S(0, 1);
  // out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();

  digitalWrite(16, LOW);
  mp3->begin(id3, out);
}

void loop() {
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      digitalWrite(16, HIGH);
      mp3->stop();
    }
  } else {
    Serial.printf("MP3 done\n");
    delay(2000);

    file = new AudioFileSourceSD("/1.mp3");
    id3 = new AudioFileSourceID3(file);
    id3->RegisterMetadataCB(MDCallback, (void *)"ID3TAG");
    out = new AudioOutputI2S(0, 1);
    mp3 = new AudioGeneratorMP3();

    digitalWrite(16, LOW);
    mp3->begin(id3, out);
  }
}
