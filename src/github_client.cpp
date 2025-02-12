#include "github_client.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include "time_utils.h"
#include <ESP8266WiFi.h>

// Example
const char* GitHubClient::GITHUB_API_ROOT = "https://api.github.com/";

bool GitHubClient::initialize(const char* ssid, const char* password) {
  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    retries++;
  }
  return WiFi.isConnected();
}

bool GitHubClient::fetchCommitData(uint8_t commits[30], const char* githubUsername, const char* githubPAT) {
  if (!WiFi.isConnected()) return false;

  WiFiClientSecure client;
  client.setInsecure(); // For testing; later replace with proper certificate handling
  HTTPClient http;

  // Configure client timeouts
  client.setTimeout(10000);
  http.setReuse(false);
  http.setTimeout(10000);

  // Build the request URL using the provided username
  String url = "https://api.github.com/users/" + String(githubUsername) + "/events";
  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " + String(githubPAT));
  http.addHeader("User-Agent", "GlowLog");
  http.addHeader("Accept", "application/vnd.github+json");

  // Execute the GET request
  int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    http.end();
    return false;
  }

  // Parse JSON from response
  DynamicJsonDocument doc(8192);
  DeserializationError error = deserializeJson(doc, http.getString());
  http.end();

  if (error) return false;

  // Reset commit counts for the 30-day array
  memset(commits, 0, 30);

  // Process each event in the returned array
  for (JsonObject event : doc.as<JsonArray>()) {
    // Only consider PushEvents
    if (String((const char*)event["type"]) != "PushEvent") continue;

    // Get the event's timestamp and convert it
    const char* createdAt = event["created_at"];
    time_t eventTime = TimeUtils::parseISO8601(createdAt);
    
    // Calculate days ago (0 = today, 29 = 29 days ago)
    int daysAgo = TimeUtils::getDaysAgo(eventTime);
    if (daysAgo < 0 || daysAgo >= 30) continue;

    // Sum the number of commits for that day
    int count = event["payload"]["size"];
    commits[daysAgo] += count;
  }

  return true;
}
