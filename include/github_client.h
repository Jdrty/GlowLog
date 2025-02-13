#pragma once
#include <Arduino.h>
#include <ESP8266HTTPClient.h>

class GitHubClient {
public:
  // Initialize WiFi connection using provided credentials
  bool initialize(const char* ssid, const char* password);
  
  // Fetch commit data (for the last 32 days) using GitHub username and PAT
  bool fetchCommitData(uint8_t commits[32], const char* githubUsername, const char* githubPAT);
  
private:
  static const char* GITHUB_API_ROOT;
};