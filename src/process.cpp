#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <time.h>
#include <cmath>
#include <iostream>
#include "process.h"
#include "linux_parser.h"

#define HERTZ sysconf(_SC_CLK_TCK)
using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : m_pid { pid } {}

// TODO: Return this process's ID
int Process::Pid() { return m_pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  // calculate most recent CPU utilization for a process over a period of 1 second;
  long prev_system_uptime = LinuxParser::UpTime(); // total time the system has been up (in seconds)
  long prev_process_uptime = LinuxParser::UpTime(m_pid); // total time elapsed since the process started
  long prev_process_time = LinuxParser::ActiveJiffies(m_pid); // total time the process has spent with the CPU (utime, stime, cutime, cstime)
  float prev_seconds = (float)(prev_system_uptime - (prev_process_uptime / HERTZ )); // elapsed time since process started in secs

  sleep(1);

  long system_uptime = LinuxParser::UpTime(); // total time the system has been up
  long process_uptime = LinuxParser::UpTime(m_pid); // total time elapsed since the process started
  long process_time = LinuxParser::ActiveJiffies(m_pid);// total time the process has spent with the CPU (utime, stime, cutime, cstime)
  float seconds = (float)(system_uptime - (process_uptime / HERTZ )); // elapsed time since process started in secs

  float process_time_secs = (float)(process_time - prev_process_time) / HERTZ;
  float cpu_usage = (1000 * process_time_secs) / (seconds - prev_seconds);
  string cpu_usage_str = to_string(cpu_usage / 10) + "." + to_string((int)cpu_usage % 10);
  m_cpu_usage =  std::stof(cpu_usage_str);
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
bool Process::operator<(Process const& a[[maybe_unused]]) const {
  return m_cpu_usage < a.m_cpu_usage;
}