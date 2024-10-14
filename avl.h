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
	tNode* left;                    // Pointer to the left child of the node
	tNode* right;                   // Pointer to the right child of the node
	user* val;                     // Value stored in the node
	int height;                     // Integer representing the height of the node

	// Constructor to initialize a tree node with user*
	tNode(user* b) : left(nullptr), right(nullptr), val(b), height(1) {}  // Initialize pointers and height
};

// AVL tree class definition (self-balancing binary search tree)
class AVL {
private:
	tNode* head;                    // Pointer to the root node of the AVL tree

	tNode* insertRec(tNode* node, user* v);    // Recursive method to insert and balance the tree
	user* retrieveRec(tNode* node, string username);  // Recursive method to retrieve a value
	tNode* removeRec(tNode* node, string username);    // Recursive method to remove and balance the tree
	tNode* rotateRight(tNode* y);    // Method to perform a right rotation
	tNode* rotateLeft(tNode* x);     // Method to perform a left rotation
	tNode* balance(tNode* node);     // Method to balance the AVL tree
	int height(tNode* node);         // Method to return the height of a node
	int getBalance(tNode* node);     // Method to get the balance factor of a node
	tNode* findMin(tNode* node);     // Method to find the node with the minimum value
	void deleteTree(tNode* node);    // Recursive method to delete the entire tree

public:
	AVL();                          // Constructor to initialize the AVL tree
	~AVL();                         // Destructor to clean up the AVL tree
	void insert(user* v);           // Method to insert a value into the AVL tree
	user* retrieve(string v);       // Method to retrieve a value
	void remove(string v);           // Method to remove a value
};

// Constructor for the AVL tree
AVL::AVL() : head(nullptr) {}       // Initialize the head of the tree to nullptr

// Destructor for the AVL tree
AVL::~AVL() {
	deleteTree(head);               // Call the recursive deleteTree method to clean up
}

// Recursive method to delete the entire AVL tree
void AVL::deleteTree(tNode* node) {
	if (node) {                     // If the node is not null
		deleteTree(node->left);      // Recursively delete the left subtree
		deleteTree(node->right);     // Recursively delete the right subtree
        delete node->val;           //Delete the node's value and by destructor its connections as well
		delete node;                 // Delete the current node
	}
}

// Public method to insert a value into the AVL tree
void AVL::insert(user* v) {
	head = insertRec(head, v);      // Call the recursive insert method, starting from the root
}

// Recursive method to insert a node into the AVL tree and balance it
tNode* AVL::insertRec(tNode* node, user* v) {
	if (!node) {
		return new tNode(v); // If the node is null, create a new node
	}

	if (v->username < node->val->username) {            // If the value is less, insert into the left subtree
		node->left = insertRec(node->left, v);
	}
	else if (v->username > node->val->username) {       // If the value is greater, insert into the right subtree
		node->right = insertRec(node->right, v);
	}
	else {                          // If the value already exists, do nothing
		return node;
	}

	int hl = height(node->left), hr = height(node->right);
	node->height = 1 + (hl > hr ? hl : hr);  // Update the node's height
	return balance(node);           // Balance the tree and return the node
}

// Public method to retrieve a value
user* AVL::retrieve(string username) {
	return retrieveRec(head, username);    // Call the recursive retrieve method
}

// Recursive method to retrieve a value
user* AVL::retrieveRec(tNode* node, string username) {
	if (!node) return nullptr;          // If the node is null, return NAN
	if (username == node->val->username) {           // If the value matches, return it
		return node->val;
	}
	else if (username < node->val->username) {       // If the value is less, search the left subtree
		return retrieveRec(node->left, username);
	}
	else {                          // If the value is greater, search the right subtree
		return retrieveRec(node->right, username);
	}
}

// Public method to remove a value
void AVL::remove(string username) {
	head = removeRec(head, username);      // Call the recursive remove method
}

// Recursive method to remove a node and balance the tree
tNode* AVL::removeRec(tNode* node, string username) {
	if (!node) {
		return nullptr;      // If the node is null, return null
	}

	if (username < node->val->username) {            // If the value is less, search the left subtree
		node->left = removeRec(node->left, username);
	}
	else if (username > node->val->username) {       // If the value is greater, search the right subtree
		node->right = removeRec(node->right, username);
	}
	else {                          // If the node to be deleted is found
		if (!node->left || !node->right) { // If the node has one or no children
			tNode* temp = node->left ? node->left : node->right; // Choose the non-null child
			if (!temp) {             // If there are no children
				temp = node;         // Temporarily store the node
				node = nullptr;      // Set the node to null
			}
			else {
				*node = *temp;       // Copy the non-null child to the current node
			}
            delete temp->val;       //Delete the node's value and by nature its connections
			delete temp;             // Delete the temporary node
		}
		else {                      // If the node has two children
			tNode* temp = findMin(node->right); // Find the in-order successor
			node->val = temp->val;   // Replace the current node's value with the successor's
			node->right = removeRec(node->right, temp->val->username); // Remove the successor
		}
	}

	if (!node) return node;         // If the tree is empty, return null

	int hl = height(node->left), hr = height(node->right);
	node->height = 1 + (hl > hr ? hl : hr);  // Update the node's height
	return balance(node);           // Balance the tree and return the node
}

// Helper method to find the node with the minimum value
tNode* AVL::findMin(tNode* node) {
	while (node->left) {            // Traverse to the leftmost node
		node = node->left;
	}
	return node;                    // Return the node with the minimum value
}

// Method to perform a right rotation to balance the tree
tNode* AVL::rotateRight(tNode* y) {
	tNode* x = y->left;             // Set x as the left child of y
	tNode* T2 = x->right;           // Store the right subtree of x

	x->right = y;                   // Perform the rotation (x becomes the new root)
	y->left = T2;                   // Move T2 to the left of y

	int hl = height(y->left), hr = height(y->right);
	y->height = (hl > hr ? hl : hr) + 1;  // Update height of y
	hl = height(x->left), hr = height(x->right);
	x->height = (hl > hr ? hl : hr) + 1;  // Update height of x

	return x;                       // Return the new root
}

// Method to perform a left rotation to balance the tree
tNode* AVL::rotateLeft(tNode* x) {
	tNode* y = x->right;            // Set y as the right child of x
	tNode* T2 = y->left;            // Store the left subtree of y

	y->left = x;                    // Perform the rotation (y becomes the new root)
	x->right = T2;                  // Move T2 to the right of x

	int hl = height(y->left), hr = height(y->right);
	y->height = (hl > hr ? hl : hr) + 1;  // Update height of y
	hl = height(x->left), hr = height(x->right);
	x->height = (hl > hr ? hl : hr) + 1;  // Update height of x

	return y;                       // Return the new root
}

// Method to balance the AVL tree after insertion or deletion
tNode* AVL::balance(tNode* node) {
	int balanceFactor = getBalance(node);  // Get the balance factor of the current node

	if (balanceFactor > 1) {          // Left-heavy case
		if (getBalance(node->left) >= 0) {
			return rotateRight(node);  // Left-left case
		}
		else {
			node->left = rotateLeft(node->left); // Left-right case
			return rotateRight(node);
		}
	}

	if (balanceFactor < -1) {         // Right-heavy case
		if (getBalance(node->right) <= 0) {
			return rotateLeft(node);   // Right-right case
		}
		else {
			node->right = rotateRight(node->right); // Right-left case
			return rotateLeft(node);
		}
	}

	return node;                      // If balanced, return the node
}

// Method to get the height of a node (helper for balancing)
int AVL::height(tNode* node) {
	if (!node) return 0;            // If the node is null, return 0
	return node->height;            // Otherwise, return the height of the node
}

// Method to get the balance factor of a node
int AVL::getBalance(tNode* node) {
	if (!node) return 0;            // If the node is null, return balance factor 0
	return height(node->left) - height(node->right);  // Return the difference in heights
}

#endif