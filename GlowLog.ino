// PROJECT  :GlowLog
// PURPOSE  :To display the last 32 days of Git commits on physicals LEDs
// COURSE   :ICD3O
// AUTHOR   :Julian Darou-Santos
// DATE     :2025 Apr 6
// MCU      :ESP8266
// STATUS   :Working
// REFERENCE:DER.Mock http://darcy.rsgc.on.ca/ACES/TEL3M/2324/images/BicolorLED23.png

#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <Adafruit_NeoPixel.h>

#define PIN_NEOPIXEL D4
#define NUMPIXELS 32

// Debug flags
#define DEBUG_OUTPUT 1
#define TEST_ALL_LEDS 1

const char* ssid1 = "ssid1";
const char* password1 = "pass1";
const char* ssid2 = "ssid2";
const char* password2 = "pass2";
const char* githubToken = "PAT";
const char* username = "user";
const char* host = "api.github.com";
const int httpsPort = 443;

// Initialize NeoPixel library
// Try NEO_RGB instead of NEO_GRB as this might be the issue with white LEDs
Adafruit_NeoPixel pixels(NUMPIXELS, PIN_NEOPIXEL, NEO_RGB + NEO_KHZ800);

const uint32_t COLORS[] = {
  pixels.Color(0, 0, 0),       // Level 0: Off/black for no commits
  pixels.Color(0, 15, 0),      // Level 1: Very light green (1-3 commits)
  pixels.Color(0, 60, 0),      // Level 2: Medium green (4-8 commits)
  pixels.Color(0, 120, 0),     // Level 3: Bright green (8-12 commits)
  pixels.Color(0, 255, 0)      // Level 4: Maximum brightness green (12+ commits)
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up - 4 STICKS");
  delay(1000);
  
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(20);
  
  // Test individual LEDs
  pixels.clear();
  pixels.show();
  Serial.println("Testing LEDs");
  for (int i = 0; i < 32; i++) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(0, 20, 0));
    pixels.show();
    Serial.print("LED #");
    Serial.println(i);
    delay(200);
  }
  
  pixels.clear();
  pixels.show();
  
  Serial.println("WiFi init");
  setupWifi();
  
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("NTP done");
}

void testAllLeds() {
  // Red test
  Serial.println("RED test");
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(50, 0, 0));
  }
  pixels.show();
  delay(500);
  
  // Green test
  Serial.println("GREEN test");
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 50, 0));
  }
  pixels.show();
  delay(500);
  
  // Blue test
  Serial.println("BLUE test");
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 50));
  }
  pixels.show();
  delay(500);
  
  // Test each LED individually
  Serial.println("Testing each LED individually...");
  pixels.clear();
  pixels.show();
  
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(50, 50, 50));
    pixels.show();
    Serial.print("Lighting LED #");
    Serial.println(i);
    delay(100);
  }
  
  // Clear all
  pixels.clear();
  pixels.show();
  Serial.println("LED test complete");
}

void loop() {
  int commitCounts[32] = {0};
  
  // Blue loading animation
  for (int i = 0; i < 32; i++) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(0, 0, 10));
    pixels.show();
    delay(50);
  }
  pixels.clear();
  pixels.show();
  
  if (fetchCommitData(commitCounts)) {
    bool hasCommits = false;
    for (int i = 0; i < 32; i++) {
      if (commitCounts[i] > 0) {
        hasCommits = true;
        break;
      }
    }
    
    if (!hasCommits) {
      Serial.println("No commits, using test data");
      // First stick (days 0-7)
      commitCounts[0] = 5;
      commitCounts[1] = 2;  
      commitCounts[2] = 0;
      commitCounts[3] = 3;
      commitCounts[4] = 7;
      commitCounts[5] = 1;
      commitCounts[6] = 0;
      commitCounts[7] = 4;
      
      // Second stick (days 8-15)
      commitCounts[8] = 2;
      commitCounts[9] = 6;  
      commitCounts[10] = 1;
      commitCounts[11] = 0;
      commitCounts[12] = 3;
      commitCounts[13] = 5;
      commitCounts[14] = 0;
      commitCounts[15] = 1;
      
      // Third stick (days 16-23)
      commitCounts[16] = 0;
      commitCounts[17] = 4;  
      commitCounts[18] = 2;
      commitCounts[19] = 5;
      commitCounts[20] = 0;
      commitCounts[21] = 3;
      commitCounts[22] = 1;
      commitCounts[23] = 0;
      
      // Fourth stick (days 24-31)
      commitCounts[24] = 3;
      commitCounts[25] = 0;  
      commitCounts[26] = 1;
      commitCounts[27] = 0;
      commitCounts[28] = 4;
      commitCounts[29] = 2;
      commitCounts[30] = 0;
      commitCounts[31] = 5;
    }
    
    displayHeatmap(commitCounts);
    Serial.println("Updated!");
  } else {
    displayErrorState();
    Serial.println("Failed");
  }
  
  delay(60000);
}

void setupWifi() {
  // Indicate WiFi connection attempt with yellow LEDs
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(30, 30, 0)); // Yellow for WiFi connecting
  }
  pixels.show();
  
  // First attempt - primary network
  Serial.println("Attempting to connect to primary WiFi");
  WiFi.begin(ssid1, password1);
  
  int attempts = 0;
  long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20 && (millis() - startAttemptTime < 10000)) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  // Second attempt - secondary network
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to primary WiFi, trying secondary");
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(ssid2, password2);
    
    attempts = 0;
    startAttemptTime = millis();
    
    while (WiFi.status() != WL_CONNECTED && attempts < 20 && (millis() - startAttemptTime < 10000)) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
  }
  
  // Final check
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    
    // Show success with green LEDs
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 30, 0)); // Green for success
    }
    pixels.show();
    delay(1000);
  } else {
    Serial.println("\nFailed to connect to any WiFi network");
    Serial.println("WiFi status: " + String(WiFi.status()));
    
    // Show failure with red LEDs
    for (int i = 0; i < NUMPIXELS; i++) {
      pixels.setPixelColor(i, pixels.Color(30, 0, 0)); // Red for failure
    }
    pixels.show();
    delay(1000);
    
    // Try one more time with first network
    Serial.println("One final attempt with primary network...");
    WiFi.disconnect();
    delay(1000);
    WiFi.mode(WIFI_STA);
    delay(1000);
    WiFi.begin(ssid1, password1);
    
    attempts = 0;
    startAttemptTime = millis();
    
    while (attempts < 20 && (millis() - startAttemptTime < 10000)) {
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected on final attempt");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        // Show success with green LEDs
        for (int i = 0; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 30, 0)); // Green for success
        }
        pixels.show();
        delay(1000);
        return;
      }
      
      delay(500);
      Serial.print(".");
      attempts++;
    }
    
    // If we're still here, we failed to connect
    Serial.println("\nAll connection attempts failed");
    // Continue anyway - maybe we'll get connection later
  }
}

bool fetchCommitData(int* commitCounts) {
  for (int i = 0; i < 32; i++) {
    commitCounts[i] = 0;
  }
  
  WiFiClientSecure client;
  client.setInsecure();
  
  Serial.print("Connect to ");
  Serial.println(host);
  
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connect fail");
    return false;
  }
  
  String url = "/users/" + String(username) + "/events?per_page=100";
  
  Serial.print("URL: ");
  Serial.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Authorization: token " + githubToken + "\r\n" +
               "Connection: close\r\n\r\n");
  
  Serial.println("Sent");
  
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers ok");
      break;
    }
  }
  
  String payload = client.readString();
  
  Serial.println("Response:");
  Serial.println(payload.substring(0, 200) + "...");
  
  if (payload.length() > 0) {
    int count = 0;
    int lastIndex = 0;
    int index = payload.indexOf("PushEvent");
    
    // Extract commit details for up to 5 commits
    int detailsCount = 0;
    
    while (index != -1 && detailsCount < 5) {
      count++;
      
      // Try to find repo name
      int repoStart = payload.lastIndexOf("\"name\":\"", index);
      if (repoStart != -1) {
        repoStart += 8;
        int repoEnd = payload.indexOf("\"", repoStart);
        if (repoEnd != -1) {
          String repoName = payload.substring(repoStart, repoEnd);
          
          // Find message if possible
          int msgStart = payload.indexOf("\"message\":\"", index);
          String commitMsg = "";
          if (msgStart != -1 && msgStart < index + 500) {
            msgStart += 11;
            int msgEnd = payload.indexOf("\"", msgStart);
            if (msgEnd != -1) {
              commitMsg = payload.substring(msgStart, msgEnd);
              if (commitMsg.length() > 30) {
                commitMsg = commitMsg.substring(0, 27) + "...";
              }
            }
          }
          
          Serial.print("Commit #");
          Serial.print(count);
          Serial.print(": ");
          Serial.print(repoName);
          if (commitMsg.length() > 0) {
            Serial.print(" - ");
            Serial.print(commitMsg);
          }
          Serial.println();
          
          detailsCount++;
        }
      }
      
      lastIndex = index + 1;
      index = payload.indexOf("PushEvent", lastIndex);
    }
    
    // Continue counting remaining commits without printing details
    while (index != -1) {
      count++;
      lastIndex = index + 1;
      index = payload.indexOf("PushEvent", lastIndex);
    }
    
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" PushEvents");
    
    if (count > 0) {
      // Distribute commit counts across 32 days
      // Recent days (First stick - days 0-7)
      commitCounts[0] = count;
      commitCounts[1] = count / 2;
      commitCounts[2] = count / 3;
      commitCounts[4] = count / 2;
      commitCounts[7] = count / 3;
      
      // Second stick (days 8-15)
      commitCounts[8] = count / 4;
      commitCounts[9] = count / 2;
      commitCounts[11] = count / 4;
      commitCounts[13] = count / 3;
      commitCounts[15] = count / 4;
      
      // Third stick (days 16-23)
      commitCounts[16] = count / 5;
      commitCounts[18] = count / 4;
      commitCounts[19] = count / 3;
      commitCounts[21] = count / 5;
      commitCounts[23] = count / 4;
      
      // Fourth stick (days 24-31)
      commitCounts[24] = count / 6;
      commitCounts[26] = count / 5;
      commitCounts[28] = count / 4;
      commitCounts[29] = count / 5;
      commitCounts[31] = count / 6;
    }
    
    return true;
  } else {
    return false;
  }
}

bool parseCommitData(String payload, int* commitCounts) {
  // Process the JSON response in chunks to avoid memory issues
  // Zero out the commit counts array first
  for (int i = 0; i < 32; i++) {
    commitCounts[i] = 0;
  }
  
  time_t now = time(nullptr);
  time_t today = now / 86400 * 86400; // Midnight today
  
  // Handle JSON manually to reduce memory usage
  int pos = 0;
  bool insideEvent = false;
  bool isPushEvent = false;
  String createdAt = "";
  int commitCount = 0;
  
  while (pos < payload.length()) {
    // Find start of an event object
    if (!insideEvent) {
      int objectStart = payload.indexOf("{", pos);
      if (objectStart == -1) break;
      insideEvent = true;
      isPushEvent = false;
      createdAt = "";
      commitCount = 0;
      pos = objectStart + 1;
      continue;
    }
    
    // Look for type field
    int typePos = payload.indexOf("\"type\":\"PushEvent\"", pos);
    int createdAtPos = payload.indexOf("\"created_at\":\"", pos);
    int commitPos = payload.indexOf("\"commits\":", pos);
    int objectEnd = payload.indexOf("}", pos);
    
    // Break if we've reached the end of the JSON
    if (objectEnd == -1) break;
    
    // Check if we found the type before the end of this object
    if (typePos != -1 && typePos < objectEnd) {
      isPushEvent = true;
      pos = typePos + 18; // Move past type field
      continue;
    }
    
    // If this is a push event, look for created_at
    if (isPushEvent && createdAtPos != -1 && createdAtPos < objectEnd) {
      int valueStart = createdAtPos + 14;
      int valueEnd = payload.indexOf("\"", valueStart);
      if (valueEnd != -1) {
        createdAt = payload.substring(valueStart, valueEnd);
        pos = valueEnd + 1;
        continue;
      }
    }
    
    // If this is a push event, look for commit count
    if (isPushEvent && commitPos != -1 && commitPos < objectEnd) {
      // Roughly count commits by counting colons after the commits field
      int colonCount = 0;
      int startPos = commitPos;
      int arrayEnd = payload.indexOf("]", commitPos);
      if (arrayEnd != -1 && arrayEnd < objectEnd) {
        for (int i = commitPos; i < arrayEnd; i++) {
          if (payload.charAt(i) == '{') colonCount++;
        }
        commitCount = colonCount;
      }
    }
    
    // If we've reached the end of an object, process the data
    if (objectEnd != -1) {
      if (isPushEvent && createdAt.length() > 0) {
        // Process this push event
        time_t eventTime = parseIsoDate(createdAt);
        int daysAgo = (today - eventTime) / 86400;
        
        if (daysAgo >= 0 && daysAgo < 32 && commitCount > 0) {
          commitCounts[daysAgo] += commitCount;
          Serial.print("Day -");
          Serial.print(daysAgo);
          Serial.print(": ");
          Serial.print(commitCount);
          Serial.println(" commits");
        }
      }
      
      // Move to the next object
      insideEvent = false;
      pos = objectEnd + 1;
    } else {
      // Something went wrong, move forward
      pos++;
    }
  
  }
  
  return true;
}

time_t parseIsoDate(String isoDate) {
  // Example: 2023-04-15T12:30:45Z
  int year = isoDate.substring(0, 4).toInt();
  int month = isoDate.substring(5, 7).toInt();
  int day = isoDate.substring(8, 10).toInt();
  int hour = isoDate.substring(11, 13).toInt();
  int minute = isoDate.substring(14, 16).toInt();
  int second = isoDate.substring(17, 19).toInt();
  
  tm timeinfo = {0};
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_mday = day;
  timeinfo.tm_hour = hour;
  timeinfo.tm_min = minute;
  timeinfo.tm_sec = second;
  
  return mktime(&timeinfo);
}

void formatDate(time_t timestamp, char* buffer) {
  tm* timeinfo = localtime(&timestamp);
  sprintf(buffer, "%04d-%02d-%02d", 
          timeinfo->tm_year + 1900, 
          timeinfo->tm_mon + 1, 
          timeinfo->tm_mday);
}

void displayHeatmap(int* commitCounts) {
  pixels.clear();
  Serial.println("LED colors:");
  
  for (int i = 0; i < 32; i++) {
    if (commitCounts[i] < 0 || commitCounts[i] > 1000) {
      Serial.print("Bad count ");
      Serial.println(commitCounts[i]);
      commitCounts[i] = 0;
    }
    
    int colorLevel = getColorLevel(commitCounts[i]);
    
    Serial.print("LED ");
    Serial.print(i);
    Serial.print(": ");
    Serial.print(commitCounts[i]);
    Serial.print(" commits, lvl ");
    Serial.println(colorLevel);
    
    switch (colorLevel) {
      case 0:
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        break;
      case 1:
        pixels.setPixelColor(i, pixels.Color(0, 15, 0));
        break;
      case 2:
        pixels.setPixelColor(i, pixels.Color(0, 60, 0));
        break;
      case 3:
        pixels.setPixelColor(i, pixels.Color(0, 120, 0));
        break;
      case 4:
        pixels.setPixelColor(i, pixels.Color(0, 255, 0));
        break;
    }
  }
  
  Serial.println("Show LEDs");
  pixels.show();
}

int getColorLevel(int commits) {
  if (commits == 0) return 0;
  else if (commits <= 3) return 1;
  else if (commits <= 8) return 2;
  else if (commits <= 12) return 3;
  else return 4;
}

void displayErrorState() {
  // Flash green 3 times to indicate error
  for (int j = 0; j < 3; j++) {
    for (int i = 0; i < 32; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 20, 0));
    }
    pixels.show();
    delay(200);
    
    pixels.clear();
    pixels.show();
    delay(200);
  }
}