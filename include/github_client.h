#pragma once
#include <Arduino.h>         // For Arduino types
#include <ESP8266HTTPClient.h> // For HTTPClient

class GitHubClient {
public:
  void initialize(const char* ssid, const char* password);
  bool fetchCommitData(uint8_t commits[30], const char* githubPAT);
  
private:
  static const char* GITHUB_API_ROOT;
};