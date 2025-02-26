#pragma once
#include <Arduino.h>

class GitHubClient {
public:
  bool initialize(const char* ssid, const char* password);
  bool fetchCommitData(uint8_t commits[32], const char* githubUsername, const char* githubPAT);
  
private:
  static const char* GITHUB_API_HOST;
  static const int HTTPS_PORT;
};