#include <Arduino.h>

#include "config_manager.h"
#include "github_client.h"
#include "led_driver.h"
#include "time_utils.h"

GitHubClient githubClient;
LedDriver ledDriver;
TimeUtils timeUtils;
ConfigManager configManager;

uint8_t commitData[30] = {0};

void setup() {
  Serial.begin(115200);
  ledDriver.initialize();
  configManager.loadConfig();
  timeUtils.initialize();
  githubClient.initialize(configManager.getWiFiSSID(), configManager.getWiFiPass());
}

void loop() {
  if(githubClient.fetchCommitData(commitData, configManager.getGitHubPAT())) {
    ledDriver.displayCommits(commitData);
    delay(3600000); // 1 hour between updates
  } else {
    ledDriver.testPattern();
    delay(300000); // Retry after 5 minutes
  }
}