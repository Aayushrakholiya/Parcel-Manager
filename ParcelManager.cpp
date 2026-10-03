/*
* FILE : project.cpp
* PROJECT : SENG1050 - Project
* PROGRAMMER : Ayushkumar Bhupatbhai Rakholiya
* FIRST VERSION : 2024-08-10
* DESCRIPTION :
* this program read raw data and use combine logic of Hash Table and Tree
* structure to arrange large chunk of data in Hash Table array. Each
* array element holds the pointer to the root element of the Tree structure
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// i disabeling the warning for scanf function
#pragma warning(disable:4996)

// Constants
#define BUCKET_COUNT 127
#define NAME_LEN 21
#define LINE_LEN 2000
#define HASH_INITIAL_VALUE 5381
#define INPUT_FILE_NAME "couriers.txt"
#define MAX_PARCELS 5000
#define MAX_WEIGHT 50000
#define MIN_WEIGHT 100

//Menu Items.
#define MENU_PRINT_ALL 1
#define MENU_PRINT_PAIR 2
#define MENU_PRINT_TOTAL 3
#define MENU_PRINT_MIN_MAX_VAL 4
#define MENU_PRINT_MIN_MAX_WGT 5
#define MENU_EXIT 6

// this is a structure for the parcel node in the tree
struct ParcelNode {
    char* destination;
    float weight;
    float valuation;
    struct ParcelNode* left;
    struct ParcelNode* right;
};

// this is a structure for the hash table
struct ParcelHashTable {
    struct ParcelNode* table[BUCKET_COUNT];
};

// function prototypes
struct ParcelNode* createParcelNode(char* destination, float weight, float valuation);
void AddParcelToHashTable(struct ParcelHashTable* hashTable, char* destination, float weight, float valuation);
void discardBuffer();
unsigned long hash(unsigned char* str);
struct ParcelNode* findParcelByDestination(struct ParcelNode* currentNode, const char* targetDestination);
void PrintAllParcels(struct ParcelNode* currentNode);
void PrintParcelsByWeight(struct ParcelNode* currentNode, int referenceWeight, int isAboveThreshold);
void AccumulateTotals(struct ParcelNode* currentNode, float* accumulatedWeight, float* accumulatedValuation);
void findMinAndMaxFlightPrice(struct ParcelNode* currentNode, struct ParcelNode** min, struct ParcelNode** max);
void findMinAndMaxWeight(struct ParcelNode* currentNode, struct ParcelNode** minWeight, struct ParcelNode** maxWeight);
void destroyHashTable(struct ParcelHashTable* table);
struct ParcelHashTable* createParcelHashTable(void);
struct ParcelNode* InsertParcelNode(ParcelNode* root, char* destination, float weight, float valuation);
void freeParcelTree(struct ParcelNode* currentNode);


int main(void) {

    // buffer to store user input
    char parcelDestination[NAME_LEN] = { 0 };

    // buffer to store country name
    char countryName[NAME_LEN] = { 0 };

    // buffer to store input line
    char inputLine[LINE_LEN] = { 0 };

    // variables to store parcel details
    float parcelWeight = 0.0;
    float parcelValuation = 0.0;
    float accumulatedWeight = 0.0;
    float accumulatedValuation = 0.0;

    // this is variable to store user input
    int userInputChoice = 0;

    // file pointer
    FILE* inputFile = NULL;

    // variables to store the lightest and heaviest parcels
    struct ParcelNode* lightestParcel = NULL;
    struct ParcelNode* heaviestParcel = NULL;

    // variables to store the cheapest and most expensive parcels
    struct ParcelNode* cheapestParcel = NULL;
    struct ParcelNode* mostExpensiveParcel = NULL;

    // this will create the hash table
    struct ParcelHashTable* parcelHashTable = createParcelHashTable();

    // here i am checking if the hash table is created or not
    if (parcelHashTable == NULL) {
        printf("Error: Hash table initialization failed\n");
        return -1;
    }

    // this will open the file
    inputFile = fopen("couries.txt", "r");
    if (inputFile == NULL) {
        printf("Unable to open the file: %s\n", INPUT_FILE_NAME);
        return -1;
    }

    // it will read the file line by line
    while (fgets(inputLine, sizeof(inputLine), inputFile) != NULL) {
        inputLine[strcspn(inputLine, "\n")] = '\0';

        // this will skip the empty lines
        if (inputLine[0] == '\0')
            continue;

        // parse the input line to extract the parcel details
        if (sscanf(inputLine, "%20[^,], %f, %f", parcelDestination, &parcelWeight, &parcelValuation) != 3) {
            printf("Error: Failed to parse line containing '%s'\n", inputLine);
            continue;
        }

        // this will add the parcel to the hash table
        AddParcelToHashTable(parcelHashTable, parcelDestination, parcelWeight, parcelValuation);// Insert the node into HashTable
    }

    // this will close the file
    if (fclose(inputFile) != 0) {
        printf("Can't close the file\n");
        return -1;
    }

    // this is the main loop for the program to run and take user input and display the output accordingly 
    while (true) {
        printf("\nPlease see the below user menu:\n");
        printf("1. Enter country name and display all the parcels details\n");
        printf("2. Enter country and weight pair\n");
        printf("3. Display the total parcel load and valuation for the country\n");
        printf("4. Enter the country name and display cheapest and most expensive parcels details\n");
        printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
        printf("6. Exit the application\n");
        printf("\nPlease enter your choice (1-6): ");

        // it will take the user input and check if the input is valid or not 
        if (scanf("%d", &userInputChoice) != 1) {

            printf("Invalid Input. Please enter a number between %d and %d.\n", MENU_PRINT_ALL, MENU_EXIT);

            discardBuffer();
            continue;
        }

        // here i initialize the found parcel to NULL
        struct ParcelNode* foundParcel = NULL;
        discardBuffer();

        // this is the switch case to check the user input and display the output accordingly
        switch (userInputChoice) {

        case MENU_PRINT_ALL:

            printf("\nPlease enter the country name to display all parcels: ");
            fgets(countryName, NAME_LEN, stdin);

            // it will remove the newline character from the input
            countryName[strcspn(countryName, "\n")] = '\0';
            foundParcel = findParcelByDestination(parcelHashTable->table[hash((unsigned char*)countryName) % BUCKET_COUNT], countryName);

            // this will check if the parcel is found or not
            if (foundParcel != NULL) {
                PrintAllParcels(foundParcel);
            }
            else {
                printf("\nNo parcels found for the country '%s'.\n", countryName);
            }
            break;

        case MENU_PRINT_PAIR:

            printf("Please enter the country name: ");
            fgets(countryName, NAME_LEN, stdin);

            //it will remove the newline character from the input
            countryName[strcspn(countryName, "\n")] = '\0';

            // this will find the parcel by destination
            printf("Please enter the weight of the parcels: ");
            if (scanf("%f", &parcelWeight) <= 0) {
                printf("Invalid input for weight. Please enter a numeric value.\n");
                discardBuffer();
            }

            // this will check if the weight is in the valid range or not
            else {
                if (parcelWeight < MIN_WEIGHT || parcelWeight > MAX_WEIGHT) {// Verify if the range of weight input is valid
                    printf("Weight is out of the valid range (%.2d - %.2d).\n", MIN_WEIGHT, MAX_WEIGHT);
                    discardBuffer();
                }
                else {
                    discardBuffer();

                    foundParcel = findParcelByDestination(parcelHashTable->table[hash((unsigned char*)countryName) % BUCKET_COUNT], countryName);
                    if (foundParcel != NULL) {
                        printf("Parcels lighter than %.2f Gms:\n\n", parcelWeight);
                        PrintParcelsByWeight(foundParcel, parcelWeight, 1);
                        printf("Parcels heavier than %.2f Gms:\n", parcelWeight);
                        PrintParcelsByWeight(foundParcel, parcelWeight, 0);
                    }

                    // if no parcels are found for the country it will display this message
                    else {
                        printf("\nNo parcels found for the country '%s'.\n", countryName);
                    }
                }
            }
            break;

        case MENU_PRINT_TOTAL:

            // now i am resetting the accumulated weight and valuation
            accumulatedWeight = 0;
            accumulatedValuation = 0;

            printf("Please enter the country name: ");
            fgets(countryName, NAME_LEN, stdin);

            // it will remove the newline character from the input
            countryName[strcspn(countryName, "\n")] = '\0';

            foundParcel = findParcelByDestination(parcelHashTable->table[hash((unsigned char*)countryName) % BUCKET_COUNT], countryName);

            // this will check if the parcel is found or not and display the total weight and valuation of the parcel 
            if (foundParcel != NULL) {
                AccumulateTotals(foundParcel, &accumulatedWeight, &accumulatedValuation);
                printf("\nTotal parcel load for '%s': %.2f Gms\n", countryName, accumulatedWeight);
                printf("Total parcel valuation for '%s': %.2f\n", countryName, accumulatedValuation);
            }
            else {
                printf("\nNo parcels found for the country '%s'.\n", countryName);
            }
            break;

        case MENU_PRINT_MIN_MAX_VAL:

            //now i am resetting the cheapest and most expensive parcels
            cheapestParcel = NULL;
            mostExpensiveParcel = NULL;

            printf("Please enter the country name: ");
            fgets(countryName, NAME_LEN, stdin);

            //this will remove the newline character from the input
            countryName[strcspn(countryName, "\n")] = '\0';

            foundParcel = findParcelByDestination(parcelHashTable->table[hash((unsigned char*)countryName) % BUCKET_COUNT], countryName);

            // this will check if the parcel is found or not and display the cheapest and most expensive parcels
            if (foundParcel != NULL) {
                findMinAndMaxFlightPrice(foundParcel, &cheapestParcel, &mostExpensiveParcel);
                if (cheapestParcel != NULL && mostExpensiveParcel != NULL) {
                    printf("\nCheapest parcel:\n");
                    printf("  Destination: %s\n", cheapestParcel->destination);
                    printf("  Weight     : %.2f Gms\n", cheapestParcel->weight);
                    printf("  Valuation  : %.2f\n", cheapestParcel->valuation);

                    printf("\nmost expensive parcel:\n");
                    printf("  Destination: %s\n", mostExpensiveParcel->destination);
                    printf("  Weight     : %.2f Gms\n", mostExpensiveParcel->weight);
                    printf("  Valuation  : %.2f\n", mostExpensiveParcel->valuation);
                }
                else {
                    printf("\nNo parcels found for the country '%s'.\n", countryName);
                }
            }
            else {
                printf("\n '%s' country was not found.\n", countryName);
            }
            break;

        case MENU_PRINT_MIN_MAX_WGT:

            // now i am resetting the lightest and heaviest parcels
            lightestParcel = NULL;
            heaviestParcel = NULL;

            printf("Please enter the country name: ");
            fgets(countryName, NAME_LEN, stdin);

            // this will remove the newline character from the input
            countryName[strcspn(countryName, "\n")] = '\0';

            foundParcel = findParcelByDestination(parcelHashTable->table[hash((unsigned char*)countryName) % BUCKET_COUNT], countryName);

            // this will check if the parcel is found or not and display the lightest and heaviest parcels
            if (foundParcel != NULL) {
                findMinAndMaxWeight(foundParcel, &lightestParcel, &heaviestParcel);
                if (lightestParcel != NULL && heaviestParcel != NULL) {
                    printf("\nLightest parcel:\n");
                    printf("  Destination: %s\n", lightestParcel->destination);
                    printf("  Weight     : %.2f kg\n", lightestParcel->weight);
                    printf("  Valuation  : %.2f\n", lightestParcel->valuation);

                    printf("\nHeaviest parcel:\n");
                    printf("  Destination: %s\n", heaviestParcel->destination);
                    printf("  Weight     : %.2f kg\n", heaviestParcel->weight);
                    printf("  Valuation  : %.2f\n", heaviestParcel->valuation);
                }
                else {
                    printf("\nNo parcels found for the country '%s'.\n", countryName);
                }
            }
            else {
                printf("\n '%s' country was not found.\n", countryName);
            }
            break;

        case MENU_EXIT:

            // this will free the memory allocated for the hash table and its nodes and exit the program
            destroyHashTable(parcelHashTable);

            printf("\nExiting application.\n");

            return 0;
            break;

        default:
            printf("Invalid Input. Please enter a number between %d and %d.\n", MENU_PRINT_ALL, MENU_EXIT);
        }
    }

    //this will free the memory allocated for the hash table and its nodes
    destroyHashTable(parcelHashTable);
    return 0;
}

//
// FUNCTION : createParcelNode
// DESCRIPTION :
// this function will create a new node and allocate memory
// PARAMETERS :
// char* destination : it will store the destination of the parcel
// float weight      : it will store the weight of the parcel
// float valuation   : it will store the valuation of the parcel
// RETURNS :
// this will return the newly created node
//

struct ParcelNode* createParcelNode(char* destination, float weight, float valuation) {
    struct ParcelNode* node = (struct ParcelNode*)malloc(sizeof(struct ParcelNode));
    if (node == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }
    node->destination = (char*)malloc(strlen(destination) + 1);
    if (node->destination == NULL) {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    strcpy(node->destination, destination);
    node->weight = weight;
    node->valuation = valuation;
    node->left = NULL;
    node->right = NULL;

    return node;
}

//
// FUNCTION : AddParcelToHashTable
// DESCRIPTION :
// this function add the new parcel to the hash table
// PARAMETERS :
// struct ParcelHashTable* hashTable : it is a pointer to the hash table where the parcel will be added
// char* destination : it is a string which represent the destination of the parcel
// float weight : it is the weight of the parcel
// float valuation : it is the valuation of the parcel
// RETURNS :
// void
//
void AddParcelToHashTable(struct ParcelHashTable* hashTable, char* destination, float weight, float valuation) {
    int bucketIndex = hash((unsigned char*)destination) % BUCKET_COUNT;
    hashTable->table[bucketIndex] = InsertParcelNode(hashTable->table[bucketIndex], destination, weight, valuation);
}

//
// FUNCTION : discardBuffer
// DESCRIPTION :
// this function will remocve any leftover character in the buffer 
// PARAMETERS :
// RETURNS :
// void
//

void discardBuffer() {
    int c = 0;
    while ((c = getchar()) != '\n' && c != EOF);
}

//
// FUNCTION : hash
// DESCRIPTION :
// this will generate hash value for the string
// PARAMETERS :
// unsigned char* str : it will store the string
// RETURNS :
// this will return the hash value of the string
//

unsigned long hash(unsigned char* str) {//using "djb2" function to generate hash, from Module-09
    unsigned long hash = HASH_INITIAL_VALUE;
    int c = 0;
    while ((c = *str++) != '\0') {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

//
// FUNCTION : findParcelByDestination
// DESCRIPTION :
// this function will search for a parcel in the tree based on the destination
// PARAMETERS :
// struct ParcelNode* currentNode : it is a pointer to the current node
// const char* targetDestination : it is a string which represent the destination of the parcel
// RETURNS :
// this will return a pointer to the node with the specified destination
//

struct ParcelNode* findParcelByDestination(struct ParcelNode* currentNode, const char* targetDestination) {
    if (currentNode == NULL) {

        return NULL;
    }


    int comparison = strcmp(targetDestination, currentNode->destination);

    if (comparison == 0) {

        return currentNode;
    }
    else if (comparison < 0) {

        return findParcelByDestination(currentNode->left, targetDestination);
    }
    else {

        return findParcelByDestination(currentNode->right, targetDestination);
    }
}

//
// FUNCTION : displayAllParcels
// DESCRIPTION :
// this function which traverse the tree in inorder and display the details of all the parcels
// PARAMETERS :
// struct ParcelNode* currentNode : it is a pointer to the current node
// RETURNS :
// void
//

void PrintAllParcels(struct ParcelNode* currentNode) {
    if (currentNode == NULL) {
        return;
    }
    else {

        PrintAllParcels(currentNode->left);


        printf("Destination: %s\t weight: %.2f Gms\t valuation: $ %.2f \n", currentNode->destination, currentNode->weight, currentNode->valuation);


        PrintAllParcels(currentNode->right);
    }
}

//
// FUNCTION : PrintParcelsByWeight
// DESCRIPTION :
// this function which traverse the tree in inorder and display the details of the parcels which are lighter or heavier than the reference weight
// PARAMETERS :
// struct ParcelNode* currentNode : it is a pointer to the current node
// int referenceWeight : it is the reference weight  to compare with the weight of the parcel 
// int isAboveThreshold : it is a flag which will indicate if the parcel is lighter or heavier than the reference weight
// RETURNS :
// void
//

void PrintParcelsByWeight(struct ParcelNode* currentNode, int referenceWeight, int isAboveThreshold) {
    if (currentNode == NULL) {
        return;
    }
    else {
        PrintParcelsByWeight(currentNode->left, referenceWeight, isAboveThreshold);


        if ((isAboveThreshold && currentNode->weight < referenceWeight) || (!isAboveThreshold && currentNode->weight > referenceWeight)) {
            printf("Destination: %s\t weight: %.2f Gms\t valuation: $ %.2f \n", currentNode->destination, currentNode->weight, currentNode->valuation);
        }


        PrintParcelsByWeight(currentNode->right, referenceWeight, isAboveThreshold);
    }
}

//
// FUNCTION : AccumulateTotals
// DESCRIPTION :
// this function which traverse the tree in inorder and calculate the total weight and valuation of all the parcels
// PARAMETERS :
// struct ParcelNode* currentNode : it is a pointer to the current node
// float* accumulatedWeight : it is a pointer to the variable that accumulate the total weight
// float* accumulatedValuation : it is a pointer to the variable that accumulate the total valuation
// RETURNS :
// void
//

void AccumulateTotals(struct ParcelNode* currentNode, float* accumulatedWeight, float* accumulatedValuation) {
    if (currentNode == NULL) {
        return;
    }
    else {

        AccumulateTotals(currentNode->left, accumulatedWeight, accumulatedValuation);


        *accumulatedWeight += currentNode->weight;
        *accumulatedValuation += currentNode->valuation;


        AccumulateTotals(currentNode->right, accumulatedWeight, accumulatedValuation);
    }
}

//
// FUNCTION : findMinAndMaxFlightPrice
// DESCRIPTION :
// this function which traverse the tree in inorder and find the cheapest and most expensive parcels
// PARAMETERS :
// struct ParcelNode* currentNode : it is a pointer to the current node
// struct ParcelNode** min : it is a pointer to the variable that store the cheapest parcel
// struct ParcelNode** max : it is a pointer to the variable that store the most expensive parcel
// RETURNS :
// void
//

void findMinAndMaxFlightPrice(struct ParcelNode* currentNode, struct ParcelNode** min, struct ParcelNode** max) {
    if (currentNode == NULL) {

        return;
    }
    else {

        if (*min == NULL || currentNode->valuation < (*min)->valuation) {
            *min = currentNode;
        }

        if (*max == NULL || currentNode->valuation > (*max)->valuation) {
            *max = currentNode;
        }

        findMinAndMaxFlightPrice(currentNode->left, min, max);

        findMinAndMaxFlightPrice(currentNode->right, min, max);
    }
}

//
// FUNCTION : findMinAndMaxWeight
// DESCRIPTION :
// this function which traverse the tree in inorder and find the lightest and heaviest parcels
// PARAMETERS :
// struct ParcelNode* currentNode : it is a pointer to the current node
// struct ParcelNode** minWeight : it is a pointer to the variable that will be updated with the lightest parcel
// struct ParcelNode** maxWeight : it is a pointer to the variable that will be updated with the heaviest parcel
// RETURNS :
// void
//


void findMinAndMaxWeight(struct ParcelNode* currentNode, struct ParcelNode** minWeight, struct ParcelNode** maxWeight) {
    if (currentNode == NULL) {

        return;
    }
    else {

        if (*minWeight == NULL || currentNode->weight < (*minWeight)->weight) {
            *minWeight = currentNode;
        }

        if (*maxWeight == NULL || currentNode->weight > (*maxWeight)->weight) {
            *maxWeight = currentNode;
        }

        findMinAndMaxWeight(currentNode->left, minWeight, maxWeight);

        findMinAndMaxWeight(currentNode->right, minWeight, maxWeight);
    }
}

//
// FUNCTION : destroyHashTable
// DESCRIPTION :
// this function will free the memory allocated for a hash table of parcel nodes. 
// PARAMETERS :
// struct ParcelHashTable* table : it is a pointer to the hash table
// RETURNS :
// void
//

void destroyHashTable(struct ParcelHashTable* table) {

    for (int i = 0; i < BUCKET_COUNT; i++) {

        freeParcelTree(table->table[i]);
    }

    free(table);
}

//
// FUNCTION : createParcelHashTable
// DESCRIPTION :
// this funcrion will create a new hash table and allocate memory
// PARAMETERS :
// RETURNS :
// this will return a pointer to the newly created hash table
//

struct ParcelHashTable* createParcelHashTable(void) {
    struct ParcelHashTable* parcelHashTable = (struct ParcelHashTable*)malloc(sizeof(struct ParcelHashTable));
    if (parcelHashTable == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    for (int i = 0; i < BUCKET_COUNT; i++) {
        parcelHashTable->table[i] = NULL;
    }

    return parcelHashTable;
}

//
// FUNCTION : InsertParcelNode
// DESCRIPTION :
// this funcrion insert the new node in the tree
// PARAMETERS :
// ParcelNode* root : it is a pointer to the root node of the tree
// char* destination : it a string which represent the destination of the parcel
// float weight : it is the weight of the parcel
// float valuation : it is the valuation of the parcel
// RETURNS :
// this will return a pointer to the root node of the tree
//

struct ParcelNode* InsertParcelNode(ParcelNode* root, char* destination, float weight, float valuation) {
    if (root == NULL) {
        return createParcelNode(destination, weight, valuation);
    }
    if (weight < root->weight) {
        root->left = InsertParcelNode(root->left, destination, weight, valuation);
    }
    else if (weight > root->weight) {
        root->right = InsertParcelNode(root->right, destination, weight, valuation);
    }

    return root;
}


//
// FUNCTION : freeParcelTree
// DESCRIPTION :
// this function will free the memory allocated for the tree and its nodes 
// PARAMETERS :
// struct ParcelNode* currentNode : it is a pointer to the current node
// RETURNS :
// void
//

void freeParcelTree(struct ParcelNode* currentNode) {
    if (currentNode == NULL) {

        return;
    }
    else {

        freeParcelTree(currentNode->left);

        freeParcelTree(currentNode->right);

        free(currentNode->destination);

        free(currentNode);
    }
}
