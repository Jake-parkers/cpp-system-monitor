#include <string>
#include <iostream>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours, minutes;

  minutes = seconds / 60;
  seconds = seconds % 60; // remaining secs after removing minutes
  hours = minutes / 60;
  minutes = minutes % 60; // remaining minutes after removing hours
  string minute_format = (minutes < 10) ? ":0" : ":";
  string second_format = (seconds < 10) ? ":0" : ":";
  string hour_format = (hours < 10) ? "0" : "";

  return hour_format + std::to_string(hours) + minute_format + std::to_string(minutes) + second_format + std::to_string(seconds);
}