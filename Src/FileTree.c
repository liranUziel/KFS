#include "./Headers/FileTree.h"


FileNode* create_node(const char* name, int inode, int depth) {
    FileNode* node = (FileNode*)malloc(sizeof(FileNode));
    node->inode = inode;
    node->depth = depth;
    strncpy(node->name, name, 16);
    node->parent = NULL;
    node->children = NULL;
    node->childrenCount = 0;
    return node;
}

void add_child(FileNode* parent, FileNode* child,char type) {
    parent->children = (FileNode**)realloc(parent->children, sizeof(FileNode*) * (parent->childrenCount + 1));
    parent->children[parent->childrenCount] = child;
    child->parent = parent;
    child->type = type;
    parent->childrenCount++;
}

FileNode* search_node(FileNode* root, const char* path) {
    if (strcmp(root->name, path) == 0) {
        return root;
    }
    for (int i = 0; i < root->childrenCount; i++) {
        FileNode* result = search_node(root->children[i], path);
        if (result != NULL) {
            return result;
        }
    }
    return NULL;
}

void print_tree(FileNode* root, int level,int end) {
    for (int i = 0; i < level; i++) {
        if(end == 1){
            if(i==0){
                printf("└─");
            }else{
                printf("──");
            }   
        }else{
            if(i==0){
                printf("├─");
            }else{
                printf("──");
            }
    
        }
    }
    //get type of file
    char type = root->type;
    if(root->depth == 0){
        printf("├🗁 / \n");
    }else{
       
        if(type == 'd'){
            
            printf("🗁");
        }else{
            printf("🗎");
        }
        printf("%s\n",root->name);
    }
    for (int i = 0; i < root->childrenCount; i++) {
        if(i == root->childrenCount - 1 && root->name[0] == '/'){
            print_tree(root->children[i], level + 1,1);
        }
        else{
            print_tree(root->children[i], level + 1,0);
        }

    }
}

FileNode* searchByPath(FileNode* root, const char* path) {
    char* token;
    char* path_copy = strdup(path);
    FileNode* current = root;

    token = strtok(path_copy, "/");
    while (token != NULL) {
        int found = 0;
        for (int i = 0; i < current->childrenCount; i++) {
            if (strcmp(current->children[i]->name, token) == 0) {
                current = current->children[i];
                found = 1;
                break;
            }
        }
        if (!found) {
            free(path_copy);
            return NULL;
        }
        token = strtok(NULL, "/");
    }

    free(path_copy);
    return current;
}

int addByPath(FileNode* root, const char* name, int inode, const char* path) {
    FileNode* parent = searchByPath(root, path);
    if (parent == NULL) {
        return 0; // Path not found
    }
    FileNode* new_node = create_node(name, inode, parent->depth + 1);
    add_child(parent, new_node,"1");
    return 1; // Success
}

void printTreeInfo(FileNode* root){
    
    printf("##Debug info##\n");
    printf("Root: %s\n", root->name);
    printf("Root inode: %d\n", root->inode);
    printf("Root depth: %d\n", root->depth);
    printf("Root children count: %d\n", root->childrenCount);
    for(size_t i = 0; i < root->childrenCount; i++){
        printSubTreeInfo(root->children[i]);
    }

}
void printSubTreeInfo(FileNode* node){
    printf("##Debug info##\n");
    printf("Node: %s\n", node->name);
    printf("Node inode: %d\n", node->inode);
    printf("Node depth: %d\n", node->depth);
    printf("Node children count: %d\n", node->childrenCount);
    for(size_t i = 0; i < node->childrenCount; i++){
        printSubTreeInfo(node->children[i]);
    }
}