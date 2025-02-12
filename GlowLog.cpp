#include <Arduino.h>

#include <Arduino.h>
#include "github_client.h"
#include "time_utils.h"
#include "commit_mapper.h"
#include "led_driver.h"

// WiFi & GitHub configuration
const char* WIFI_SSID     = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";
const char* GITHUB_USERNAME = "your_github_username";
const char* GITHUB_PAT      = "ghp_yourPAT"; // Keep this secret!

// Global instances
GitHubClient githubClient;
LedDriver ledDriver;
uint8_t commitData[30] = {0};

void setup() {
  Serial.begin(115200);
  ledDriver.initialize();

  if (!githubClient.initialize(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("WiFi Failed!");
    while (1) delay(1000);
  }
  
  TimeUtils::initialize();
}

void loop() {
  // Use the username in the GitHub client call
  if (githubClient.fetchCommitData(commitData, GITHUB_USERNAME, GITHUB_PAT)) {
    CommitMapper::normalizeBrightness(commitData);
    ledDriver.displayCommits(commitData);
    delay(3600000); // 1 hour between updates
  } else {
    ledDriver.errorPattern();
    delay(300000); // Retry after 5 minutes
  }
}