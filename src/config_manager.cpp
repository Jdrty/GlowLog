// src/config_manager.cpp

#include "config_manager.h"
#include <string.h>

#define EEPROM_SIZE 128  // Enough EEPROM space for the config struct

// Load configuration from EEPROM; if invalid, use default credentials
void ConfigManager::loadConfig() {
  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(0, config);
  // Check if config appears uninitialized (first byte empty or 0xFF)
  if (config.wifiSSID[0] == '\0' || config.wifiSSID[0] == 0xFF) {
    // Use .env macros if defined, else fallback to defaults
#ifdef WIFI_SSID
    strncpy(config.wifiSSID, WIFI_SSID, sizeof(config.wifiSSID));
#else
    strncpy(config.wifiSSID, "your_ssid", sizeof(config.wifiSSID));
#endif

#ifdef WIFI_PASS
    strncpy(config.wifiPass, WIFI_PASS, sizeof(config.wifiPass));
#else
    strncpy(config.wifiPass, "your_password", sizeof(config.wifiPass));
#endif

#ifdef GITHUB_PAT
    strncpy(config.githubPAT, GITHUB_PAT, sizeof(config.githubPAT));
#else
    strncpy(config.githubPAT, "your_github_pat", sizeof(config.githubPAT));
#endif

    saveConfig();
  }
}

// Save configuration to EEPROM
void ConfigManager::saveConfig() {
  EEPROM.put(0, config);
  EEPROM.commit();
}

const char* ConfigManager::getWiFiSSID() const {
  return config.wifiSSID;
}

const char* ConfigManager::getWiFiPass() const {
  return config.wifiPass;
}

const char* ConfigManager::getGitHubPAT() const {
  return config.githubPAT;
}