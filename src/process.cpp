#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <time.h>
#include <cmath>
#include "process.h"
#include "linux_parser.h"
#include <iostream>

#define HERTZ sysconf(_SC_CLK_TCK)
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : m_pid { pid } {}

// TODO: Return this process's ID
int Process::Pid() { return m_pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  // calculate most recent CPU utilization for a process over a period of 1/4th of a second;
  float prev_system_uptime = LinuxParser::ActiveJiffies() / (float)HERTZ; // total time the system has been up (in seconds)
  float prev_process_time = LinuxParser::ActiveJiffies(m_pid) / (float)HERTZ; // total time the process has spent with the CPU (utime, stime, cutime, cstime)
  sleep(0.125);

  float system_uptime = LinuxParser::ActiveJiffies() / (float)HERTZ;
  float process_time = LinuxParser::ActiveJiffies(m_pid) / (float)HERTZ;

  if (system_uptime - prev_system_uptime == 0) {
    m_cpu_usage = 0.00;
    return m_cpu_usage;
  }
  m_cpu_usage = (process_time - prev_process_time) / (float)(system_uptime - prev_system_uptime);
  return m_cpu_usage;
}

// TODO: Return the command that generated this process
string Process::Command() {
  if (m_command == "") {
    m_command = LinuxParser::Command(m_pid);
  }
  return m_command;
}

// TODO: Return this process's memory utilization
string Process::Ram() {
  m_ram = LinuxParser::Ram(m_pid);
  return m_ram;
}

// TODO: Return the user (name) that generated this process
string Process::User() {
  if (m_username == "") {
    m_username = LinuxParser::User(m_pid);
  }
  return m_username;
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime(m_pid);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process &a) {
  return a.CpuUtilization() < CpuUtilization();
}