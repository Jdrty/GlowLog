// include/time_utils.h

#pragma once
#include <NTPClient.h>
#include <WiFiUdp.h>

class TimeUtils {
public:
  // Initialize NTP client and update time
  static void initialize();
  
  // Calculate the number of days ago from the given event time
  static int getDaysAgo(time_t eventTime);
  
  // Parse an ISO8601 date string
  static time_t parseISO8601(const char* dateStr);
  
private:
  static const long UTC_OFFSET;
};
