#ifndef FILE_TREE_H
#define FILE_TREE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _FileNode{
    int inode;
    int depth;
    char name[16];
    struct _FileNode* parent;
    struct _FileNode** children;
    int childrenCount;
    char type;
}FileNode;

FileNode* create_node(const char* name, int inode, int depth);
void add_child(FileNode* parent, FileNode* child,char type) ;
FileNode* search_node(FileNode* root, const char* path);
void print_tree(FileNode* root, int level,int end) ;
int addByPath(FileNode* root, const char* name, int inode, const char* path);
FileNode* searchByPath(FileNode* root, const char* path);
void printTreeInfo(FileNode* root);


#endif // FILE_TREE_H