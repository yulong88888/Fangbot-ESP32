#include <WiFi.h>
#include <AsyncTCP.h>
#include <DNSServer.h>
#include "ESPAsyncWebServer.h"

DNSServer dnsServer;
AsyncWebServer tserver(80);

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print("<!DOCTYPE html><html><head><title>Captive Portal</title></head><body>");
    response->print("<p>This is out captive portal front page.</p>");
    response->printf("<p>You were trying to reach: http://%s%s</p>", request->host().c_str(), request->url().c_str());
    response->printf("<p>Try opening <a href='http://%s'>this link</a> instead</p>", WiFi.softAPIP().toString().c_str());
    response->print("</body></html>");
    request->send(response);
  }
};


void setup(){
  //your other setup stuff...
  WiFi.softAP("esp-captive");
  dnsServer.start(80, "*", WiFi.softAPIP());
  tserver.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  //more handlers...
  tserver.begin();
}

void loop(){
  dnsServer.processNextRequest();
}
