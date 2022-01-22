#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <filesystem>
//#include <experimental/filesystem>

#include "linux_parser.h"

namespace fs = std::filesystem;

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
//vector<int> LinuxParser::Pids() {
//  vector<int> pids;
//  DIR* directory = opendir(kProcDirectory.c_str());
//  struct dirent* file;
//  while ((file = readdir(directory)) != nullptr) {
//    // Is this a directory?
//    if (file->d_type == DT_DIR) {
//      // Is every character of the name a digit?
//      string filename(file->d_name);
//      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
//        int pid = stoi(filename);
//        pids.push_back(pid);
//      }
//    }
//  }
//  closedir(directory);
//  return pids;
//}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  fs::path dir { kProcDirectory};
  // go through each path in the directory
  for (const fs::directory_entry &dir_entry: fs::directory_iterator{ dir }) {
    // we only want to work with directories, so filter our non directories
    if (dir_entry.is_directory()) {
      // check if the directory we're currently looking at contains only integers (i.e corresponds to a process)
      // if it does add it to the pids array
      string file_name = dir_entry.path().filename().string();
      if (std::all_of(file_name.begin(), file_name.end(), isdigit)) {
        int pid;
        std::stringstream pid_str{file_name};
        pid_str >> pid;
        pids.push_back(pid);
      }
    }
  }
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  string line, name, key;
  float value;
  std::unordered_map<string, float> mem_map;
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> key >> value;
        if (key == "MemTotal") mem_map[key] = value;
        if (key == "MemFree") mem_map[key] = value;
        if (mem_map.size() == 2) break;
    }
  }
  return mem_map["MemTotal"] - mem_map["MemFree"];
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  string line;
  long uptime;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::stringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key;
  vector<string> cpuvalues {};
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  long idletime, systemtime, virtualtime, totaltime;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      string needle("cpu");
      linestream >> key;
      if (key.find(needle) != string::npos) {
        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
        cpuvalues.emplace_back(user);
        cpuvalues.emplace_back(nice);
        cpuvalues.emplace_back(system);
        cpuvalues.emplace_back(idle);
        cpuvalues.emplace_back(iowait);
        cpuvalues.emplace_back(irq);
        cpuvalues.emplace_back(softirq);
        cpuvalues.emplace_back(steal);
        cpuvalues.emplace_back(guest);
        cpuvalues.emplace_back(guest_nice);
        return cpuvalues;
      }
    }
  }
  return {};
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key;
  int value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") return value;
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key;
  int value;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::stringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") return value;
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }


int main() {
  vector<int> test = LinuxParser::Pids();
  for (auto val: test) {
    std::cout << val << "\n";
  }
}