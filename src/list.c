#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "list.h"

// Init list structure
int init_list(int_ll_t *list)
{
    list->size = 0;
    list->head = (struct node *)malloc(sizeof(struct node));
    list->head->index = -1;

    list->head->next = NULL;    
    pthread_mutex_init(&list->locker,NULL);
    return 0;
}

// Free list structure
int free_list(int_ll_t *list)
{    
    pthread_mutex_lock(&list->locker);
    struct node *previous = list->head->next;
    int curr_index = previous->index;

    while(curr_index < list->size)
    {
      struct node *old = previous;
      previous = previous->next;
      free(old);
      curr_index ++;
    }  

    free(list->head);
    free(list);
    pthread_mutex_unlock(&list->locker);
    return 0;
}

// Get list size
int size_list(int_ll_t *list)
{
    return list->size;
}

int set_index_in_range(int_ll_t *list, int index){
    if(list->size < index)
    {
        return list->size - 1;
    }else if(index <0){
        return 0;
    }
    return index;
}
// Get element at index
int index_list(int_ll_t *list, int index, int *out_value)
{
    pthread_mutex_lock(&list->locker);
    // if the linked list is empty
    if(list->size == 0)
    {
        pthread_mutex_unlock(&list->locker);
        return 1;
    }
    int new_index = set_index_in_range(list,index);
    

    // find the element
    struct node *previous = list->head;    
    int curr_index = previous->index;
    while (curr_index < list->size)
    {
        if(curr_index == new_index)
        {
            // set the value of the element at curr_index
            *out_value = previous->value;
            pthread_mutex_unlock(&list->locker);
            return 0;
        }
        previous = previous->next;
        curr_index ++;
    }
    pthread_mutex_unlock(&list->locker);    
    return 1;    
}

void insert_at_index(int_ll_t *list, int pos, int value)
{
   struct node *new_elt = (struct node *)malloc(sizeof(struct node));
    new_elt->next = list->head;
    list->head = new_elt;
    list->size ++;

    int curr_index = 0;  
    struct node *current = list->head; 
    while(curr_index < list->size)
    {
        if(curr_index == pos)
        {
            current->value = value;            
        }else if(curr_index < pos){
            current->value = current->next->value;
        }
        current->index = curr_index;
        current = current->next;
        curr_index ++;
    }
}
// Insert element at index
int insert_list(int_ll_t *list, int index, int value)
{
    pthread_mutex_lock(&list->locker);
    // if the linked list is empty
    if(list->size == 0){
        list->head->index = 0;
        list->head->value = value;
        list->size ++;
        pthread_mutex_unlock(&list->locker);
        return 0;
    }

    if (index < 0)
    {
        index = 0;
    }
    if (index > list->size)
    {
        index = list->size;
    }
    
    // int new_index = set_index_in_range(list,index);
    insert_at_index(list,index,value);
    pthread_mutex_unlock(&list->locker);
    return 0;
}

void remove_at_index(int_ll_t *list, int pos, int *out_value)
{
    // remove the first element in the linked list
    // if the linked list size is 1 
    // else if the linked size is greater than 1
    if(list->size == 1 && pos == 0)
    {
        *out_value = list->head->value; 
        list->head->value = -1;       
        list->size = 0;
        return;
    }else if(list->size > 1 && pos == 0)
    {
        struct node *removed = list->head;
        *out_value = removed->value;
        list->head = list->head->next;        
        
        free(removed);
        list->size --;
        // update index values in the linked list       
        struct node *current = list->head;    
        
        for (int i = 0; i < list->size; i++)
        {
            current->index = i;
            current = current->next;            
        }       
        return;
    }
    // search for the previous node in the linked list to pos
    struct node *current = list->head;     
       
    while (current->index < pos-1)
    {     
        current = current->next;    
    }    

    *out_value = current->next->value;
    struct node *removed = current->next;            
    current->next = current->next->next; 
    free(removed); 
    list->size --; 
    // if the removed element is not the last element in the linked list
    // update the index of the next reference   
    for (int i = current->index; i < list->size; i++)
    {
        current->index = i;
        current = current->next;
    }
}
// Remove element at index
int remove_list(int_ll_t *list, int index, int *out_value)
{
    pthread_mutex_lock(&list->locker);
    // if the linked list is empty
    if(list->size == 0)
    {
        pthread_mutex_unlock(&list->locker);
        return 1;
    }
    
    if (index < 0)
    {
        index = 0;
    }
    if (index >= list->size)
    {
        index = list->size-1;
    }
    
    
    remove_at_index(list,index,out_value);
    
    pthread_mutex_unlock(&list->locker);
    return 0;
}
