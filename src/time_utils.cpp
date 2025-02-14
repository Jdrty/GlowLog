#include "time_utils.h"
#include <time.h>    // for strptime and mktime

// Global objects for NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0); // UTC offset 0

const long TimeUtils::UTC_OFFSET = 0;

// Initialize the NTP client
void TimeUtils::initialize() {
  timeClient.begin();
  timeClient.update();
}

// Calculate how many days ago the event occurred
int TimeUtils::getDaysAgo(time_t eventTime) {
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  return (now - eventTime) / 86400; // 86400 seconds per day
}

// Parse an ISO8601 formatted date string into a time_t value
time_t TimeUtils::parseISO8601(const char* dateStr) {
  struct tm tm;
  strptime(dateStr, "%Y-%m-%dT%H:%M:%SZ", &tm);
  return mktime(&tm);
}