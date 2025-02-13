#pragma once
#include <NTPClient.h>
#include <WiFiUdp.h>

class TimeUtils {
public:
  // Initialize NTP client and update time
  static void initialize();
  
  // Calculate the number of days ago from the given event time
  static int getDaysAgo(time_t eventTime);
  
  // Parse an ISO8601 date string (e.g., "2021-05-01T12:34:56Z") into epoch time
  static time_t parseISO8601(const char* dateStr);
  
private:
  static const long UTC_OFFSET;
};
