#ifndef DISK_INFO_H

#include "diskoperations.hpp"

class Disk : public DiskOperations {
  private:
    unsigned long long int files;
    unsigned long long int directories;
    unsigned long long int freeSpace;
    unsigned long long int wastedSpace;
    void fillInfo(int blockAddress, int total);
    void directoryWastedSpace(int numberOfFiles);
    void fileWastedSpace(int sizeOfFile);
  
  public:
    Disk();
    void df();
};

#endif
