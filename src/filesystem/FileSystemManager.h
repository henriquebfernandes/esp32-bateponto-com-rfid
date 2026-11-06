#ifndef FILESYSTEM_MANAGER_H
#define FILESYSTEM_MANAGER_H

#include <FS.h>

bool initFileSystem();
File openFile(const char* path, const char* mode);
bool fileExists(const char* path);
bool renameFile(const char* from, const char* to);
bool removeFile(const char* path);

#endif
