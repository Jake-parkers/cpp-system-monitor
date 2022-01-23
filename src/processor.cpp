#include "processor.h"
#include "linux_parser.h"
#include <unistd.h>
#include <iostream>

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  // get utilization over a 1 second period
  long prev_idle = LinuxParser::IdleJiffies();
  long prev_non_idle = LinuxParser::ActiveJiffies();
  long prev_total = prev_idle + prev_non_idle;
  sleep(1);
  long idle = LinuxParser::IdleJiffies();
  long non_idle = LinuxParser::ActiveJiffies();
  long total = idle + non_idle;

  long total_diff = total - prev_total;
  long idle_diff = idle - prev_idle;
  float cpu_percent = (total_diff - idle_diff) / (float)total_diff;
  return cpu_percent;
}