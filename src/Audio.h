#ifndef __AUDIO_H__
#define __AUDIO_H__

/**
 * 使用前需要先初始化SD卡
 */

#include "Arduino.h"
#include "AudioFileSourceID3.h"
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

class Audio {
 public:
  void setup();
  void select(char *name);
  bool isFinish();
  void loop();

 private:
  AudioGeneratorMP3 *mp3;
  AudioFileSourceSD *file;
  AudioOutputI2S *out;
  AudioFileSourceID3 *id3;

  bool finishFlag = false;
};

#endif
