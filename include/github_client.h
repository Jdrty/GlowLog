#pragma once
#include <Arduino.h>
#include <ESP8266HTTPClient.h>

class GitHubClient {
public:
  bool initialize(const char* ssid, const char* password);
  // Accepts username and PAT:
  bool fetchCommitData(uint8_t commits[30], const char* githubUsername, const char* githubPAT);
  
private:
  static const char* GITHUB_API_ROOT;
};
