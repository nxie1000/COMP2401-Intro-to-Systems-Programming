#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "readings.h"

int init_readings(ReadingCollection *collection){
    if (collection==NULL){
        return ERR_NULL_POINTER;//account for null pointer error
    }
    collection->readings = (Reading*)calloc(1,sizeof(Reading));//calloc for initial allocation
    if (collection->readings==NULL){//memory allocation error check
        return ERR_MEMORY_ALLOCATION;
    }
    collection->size = 0;
    collection->capacity = 1;//set initial capacity of collection to 1,initial size to 0 (per assignmnt specs)
    return ERR_OK;
}
int free_readings(ReadingCollection *collection){
    if (collection==NULL){
        return ERR_NULL_POINTER;//null pointer error check
    }
    free(collection->readings);//(assign. specifications) frees readings field from collection and sets capacity to zero
    collection->readings = NULL;
    collection->size = 0;
    collection->capacity = 0;
    return ERR_OK;
}
int init_reading(Reading *reading,int id, int device, float value,const char *room){
    if (reading==NULL || room==NULL){
        return ERR_NULL_POINTER;
    }
    reading->id = id;
    reading->device = device;
    reading->value = value;//populate memory with values of input parameters
    strncpy(reading->room,room,MAX_STR-1);//copy room string to reading->room
    reading->room[MAX_STR-1] ='\0';//ensures null termination

    return ERR_OK;
}
int find_reading(ReadingCollection *collection,int id){
    if (collection==NULL || collection->readings==NULL){
        return ERR_NULL_POINTER; //if either are null, error
    }
    for (int i=0; i<collection->size; i++){
        if (collection->readings[i].id==id){
            return i;//return the index where it found the reading
        }
    }
    return ERR_ID_NOT_FOUND;
}

//THIS IS THE HELPER FUNCTION FOR THE SIDE QUEST
int resize_readings_array(ReadingCollection *collection,int new_capacity){
    if (collection==NULL){
        return ERR_NULL_POINTER;//check for null pointer error
    }
    Reading *new_array = (Reading*)calloc(new_capacity,sizeof(Reading));//use calloc to allocate the new memory for new array
    if (new_array==NULL){//check for memory allocation error
        return ERR_MEMORY_ALLOCATION;
    }
    int elements_to_copy = collection->size < new_capacity ? collection->size : new_capacity;
    //ternary operator instead of if statement to determine how many elements to copy

    for (int i=0; i<elements_to_copy;i++){//iterate through the old array and copy elements to new array
        new_array[i] = collection->readings[i];
    }
    free(collection->readings);//frees old memory
    collection->readings = new_array;//update pointer to new array
    collection->capacity = new_capacity;//update to new capacity for collection
    return ERR_OK;
}
int add_reading(ReadingCollection *collection,const Reading *reading){
    if (collection==NULL || reading==NULL){
        return ERR_NULL_POINTER;//check for null pointer error
    }
    if (find_reading(collection,reading->id) >=0){//if reading with same id already exists, return error
        return ERR_DUPLICATE_ID;
    }
    if (collection->size == collection->capacity){//if size is equal to capacity,we need to resize the array
        int new_capacity = collection->capacity +2;//increase capacity by 2 as per assignment specifications
        int error = resize_readings_array(collection,new_capacity);
        if(error!=ERR_OK){
            return error;
        }
    }
    collection->readings[collection->size] = *reading;//copy the reading to the collection
    collection->size++;//increment size of collection by 1
    return ERR_OK;
}
int remove_reading(ReadingCollection *collection,int id){
    if (collection==NULL){
        return ERR_NULL_POINTER;//check for null pointer error
    }
    int index = find_reading(collection,id);//call find reading and assign it
    if (index<0){
        return index;
    }
    for (int i=index;i<collection->size -1; i++){//iterate through all the collection
        collection->readings[i] = collection->readings[i+1];//shift all elems to left
    }
    collection->size--;//collection size reduced by 1
    if(collection->size >0 && (float)collection->size <=SHRINK_THRESHOLD* collection->capacity){
        //formula given by specification, shrink the array
        int new_capacity = collection->size +1;//new_capacity is for new array
        int error = resize_readings_array(collection,new_capacity); //call the helper function
        if (error!=ERR_OK){
            return error;//error case
        }
    } else if(collection->size ==0 && collection->capacity >0){
        free(collection->readings);//free memory if size is 0
        collection->readings = NULL;//set to NULL if size is 0
        collection->capacity = 0;//capacity is also set to 0
    }
    return ERR_OK;
}
int print_reading(const Reading *reading){
    if (reading==NULL){
        return ERR_NULL_POINTER;
    }//check for null pointer error
    const char* device_type_str;//we print out the device type as string instead of number (same with Prof. Hillen's video)
    switch (reading->device){//all cases
        case DEVICE_EMF:
            device_type_str ="EMF";
            break;
        case DEVICE_THERMAL:
            device_type_str ="Thermal";
            break;
        case DEVICE_SOUND:
            device_type_str ="Sound";
            break;
        //we could add default case for good practice, but not really needed
    }
    printf("ID:%d, Device:%s, Value:%.2f, Room:%s\n",reading->id, device_type_str, reading->value, reading->room);
    return ERR_OK;
}
int print_readings(const ReadingCollection *collection) {
    int i;
    if (collection == NULL || collection->readings == NULL) {
        return ERR_NULL_POINTER;
    }
    printf("Readings:\n");
    for (i = 0; i < collection->size; i++) {
        print_reading(&collection->readings[i]);
    }
    return ERR_OK;//default code, mostly untouched
}
