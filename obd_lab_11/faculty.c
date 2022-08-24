#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "faculty.h"

Faculty* createFaculty(char name[100], char address[150], int uni_id) {
    Faculty* faculty = malloc(sizeof(Faculty));
    strcpy(faculty->name, name);
    strcpy(faculty->address, address);
    faculty->uni_id = uni_id;
    faculty->deleted = false;
    faculty->nextId = INT_MAX;
    return faculty;
}

void printFaculty(Faculty* faculty) {
    if(faculty == NULL) {
        printf("No data :(\n");
        return;
    }
    printf("Name: %s; Address: %s; University ID: %d\n", faculty->name, faculty->address, faculty->uni_id);
}
