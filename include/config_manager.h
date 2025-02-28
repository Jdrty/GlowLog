// include/config_manager.h

#pragma once
#include <EEPROM.h>

class ConfigManager {
public:
  struct Config {
    char wifiSSID[32];
    char wifiPass[32];
    char githubPAT[40];
  };

  void loadConfig();
  void saveConfig();
  
  const char* getWiFiSSID() const;
  const char* getWiFiPass() const;
  const char* getGitHubPAT() const;

private:
  Config config;
};