#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <cmath>
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
  long value;
  std::unordered_map<string, long> mem_map;
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
  float mem_util = (mem_map["MemTotal"] - mem_map["MemFree"]) / (float)mem_map["MemTotal"];
//  std::cout << "mem_util = " << mem_util << " " << mem_util * 100 <<'\n';
//  std::cout << mem_map["MemTotal"] << " " << mem_map["MemFree"] << " " << (float)(mem_map["MemTotal"] - mem_map["MemFree"]) / mem_map["MemTotal"];
  return mem_util * 100;
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
long LinuxParser::Jiffies() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key;
  long user, nice, system, idle, iowait, irq, softirq, steal;
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string needle("cpu");
      linestream >> key;
      // only use the aggregate value of all the cpus
      if (key.find(needle) != string::npos) {
        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        return user + nice + system + idle + iowait + irq + softirq + steal;
      }
    }
  }
  return 0;
}

void LinuxParser::validateProcess(string process_path) {
  fs::path pid_dir {process_path };
  if (!fs::is_directory(fs::status(pid_dir))) {
    throw std::invalid_argument("Pid does not correspond to a valid process");
  }
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string pid_stat_path= kProcDirectory + std::to_string(pid);
  // confirm that the pid passed is valid
  validateProcess(pid_stat_path);

  std::ifstream filestream (pid_stat_path + kStatFilename );
  string line, comm;
  char state;
  int _, ppid, pgrp, session, tty_nr, tpgid;
  unsigned int flags;
  unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
  long cutime, cstime;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> _ >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime;
      return utime + stime + cutime + cstime;
    }
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto cpu_data = LinuxParser::CpuUtilization();
  return std::stol(cpu_data[LinuxParser::CPUStates::kUser_]) + std::stol(cpu_data[LinuxParser::CPUStates::kNice_]) + std::stol(cpu_data[LinuxParser::CPUStates::kSystem_]) + std::stol(cpu_data[LinuxParser::CPUStates::kIRQ_]) + std::stol(cpu_data[LinuxParser::CPUStates::kSoftIRQ_]) + std::stol(cpu_data[LinuxParser::CPUStates::kSteal_]);
//  std::ifstream filestream(kProcDirectory + kStatFilename);
//  string line, key;
//  long user, nice, system, idle, iowait, irq, softirq, steal;
//  if (filestream.is_open()) {
//    while(std::getline(filestream, line)) {
//      std::istringstream linestream(line);
//      string needle("cpu");
//      linestream >> key;
//      // only use the aggregate value of all the cpus
//      if (key.find(needle) != string::npos) {
//        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
//        return user + nice + system + irq + softirq + steal;
//      }
//    }
//  }
//  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto cpu_data = LinuxParser::CpuUtilization();
  return std::stol(cpu_data[LinuxParser::CPUStates::kIdle_]) + std::stol(cpu_data[LinuxParser::CPUStates::kIOwait_]);
//  std::ifstream filestream(kProcDirectory + kStatFilename);
//  string line, key;
//  long user, nice, system, idle, iowait, irq, softirq, steal;
//  if (filestream.is_open()) {
//    while(std::getline(filestream, line)) {
//      std::istringstream linestream(line);
//      string needle("cpu");
//      linestream >> key;
//      // only use the aggregate value of all the cpus
//      if (key.find(needle) != string::npos) {
//        linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
//        return idle + iowait;
//      }
//    }
//  }
//  return 0;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  string line, key;
  vector<string> cpuvalues {};
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
//  long idletime, systemtime, virtualtime, totaltime;
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
string LinuxParser::Command(int pid) {
  string pid_stat_path= kProcDirectory + std::to_string(pid);
  // confirm that the pid passed is valid
  validateProcess(pid_stat_path);

  std::ifstream filestream{pid_stat_path + kCmdlineFilename };
  string line;
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;

}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string pid_stat_path= kProcDirectory + std::to_string(pid);
  // confirm that the pid passed is valid
  validateProcess(pid_stat_path);

  std::ifstream filestream { pid_stat_path + kStatusFilename };
  string line, key, unit;
  long utilization;
  string utilization_mb;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream {line};
      linestream >> key >> utilization >> unit;

      if (key == "VmSize") {
        double u_mb = (utilization / 1000.0); // convert to megabyte
        double rounded = std::round(u_mb * 10.0) / 10.0; // round to 1 decimal place
        utilization_mb = to_string(rounded);
        utilization_mb = utilization_mb.substr(0, utilization_mb.find('.') + 2);
        return utilization_mb;
      }
    }
  }
  return "";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string pid_stat_path= kProcDirectory + std::to_string(pid);
  // confirm that the pid passed is valid
  validateProcess(pid_stat_path);

  std::ifstream filestream { pid_stat_path + kStatusFilename };
  string line, key, user;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream {line};
      linestream >> key >> user;

      if (key == "Uid") return user;
    }
  }
  return "";
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string user_id = Uid(pid);

  std::ifstream filestream {  kPasswordPath };
  string line, name, x, uid;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream {line};
      linestream >> name >> x >> uid;

      if (uid == user_id) return name;
    }
  }
  return "";
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string pid_stat_path= kProcDirectory + std::to_string(pid);
  // confirm that the pid passed is valid
  validateProcess(pid_stat_path);

  std::ifstream filestream (pid_stat_path + kStatFilename );
  string line, comm;
  char state;
  int _, ppid, pgrp, session, tty_nr, tpgid;
  unsigned int flags;
  unsigned long minflt, cminflt, majflt, cmajflt, utime, stime;
  long cutime, cstime, priority, nice, num_threads, itrealvalue, starttime;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> _ >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >> stime >> cutime >> cstime >> priority >> nice >> num_threads >> itrealvalue >> starttime;
      return starttime;
    }
  }
  return 0;
}
//
//int main() {
//  LinuxParser::MemoryUtilization();
//}