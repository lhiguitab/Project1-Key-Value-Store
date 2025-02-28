#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <omp.h>
#include <time.h>

#define MAX_LINE 4096
#define BLOCK_SIZE 4096

typedef struct {
    int review_id;  
    int app_id;
    int user_id;
} Review;

typedef struct Node {
    Review data;
    struct Node* next;
} Node;

typedef struct {
    Node** table;
    int capacity;
} HashTable;

int hashFunction(int key, int capacity) {
    return key % capacity;
}

HashTable* createHashTable(int size) {
    HashTable* ht = (HashTable*)malloc(sizeof(HashTable));
    ht->capacity = size;
    ht->table = (Node**)calloc(size, sizeof(Node*));
    return ht;
}

void insert(HashTable* ht, Review r) {
    int index = hashFunction(r.review_id, ht->capacity);
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = r;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

Review* get(HashTable* ht, int key) {
    int index = hashFunction(key, ht->capacity);
    Node* current = ht->table[index];
    while (current) {
        if (current->data.review_id == key) {
            return &current->data;
        }
        current = current->next;
    }
    return NULL;
}

void freeHashTable(HashTable* ht) {
    for (int i = 0; i < ht->capacity; i++) {
        Node* current = ht->table[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }
    free(ht->table);
    free(ht);
}

void process_block(char *block, size_t size, HashTable *reviews) {
    char *start = block;
    int is_first_line = 1;

    for (size_t i = 0; i < size; i++) {
        if (block[i] == '\n' || i == size - 1) {
            //block[i] = '\0';

            char *token = strtok(start, ",");

            int app_id, user_id, review_id;

            if (token) {
                app_id = atoi(token);
                for (int i = 0; i < 5; i++) token = strtok(NULL, ","); // helpful, funny, date, is_recommended, hours
                token = strtok(NULL, ","); // user_id
                if (token) {
                    user_id = atoi(token);
                    token = strtok(NULL, ","); // review_id
                    if (token) {
                        review_id = atoi(token);

                        Review r = {review_id, app_id, user_id};

                        #pragma omp critical
                        insert(reviews, r);

                        start = &block[i + 1];
                    }
                }
            }


            /*
                char *token = strtok(start, ",");
            if (!token) continue;

            if (is_first_line) { 
                if (strcmp(token, "app_id") == 0) { 
                    start = &block[i + 1]; 
                    is_first_line = 0;
                    continue;
                }
            }

            int app_id = atoi(token);
            for (int j = 0; j < 5; j++) {
                token = strtok(NULL, ",");
                if (!token) break;
            }
            if (!token) continue;

            int user_id = atoi(token);
            token = strtok(NULL, ",");
            if (!token) continue;

            int review_id = atoi(token);
            Review r = {review_id, app_id, user_id};
            */

           /*
            #pragma omp critical
            insert(reviews, r);
           */

            
        }
    }
}


void print_reviews(HashTable *reviews) {
    for (int i = 0; i < reviews->capacity; i++) {
        Node* current = reviews->table[i];
        while (current) {
            printf("Review ID: %d, App ID: %d, User ID: %d\n", 
                   current->data.review_id, current->data.app_id, current->data.user_id);
            current = current->next;
        }
    }
}

int main() {
    HashTable* reviews = createHashTable(41154793);

    omp_set_num_threads(omp_get_max_threads());
    clock_t start_time = clock();

    const char *filename = "./Dataset/recommendations.csv";
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting file size");
        close(fd);
        return 1;   
    }

    size_t filesize = sb.st_size;
    char *file_map = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (file_map == MAP_FAILED) {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    printf("First bytes of mapped file:\n");
    for (size_t i = 0; i < 100 && i < filesize; i++) {
        putchar(file_map[i]); // imprime los caracteres uno por uno
    }
    printf("\n");
    

    printf("Processing file...\n");

    #pragma omp parallel for schedule(dynamic)
    for (size_t offset = 0; offset < filesize; offset += BLOCK_SIZE) {
        size_t block_size = (offset + BLOCK_SIZE < filesize) ? BLOCK_SIZE : (filesize - offset);
        process_block(file_map + offset, block_size, reviews);
    }

    print_reviews(reviews);

    int search_id = 0;
    Review* found_review = get(reviews, search_id);
    if (found_review) {
        printf("Review found: app_id=%d, user_id=%d, review_id=%d\n",
               found_review->app_id, found_review->user_id, found_review->review_id);
    } else {
        printf("Review with review_id=%d not found\n", search_id);
    }

    munmap(file_map, filesize);
    close(fd);
    freeHashTable(reviews);

    clock_t end = clock();
    printf("Execution time: %f seconds\n", (double)(end - start_time) / CLOCKS_PER_SEC);
    return 0;
}
