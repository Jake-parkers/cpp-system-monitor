#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;
/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

// TODO: Return the system's CPU
Processor& System::Cpu() {
  cpu_ = Processor {};
  return cpu_;
}

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  for (auto pid: pids) {
    processes_.emplace_back(pid);
  }
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() {
  if (kernel_ == "") {
    kernel_ = LinuxParser::Kernel();
  }
  return kernel_;
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() {
  if (osname_ == "") {
    osname_ = LinuxParser::OperatingSystem();
  }
  return osname_;
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }

//int main() {
// System sys;
// std::cout << "Uptime = " << sys.UpTime() << "\n";
// std::cout << "TP = " << sys.TotalProcesses() << "\n";
// std::cout << "Running = " << sys.RunningProcesses() << "\n";
// std::cout << "Os name = " << sys.OperatingSystem() << "\n";
// std::cout << "Memory Util = " << sys.MemoryUtilization() << "\n";
// std::cout << "kernel = " << sys.Kernel() << "\n";
// auto processes = sys.Processes();
// for (auto process: processes) {
//   std::cout << "Process = " << process.Pid() << "\n";
// }
//}
