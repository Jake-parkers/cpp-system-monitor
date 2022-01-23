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
  return std::to_string(hours) + ":" + std::to_string(minutes) +  ":" + std::to_string(seconds);
}