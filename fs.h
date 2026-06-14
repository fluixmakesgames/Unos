#ifndef FS_H
#define FS_H

#define MAX_FILE_SIZE 246

typedef struct {
  char name[MAX_SIZE];
  uint8_t size;
  char data[MAX_FILE_SIZE];
}File;
#endif
