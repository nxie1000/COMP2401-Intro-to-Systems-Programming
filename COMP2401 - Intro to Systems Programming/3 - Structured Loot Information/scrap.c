#include <stdio.h>
#include <string.h>
#include "scrap.h"

int init_scrap_type(struct ScrapType* type,const char* name,float base_value){
    if (type==NULL || name==NULL){//making sure type and name are not null
        return ERR_NULL_POINTER;
    }
    strncpy(type->name,name,MAX_STR-1);//use stringcpy to copy the name
    type->name[MAX_STR-1] ='\0';//makes sure about null termination
    type->base_value = base_value; //assigns value to type
    type->discovered_count = 0;//set discovered count to 0 (feature 1 requirement)

    return ERR_OK;
}
int init_scrap(struct Scrap* scrap, struct ScrapType* type,int condition){
    if (scrap==NULL || type==NULL){ //making sure scrap and type are not null
        return ERR_NULL_POINTER;
    }
    scrap->type = type;//assign pointer to scraptype
    scrap->condition = condition;//assigning condition to scrap
    type->discovered_count++; //increase the discovered count for type by 1
    return ERR_OK;
}
int get_value(const struct Scrap* scrap,float* value){
    if (scrap==NULL || scrap->type==NULL || value==NULL){
        return ERR_NULL_POINTER;//return if either are null
    }
    //using scarcity & value formulas in assignment specification
    float scarcity = (float)(MAX_SCRAP- scrap->type->discovered_count)/MAX_SCRAP;
    *value = ((float)scrap->condition /MAX_CONDITION)*(scrap->type->base_value *scarcity);
    
    return ERR_OK;
}
int print_scrap(const struct Scrap* scrap){
    if (scrap==NULL || scrap->type==NULL){
        return ERR_NULL_POINTER; //return null pointer error if scrap or type is null
    }
    float value = 0.0f;
    int err = get_value(scrap,&value);//call get_value
    if (err!=ERR_OK){
        return err; 
    }
    const char* condition_str;//string holds condition's value
    switch (scrap->condition){//instead of always repeating if/else statements
        case CONDITION_USELESS: 
            condition_str ="Useless";
            break;
        case CONDITION_POOR:
            condition_str ="Poor";
            break;
        case CONDITION_TYPICAL:
            condition_str ="Typical";
            break;
        case CONDITION_PRISTINE:
            condition_str ="Pristine";
            break;
        default:
            condition_str ="Unknown";
            break;
    }
    printf("%-25s | %-25s | %12.2f | %-10d\n",scrap->type->name,condition_str,value,scrap->type->discovered_count);
    //2 decimal places for value

    return ERR_OK;
}//OK!
int init_scrap_collection(struct ScrapCollection* collection){
    if (collection==NULL){
        return ERR_NULL_POINTER;//check if collection is NULL or not
    }
    collection->size = 0;//initializing size  to 0
    return ERR_OK;
}//OK!
int compare_scrap(const struct Scrap* a,const struct Scrap* b){
    if (a==NULL || b==NULL || a->type==NULL || b->type==NULL){
        return ERR_NULL_POINTER;
    }
    int name_from_cmp = strcmp(a->type->name,b->type->name);//use string copy function
    if (name_from_cmp!=0){
        return name_from_cmp;//sort name (alphabetical & ascending)
    } else{
        // Same name, compare condition descending (higher condition value comes first)
        return b->condition - a->condition;//positive if b.condition>a.condition,negative if a.condition>b.condition,and 0 if both are equal
    }
}//OK!
int add_scrap(struct ScrapCollection* collection,const struct Scrap scrap){
    if (collection==NULL){
        return ERR_NULL_POINTER;//null pointer error
    }
    if (collection->size >=MAX_SCRAP){
        return ERR_MAX_CAPACITY;//max capacity reached or exceeded
    }
    int i = 0;
    while (i< collection->size){//while i< size of collection, compare new scrap with current elemnt
        int cmp_result = compare_scrap(&scrap,&collection->elements[i]);//call compare scrap
        if (cmp_result==ERR_NULL_POINTER){
            return ERR_NULL_POINTER;//null pointer error
        }
        if (cmp_result>0){
            i++;//if current elemnt greater by comparison than new scrap, i+=1
        } else{
            break;
        }
    }
    for (int j= collection->size; j>i; --j){
        collection->elements[j] = collection->elements[j-1];//shift element by 1
    }
    collection->elements[i] = scrap;//new scrap at index i
    collection->size++;//size increases when new scrap is added

    return ERR_OK;
}//OK!
int print_scrap_collection(const struct ScrapCollection* collection){
    if (collection==NULL){//ensure collection isn't null, or else return ERR_NULL_POINTER
        return ERR_NULL_POINTER;
    }
    //format to make it look nice
    printf("%-25s | %-25s | %-12s | %-10s\n","Scrap Type", "Condition", "Value", "Discovered");

    printf("-------------------------------------------------------------------------------------------\n");
    for (int i=0; i<collection->size; ++i){//print scraps in collection
        print_scrap(&collection->elements[i]);//call print_scrap for each element in collection
    }
    printf("-------------------------------------------------------------------------------------------\n");
    return ERR_OK;
}//OK!
