#include "linkedlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "talloc.h" // Add talloc here

// Create a new EMPTY_TYPE value node.
SchemeVal *makeEmpty() {
    SchemeVal *head = talloc(sizeof(SchemeVal));
    head->type = EMPTY_TYPE;
    return head;
}

// Create a new CONS_TYPE value node.
SchemeVal *cons(SchemeVal *newCar, SchemeVal *newCdr) {
    SchemeVal *cons = talloc(sizeof(SchemeVal));
    cons->type = CONS_TYPE;
    cons->car = newCar;
    cons->cdr = newCdr;
    return cons;
}

// Display the contents of the linked list
void display(SchemeVal *list) {
    SchemeVal *current = list;
    while (current->type != EMPTY_TYPE) {
        SchemeVal *currCar = current->car;
        switch (currCar->type) {
            case INT_TYPE:
                printf("%d\n", currCar->i);
                break;
            case DOUBLE_TYPE:
                printf("%f\n", currCar->d);
                break;
            case STR_TYPE:
                printf("%s\n", currCar->s);
                break;
            default:
                printf("Unknown type\n");
                break;
        }
        current = current->cdr;
    }
}

// Return a new list that is the reverse of the one that is passed in
SchemeVal *reverse(SchemeVal *list) {
    assert(list != NULL);
    SchemeVal *reversed = makeEmpty();
    while (list->type != EMPTY_TYPE) {
        reversed = cons(list->car, reversed);
        list = list->cdr;
    }
    return reversed;
}

// car utility
SchemeVal *car(SchemeVal *list) {
    assert(list != NULL);
    assert(list->type == CONS_TYPE);
    return list->car;
}

// cdr utility
SchemeVal *cdr(SchemeVal *list) {
    assert(list != NULL);
    assert(list->type == CONS_TYPE);
    return list->cdr;
}

// isEmpty utility
bool isEmpty(SchemeVal *value) {
    assert(value != NULL);
    return (value->type == EMPTY_TYPE);
}

// length utility
int length(SchemeVal *value) {
    assert(value != NULL);
    int size = 0;
    while (value->type != EMPTY_TYPE) {
        size++;
        value = value->cdr;
    }
    return size;
}
