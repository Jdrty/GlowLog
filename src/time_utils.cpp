#include "time_utils.h"
#include <time.h>    // for strptime and mktime, if needed

// Global objects (or you could make these static members)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0); // UTC

void TimeUtils::initialize() {
  timeClient.begin();
  timeClient.update();
}

int TimeUtils::getDaysAgo(time_t eventTime) {
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  return (now - eventTime) / 86400; // 86400 seconds in a day
}

time_t TimeUtils::parseISO8601(const char* dateStr) {
  struct tm tm;
  strptime(dateStr, "%Y-%m-%dT%H:%M:%SZ", &tm);
  return mktime(&tm);
}
