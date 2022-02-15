#include <vector>

#include "processor.h"
#include "linux_parser.h"

using LinuxParser::ActiveJiffies;
using LinuxParser::Jiffies;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  long total = Jiffies();
  long active = ActiveJiffies();
  float CPU_Percentage = active * 1.0 / total;
  return CPU_Percentage; 
}