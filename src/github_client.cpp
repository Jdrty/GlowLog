#include "github_client.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "time_utils.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;

const char* GitHubClient::GITHUB_API_HOST = "api.github.com";
const int GitHubClient::HTTPS_PORT = 443;

bool GitHubClient::initialize(const char* dummySSID, const char* dummyPass) {
#ifdef WIFI_SSID1
  wifiMulti.addAP(WIFI_SSID1, WIFI_PASS1);
#endif
#ifdef WIFI_SSID2
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASS2);
#endif

  int retries = 0;
  while (wifiMulti.run() != WL_CONNECTED && retries < 20) {
    delay(500);
    retries++;
  }
  return WiFi.isConnected();
}

bool GitHubClient::fetchCommitData(uint8_t commits[32], const char* githubUsername, const char* githubPAT) {
  if (!WiFi.isConnected()) return false;

  WiFiClientSecure client;
  client.setInsecure();
  
  if (!client.connect(GITHUB_API_HOST, HTTPS_PORT)) {
    return false;
  }
  
  String path = String("/users/") + String(githubUsername) + "/events";
  
  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + GITHUB_API_HOST + "\r\n" +
               "Authorization: Bearer " + githubPAT + "\r\n" +
               "User-Agent: GlowLog\r\n" +
               "Accept: application/vnd.github+json\r\n" +
               "Connection: close\r\n\r\n");
  
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 10000) {
      client.stop();
      return false;
    }
  }
  
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders)) {
    client.stop();
    return false;
  }
  
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, client);
  client.stop();
  
  if (error) return false;
  
  memset(commits, 0, 32);
  
  for (JsonObject event : doc.as<JsonArray>()) {
    if (String((const char*)event["type"]) != "PushEvent") continue;
    
    const char* createdAt = event["created_at"];
    time_t eventTime = TimeUtils::parseISO8601(createdAt);
    
    int daysAgo = TimeUtils::getDaysAgo(eventTime);
    if (daysAgo < 0 || daysAgo >= 32) continue;
    
    int count = event["payload"]["size"];
    commits[daysAgo] += count;
  }
  
  return true;
}