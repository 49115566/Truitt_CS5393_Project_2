#ifndef _AVL_H_
#define _AVL_H_

/*
AVL:
-Used for graph to have easy access to each adjacency list and user
-Very normal AVL tree
-Stores users as pointers
*/

#include "adjList.h"
using namespace std;

// Node structure for an AVL tree
struct tNode {
    tNode* left;  // Pointer to the left child of the node
    tNode* right;  // Pointer to the right child of the node
    user* val;  // Value stored in the node (user pointer)
    int height;  // Integer representing the height of the node

    tNode(user* b) : left(nullptr), right(nullptr), val(b), height(1) {}  // Constructor to initialize a tree node with a user pointer
};

// AVL tree class definition (self-balancing binary search tree)
class AVL {
private:
    tNode* head;  // Pointer to the root node of the AVL tree

    tNode* insertRec(tNode* node, user* v);  // Recursive method to insert and balance the tree
    user* retrieveRec(tNode* node, string k) const;  // Recursive method to retrieve a user by username
    tNode* removeRec(tNode* node, string k);  // Recursive method to remove and balance the tree
    void getArrRec(tNode* node, user** arr, int* i) const;  // Recursive method to fill an array with users in the tree

    int height(tNode* N);  // Utility method to return the height of a node
    tNode* rotateRight(tNode* y);  // Utility method for right rotation
    tNode* rotateLeft(tNode* x);  // Utility method for left rotation
    int getBalance(tNode* N);  // Utility method to get the balance factor of a node

public:
    AVL();  // Constructor
    ~AVL();  // Destructor

    user** getArr(int len) const;  // Method to get an array of all users in the tree
    bool insert(user* k);  // Method to insert a user into the tree
    bool remove(string k);  // Method to remove a user by username
    user* retrieve(string k) const;  // Method to retrieve a user by username
};

AVL::AVL() : head(nullptr) {}  // Constructor to initialize the AVL tree with an empty head

AVL::~AVL() {  // Destructor for the AVL tree
    while (head) remove(head->val->username);  // Remove all nodes in the tree
}

bool AVL::insert(user* k) {  // Insert a user into the AVL tree
    if (retrieve(k->username)) return false;  // If the user already exists, return false
    head = insertRec(head, k);  // Insert the user and update the tree
    return true;  // Return true if insertion was successful
}

bool AVL::remove(string k) {  // Remove a user from the AVL tree by username
    if (!retrieve(k)) return false;  // If the user doesn't exist, return false
    head = removeRec(head, k);  // Remove the user and update the tree
    return true;  // Return true if removal was successful
}

user* AVL::retrieve(string k) const {  // Retrieve a user from the AVL tree by username
    return retrieveRec(head, k);  // Call the recursive method to retrieve the user
}

user** AVL::getArr(int len) const {  // Get an array of all users in the AVL tree
    user** arr = new user*[len];  // Create an array of user pointers
    int i = 0;  // Initialize index for array
    getArrRec(head, arr, &i);  // Fill the array with users using recursion
    return arr;  // Return the array of users
}

int AVL::height(tNode* N) {  // Utility method to return the height of a node
    if (!N) return 0;  // If the node is null, return height 0
    return N->height;  // Return the height of the node
}

tNode* AVL::rotateRight(tNode* y) {  // Utility method for right rotation
    tNode* x = y->left;  // Set x as the left child of y
    tNode* T2 = x->right;  // Set T2 as the right child of x

    // Perform rotation
    x->right = y;  // Set y as the right child of x
    y->left = T2;  // Set T2 as the left child of y

    // Update heights
    y->height = max(height(y->left), height(y->right)) + 1;  // Update height of y
    x->height = max(height(x->left), height(x->right)) + 1;  // Update height of x

    return x;  // Return the new root
}

tNode* AVL::rotateLeft(tNode* x) {  // Utility method for left rotation
    tNode* y = x->right;  // Set y as the right child of x
    tNode* T2 = y->left;  // Set T2 as the left child of y

    // Perform rotation
    y->left = x;  // Set x as the left child of y
    x->right = T2;  // Set T2 as the right child of x

    // Update heights
    x->height = max(height(x->left), height(x->right)) + 1;  // Update height of x
    y->height = max(height(y->left), height(y->right)) + 1;  // Update height of y

    return y;  // Return the new root
}

int AVL::getBalance(tNode* N) {  // Utility method to get the balance factor of a node
    if (!N) return 0;  // If the node is null, return balance factor 0
    return height(N->left) - height(N->right);  // Return the difference in height between left and right subtrees
}

tNode* AVL::insertRec(tNode* node, user* v) {  // Recursive method to insert and balance the tree
    if (!node) return new tNode(v);  // If the node is null, create a new tree node with the user

    // Insert the user into the correct subtree
    if (v->username < node->val->username)
        node->left = insertRec(node->left, v);  // Insert in the left subtree
    else if (v->username > node->val->username)
        node->right = insertRec(node->right, v);  // Insert in the right subtree
    else return node;  // If the username is the same, return the current node (no duplicates)

    // Update the height of this node
    node->height = 1 + max(height(node->left), height(node->right));

    // Get the balance factor of this node
    int balance = getBalance(node);

    // If the node is unbalanced, perform rotations
    if (balance > 1 && v->username < node->left->val->username)  // Left Left case
        return rotateRight(node);
    if (balance < -1 && v->username > node->right->val->username)  // Right Right case
        return rotateLeft(node);
    if (balance > 1 && v->username > node->left->val->username) {  // Left Right case
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && v->username < node->right->val->username) {  // Right Left case
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;  // Return the (potentially) balanced node
}

user* AVL::retrieveRec(tNode* node, string k) const {  // Recursive method to retrieve a user by username
    if (!node) return nullptr;  // If the node is null, return null (user not found)

    // Traverse the tree to find the user
    if (k < node->val->username)
        return retrieveRec(node->left, k);  // Search in the left subtree
    if (k > node->val->username)
        return retrieveRec(node->right, k);  // Search in the right subtree

    return node->val;  // If the user is found, return the user
}

tNode* AVL::removeRec(tNode* node, string k) {  // Recursive method to remove and balance the tree
    if (!node) return node;  // If the node is null, return null

    // Traverse the tree to find the user to remove
    if (k < node->val->username)
        node->left = removeRec(node->left, k);  // Remove from the left subtree
    else if (k > node->val->username)
        node->right = removeRec(node->right, k);  // Remove from the right subtree
    else {  // User to be removed is found
        if (!node->left || !node->right) {  // Node with only one child or no child
            tNode* temp = node->left ? node->left : node->right;  // Get the non-null child (if any)
            if (!temp) {  // If no child, just delete the node
                temp = node;
                node = nullptr;
            }
            else *node = *temp;  // Copy the contents of the non-empty child
            delete temp;  // Delete the node
        }
        else {  // Node with two children
            tNode* temp = node->right;  // Find the smallest node in the right subtree
            while (temp->left) temp = temp->left;  // Traverse to the leftmost leaf

            node->val = temp->val;  // Copy the value of the smallest node
            node->right = removeRec(node->right, temp->val->username);  // Remove the smallest node
        }
    }

    if (!node) return node;  // If the tree has only one node, return it

    // Update the height of the current node
    node->height = 1 + max(height(node->left), height(node->right));

    // Get the balance factor of this node
    int balance = getBalance(node);

    // If the node is unbalanced, perform rotations
    if (balance > 1 && getBalance(node->left) >= 0)  // Left Left case
        return rotateRight(node);
    if (balance > 1 && getBalance(node->left) < 0) {  // Left Right case
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0)  // Right Right case
        return rotateLeft(node);
    if (balance < -1 && getBalance(node->right) > 0) {  // Right Left case
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }

    return node;  // Return the (potentially) balanced node
}

void AVL::getArrRec(tNode* node, user** arr, int* i) const {  // Recursive method to fill an array with users in the tree
    if (!node) return;  // If the node is null, return
    getArrRec(node->left, arr, i);  // Traverse the left subtree
    arr[(*i)++] = node->val;  // Add the user to the array
    getArrRec(node->right, arr, i);  // Traverse the right subtree
}

#endif