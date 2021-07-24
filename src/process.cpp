#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;


 Process::Process(int pid){
  Process::pid_=pid;
 }
int Process::Pid() { 
  return pid_;
}

// TODO: Return this process's CPU utilization
//Question to fix CPU 0 to number https://knowledge.udacity.com/questions/288943
float Process::CpuUtilization() { 
    float active = LinuxParser::ActiveJiffies();
    float idle = LinuxParser::IdleJiffies();
    if (active + idle > 0.0)
        return active / (active + idle);
    else
        return 0.0;
}

// TODO: Return the command that generated this process
string Process::Command() { 
  return LinuxParser::Command(pid_);
}

// TODO: Return this process's memory utilization
string Process::Ram() { 
  return LinuxParser::Ram(pid_);
}

// TODO: Return the user (name) that generated this process
string Process::User() { 
  return LinuxParser::User(pid_);
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
  return LinuxParser::UpTime(pid_);
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process& a)  {
  return CpuUtilization() < a.CpuUtilization() ;
}