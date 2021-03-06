#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line,key,value;
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
  string os, kernel, version,line;
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
  // Total memory - used memory  / Total menory
  string line , key ;
  float total , free ;
  std::ifstream stream (kProcDirectory + kMeminfoFilename );
  if( stream.is_open()){
    while ( std::getline( stream , line)){    
	std::istringstream linestream(line);
      linestream >>key;
      if(key == "MemoTotal:"){
          linestream >> total;
      	}else 
        if(key == "MemAvaliable:"){
        	linestream >> free;
          break;
        }
      }
   }
   return  ( total - free ) / total; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string token,line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream stream(line);
    if (stream >> token) {
      // Parse token as an int.
      return stoi(token);
    }
  }
  // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  stream.close();
  return 0; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return UpTime() * sysconf(_SC_CLK_TCK); 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, token;
  vector<string> values;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> token) {
      values.emplace_back(token);
    }
  }
    long jiffies{0};
  if (values.size() > 21) {
    long user = stol(values[13]);
    long kernel = stol(values[14]);
    long children_user = stol(values[15]);
    long children_kernel = stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }
  // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  stream.close();
  return jiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> time = CpuUtilization();
  return (stol(time[CPUStates::kUser_]) + stol(time[CPUStates::kNice_]) +stol(time[CPUStates::kSystem_]) + stol(time[CPUStates::kIRQ_]) + stol(time[CPUStates::kSoftIRQ_]) + stol(time[CPUStates::kSteal_]) +
          stol(time[CPUStates::kGuest_]) + stol(time[CPUStates::kGuestNice_])); 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> time = CpuUtilization();
  return (stol(time[CPUStates::kIdle_]) + stol(time[CPUStates::kIOwait_])); 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line , cpu , value ;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
  	std::getline(stream , line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while ( linestream >> value){
    	jiffies.push_back(value);
    }
  }
  // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  stream.close();
  return jiffies;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
 int processes;
  string key , line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()){
    while (std::getline(stream,line)){
    	std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes"){
      	linestream >> processes;
        break;
      }
    }
  }
  // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  stream.close();
  return processes; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  int Rprocesses;
  string key , line;
  std::ifstream stream(kProcDirectory+kStatFilename);
  if (stream.is_open()){
  	while(std::getline(stream,line)){
    	std::istringstream linestream(line);
      linestream >> key;
      if(key == "procs_running"){
      linestream >> Rprocesses;
        break;
      }
    }
  }
  // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  stream.close();
  return Rprocesses; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string command;
  std::ifstream stream(kProcDirectory + std::to_string(pid)+kCmdlineFilename);
  if(stream.is_open()){
  	std::getline(stream,command);
  }
  // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  stream.close();
  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string token;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> token) {
      if (token == "VmSize:") {
        if (stream >> token) return std::to_string(stoi(token) / 1024);
      }
    }
  }
        // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  		stream.close();
  return string("0"); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
    string token;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (stream >> token) {
      if (token == "Uid:") {
        if (stream >> token) return token;
      }
    }
  }
  return string(""); 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
    std::ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) {
    string line;
    string token = "x:" + Uid(pid);
    while (std::getline(stream, line)) {
      auto marker = line.find(token);
      if (marker != string::npos) {
        return line.substr(0, marker - 1);
      }
    }
  }
  return "0"; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
    string token;
  long int time{0};
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    for (int i = 0; stream >> token; ++i)
      if (i == 22) {
        long int time{stol(token)};
        time /= sysconf(_SC_CLK_TCK);
      }
  }
   // https://www.quora.com/Why-we-must-close-every-opened-file-in-C++
  stream.close();
   return   UpTime() - time;
}
