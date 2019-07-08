#include "Network.h"
#include "WiFi.h"
#include "config.h"

char *Network::getAPName() {
  if (WiFi.getMode() == WIFI_MODE_NULL) {
    WiFi.mode(WIFI_MODE_APSTA);
  }
  String temp = WiFi.softAPmacAddress();
  temp.replace(":", "");
  String mac = temp.substring(8, 12);
  String result = "";
  result += ap_name;
  result += "-";
  result += mac;
  return (char *)result.c_str();
}

DynamicJsonDocument Network::getWifiScanData() {
  // Json数组
  DynamicJsonDocument doc(1024);
  //搜索的条数
  int count = 0;
  while (count == 0) {
    count = WiFi.scanComplete();
  }
  //有重复被拒绝
  bool rejected;
  for (int i = 0; i < count; i++) {
    rejected = false;
    for (int j = 0; j < i; j++) {
      if (WiFi.SSID(i) == WiFi.SSID(j)) {
        rejected = true;
        break;
      }
    }
    //如果没有重复的SSID
    if (!rejected) {
      JsonArray data = doc.createNestedArray();
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");

      data.add(WiFi.SSID(i));
      data.add(WiFi.RSSI(i));
      data.add(WiFi.encryptionType(i));
    }
  }
  return doc;
}

void Network::startScanWifi() { WiFi.scanNetworks(); }

String Network::getIp() { return WiFi.localIP().toString(); }