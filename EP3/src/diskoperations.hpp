#ifndef DISK_OPERATIONS_H

#include "diskfile.hpp"

class DiskOperations : public DiskFile {
  public:
    void mkdir(string path);
    int touch(string path);
    void cp(string origin, string destiny);
    void cat(string path);
    void ls(string path);
    void rm(string path);
    void listOfBlocks(string path, int block, vector<int> & blocks, vector<string> & eliminated);
    void searchFiles(string path, string fileName, int block, vector<string> & foundFiles);
    void rmdir(string path);
    void find(string path, string fileName);
    void df();
    
};

#endif
