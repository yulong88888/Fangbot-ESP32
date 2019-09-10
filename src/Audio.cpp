#include "Audio.h"
#include "config.h"

void Audio::setup() {
  pinMode(PIN_AUDIO_EN, OUTPUT);
  digitalWrite(PIN_AUDIO_EN, HIGH);
}

void Audio::select(char *name) {
  file = new AudioFileSourceSD(name);
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S(0, 1);
  mp3 = new AudioGeneratorMP3();

  finishFlag = false;
  digitalWrite(PIN_AUDIO_EN, LOW);
  mp3->begin(id3, out);
}

bool Audio::isFinish() { return finishFlag; }

void Audio::loop() {
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      digitalWrite(PIN_AUDIO_EN, HIGH);
      mp3->stop();
    }
  } else {
    Serial.printf("MP3 done\n");
    finishFlag = true;
  }
}
