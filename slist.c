
#include "slist.h"
#include <stdio.h>
#include <stdlib.h>
//-----------slist_init-------------
// Initialize a single linked list
void slist_init(slist_t * list)
{
slist_head(list) = NULL;
slist_tail(list) = NULL;
slist_size(list) = 0;

}
//--------------slist_destroy-----------
//Destroy and de-allocate the memory hold by a list
void slist_destroy(slist_t* list, slist_destroy_t t)
{
if (list == NULL)
    return;
slist_node_t* temp;
temp = slist_head(list);
while (slist_head(list) != NULL)
{
    temp = slist_next(temp);
    if (t == SLIST_FREE_DATA)
    {
        free(slist_data(slist_head(list)));
    }
    free(slist_head(list));
    slist_head(list) = temp;
}
slist_size(list) = 0;
list = NULL;
}
//----------slist_pop_first-------------
// Pop the first element in the list
void* slist_pop_first(slist_t* list)
{
if (slist_head(list) == NULL)
    return NULL;
void* data = slist_data(slist_head(list));
slist_node_t* temp = slist_head(list);
slist_head(list) = slist_next(slist_head(list));
slist_size(list)--;
free(temp);
return data;
}
//----------slist_append--------
//Append data to list (add as last node of the list)
int slist_append(slist_t* l, void* dat)
{

    //if(l==NULL) return -1;
slist_node_t* newNode = (slist_node_t*)malloc(sizeof(slist_node_t));
if (newNode == NULL)
    return -1;
slist_data(newNode) = dat;
slist_next(newNode) = NULL;
if (slist_head(l) == NULL)
{
    slist_head(l) = newNode;
    slist_tail(l) = newNode;
    slist_size(l) += 1;
    return 0;
}

slist_next(slist_tail(l)) = newNode;
slist_tail(l) = newNode;
slist_size(l) += 1;
return 0;
}



//-------------slist_prepend----------------
//Prepend data to list (add as first node of the list)
int slist_prepend(slist_t* list, void* data)
{
slist_node_t* newNode = (slist_node_t*)malloc(sizeof(slist_node_t));
if (newNode == NULL)
    return -1;
slist_next(newNode) = slist_head(list);
slist_data(newNode) = data;
slist_head(list) = newNode;
if (slist_tail(list) == NULL) {slist_tail(list) = newNode;}//if the list was empty
slist_size(list)++;
return 0;
}




//--------------slist_append_list-----------------
//Append elements from the second list to the first list, use the slist_append function.
int slist_append_list(slist_t* list1, slist_t* list2)
{
if (list2 == NULL)
    return 0;
slist_node_t* temp = slist_head(list2);
while (temp != NULL)
{
    slist_append(list1, slist_data(temp));
    temp = slist_next(temp);
}
return 0;

}










