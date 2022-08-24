#include <stdio.h>

#include "file_worker.h"
#include "university.h"
#include "faculty.h"
#include <stdlib.h>
#include <limits.h>
#include "keys.h"

#define INPUT_MAX_LEN 150

const char universityTableName[] = "university";
const char facultyTableName[] = "faculty";

int* inputId(void) {
    int* id = malloc(sizeof(int));
    printf("%s: ", "ID");
    scanf("%d", id);
    fflush(stdin);
    return id;
}

University* inputUniversity(void) {
    char name[INPUT_MAX_LEN];
    printf("%s: ", "Name");
    gets(name);
    University* university = createUniversity(name);
    return university;
}

Faculty* inputFaculty(void) {
    char name[INPUT_MAX_LEN];
    char address[INPUT_MAX_LEN];
    int uni_id;
    printf("%s: ", "Name");
    gets(name);
    printf("%s: ", "Address");
    gets(address);
    printf("%s: ", "University id");
    scanf("%d", &uni_id);
    Faculty* faculty = createFaculty(name, address, uni_id);
    fflush(stdin);
    return faculty;
}

void insert_m(University* item, int id, Array* masterKeys, Array* masterEmptyPositions) {
    insert(item, universityTableName, id, masterKeys, masterEmptyPositions);
}

void insert_s(Faculty* item, int id, Array* masterKeys, Array* slaveKeys, Array* slaveEmptyPositions) {
    if(binarySearch(slaveKeys, &id) != -1 || binarySearch(masterKeys, &(item->uni_id)) == -1) {
        return;
    }
    insert(item, facultyTableName, id, slaveKeys, slaveEmptyPositions);
    University* university = (University*) getItem(universityTableName, item->uni_id, masterKeys);
    int index = binarySearch(slaveKeys, &id);
    Keys* keys = slaveKeys->arr[index];
    int position = keys->position;
    if(university->slaveId == INT_MAX) {
        university->slaveId = id;
        replace(university, universityTableName, item->uni_id, masterKeys);
        return;
    }
    int currentId = university->slaveId;
    free(university);
    Faculty* faculty = (Faculty*) getItem(facultyTableName, currentId, slaveKeys);
    index = binarySearch(slaveKeys, &(faculty->nextId));
    Keys* nextKeys = slaveKeys->arr[index];
    while (index != -1 && nextKeys->position < position) {
        currentId = nextKeys->id;
        Faculty* temp = faculty;
        faculty = (Faculty*) getItem(facultyTableName, currentId, slaveKeys);
        free(temp);
        index = binarySearch(slaveKeys, &(faculty->nextId));
        nextKeys = slaveKeys->arr[index];
    }
    item->nextId = faculty->nextId;
    faculty->nextId = id;
    replace(faculty, facultyTableName, currentId, slaveKeys);
    replace(item, facultyTableName, id, slaveKeys);
    free(faculty);
}

int count_m(Array* masterKeys) {
    return masterKeys->actualSize;
}

int count_s(Array* slaveKeys) {
    return slaveKeys->actualSize;
}

int count_m_s(int id, Array* masterKeys, Array* slaveKeys) {
    University* university = getItem(universityTableName, id, masterKeys);
    if(university == NULL || university->slaveId == INT_MAX) {
        free(university);
        return 0;
    }
    Faculty* faculty = getItem(facultyTableName, university->slaveId, slaveKeys);
    free(university);
    int counter = 1;
    while (faculty->nextId != INT_MAX) {
        Faculty* temp = faculty;
        faculty = getItem(facultyTableName, faculty->nextId, slaveKeys);
        free(temp);
        ++counter;
    }
    return counter;
}

University* get_m(int id, Array* masterKeys) {
    University* university = getItem(universityTableName, id, masterKeys);
    return university;
}

Faculty* get_s(int id, Array* slaveKeys) {
    Faculty* faculty = getItem(facultyTableName, id, slaveKeys);
    return faculty;
}

Array* get_m_s(int id, Array* masterKeys, Array* slaveKeys) {
    University* university = getItem(universityTableName, id, masterKeys);
    if(university == NULL) {
        return NULL;
    }
    int count = count_m_s(id, masterKeys, slaveKeys);
    if(count == 0) {
        return NULL;
    }
    Array* faculties = createArray(facultyTableName, count, sizeof(Faculty));
    Faculty* faculty = getItem(facultyTableName, university->slaveId, slaveKeys);
    free(university);
    arrayInsert(faculties, faculty);
    for(int i = 1; i < count; ++i) {
        faculty = getItem(facultyTableName, faculty->nextId, slaveKeys);
        arrayInsert(faculties, faculty);
    }
    return faculties;
}

void del_m(int id, Array* masterKeys, Array* masterEmptyPositions, Array* slaveKeys, Array* slaveEmptyPositions) {
    University* university = getItem(universityTableName, id, masterKeys);
    if(university == NULL) {
        return;
    }
    removeRecording(universityTableName, id, masterKeys, masterEmptyPositions);
    if(university->slaveId == INT_MAX) {
        return;
    }
    int currId = university->slaveId;
    free(university);
    Faculty* faculty = getItem(facultyTableName, currId, slaveKeys);
    do {
        removeRecording(facultyTableName, currId, slaveKeys, slaveEmptyPositions);
        Faculty* temp = faculty;
        currId = faculty->nextId;
        faculty = getItem(facultyTableName, currId, slaveKeys);
        free(temp);
    } while(faculty != NULL);
    if(masterEmptyPositions->actualSize > 20) {
        resetTable(universityTableName, &masterKeys, &masterEmptyPositions);
    }
}

void del_s(int id, Array* masterKeys, Array* slaveKeys, Array* slaveEmptyPositions) {
    Faculty* faculty = getItem(facultyTableName, id, slaveKeys);
    if(faculty == NULL) {
        return;
    }
    University* university = getItem(universityTableName, faculty->uni_id, masterKeys);
    if(university->slaveId == id) {
        university->slaveId = faculty->nextId;
        replace(university, universityTableName, faculty->uni_id, masterKeys);
        free(university);
        removeRecording(facultyTableName, id, slaveKeys, slaveEmptyPositions);
        return;
    }
    int itemNextId = faculty->nextId;
    free(faculty);
    int currId = university->slaveId;
    free(university);
    faculty = getItem(facultyTableName, currId, slaveKeys);
    while (faculty->nextId != INT_MAX && faculty->nextId != id) {
        Faculty* temp = faculty;
        currId = faculty->nextId;
        faculty = getItem(facultyTableName, currId, slaveKeys);
        free(temp);
    }
    if(faculty->nextId == INT_MAX) {
        return;
    }
    faculty->nextId = itemNextId;
    replace(faculty, facultyTableName, currId, slaveKeys);
    free(faculty);
    removeRecording(facultyTableName, id, slaveKeys, slaveEmptyPositions);
    if(slaveEmptyPositions->actualSize > 20) {
        resetTable(facultyTableName, &slaveKeys, &slaveEmptyPositions);
    }
}

void update_m(University* newItem, int id, Array* masterKeys) {
    University* item = getItem(universityTableName, id, masterKeys);
    if(item == NULL) {
        return;
    }
    newItem->slaveId = item->slaveId;
    free(item);
    replace(newItem, universityTableName, id, masterKeys);
}

void update_s(Faculty* newItem, int id, Array* masterKeys, Array* slaveKeys, Array* slaveEmptyPositions) {
    Faculty* item = getItem(facultyTableName, id, slaveKeys);
    if(item == NULL) {
        return;
    }
    if(item->uni_id == newItem->uni_id) {
        newItem->nextId = item->nextId;
        free(item);
        replace(newItem, facultyTableName, id, slaveKeys);
        return;
    }
    free(item);
    del_s(id, masterKeys, slaveKeys, slaveEmptyPositions);
    insert_s(newItem, id, masterKeys, slaveKeys, slaveEmptyPositions);
}

void clearAll(Array** masterKeys, Array** masterEmptyPositions, Array** slaveKeys, Array** slaveEmptyPositions) {
    clearTable(universityTableName);
    clearTable(facultyTableName);
    deleteArray(*masterKeys);
    deleteArray(*masterEmptyPositions);
    deleteArray(*slaveKeys);
    deleteArray(*slaveEmptyPositions);
    *masterKeys = createArray(KEYS_TYPE, 0, sizeof(Keys));
    *masterEmptyPositions = createArray(INTEGERS_TYPE, 0, sizeof(int));
    *slaveKeys = createArray(KEYS_TYPE, 0, sizeof(Keys));
    *slaveEmptyPositions = createArray(INTEGERS_TYPE, 0, sizeof(int));
}

int main(int argc, const char * argv[]) {
    Array* masterKeys = getKeysArray(universityTableName);
    Array* masterEmptyPositions = getDeletedPositionsArray(universityTableName);
    Array* slaveKeys = getKeysArray(facultyTableName);
    Array* slaveEmptyPositions = getDeletedPositionsArray(facultyTableName);
    /*------- INSERT CODE HERE --------*/
    
    char input[INPUT_MAX_LEN];
    while (true) {
        printf("%s: ", "Enter command");
        gets(input);
        if(strcmp(input, "insert-m") == 0) {
            University* university = inputUniversity();
            int* id = inputId();
            insert_m(university, *id, masterKeys, masterEmptyPositions);
            free(university);
            free(id);
            printf("Inserted successfully:)\n");
        } else if(strcmp(input, "insert-s") == 0) {
            Faculty* faculty = inputFaculty();
            int* id = inputId();
            insert_s(faculty, *id, masterKeys, slaveKeys, slaveEmptyPositions);
            free(faculty);
            free(id);
            printf("Inserted successfully:)\n");
        } else if(strcmp(input, "count-m") == 0) {
            printf("%d\n", count_m(masterKeys));
        } else if(strcmp(input, "count-s") == 0) {
            printf("%d\n", count_s(slaveKeys));
        } else if(strcmp(input, "count-m-s") == 0) {
            int* id = inputId();
            printf("%d\n", count_m_s(*id, masterKeys, slaveKeys));
            free(id);
        } else if(strcmp(input, "get-m") == 0) {
            int* id = inputId();
            University* university = get_m(*id, masterKeys);
            printUniversity(university);
            free(university);
            free(id);
        } else if(strcmp(input, "get-s") == 0) {
            int* id = inputId();
            Faculty* faculty = get_s(*id, slaveKeys);
            printFaculty(faculty);
            free(faculty);
            free(id);
        } else if(strcmp(input, "get-m-s") == 0) {
            int* id = inputId();
            Array* slaves = get_m_s(*id, masterKeys, slaveKeys);
            printArray(slaves);
            free(slaves);
            free(id);
        } else if(strcmp(input, "del-m") == 0) {
            int* id = inputId();
            del_m(*id, masterKeys, masterEmptyPositions, slaveKeys, slaveEmptyPositions);
            free(id);
            printf("Deleted successfully:)\n");
        } else if(strcmp(input, "del-s") == 0) {
            int* id = inputId();
            del_s(*id, masterKeys, slaveKeys, slaveEmptyPositions);
            free(id);
            printf("Deleted successfully:)\n");
        } else if(strcmp(input, "update-m") == 0) {
            University* university = inputUniversity();
            int* id = inputId();
            update_m(university, *id, masterKeys);
            free(university);
            free(id);
            printf("Updated successfully:)\n");
        } else if(strcmp(input, "update-s") == 0) {
            Faculty* faculty = inputFaculty();
            int* id = inputId();
            update_s(faculty, *id, masterKeys, slaveKeys, slaveEmptyPositions);
            free(faculty);
            free(id);
            printf("Updated successfully:)\n");
        } else if(strcmp(input, "clear") == 0) {
            clearAll(&masterKeys, &masterEmptyPositions, &slaveKeys, &slaveEmptyPositions);
            printf("Cleared successfully:)\n");
        } else if(strcmp(input, "utils-m") == 0) {
            Array* universities = getItems(universityTableName);
            printArray(universities);
            deleteArray(universities);
        } else if(strcmp(input, "utils-s") == 0) {
            Array* faculties = getItems(facultyTableName);
            printArray(faculties);
            deleteArray(faculties);
        } else if(strcmp(input, "stop") == 0) {
            break;
        } else {
            printf("Invalid input! Please, retry.\n");
        }
    }
        
    /*---------------------------------*/
    saveArray(universityTableName, 1, masterKeys);
    saveArray(universityTableName, 2, masterEmptyPositions);
    
    saveArray(facultyTableName, 1, slaveKeys);
    saveArray(facultyTableName, 2, slaveEmptyPositions);

    
    deleteArray(masterKeys);
    deleteArray(masterEmptyPositions);
    deleteArray(slaveKeys);
    deleteArray(slaveEmptyPositions);
    
    return 0;
}

