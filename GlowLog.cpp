#include <Arduino.h>
#include "config_manager.h"
#include "github_client.h"
#include "led_driver.h"
#include "time_utils.h"
#include "commit_mapper.h"

// Use the .env macro for GitHub username if defined, otherwise fallback
#ifndef GITHUB_USERNAME
#define GITHUB_USERNAME "Jdrty"
#endif

// Instantiate global objects
ConfigManager configManager;
GitHubClient githubClient;
LedDriver ledDriver;

const char* githubUsername = GITHUB_USERNAME; // Set from .env or fallback

// Array to hold commit counts for the last 32 days
uint8_t commits[32];

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("GlowLog starting...");

  // Load configuration from EEPROM (WiFi credentials and GitHub PAT)
  configManager.loadConfig();
  Serial.println("Config loaded.");

  // Initialize LED driver (WS2812B LEDs)
  ledDriver.initialize();
  Serial.println("LEDs initialized.");

  // Initialize time utilities (NTP synchronization)
  TimeUtils::initialize();
  Serial.println("Time utilities initialized.");

  // Connect to WiFi using stored credentials
  Serial.println("Connecting to WiFi...");
  if (!githubClient.initialize(configManager.getWiFiSSID(), configManager.getWiFiPass())) {
    Serial.println("WiFi connection failed!");
    ledDriver.errorPattern();
    return;
  }
  Serial.println("WiFi connected.");

  // Fetch commit data from GitHub
  Serial.println("Fetching commit data...");
  if (!githubClient.fetchCommitData(commits, githubUsername, configManager.getGitHubPAT())) {
    Serial.println("Failed to fetch commit data!");
    ledDriver.errorPattern();
    return;
  }
  Serial.println("Commit data fetched.");

  // Normalize raw commit counts to brightness values
  CommitMapper::normalizeBrightness(commits);

  // Display commit data on the LED strip
  ledDriver.displayCommits(commits);
  Serial.println("Displayed commit data on LEDs.");
}

void loop() {
  // Optionally, refresh commit data periodically (e.g., every hour)
  // For now, the sketch runs once after startup.
}