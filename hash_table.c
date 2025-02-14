#include "hash_table.h"
#include <stdio.h>

// Start the hash table with all the pointers set to NULL
Node *table[TABLE_SIZE] = {NULL};

// Hash function
unsigned int hash(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash *31) + *key++;
    }
    return hash % TABLE_SIZE;
}

// Insert a key-value pair into the hash table
void insert(const char *key, const char *value) {
    unsigned int index = hash(key);
    Node *new_node = (Node *)malloc(sizeof(Node));

    if (!new_node) {
        perror("It was not possible to allocate memory");
        return;
    }
    new_node->key = strdup(key); 
    new_node->value = strdup(value); 
    new_node->next = table[index];
    table[index] = new_node;//
}

// Search for a key in the hash table
char *search(const char *key) {
    unsigned int index = hash(key);
    Node *current = table[index];

    while (current) {
        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return NULL; // Key not found
}

// Remove a key from the hash table
void remove_key(const char *key) {
    unsigned int index = hash(key);
    Node *current = table[index];
    Node *previous = NULL;

    while (current) {
        if (strcmp(current->key, key) == 0) {
            if (previous) {
                previous->next = current->next;
            } else {
                table[index] = current->next;
            }
            free(current->key);
            free(current->value);
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

// Liberate the memory used by the hash table
void free_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Node *current = table[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
        table[i] = NULL;
    }
}