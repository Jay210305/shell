#include "vfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct file_node {
    char name[256];
    struct file_node *next;
} file_node;

static file_node *head = NULL;

void init_vfs() {
    head = NULL;
}

void destroy_vfs() {
    file_node *curr = head;
    while (curr) {
        file_node *next = curr->next;
        free(curr);
        curr = next;
    }
    head = NULL;
}

void vfs_create(const char *name) {
    file_node *node = malloc(sizeof(file_node));
    strncpy(node->name, name, 255);
    node->name[255] = '\0';
    node->next = head;
    head = node;
}

void vfs_list() {
    file_node *curr = head;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
    }
}