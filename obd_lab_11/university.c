#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "university.h"

University* createUniversity(char name[100]) {
    University* university = malloc(sizeof(University));
    strcpy(university->name, name);
    university->deleted = false;
    university->slaveId = INT_MAX;
    return university;
}

void printUniversity(University* university) {
    if(university == NULL) {
        printf("No data:(\n");
        return;
    }
    printf("Name: %s\n", university->name);
}
