#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 10000


typedef struct {
    char username[50];
    char password[50];
    char interests[100];
    int age;
} user;


typedef struct AdjListNode {
    int dest;
    struct AdjListNode* next;
} AdjListNode;


typedef struct {
    AdjListNode* head;
} AdjList;


typedef struct {
    int num_users;
    int max_users;
    user users[MAX_USERS];
    AdjList* array;
} Graph;


AdjListNode* newAdjListNode(int dest) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}


Graph* createGraph(int num_users) {
    if(num_users>MAX_USERS) return NULL;
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->max_users = num_users;
    graph->num_users = 0;

    graph->array = (AdjList*)malloc(num_users * sizeof(AdjList));

    for (int i = 0; i < num_users; ++i)
        graph->array[i].head = NULL;

    return graph;
}


void addEdge(Graph* graph, int src, int dest) {
    //check if it's already there
    AdjListNode *curr = graph->array[src].head;
    for(; curr!=NULL; curr=curr->next){
        if(curr->dest==dest){
            printf("Edge already exists.\n");
            return;
        }
    }
    //here we are inserting at the front of the linked list

    //create a new node for src adjacency list
    AdjListNode* newNode = newAdjListNode(dest);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;

    //create a new node for dest adjacency list
    newNode = newAdjListNode(src);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}


void printGraph(Graph* graph) {
    for (int v = 0; v < graph->num_users; ++v) {
        AdjListNode* pCrawl = graph->array[v].head;
        printf("\n Adjacency list of user %s\n head ", graph->users[v].username);
        while (pCrawl) {
            printf("-> %s", graph->users[pCrawl->dest].username);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

void addUser(Graph* graph, user user) {
    if (graph->num_users < graph->max_users) {
        graph->users[graph->num_users] = user;
        graph->num_users++;
    } else {
        printf("Maximum number of users reached.\n");
    }
}

void removeUser(Graph* graph, int userIndex) {
    if (userIndex >= 0 && userIndex < graph->num_users) {
        
        for (int i = 0; i < graph->num_users; ++i) {
            AdjListNode* curr = graph->array[i].head;
            AdjListNode* prev = NULL;
            while (curr) {
                //if the node is of the user to be deleted
                if (curr->dest == userIndex) {
                    if (prev == NULL) {
                        graph->array[i].head = curr->next;
                    } else {
                        prev->next = curr->next;
                    }
                    free(curr);
                    //assign current the value of previous if previous is not null, i.e not the head of the linked list
                    curr = prev ? prev->next : graph->array[i].head;
                } else {
                    prev = curr;
                    curr = curr->next;
                }
            }
        }

        
        for (int i = userIndex; i < graph->num_users - 1; ++i) {
            graph->users[i] = graph->users[i + 1];
        }
        graph->num_users--;
    } else {
        printf("Invalid user index.\n");
    }
}

#define MAX_LINE_LENGTH 1024
int main() {

    Graph* graph = createGraph(MAX_USERS);

    FILE *file;
    char line[MAX_LINE_LENGTH];
    char *name;
    char *token;

    // Open the file for reading in text mode
    file = fopen("../usernames.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }

    char *interests[] = {
            "Sports",
            "Music",
            "Movies",
            "Books"
    };
    int numInterests = 4;
    srand(42);
    int min_age = 18, max_age = 90;
    int age_range = max_age - min_age + 1;

    user users[MAX_USERS]; // Replace MAX_USERS with your expected user count

    int userIndex = 0;
    // Read lines from the file until EOF (End Of File)
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        // Remove trailing newline character if present
        strtok(line, "\n");

        // Use strtok to split the line into comma-separated names
        token = strtok(line, ", ");
        int interestIndex = 0;
        while (token != NULL) {
            name = token;

            //create the struct
            user* newUser = &users[userIndex++];
            strcpy(newUser->username,name);
            strcpy(newUser->password,name);
            strcpy(newUser->interests, interests[interestIndex++ % numInterests]);
            newUser->age = rand() % age_range + min_age;

            addUser(graph, *newUser);
            // Get the next token
            token = strtok(NULL, ", ");
        }
    }

    printf("Num users: %d", userIndex);
    // Close the file
    fclose(file);

    for(int i = 0; i<userIndex*3; i++){
        addEdge(graph, rand()%userIndex, rand()%userIndex);
    }

    printGraph(graph);

    free(graph->array);
    free(graph);

    return 0;
}
