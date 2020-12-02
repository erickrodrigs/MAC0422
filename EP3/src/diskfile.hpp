#ifndef DISK_FILE_H

#include <iostream>
#include <vector>
#include "constants.hpp"
using namespace std;

class DiskFile {
  protected:
    FILE *disk;
    vector<int> FAT;
    vector<int> bitMap;

  public:
    void mount(string name);
    void unmount();
    bool isMounted();
    void openBITMAP();
    void openFAT();
    void saveBITMAP();
    void saveFAT();
    void createFileSystem(string path);
    int searchFreeBlock();
    void freeBlocks(int currentBlock);
    bool findFile(string file, char type, int & parentSizePosition);
    bool findDirectory(vector<string> directories, int & parentSizePosition);
    vector<string> parse(string path);
    void debug(); // lembrar de remover
};

#endif
