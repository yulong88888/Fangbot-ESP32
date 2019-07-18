/*
 * Generated partition that would work perfectly with this example
 * #Name,   Type, SubType, Offset,   Size,    Flags
 * nvs,     data, nvs,     0x9000,   0x5000,
 * otadata, data, ota,     0xe000,   0x2000,
 * app0,    app,  ota_0,   0x10000,  0x140000,
 * app1,    app,  ota_1,   0x150000, 0x140000,
 * eeprom0, data, 0x99,    0x290000, 0x1000,
 * eeprom1, data, 0x9a,    0x291000, 0x500,
 * eeprom2, data, 0x9b,    0x292000, 0x100,
 * spiffs,  data, spiffs,  0x293000, 0x16d000,
 */
#include "config.h"
#include "EEPROM.h"

#define ADDRESS_SSID 0
#define ADDRESS_PASSWORD 512
#define ADDRESS_STEPS_PER_TURN 1024
#define ADDRESS_CIRCUMFERENCE_MM 1152
#define ADDRESS_WHEEL_DISTANCE 1280

#define EEPROM_SIZE 2048

void Config::setup() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise eeprom");
    delay(1000);
    setup();
  }
}

void Config::set_ssid(String ssid) {
  EEPROM.writeString(ADDRESS_SSID, ssid);
  EEPROM.commit();
}

String Config::get_ssid() {
  String value = EEPROM.readString(ADDRESS_SSID);
  return value;
}

void Config::set_password(String password) {
  EEPROM.writeString(ADDRESS_PASSWORD, password);
  EEPROM.commit();
}

String Config::get_password() {
  String value = EEPROM.readString(ADDRESS_PASSWORD);
  return value;
}

void Config::set_step_per_turn(String value) {
  EEPROM.writeString(ADDRESS_STEPS_PER_TURN, value);
  EEPROM.commit();
}

String Config::get_step_per_turn() {
  String value = EEPROM.readString(ADDRESS_STEPS_PER_TURN);
  if (value.equals("")) {
    return STEPS_PER_TURN;
  } else {
    return value;
  }
}

void Config::set_circumference_mm(String value) {
  EEPROM.writeString(ADDRESS_CIRCUMFERENCE_MM, value);
  EEPROM.commit();
}

String Config::get_circumference_mm() {
  String value = EEPROM.readString(ADDRESS_CIRCUMFERENCE_MM);
  if (value.equals("")) {
    return CIRCUMFERENCE_MM;
  } else {
    return value;
  }
}

void Config::set_wheel_distance(String value) {
  EEPROM.writeString(ADDRESS_WHEEL_DISTANCE, value);
  EEPROM.commit();
}

String Config::get_wheel_distance() {
  String value = EEPROM.readString(ADDRESS_WHEEL_DISTANCE);
  if (value.equals("")) {
    return WHEEL_DISTANCE;
  } else {
    return value;
  }
}
