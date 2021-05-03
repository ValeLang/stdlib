#include "native/list.h"
#include<string.h>
#include<stdio.h>
char* copy_into(const char* msg) {
    char* retval = malloc(strlen(msg) + 1);
    strcpy(retval, msg);
    return retval;
}

int main() {
    vale_queue* q = vale_queue_empty();
    char* message = copy_into("hello world");
    vale_queue_push(q, message);
    char* second = copy_into("this is another message");
    vale_queue_push(q, second);
    char* third = copy_into("third message to enqueue");
    char* fourth = copy_into("how many more should I do");
    vale_queue_push(q, third);
    vale_queue_push(q, fourth);
    vale_basic_array* arr = vale_queue_to_array(q);
    for(long i = 0; i < arr->length; ++i) {
	    printf("%s, i: %d\n", arr->elements[i], i);
        free(arr->elements[i]); 
    }
    vale_queue_destroy(q);
    free(arr);
}
