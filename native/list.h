#ifndef VALE_STDLIB_QUEUE
#define VALE_STDLIB_QUEUE
#include<stdlib.h>

typedef struct vale_basic_array{
    long length;
    void* elements[0];
} vale_basic_array;

typedef struct vale_node_q {
    void* data;
    struct vale_node_q* next;
} vale_node_q;

typedef struct vale_queue {
    long length;
    vale_node_q* head;
} vale_queue;

vale_queue* vale_queue_empty() {
    vale_queue* ret = malloc(sizeof(vale_queue));
    ret->head = NULL;
    ret->length = 0;
    return ret;
}

void vale_queue_push(vale_queue* q, void* data) {
    if(!q->head) {
        q->head = malloc(sizeof(vale_node_q)); 
        q->head->next = NULL;
    }else{
        vale_node_q* temp = q->head;
        q->head = malloc(sizeof(vale_node_q));
        q->head->next = temp;
    }
    q->head->data = data;
    q->length++; 
}

void* vale_queue_pop(vale_queue* q) {
    if(!q->head) { return NULL; }
    vale_node_q* temp = q->head->next;
    void* retval = q->head->data;
    //free(q->head);   
    q->head = temp;
    q->length--;
    return retval; 
}

vale_basic_array* vale_queue_to_array(vale_queue* q) {
    vale_basic_array* array = malloc(sizeof(long) + q->length + 1);
    for(unsigned long i = 0; i < q->length; i++){
        array->elements[i] = vale_queue_pop(q);
    }
    return array; 
}

void vale_queue_destroy(vale_queue* q) {
    while(vale_queue_pop(q) != NULL) {}
    free(q);
}

#endif
