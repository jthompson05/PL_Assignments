#include "talloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// The active list head (global)
static SchemeVal *activeList = NULL;

// Replacement for malloc that tracks pointers
void *talloc(size_t size) {
    // Allocate user memory
    void *newMemory = malloc(size);
    assert(newMemory != NULL);

    // Create a new SchemeVal node to track it
    SchemeVal *tracker = malloc(sizeof(SchemeVal));
    assert(tracker != NULL);

    tracker->type = PTR_TYPE;
    tracker->ptr = newMemory;

    // Add tracker to the front of the active list
    tracker->cdr = activeList;
    activeList = tracker;

    return newMemory;
}

// Free all memory allocated via talloc
void tfree() {
    SchemeVal *current = activeList;
    while (current != NULL) {
        assert(current->type == PTR_TYPE);

        // Free the memory block being tracked
        free(current->ptr);

        // Move to the next node
        SchemeVal *next = current->cdr;

        // Free the tracker node itself
        free(current);

        current = next;
    }
    activeList = NULL; // Reset active list after freeing
}

// Replacement for exit that frees first
void texit(int status) {
    tfree();
    exit(status);
}
