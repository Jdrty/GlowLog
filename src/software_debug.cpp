// src/software_debug.cpp

#include <Arduino.h>
#include <ESP8266WiFi.h> 
#include "config_manager.h"
#include "github_client.h"
#include "time_utils.h"
#include "commit_mapper.h"

// Mock LED driver to avoid hardware dependencies
class MockLedDriver {
public:
  void initialize() {
    Serial.println("LED: Initialized LED strip (mock)");
  }
  
  void setBrightness(uint8_t brightness) {
    Serial.print("LED: Set brightness to ");
    Serial.println(brightness);
  }
  
  void displayCommits(const uint8_t commits[]) {
    Serial.println("LED: Displaying commit data (mock visualization):");
    for (int i = 0; i < 32; i++) {
      Serial.print(i);
      Serial.print(": ");
      
      if (commits[i] == 0) {
        Serial.println("□ (off)");
      } else if (commits[i] <= 63) {
        Serial.println("▓ (25%)");
      } else if (commits[i] <= 127) {
        Serial.println("▒ (50%)");
      } else {
        Serial.println("█ (100%)");
      }
    }
  }
  
  void testPattern() {
    Serial.println("LED: Running test pattern (mock)");
    delay(500);
  }
  
  void errorPattern() {
    Serial.println("LED: Displaying error pattern (mock)");
    delay(500);
  }
};

ConfigManager configManager;
GitHubClient githubClient;
MockLedDriver ledDriver;

// Test function prototypes
void testConfigManager();
void testGitHubFetch();
void testCommitMapping();
void testSimulation();
void testErrorHandling();

void setup() {
  Serial.begin(115200);
  delay(2000); // Give time for serial monitor to connect
  
  Serial.println("\n\n======== GlowLog Software-Only Debug Tool ========");
  Serial.println("Send one of the following commands:");
  Serial.println("1: Test Config Manager");
  Serial.println("2: Test GitHub API Fetch");
  Serial.println("3: Test Commit Mapping Algorithm");
  Serial.println("4: Run System Simulation");
  Serial.println("5: Test Error Handling");
  Serial.println("=================================================");
}

void loop() {
  if (Serial.available() > 0) {
    int command = Serial.read() - '0';
    Serial.println();
    
    switch (command) {
      case 1:
        testConfigManager();
        break;
      case 2:
        testGitHubFetch();
        break;
      case 3:
        testCommitMapping();
        break;
      case 4:
        testSimulation();
        break;
      case 5:
        testErrorHandling();
        break;
      default:
        Serial.println("Invalid command. Please try again.");
        break;
    }
    
    Serial.println("\n=================================================");
    Serial.println("Send another command (1-5) to continue testing:");
  }
  
  delay(100);
}

void testConfigManager() {
  Serial.println("Testing Config Manager...");
  
  Serial.println("Loading configuration...");
  configManager.loadConfig();
  
  Serial.print("WiFi SSID: ");
  Serial.println(configManager.getWiFiSSID());
  
  Serial.print("WiFi Password: ");
  Serial.print(configManager.getWiFiPass()[0]);
  for (int i = 1; i < strlen(configManager.getWiFiPass()); i++) {
    Serial.print("*");
  }
  Serial.println();
  
  Serial.print("GitHub PAT: ");
  Serial.print(configManager.getGitHubPAT()[0]);
  Serial.print(configManager.getGitHubPAT()[1]);
  Serial.print(configManager.getGitHubPAT()[2]);
  Serial.print(configManager.getGitHubPAT()[3]);
  for (int i = 4; i < strlen(configManager.getGitHubPAT()); i++) {
    Serial.print("*");
  }
  Serial.println();
  
  if (strcmp(configManager.getWiFiSSID(), "your_ssid") == 0 &&
      strcmp(configManager.getWiFiPass(), "your_password") == 0 &&
      strcmp(configManager.getGitHubPAT(), "your_github_pat") == 0) {
    Serial.println("WARNING: Using default configuration values!");
    Serial.println("You need to set your actual values in the code or environment variables.");
  } else {
    Serial.println("Custom configuration detected.");
  }
  
  Serial.println("Config Manager test completed.");
}

void testGitHubFetch() {
  Serial.println("Testing GitHub API Fetch...");
  
  // Load configuration
  configManager.loadConfig();
  
  // Initialize WiFi and time
  Serial.println("Connecting to WiFi...");
  bool wifiConnected = githubClient.initialize(configManager.getWiFiSSID(), configManager.getWiFiPass());
  
  if (!wifiConnected) {
    Serial.println("ERROR: WiFi connection failed! Cannot test GitHub API fetch.");
    return;
  }
  
  Serial.println("WiFi Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  TimeUtils::initialize();
  Serial.println("Time initialized.");
  
  // Fetch GitHub data
  uint8_t commits[32] = {0};
  
  #ifndef GITHUB_USERNAME
  #define GITHUB_USERNAME "Jdrty"
  #endif
  
  const char* githubUsername = GITHUB_USERNAME;
  
  Serial.print("Fetching commit data for user '");
  Serial.print(githubUsername);
  Serial.println("'...");
  
  unsigned long startTime = millis();
  bool success = githubClient.fetchCommitData(commits, githubUsername, configManager.getGitHubPAT());
  unsigned long duration = millis() - startTime;
  
  if (success) {
    Serial.print("Fetch successful! Time taken: ");
    Serial.print(duration);
    Serial.println(" ms");
    
    Serial.println("Raw commit data:");
    int totalCommits = 0;
    for (int i = 0; i < 32; i++) {
      Serial.print("Day ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(commits[i]);
      totalCommits += commits[i];
    }
    
    Serial.print("Total commits in last 32 days: ");
    Serial.println(totalCommits);
  } else {
    Serial.println("ERROR: GitHub API fetch failed!");
    Serial.println("Please check your GitHub PAT and username.");
  }
  
  Serial.println("GitHub API Fetch test completed.");
}

void testCommitMapping() {
  Serial.println("Testing Commit Mapping Algorithm...");
  
  // Create test data with various commit counts
  Serial.println("Creating test commit data...");
  uint8_t testCommits[32] = {0};
  
  // Fill with test data
  testCommits[0] = 0;   // No commits
  testCommits[1] = 1;   // 1 commit
  testCommits[2] = 2;   // 2 commits
  testCommits[3] = 3;   // 3 commits
  testCommits[4] = 4;   // 4 commits
  testCommits[5] = 8;   // 8 commits
  testCommits[6] = 10;  // 10 commits
  
  // Fill remaining days with pattern
  for (int i = 7; i < 32; i++) {
    testCommits[i] = i % 6;
  }
  
  Serial.println("Original commit counts:");
  for (int i = 0; i < 32; i++) {
    Serial.print("Day ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(testCommits[i]);
  }
  
  // Apply normalization
  Serial.println("Applying normalization algorithm...");
  CommitMapper::normalizeBrightness(testCommits);
  
  Serial.println("Normalized brightness values:");
  for (int i = 0; i < 32; i++) {
    Serial.print("Day ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(testCommits[i]);
    
    // Add description of brightness level
    if (testCommits[i] == 0) {
      Serial.println(" (Off)");
    } else if (testCommits[i] == 63) {
      Serial.println(" (25% brightness)");
    } else if (testCommits[i] == 127) {
      Serial.println(" (50% brightness)");
    } else if (testCommits[i] == 255) {
      Serial.println(" (100% brightness)");
    } else {
      Serial.println(" (Unknown brightness level)");
    }
  }
  
  Serial.println("Commit Mapping test completed.");
}

void testSimulation() {
  Serial.println("Running System Simulation...");
  
  Serial.println("1. Initializing components...");
  configManager.loadConfig();
  ledDriver.initialize();
  ledDriver.setBrightness(64);
  ledDriver.testPattern();
  
  bool wifiConnected = githubClient.initialize(configManager.getWiFiSSID(), configManager.getWiFiPass());
  
  if (!wifiConnected) {
    Serial.println("WiFi connection failed! Displaying error pattern and halting simulation.");
    ledDriver.errorPattern();
    return;
  }
  
  Serial.println("WiFi Connected!");
  TimeUtils::initialize();
  
  Serial.println("2. Fetching GitHub commit data...");
  uint8_t commits[32] = {0};
  bool fetchSuccess = githubClient.fetchCommitData(commits, GITHUB_USERNAME, configManager.getGitHubPAT());
  
  if (!fetchSuccess) {
    Serial.println("GitHub API fetch failed! Displaying error pattern.");
    ledDriver.errorPattern();
    return;
  }
  
  Serial.println("3. Normalizing commit data...");
  CommitMapper::normalizeBrightness(commits);
  
  Serial.println("4. Displaying commit data on LED strip...");
  ledDriver.displayCommits(commits);
  
  Serial.println("5. Simulation of main loop...");
  for (int i = 0; i < 3; i++) {
    Serial.print("Loop iteration ");
    Serial.println(i + 1);
    delay(1000);
  }
  
  Serial.println("System Simulation completed successfully!");
}

void testErrorHandling() {
  Serial.println("Testing Error Handling...");
  
  Serial.println("1. Testing WiFi connection failure...");
  // Force a WiFi failure by using invalid credentials
  bool wifiConnected = githubClient.initialize("invalid_ssid", "invalid_password");
  
  if (!wifiConnected) {
    Serial.println("WiFi connection failed as expected!");
    ledDriver.errorPattern();
  } else {
    Serial.println("WiFi connected unexpectedly with invalid credentials.");
  }
  
  Serial.println("2. Testing GitHub API failure...");
  uint8_t commits[32] = {0};
  // Try to fetch with invalid PAT
  bool fetchSuccess = githubClient.fetchCommitData(commits, GITHUB_USERNAME, "invalid_pat");
  
  if (!fetchSuccess) {
    Serial.println("GitHub API fetch failed as expected!");
    ledDriver.errorPattern();
  } else {
    Serial.println("GitHub API fetch succeeded unexpectedly with invalid PAT.");
  }
  
  Serial.println("3. Testing recovery...");
  // Try again with correct credentials
  wifiConnected = githubClient.initialize(configManager.getWiFiSSID(), configManager.getWiFiPass());
  
  if (wifiConnected) {
    Serial.println("WiFi recovery successful!");
    
    fetchSuccess = githubClient.fetchCommitData(commits, GITHUB_USERNAME, configManager.getGitHubPAT());
    
    if (fetchSuccess) {
      Serial.println("GitHub API recovery successful!");
      CommitMapper::normalizeBrightness(commits);
      ledDriver.displayCommits(commits);
    } else {
      Serial.println("GitHub API recovery failed.");
    }
  } else {
    Serial.println("WiFi recovery failed.");
  }
  
  Serial.println("Error Handling test completed.");
}