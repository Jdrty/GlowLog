// src/time_utils.cpp

#include "time_utils.h"
#include <time.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);

const long TimeUtils::UTC_OFFSET = 0;

void TimeUtils::initialize() {
  timeClient.begin();
  timeClient.update();
}

int TimeUtils::getDaysAgo(time_t eventTime) {
  timeClient.update();
  time_t now = timeClient.getEpochTime();
  return (now - eventTime) / 86400;
}

time_t TimeUtils::parseISO8601(const char* dateStr) {
  struct tm tm;
  
  tm.tm_year = (dateStr[0] - '0') * 1000 + (dateStr[1] - '0') * 100 +  
               (dateStr[2] - '0') * 10 + (dateStr[3] - '0') - 1900;    
  tm.tm_mon = (dateStr[5] - '0') * 10 + (dateStr[6] - '0') - 1;
  tm.tm_mday = (dateStr[8] - '0') * 10 + (dateStr[9] - '0');
  
  tm.tm_hour = (dateStr[11] - '0') * 10 + (dateStr[12] - '0');
  tm.tm_min = (dateStr[14] - '0') * 10 + (dateStr[15] - '0');
  tm.tm_sec = (dateStr[17] - '0') * 10 + (dateStr[18] - '0');
  
  tm.tm_isdst = 0;
  
  return mktime(&tm);
}