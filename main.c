#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_USERS 1000
int edges = 0;

typedef struct {
    char username[50];
    char password[50];
    char interests[100];
    int age, community;
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
    int num_communities;
    int max_communities;
    user users[MAX_USERS];
    AdjList* array;
    AdjList* communities;
} Graph;

typedef struct visitedList{
    int src, dest;
    struct visitedList* next;
} visitedList;

AdjListNode* newAdjListNode(int dest) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->dest = dest;
    newNode->next = NULL;
    return newNode;
}


Graph* createGraph(int num_users) {
    if(num_users>MAX_USERS) return NULL;
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->max_users = graph->max_communities = num_users;
    graph->num_users = graph->num_communities = 0;

    graph->array = (AdjList*)malloc(num_users * sizeof(AdjList));
    graph->communities = malloc(num_users * sizeof(AdjList));

    for (int i = 0; i < num_users; ++i)
        graph->array[i].head = NULL;

    for (int i = 0; i < graph->max_communities; ++i)
        graph->communities[i].head = NULL;

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
    edges+=2;
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

void findTotalEdges(Graph* graph) {
    int currEdges = 0;
    for (int v = 0; v < graph->num_users; ++v) {
        AdjListNode* pCrawl = graph->array[v].head;
        while (pCrawl) {
            currEdges++;
            pCrawl = pCrawl->next;
        }
    }
    edges = currEdges;
}

void addUser(Graph* graph, user user) {
    if (graph->num_users < graph->max_users) {
        user.community = graph->num_communities;
        graph->users[graph->num_users] = user;
        graph->communities[graph->num_communities] = *(AdjList*)malloc(sizeof (AdjList));
        graph->communities[graph->num_communities].head = newAdjListNode(graph->num_users);
        graph->num_users++;
        graph->num_communities++;
    } else {
        printf("Maximum number of users reached.\n");
    }
}

void printCommunities(Graph* graph) {
    for (int v = 0; v < graph->num_communities; ++v) {
        AdjListNode* pCrawl = graph->communities[v].head;
        printf("\n Community %d :", v);
        while (pCrawl) {
            printf("-> %s", graph->users[pCrawl->dest].username);
            pCrawl = pCrawl->next;
        }
        printf("\n");
    }
}

//add the communities check
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

visitedList* insertBeginVisited(visitedList* head, int src, int dest){
    visitedList *newNode = malloc(sizeof(visitedList));
    newNode->src = src;
    newNode->dest = dest;
    newNode->next = head;
    return newNode;
}

//call findTotalEdges before this
double calculateModularity(Graph* graph, int community) {
    int src, dest;
    double modularity = 0.0;
    int e_ii = 0, a_i = 0;
    visitedList *visitedHead = NULL;
    AdjListNode *currCommMember = graph->communities[community].head;

    //iterate through the adjList of the user

    while(currCommMember){
        src = currCommMember->dest;
        AdjListNode *listIterator = graph->array[src].head;
        //iterate through each community member's adjList
        while(listIterator){
            bool isVisited = false;
            dest = listIterator->dest;
            if(visitedHead==NULL){
                visitedHead = insertBeginVisited(visitedHead, src, dest);
                e_ii++;
            }
            visitedList *currVisited = visitedHead;
            //traverse the visited list to find if the src dest pair exists
            while(currVisited){
                //the pair exists
                if ((currVisited->dest==dest && currVisited->src==src) || (currVisited->src==dest && currVisited->dest==src)){
                    isVisited = true;
                    break;
                }

                currVisited = currVisited->next;
            }
            if(!isVisited){
                visitedHead = insertBeginVisited(visitedHead, src, dest);
                if(graph->users[src].community == graph->users[dest].community){
                    e_ii++;
                }
            }
            a_i++;
            listIterator = listIterator->next;
        }
        currCommMember = currCommMember->next;
    }
    modularity = (double)(e_ii/(float)edges - (a_i/(float)edges)*(a_i/(float)edges));

    return modularity;
}

// Function to calculate the total degree of nodes in a community
int calculateCommunityDegree(Graph* graph, int* communities, int community) {
    int i;
    int degree = 0;

    //find the out-degree or number of connections of whichever users are in the community
    for (i = 0; i < graph->num_users; ++i) {
        if (communities[i] == community) {
            AdjListNode * current = graph->array[i].head;
            while (current != NULL) {
                degree++;
                current = current->next;
            }
        }
    }

    return degree;
}

AdjListNode* insertInCommunity(int user, int community, Graph* graph){
    AdjListNode* userNode = malloc(sizeof(AdjListNode));
    userNode->dest = user;
    AdjListNode* temp = graph->communities[community].head;
    userNode->next = temp;
    graph->communities[community].head = userNode;
    return userNode;
}

void removeFromCommunity(int user, int community, Graph* graph){
    AdjListNode *curr = graph->communities[community].head;
    if(!curr) return;
    //delete from the start
    if(curr->dest==user){
        AdjListNode *temp = curr->next;
        free(graph->communities[community].head);
        graph->communities[community].head = temp;
        return;
    }
    AdjListNode *prev = curr;
    curr = curr->next;
    for(; curr; curr = curr->next, prev = prev->next){
        if(curr->dest==user){
            prev->next = curr->next;
            free(curr);
            return;
        }
    }
}

bool isInCommunity(int user, int community, Graph* graph){
    AdjListNode *curr = graph->communities[community].head;
    for(; curr; curr = curr->next){
        if(curr->dest==user) return true;
    }
    return false;
}

void optimizeModularityAggressive(Graph* graph){
    if(graph->communities==NULL) return;

    for(int user = 0; user<graph->num_users; user++){
        double maxModularity = INT_MIN;
        int optimalCommunity = 0;
        for(int community = 0; community<graph->num_communities; community++){
            if(!isInCommunity(user, community, graph)) insertInCommunity(user, community, graph);
            double modularity = calculateModularity(graph, community);
            if(modularity>maxModularity){
                maxModularity = modularity;
                optimalCommunity = community;
            }
            removeFromCommunity(user, community, graph);
        }
        insertInCommunity(user, optimalCommunity, graph);
        graph->users[user].community = optimalCommunity;
    }
}

void optimizeModularityConservative(Graph* graph){
    if(graph->communities==NULL) return;

    for(int user = 0; user<graph->num_users; user++){
        double maxModularity = INT_MIN;
        int optimalCommunity = 0;
        int originalCommunity = 0;
        for(int community = 0; community<graph->num_communities; community++){
            double prevModularity = calculateModularity(graph, community);
            bool wasInCommunity = false;
            if(isInCommunity(user, community, graph)){
                wasInCommunity = true;
                originalCommunity = community;
            }
            if(!wasInCommunity) insertInCommunity(user, community, graph);
            double modularity = calculateModularity(graph, community);
            if(modularity>maxModularity && prevModularity<=modularity){
                maxModularity = modularity;
                optimalCommunity = community;
            }
            if(!wasInCommunity) removeFromCommunity(user, community, graph);
        }
        if(optimalCommunity!=originalCommunity) insertInCommunity(user, optimalCommunity, graph);
        graph->users[user].community = optimalCommunity;
    }
}

void resetCommunnities(Graph* graph){
    for(int i = 0; i<graph->num_users; i++){
        AdjListNode *curr = graph->communities[i].head;
        while(curr){
            AdjListNode *temp = curr;
            curr = curr->next;
            free(temp);
        }
        AdjListNode *freshNode = malloc(sizeof(AdjListNode));
        freshNode->dest = i;
        freshNode->next = NULL;
        graph->communities[i].head = freshNode;
        graph->users[i].community = i;
    }
}

void aggressiveOptimizationLoop(Graph* graph, int* increased, int* same){
    double prevModularity[graph->num_communities];
    for(int i = 0; i<graph->num_communities; i++){
        double modularity = calculateModularity(graph, i);
        prevModularity[i] = modularity;
    }
    
    int prevIncreased = 0;
    int prevSame = 0;
    *increased = 0;
    *same = 0;
    while(1) {
        optimizeModularityAggressive(graph);
        *increased = *same = 0;
        for(int i = 0; i<graph->num_communities; i++){
            double modularity = calculateModularity(graph, i);
            if(modularity>prevModularity[i]) (*increased)++;
            if(modularity==prevModularity[i]) (*same)++;
        }
        if(*increased==prevIncreased && *same==prevSame) break;
        prevIncreased = *increased;
        prevSame = *same;
    }
}
// Function for modularity optimization
//void modularityOptimization(Graph* graph, int* communities, int numEdges) {
//    int i, j;
//    double initialModularity, currentModularity, maxModularityChange;
//    int nodeToMove, moveToCommunity;
//
//    // Calculate initial modularity
//    initialModularity = calculateModularity(graph, communities);
//
//    // Iterative optimization
//    do {
//        maxModularityChange = 0;
//
//        // For each node
//        for (i = 0; i < graph->num_users; ++i) {
//            // Try moving to each neighboring community
//            for (j = 0; j < ; ++j) {
//                // Temporarily move the node to the new community
//                int originalCommunity = communities[i];
//                communities[i] = j;
//
//                // Calculate modularity change
//                currentModularity = calculateModularity(graph, communities);
//                double modularityChange = currentModularity - initialModularity;
//
//                // If improvement, update
//                if (modularityChange > maxModularityChange) {
//                    maxModularityChange = modularityChange;
//                    nodeToMove = i;
//                    moveToCommunity = j;
//                }
//
//                // Move the node back for the next iteration
//                communities[i] = originalCommunity;
//            }
//        }
//
//        // Update the community assignment
//        if (maxModularityChange > 0) {
//            communities[nodeToMove] = moveToCommunity;
//            initialModularity = currentModularity;
//        }
//
//    } while (maxModularityChange > 0);
//}

void dijkstra(Graph* graph, int src, int* dist) {
    int visited[MAX_USERS] = {0};

    // Initialize distances
    for (int i = 0; i < graph->num_users; ++i)
        dist[i] = INT_MAX;
    dist[src] = 0;

    // Find shortest path for all vertices
    for (int count = 0; count < graph->num_users - 1; ++count) {
        int u = -1;
        for (int v = 0; v < graph->num_users; ++v) {
            if (!visited[v] && (u == -1 || dist[v] < dist[u]))
                u = v;
        }

        visited[u] = 1;

        AdjListNode* pCrawl = graph->array[u].head;
        while (pCrawl) {
            int v = pCrawl->dest;
            if (!visited[v] && dist[u] + 1 < dist[v])
                dist[v] = dist[u] + 1;
            pCrawl = pCrawl->next;
        }
    }
}

void findPotentialUsers(Graph* graph, int userIndex) {
    printf("Potential users for %s based on shared interests:\n", graph->users[userIndex].username);
    for (int i = 0; i < graph->num_users; ++i) {
        // Skip the user itself
        if (i == userIndex)
            continue;

        // Check if there are shared interests
        if (strcmp(graph->users[userIndex].interests, graph->users[i].interests) == 0) {
            printf("%s\n", graph->users[i].username);
        }
    }
}

bool isFriend(Graph* graph, int src, int dest){
    AdjListNode *curr = graph->array[src].head;
    while(curr){
        if(curr->dest==dest) return true;
        curr = curr->next;
    }
    return false;
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
//
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
//
    int userIndex = 0;
     //Read lines from the file until EOF (End Of File)
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

    printf("Num users: %d\n", userIndex);
//    // Close the file
    fclose(file);

    for(int i = 0; i<userIndex*2; i++){
        addEdge(graph, rand()%userIndex, rand()%userIndex);
    }

    printGraph(graph);
    printCommunities(graph);
    int* dist = (int*)malloc(graph->num_users * sizeof(int));

//    char searchName[50];
//    printf("Enter a name: ");
//    scanf("%s", searchName);
//
//    int src = -1;
//    for (int i = 0; i < graph->num_users; ++i) {
//        if (strcmp(graph->users[i].username, searchName) == 0) {
//            src = i;
//            break;
//        }
//    }

//    if (src != -1) {
//        dijkstra(graph, src, dist);
//        printf("Users within distance <= 2 from %s:\n", searchName);
//        for (int i = 0; i < graph->num_users; ++i) {
//            if (dist[i] == 2 && i != src && !isFriend(graph, src, i))
//                printf("%s (Distance: %d)\n", graph->users[i].username, dist[i]);
//        }
//    } else {
//        printf("User not found.\n");
//    }
//
//    findPotentialUsers(graph, src);
//
//    free(dist);

    findTotalEdges(graph);
    printf("\nTotal edges: %d\n",(int)(edges/2));
    double prevModularity[graph->num_communities];
    for(int i = 0; i<graph->num_communities; i++){
        double modularity = calculateModularity(graph, i);
        printf("Modularity of community %d : %f\n", i, modularity);
        prevModularity[i] = modularity;
    }
    int increased = 0; int same = 0;
    aggressiveOptimizationLoop(graph, &increased, &same);
//    resetCommunnities(graph);
//    printCommunities(graph);
//    aggressiveOptimizationLoop(graph, &increased, &same);
    for(int i = 0; i<graph->num_communities; i++){
        double modularity = calculateModularity(graph, i);
        if(modularity>prevModularity[i]) increased++;
        if(modularity==prevModularity[i]) same++;
    }

    printf("\nNumber of communities with increased modularity = %d\n", increased);
    printf("Number of communities with increased or same modularity = %d\n", increased + same);

    free(graph->array);
    free(graph);

    return 0;
}
