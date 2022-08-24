#ifndef faculty_h
#define faculty_h
#include <stdbool.h>

typedef struct {
    char name[100];
    char address[150];
    int uni_id;
    bool deleted;
    int nextId;
} Faculty;

Faculty* createFaculty(char name[100], char address[150], int uni_id);
void printFaculty(Faculty* faculty);

#endif
