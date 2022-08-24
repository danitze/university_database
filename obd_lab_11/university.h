#ifndef university_h
#define university_h
#include <stdbool.h>

typedef struct {
    char name[100];
    bool deleted;
    int slaveId;
} University;

University* createUniversity(char name[100]);
void printUniversity(University* university);

#endif
