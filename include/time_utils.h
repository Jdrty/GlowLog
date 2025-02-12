#pragma once
#include <NTPClient.h>
#include <WiFiUdp.h>

class TimeUtils {
public:
  static void initialize();
  static int getDaysAgo(time_t eventTime);
  static time_t parseISO8601(const char* dateStr);
  
private:
  static const long UTC_OFFSET;
};
