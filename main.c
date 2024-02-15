#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100


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

int main() {

    Graph* graph = createGraph(5);

  
    user user1 = {"user1", "password1", "Sports", 25};
    user user2 = {"user2", "password2", "Music", 30};
    user user3 = {"user3", "password3", "Movies", 35};
    user user4 = {"user4", "password4", "Books", 40};
    user user5 = {"user5", "password5", "Travel", 45};

    addUser(graph, user1);
    addUser(graph, user2);
    addUser(graph, user3);
    addUser(graph, user4);
    addUser(graph, user5);

    printf("%s\n\n", graph->users[0].username);
    addEdge(graph, 0, 1); 
    addEdge(graph, 0, 3); 
    addEdge(graph, 1, 2); 
    addEdge(graph, 3, 4); 


    printGraph(graph);

    removeUser(graph, 2);

    
    printf("\nGraph after removing user3:\n");
    printGraph(graph);

    free(graph);

    return 0;
}
