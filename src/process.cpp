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
  long prev_system_uptime = LinuxParser::UpTime(); // total time the system has been up
  long prev_process_uptime = LinuxParser::UpTime(m_pid); // total time elapsed since the process started
  long prev_process_time = LinuxParser::ActiveJiffies(m_pid); // total time the process has spent with the CPU (utime, stime, cutime, cstime)
  float prev_seconds = (float)(prev_system_uptime - (prev_process_uptime / HERTZ )); // elapsed time since process started in secs
//  std:: cout << "Prev: " << prev_system_uptime << " " << prev_process_uptime << " " << prev_process_time << " " << prev_seconds << '\n';
  sleep(1);
  long system_uptime = LinuxParser::UpTime(); // total time the system has been up
  long process_uptime = LinuxParser::UpTime(m_pid); // total time elapsed since the process started
  long process_time = LinuxParser::ActiveJiffies(m_pid);// total time the process has spent with the CPU (utime, stime, cutime, cstime)
  float seconds = (float)(system_uptime - (process_uptime / HERTZ )); // elapsed time since process started in secs

  float process_time_secs = (float)(process_time - prev_process_time) / HERTZ;
//  std:: cout << "Curr: " << system_uptime << " "  << process_uptime << " "  << process_time << " " << seconds << "hertz = "  << HERTZ << " " << process_time_secs << '\n';
  float cpu_usage = (1000 * process_time_secs) / (seconds - prev_seconds);
//  std::cout << "CU: " << cpu_usage << " " << cpu_usage / 10 << "." << (int)cpu_usage % 10 << '\n';
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
  return a.m_cpu_usage < m_cpu_usage;
}



int main() {
  Process p{3577};
  float test = p.CpuUtilization();
  std::cout << test << '\n';
  std::cout << p.Command() << '\n';
  std::cout << p.Command() << '\n';
  std::cout << p.Ram() << '\n';
  std::cout << p.User() << '\n';
  std::cout << p.UpTime() << '\n';
}