// GlowLog.cpp

#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include "config_manager.h"
#include "github_client.h"
#include "led_driver.h"
#include "time_utils.h"
#include "commit_mapper.h"

#ifndef GITHUB_USERNAME
#define GITHUB_USERNAME "Jdrty"
#endif

ConfigManager configManager;
GitHubClient githubClient;
LedDriver ledDriver;

const char* githubUsername = GITHUB_USERNAME;
uint8_t commits[32];

unsigned long lastUpdateTime = 0;
const unsigned long UPDATE_INTERVAL = 3600000;

bool refreshCommitData() {
  if (WiFi.status() != WL_CONNECTED) {
    if (!githubClient.initialize(configManager.getWiFiSSID(), configManager.getWiFiPass())) {
      return false;
    }
  }
  
  if (!githubClient.fetchCommitData(commits, githubUsername, configManager.getGitHubPAT())) {
    return false;
  }
  
  CommitMapper::normalizeBrightness(commits);
  ledDriver.displayCommits(commits);
  
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("GlowLog starting...");

  configManager.loadConfig();
  
  ledDriver.initialize();
  ledDriver.setBrightness(64);
  ledDriver.testPattern();

  TimeUtils::initialize();

  if (!githubClient.initialize(configManager.getWiFiSSID(), configManager.getWiFiPass())) {
    ledDriver.errorPattern();
  } else {
    if (!refreshCommitData()) {
      ledDriver.errorPattern();
    }
  }
  
  lastUpdateTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL || currentTime < lastUpdateTime) {
    if (!refreshCommitData()) {
      ledDriver.errorPattern();
    }
    
    lastUpdateTime = millis();
  }
  
  delay(100);
}