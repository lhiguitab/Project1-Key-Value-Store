#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 4096

// Define the structure of a node
typedef struct Node {
    char *key;
    char *value;
    struct Node *next;
} Node;

// Define the structure of a hash table
typedef struct HashTable {
    Node *table[TABLE_SIZE];
} HashTable;

// Define the hash function
unsigned int hash(const char *key);

// Insert a key-value pair into the hash table
void insert(const char *key, const char *value);

// Search for a key in the hash table
char *search(const char *key);

// Remove a key from the hash table
void remove_key(const char *key);

// Liberate the memory used by the hash table
void free_table();


#endif