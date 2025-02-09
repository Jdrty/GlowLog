#pragma once
#include <NTPClient.h>
#include <WiFiUdp.h>

class TimeUtils {
public:
  void initialize();
  int getDaysAgo(time_t eventTime);
  
private:
  static const long UTC_OFFSET;
  WiFiUDP ntpUDP;
  NTPClient timeClient;
};