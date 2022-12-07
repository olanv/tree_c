#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// FLAGS:

    // Aucune spécification, on souhaite juste trouver une node spécifique
#define SEARCH_FLAG 0
    // Permet de créer une nouvelle node si la clef indiquée n'existe pas
#define CREATE_NODE_FLAG 1
    // Permet de renvoyer NULL si la clef existe déjà dans l'arbre
#define ERR_IF_NODE_EXISTS_FLAG 2

    // Flag de debug pour l'affichage des différentes étapes
#define DEBUG_FLAG 4
    // Flags permettant de spécifier la branche sur laquelle on travaille
#define LEFT_BRANCH_FLAG 8
#define RIGHT_BRANCH_FLAG 16

#define MAX_KEY 100

typedef struct node node_t;

struct node {
    unsigned int key;
    int value;
    node_t *left_child;
    node_t *right_child;
};

node_t *getStepNode(node_t *actual_node, node_t *next_node, unsigned int wanted_key, int flags); 
node_t *getNode(node_t *actual_node, unsigned int wanted_key, int flags);
node_t *newNode(unsigned int key);

int insertNode(node_t *node_head, unsigned key, int val, int DEBUG);
node_t *searchNode(node_t *node_head, unsigned wanted_key);

node_t *generateRandTree(int size, int DEBUG); 

/// @brief Libère la mémoire de l'arbre
/// @param head tête de l'arbre
void freeTree(node_t *head) {
    if (head->left_child) {
        freeTree(head->left_child);
    } 
    if (head->right_child) {
        freeTree(head->right_child);
    }
    free(head);
}

/// @brief Alloue la mémoire pour une nouvelle node
/// @param key clef de la node
/// @return la node créé
///         NULL si non créé
node_t *newNode(unsigned key) {
    node_t * new_node = (node_t*)malloc(sizeof(node_t));
    new_node->key = key;
    new_node->left_child = NULL;
    new_node->right_child = NULL;
    return new_node;
}

/// @brief Suite générique de getNode, la notion de "gauche" ou "droite" est abstraite ici
/// @param actual_node étape de la recherche récursive
/// @param wanted_key Clef recherché
/// @param flags changent le fonctionnement de la fonction voir les defines
/// @return renvoie la node cherchée ou NULL en fonction des flags
node_t *getStepNode(node_t *actual_node, node_t *next_node, unsigned int wanted_key, int flags) {
        if (flags & DEBUG_FLAG) printf("\tlooking %s child\n", (flags & RIGHT_BRANCH_FLAG ? "right" : "left"));

        // Si la prochaine node n'existe pas
        if ((flags & LEFT_BRANCH_FLAG ? actual_node->left_child : actual_node->right_child) == NULL) {
            if (flags & DEBUG_FLAG) printf("\t\tdoesn't exists\n");

            // Créé la node si le flag le demande
            if (flags & CREATE_NODE_FLAG) {
                if (flags & DEBUG_FLAG) printf("\t\t\tHas been created\n");

                if (flags & LEFT_BRANCH_FLAG) {
                    next_node = newNode(wanted_key);
                    actual_node->left_child = next_node;
               //     actual_node->left_child = newNode(wanted_key);
                } else if (flags & RIGHT_BRANCH_FLAG) {
                    next_node = newNode(wanted_key);
                    actual_node->right_child = next_node;
                //    actual_node->right_child = newNode(wanted_key);
                }
                
                return next_node;
            
            // Sinon renvoyer NULL
            } else {
                if (flags & DEBUG_FLAG) printf("Cannot create new node\n");
                return NULL;
            }
        }
        if (flags & DEBUG_FLAG) printf("Looking for the %s child (key: %d):\n", (flags & RIGHT_BRANCH_FLAG ? "right" : "left"), next_node->key);
       
        // On rappelle getNode sur la node suivante
        return getNode(next_node, wanted_key, flags);
}

/// @brief Permet de récupérer ou non une node avec une clef spécifique en fonction des flags
/// RECURSIF
/// @param actual_node étape de la recherche récursive
/// @param wanted_key Clef recherché
/// @param flags changent le fonctionnement de la fonction voir les defines
/// @return renvoie la node cherchée ou NULL en fonction des flags
node_t *getNode(node_t *actual_node, unsigned int wanted_key, int flags) {
    node_t *next_node = NULL;
    int branch = 0;

    if (actual_node->key == wanted_key) {
        if (flags & DEBUG_FLAG) printf("FOUND: actual node key %d\n", wanted_key);
        
        if (flags & ERR_IF_NODE_EXISTS_FLAG) return NULL;
        return actual_node;
    }

    if (flags & DEBUG_FLAG) 
        printf("actual node: %d\n", actual_node->key);

    if (actual_node->key > wanted_key) {
        // On continue la recherche sur la branche de gauche
        next_node = actual_node->left_child;
        branch = LEFT_BRANCH_FLAG;
    }
    else if (actual_node->key < wanted_key) {
        // On continue la recherche sur la branche de droite
        next_node = actual_node->right_child;
        branch = RIGHT_BRANCH_FLAG;
    }
    return getStepNode(actual_node, next_node, wanted_key, flags | branch);
}

/// @brief Insert une node dans un arbre binaire
/// @param node_head Tête de l'arbre
/// @param key clef de la node à insérer
/// @param val valeur de la node à insérer
/// @return 
///     1 : création impossible car la clef existe déjà
///     0 : création réussie
int insertNode(node_t *node_head, unsigned key, int val, int DEBUG) {
    node_t *new_node;
    
    // Le flag "CREATE_NODE_FLAG" permet de spécifié que l'on veut créé la node si elle n'existe pas
    // Le flag "ERR_IF_NODE_EXISTS_FLAG" permet de renvoyer NULL si la node existe déjà 
    if (!(new_node = getNode(node_head, key, CREATE_NODE_FLAG | ERR_IF_NODE_EXISTS_FLAG | DEBUG)))
        return 1;
    // Pour afficher les différentes étapes, ajouter le flag "DEBUG_FLAG" comme ci-dessous
    // if (!(new_node = getNode(node_head, key, CREATE_NODE_FLAG | ERR_IF_NODE_EXISTS_FLAG | DEBUG_FLAG)))
    //    return 1;
    new_node->value = val;
    return 0;
}

/// @brief recherche une node avec une clef spécifique
/// @param node_head tête de l'arbre
/// @param wanted_key clef recherché
/// @return 
///     NULL: si la clef n'existe pas dans l'arbre
///     node_t : La node comportant la clef spécifiée
node_t *searchNode(node_t *node_head, unsigned wanted_key) {
    node_t *node = getNode(node_head, wanted_key, SEARCH_FLAG);
    // Pour afficher les différentes étapes, ajouter le flag "DEBUG_FLAG" comme ci-dessous
    // node_t *node = getNode(node_head, wanted_key, SEARCH_FLAG | DEBUG_FLAG);
    return node;
}


/// @brief Génère un arbre de cellules avec des valeurs et clefs aléatoires 
/// @param size: nombre de cellules de l'arbres
/// @return la tête de l'abre
node_t *generateRandTree(int size, int DEBUG) {
    if (size == 0) return NULL;
    
    srand(time(NULL));

// On créé la tête de l'arbre avec une val aléatoire
    node_t *tree_head = newNode(rand()%MAX_KEY);
    tree_head->value = rand();

// On insert size-1 node(s) dans l'abre
    for (int i = 0 ; i < size-1 ; i++)
    {
        // On génère une nouvelle clef
        int new_key = rand()%MAX_KEY;
        if (DEBUG) {
            printf("new key generated: %d\n", new_key);
        }
        // Si l'insertion a echoué, on revient en arrière
        // L'insertion est censé echoué si la clef "new_key" existe déjà
        if(insertNode(tree_head, new_key, rand(), DEBUG)) 
            i--;
    }
    return tree_head;
} 

/// @brief Test la fonctionalité de recherche de l'arbre
/// @param tree_head tête de l'arbre
/// @return 0 si recherche fonctionne, 1 sinon
int testSearchTree(node_t *tree_head) {
    node_t * try_node = NULL;
    int i = 0;

    while(try_node == NULL) {
        printf("Searching node %d: \n", i);
        try_node = searchNode(tree_head, i++);
        if (i >= MAX_KEY) {
            printf("SEARCH MODULE DOESNT WORK\n");
            return 1;
        }
    }
    printf("node found has %d val in it\n",try_node->value);
    return 0;
}

int main() {
    node_t *tree_head = generateRandTree(5, DEBUG_FLAG);

    if (testSearchTree(tree_head))
        printf("test research doesn't work\n");

    freeTree(tree_head);

    return 0;
}