//
// Created by massi on 19.03.2020.
//

#include <stdlib.h>
#include "tqueue.h"

typedef struct TQueueNode {
    struct TQueueNode* next;
    void* data;
} TQueueNode;

TQueueNode* create_node(void* data) {
    TQueueNode* node = malloc(sizeof(TQueueNode));
    node->next = node;
    node->data = data;
    return node;
}

/* Adds a new element at the end of the list, returns its position */
unsigned long int tqueue_enqueue(TQueue* q, void* data) {
    unsigned long int pos = -1;
    if(q == NULL)
        return pos;

    TQueueNode* head = (*q);
    if(head == NULL) {
        (*q) = create_node(data);
        pos++;
    } else {
        TQueueNode *current= head;
        while(current->next != head) {
            pos++;
            current = current->next;
        }
        current->next = create_node(data);
        current->next->next = head;
        pos++;
    }
    return pos;
}

/* Removes and returns the element at the beginning of the list, NULL if the queue is empty */
void* tqueue_pop(TQueue* q) {
    if(q == NULL)
        return NULL;

    TQueueNode* head = (*q);
    void* data = NULL;
    if(head != NULL) {
        TQueueNode *prev= head;
        TQueueNode *current= prev->next;
        if(prev != current) {
            while(current->next != head) {
                prev = current;
                current = current->next;
            }
            prev->next = head;
        } else {
            *q = NULL;
        }
        data = current->data;
        free(current);
    }
    return data;
}

/* Returns the data on the first node of the given list */
void* tqueue_get_data(TQueue q) {
    if(q == NULL)
        return NULL;

    return q->data;
}

/* Returns the number of elements in the list */
unsigned long int tqueue_size(TQueue q) {
    unsigned long int sz = 0;
    if(q != NULL) {
        sz++;
        TQueueNode *head = q;
        TQueueNode *current = head;
        while (current->next != head) {
            sz++;
            current = current->next;
        }
    }
    return sz;
}

/* Returns a 'view' on the list starting at (a positive) offset distance, * NULL if the queue is empty */
TQueue tqueue_at_offset(TQueue q, unsigned long int offset) {
    if(q == NULL)
        return NULL;

    for (int i = 0; i < offset; ++i) {
        q = q->next;
    }
    return q;
}
