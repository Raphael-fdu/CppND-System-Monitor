#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

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
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  std::string key;
  long value;
  long memTotal;
  long memFree;
  float memUtil;
  
  string line;
  std::ifstream stream( kProcDirectory + kMeminfoFilename );
  if (stream.is_open())
  {
    while(std::getline(stream,line)){
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if ( key == "MemTotal:")
      	memTotal = value;
      else if (key == "MemFree:")
        memFree = value;
   	}
  }
  memUtil = (memTotal - memFree) * 1.0 / memTotal; //memUtil is a percentate number.
  return memUtil; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long time;
  std::ifstream fileStream( kProcDirectory + kUptimeFilename );
  std::string line;
  std::getline(fileStream,line);
  std::istringstream lineStream(line);
  lineStream >> time;
  return time; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long totalJiffies = 0;
  // Retrieve CpuUtilization
  vector<string> CpuUtil = CpuUtilization();
  for (auto i:CpuUtil){
  	totalJiffies += stof(i);
  }
  return totalJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  std::string temp;
  std::string line;
  long stime;
  long utime;
  long cutime;
  long cstime;
  
  std::vector<string> pid_stat;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  std::getline(stream, line);
  std::istringstream lineStream(line);

  while(lineStream >> temp){
  	pid_stat.push_back(temp);
  }
  
  utime = stof(pid_stat[13]);
  stime = stof(pid_stat[14]);
  cutime = stof(pid_stat[15]);
  cstime = stof(pid_stat[16]);
  
  return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
	vector<string> CpuUtil = CpuUtilization();
  	long totalJiffies = Jiffies();
  	long activeJiffies = totalJiffies - stof(CpuUtil[CPUStates::kIdle_]) - stof(CpuUtil[CPUStates::kIOwait_]);
  	return activeJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long totalJiffies = Jiffies();
  long activeJiffies = ActiveJiffies();
  long idleJiffies = totalJiffies - activeJiffies;
  return idleJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_states(10);
  std::string line;
  std::string _;
  std::ifstream fileStream( kProcDirectory + kStatFilename );
  std::getline(fileStream, line);
  std::istringstream lineStream(line);
  lineStream >> _ >> cpu_states[CPUStates::kUser_];
  lineStream >> cpu_states[CPUStates::kNice_];
  lineStream >> cpu_states[CPUStates::kSystem_];
  lineStream >> cpu_states[CPUStates::kIdle_];
  lineStream >> cpu_states[kIOwait_];
  lineStream >> cpu_states[kIRQ_];
  lineStream >> cpu_states[kSoftIRQ_];
  lineStream >> cpu_states[kSteal_];
  lineStream >> cpu_states[kGuest_];
  lineStream >> cpu_states[kGuestNice_];
  return cpu_states;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stof(value);
        }
      }
    }
  }
  return stof(value);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stof(value);
        }
      }
    }
  }
  return stof(value);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::string command;
  std::ifstream fileStream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  std::getline(fileStream, command);
  return command; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
  return string(); 

}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string uid = Uid(pid);
  //parse /etc/passwd, replace : with space, convert line to sstream and do the >> thing
  std::string line;
  std::ifstream fileStream(kPasswordPath);
  std::string userName;
  std::string x;
  std::string uid_;
  while(std::getline(fileStream, line)){
	std::replace(line.begin(), line.end(), ':', ' ');
    std::istringstream lineStream(line);
    while( lineStream >> userName >> x >> uid_ ){
    	if (uid_ == uid){
        	return userName;
        }
    }
  }
  return userName; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  	long uptime;
	std::string line;
  	std::string temp;
  	std::ifstream fileStream(kProcDirectory + std::to_string(pid) + kStatFilename);
  	std::getline(fileStream, line);
  	std::istringstream lineStream(line);
  	for (auto i = 0; i < 21; i++) { lineStream >> temp; }
  	lineStream >> uptime;
  
  	return uptime; 
}
