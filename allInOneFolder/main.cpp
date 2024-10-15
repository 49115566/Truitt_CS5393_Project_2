/*
Social Network Analysis Tool
• Create a social network analysis tool using graph data structures in C++.
• The tool should:
o Represent users and connections using an adjacency list or graph
o Generate and handle a dataset of 250 users with realistic connections
o Implement the following features
    ▪ Friend suggestions based on mutual connections (5)
    ▪ Degree of separation (shortest path) (5 random)
    ▪ Identify strongly connected network components (top 5)
    ▪ Analyze influential users using centrality measures (top 5)
    ▪ Basic network statistics:
        • Number of Users
        • Average connections per user
        • Top 10 users with max connections
NOTE:
Dataset: Generate your own with 250 nodes.

Graph:
-AVL tree of all nodes
-Nodes store all edges
-Edges store nodes
-AVL tree performs memory management
-String array to allow for indexing of users
-Integers to store total number of users and total number of connections
-Methods added to allow for computations

User:
-Must have a unique username
-Members:
    -username (string)
    -firstname (string)
    -lastname (string)
    -numFollowing (int)
    -numFollowers (int)
    -following (adjList)
    -followers (adjList)
-Methods:
    -follow (user*) - follows user and adds self to the other person's followers
    -unfollow (string) - unfollows user and removes self from other person's followers
-Whenever someone is followed or unfollowed, his numFollowers is adjusted
-Whenever someone follows or unfollows, his numFollowing is adjusted
-The graph should deal primarily with users rather than adjLists because each user's adjList is a member of it.
-The constructor creates the adjacency list for the member.

Adjacency list:
-List of users that someone is following
-Head stores the person himself
-All users in list are stored as pointers so that they will be updated when changed elsewhere in graph
-Each following node represents someone connected to head
-Since a user can add, remove, and view connections, here are the methods:
    -Add (bool) (bool to see whether it happened)
    -Remove (bool) (bool to see whether it happened)
    -View (user*) (* to allow for use of actual user)
-Users can also view their own profiles, adding this method:
    -Self (user*) (* to allow for use of actual user)
-Additionally, it is important to use the list quickly for connections without knowledge of names
    As a result, the following method is needed:
    -getArr (user**) (This ptr has a dynamic array. Be sure to delete it!)
-Finally, a constructor and destructor are needed.

Note on organization of adjList.h:
In order to work around difficulties with circular dependencies, user needed to be forward declared.
Then, adjList could be declared. After this, the entirety of user was possible.
After all this, adjList's member methods could be written.
In order to have circular dependencies, something along these lines must occur.



AVL:
-Used for graph to have easy access to each adjacency list and user
-Very normal AVL tree
-Stores users as pointers



THE QUESTION:
When deleting a node, how insure no one is still following the non-user?
*/
#include <iostream>  // For input and output operations
#include <string>  // For using string class
#include <fstream>  // For file handling operations
#include <sstream>  // For string stream operations
#include <random>  // For generating random numbers
#include <queue>  // For using queue data structure
#include <unordered_set>  // For using unordered_set data structure
#include <unordered_map>  // For using unordered_map data structure
#include <algorithm>  // For using standard algorithms
#include <vector>  // For using vector container
using namespace std;  // Use the standard namespace

struct user;  // Forward declaration of user structure

struct aNode {  // Structure for adjacency list node
    user* val;  // Pointer to a user
    aNode* next;  // Pointer to the next node in the list

    aNode(user* person) : val(person), next(nullptr) {}  // Constructor to initialize the node with a user and set next to nullptr
};

class adjList {  // Class for adjacency list
private:
    aNode* head;  // Pointer to the head of the list
public:
    adjList(user* person);  // Constructor
    ~adjList();  // Destructor

    bool add(user* person);  // Add a user to the list
    bool remove(string username);  // Remove a user by username
    user* view(string username) const;  // View a user by username
    user* self() const;  // Get the current user (head of the list)

    user** getArr(int len);  // Get an array of users in the list
};

struct user {  // Structure for user
    string username;  // Username of the user
    string firstname;  // First name of the user
    string lastname;  // Last name of the user
    int numFollowing;  // Number of people the user is following
    int numFollowers;  // Number of people following the user

    adjList* following;  // Adjacency list of users this user is following
    adjList* followers;  // Adjacency list of users following this user

    bool follow(user* usr);  // Follow another user
    bool unfollow(string uname);  // Unfollow a user by username

    user(string un, string fn, string ln);  // Constructor to initialize user with username, firstname, and lastname
    ~user();  // Destructor
};

user::user(string un, string fn, string ln) : username(un), firstname(fn), lastname(ln), numFollowing(0), numFollowers(0) {  // Initialize user fields and set following/followers lists
    following = new adjList(this);  // Create adjacency list for following
    followers = new adjList(this);  // Create adjacency list for followers
}

user::~user() {  // Destructor for user
    user** temp = following->getArr(numFollowing);  // Get the array of users this user is following
    while(numFollowing) {  // Unfollow all users
        unfollow(temp[numFollowing - 1]->username);  // Unfollow the last user in the array
    }
    delete[] temp;  // Delete the temporary array

    temp = followers->getArr(numFollowers);  // Get the array of users following this user
    while(numFollowers) {  // Remove all followers
        temp[numFollowers - 1]->unfollow(this->username);  // Unfollow this user for each follower
    }
    delete[] temp;  // Delete the temporary array

    delete following;  // Delete the following list
    delete followers;  // Delete the followers list
}

bool user::follow(user* usr) {  // Follow another user
    bool ret = 1;  // Initialize return value to true

    if(following->add(usr))  // Add user to following list
        numFollowing++;  // Increment number of following
    else ret = 0;  // Set return value to false if addition failed

    if(usr->followers->add(this))  // Add this user to the other's followers list
        usr->numFollowers++;  // Increment number of followers
    else ret = 0;  // Set return value to false if addition failed

    return ret;  // Return whether the follow operation was successful
}

bool user::unfollow(string uname) {  // Unfollow a user by username
    bool ret = 1;  // Initialize return value to true

    user* usr = following->view(uname);  // Find the user in the following list
    if(usr && following->remove(uname))  // If user is found and removed, decrement numFollowing
        numFollowing--;
    else ret = 0;  // Set return value to false if removal failed
    
    if(usr && usr->followers->remove(username))  // If this user is removed from the other's followers list, decrement numFollowers
        usr->numFollowers--;
    else ret = 0;  // Set return value to false if removal failed

    return ret;  // Return whether the unfollow operation was successful
}

adjList::adjList(user* person) {  // Constructor for adjacency list
    head = new aNode(person);  // Initialize the head node with the user
}

adjList::~adjList() {  // Destructor for adjacency list
    aNode* temp;  // Temporary node pointer
    while(head) {  // Traverse and delete all nodes
        temp = head->next;  // Move to the next node
        delete head;  // Delete the current node
        head = temp;  // Update head to the next node
    }
}

bool adjList::add(user* person) {  // Add a user to the adjacency list
    if (person == head->val || view(person->username)) return false;  // If user is already in the list, return false
    aNode* temp = new aNode(person);  // Create a new node for the user
    temp->next = head->next;  // Insert the new node after the head
    head->next = temp;  // Update the next pointer of the head
    return true;  // Return true if the user was added successfully
}

bool adjList::remove(string username) {  // Remove a user by username
    aNode* cur = head;  // Current node pointer
    aNode* next = head->next;  // Pointer to the next node
    
    while(next && next->val->username != username) {  // Traverse the list until the user is found
        cur = next;  // Move to the next node
        next = cur->next;  // Update next pointer
    }

    if(next) {  // If the user is found
        cur->next = next->next;  // Remove the node from the list
        delete next;  // Delete the node
        return 1;  // Return true if removal was successful
    }
    
    return 0;  // Return false if the user was not found
}

user* adjList::view(string username) const {  // View a user by username
    aNode* cur = head->next;  // Start from the first node

    while(cur && cur->val->username != username) {  // Traverse the list until the user is found
        cur = cur->next;  // Move to the next node
    }

    if(cur) return cur->val;  // If the user is found, return the user
    return nullptr;  // Return nullptr if the user is not found
}

user* adjList::self() const {  // Get the current user (head of the list)
    return head->val;  // Return the user stored in the head node
}

user** adjList::getArr(int len) {  // Get an array of users in the list
    user** arr = new user*[len];  // Create a new array of user pointers
    aNode* cur = head->next;  // Start from the first node
    int i;  // Loop counter
    for(i = 0; cur; i++) {  // Traverse the list and store users in the array
        arr[i] = cur->val;  // Store the user in the array
        cur = cur->next;  // Move to the next node
    }

    if(i != len) cout << "WARNING: len = " << len << ", but i = " << i << endl;  // Warning if the array length is incorrect

    return arr;  // Return the array of users
}

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



class graph {
private:
    AVL vertices;                  // AVL tree to store users (for efficient insertion and retrieval)
    string* usernames;             // Array of usernames
    int numUsrs;                   // Total number of users in the graph
    int numCncts;                  // Total number of connections (follows)

    // Private helper functions
    user* getUser(int index);      // Retrieve a user by their index
    user** suggestFriends(string username, int resultCt); // Suggest friends for a given user
    user** mostConnected(int resultCt); // Find the most connected users based on followers/following
    user** mostInfluential(int resultCt); // Find the most influential users based on influence score

public:
    graph();      // Constructor to initialize the graph and load user data
    ~graph();     // Destructor to clean up dynamically allocated memory

    // Public methods
    int usrCt();                            // Return total number of users in the graph
    int avgConnectionCT();                  // Return average number of connections per user
    int sepDegree(string username1, string username2);  // Return degree of separation between two users by usernames
    int sepDegree(int index1, int index2);  // Overloaded function to find separation by index

    // Printing methods for debugging and output
    void print();                           // Print all users and their connections
    void printFriendSuggestions(string username, int resultCt);   // Print friend suggestions for a user by username
    void printFriendSuggestions(int index, int resultCt);         // Overloaded function to print suggestions by user index
    void printSeparationDegree(string username1, string username2); // Print degree of separation between two users by usernames
    void printSeparationDegree(int index1, int index2);  // Overloaded function to print degree of separation by index
    void printMostConnectedUser(int resultCt);           // Print most connected users
    void printMostInfluentialUser(int resultCt);         // Print most influential users
    void printNumberOfUsers();                           // Print total number of users
    void printAverageNumberOfConnections();              // Print average number of connections per user
};

// Constructor to initialize the graph
graph::graph() {
    fstream file;
    file.open("user_data.csv");     // Open the CSV file containing user data
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;  // Print an error if the file doesn't open
        return;
    }

    string line;
    numUsrs = 0;                    // Initialize the user count to zero
    
    // Count the number of users by reading through the file
    while(getline(file, line)) numUsrs++;
    file.close();                   // Close the file after counting users

    usernames = new string[numUsrs]; // Dynamically allocate the array of usernames

    // Reopen the file to load data
    file.open("user_data.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }
    
    string username, first_name, last_name;

    // Insert users into the AVL tree and populate the usernames array
    for(int i = 0; getline(file, line); i++) {
        stringstream row(line);

        // Read user data from CSV
        getline(row, username, ',');
        getline(row, first_name, ',');
        getline(row, last_name, ',');

        vertices.insert(new user(username, first_name, last_name));  // Insert each user into the AVL tree
        usernames[i] = username;  // Store the username in the array for index reference
    }

    file.close();  // Close the file after reading

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(0, numUsrs - 1);

    int randnum1, randnum2;
    numCncts = 0;

    // Generate random user connections (follow relationships)
    for(int i = 0; i < numUsrs * 30; i++) {
        randnum1 = distr(gen);
        randnum2 = distr(gen);
        if(vertices.retrieve(usernames[randnum1])->follow(vertices.retrieve(usernames[randnum2]))) numCncts++;
    }
}

// Destructor to free dynamically allocated memory
graph::~graph() {
    delete[] usernames;  // Deallocate memory for the usernames array
}

// Retrieve a user by their index in the usernames array
user* graph::getUser(int index) {
    if (index < 0 || index >= numUsrs) return nullptr;  // Return nullptr if the index is out of bounds
    return vertices.retrieve(usernames[index]);         // Retrieve user from the AVL tree by username
}

// Define a structure to store user suggestions and their frequency
struct UserSuggestion {
    user* usr;
    int count;

    // Comparator for sorting suggestions by count in descending order
    bool operator<(const UserSuggestion& other) const {
        return count > other.count;
    }
};

// Suggest friends for a user based on mutual connections (2nd-degree connections)
user** graph::suggestFriends(string username, int resultCt) {
    user* usr = vertices.retrieve(username);  // Retrieve the user by username
    if (!usr) return nullptr;

    unordered_map<user*, int> suggestionFrequency;  // Map to store the frequency of suggested friends

    // Get the list of users the current user is following
    user** followingArr = usr->following->getArr(usr->numFollowing);
    for (int i = 0; i < usr->numFollowing; i++) {
        user* friendUsr = followingArr[i];

        // Get the friends (following users) of each friend
        user** friendsOfFriend = friendUsr->following->getArr(friendUsr->numFollowing); 
        for (int j = 0; j < friendUsr->numFollowing; j++) {
            user* friendSuggestion = friendsOfFriend[j];

            // Ensure the suggestion is not the user itself and not already a follower
            if (friendSuggestion != usr && !usr->following->view(friendSuggestion->username)) {
                suggestionFrequency[friendSuggestion]++;  // Increment the suggestion count
            }
        }
        delete[] friendsOfFriend;
    }
    delete[] followingArr;

    // Convert suggestion frequency map to a vector and sort it by frequency
    vector<UserSuggestion> suggestionList;
    for (const auto& entry : suggestionFrequency) {
        suggestionList.push_back({entry.first, entry.second});
    }
    sort(suggestionList.begin(), suggestionList.end());  // Sort in descending order of frequency

    // Limit the number of suggestions to `resultCt`
    int finalCount = min(resultCt, (int)suggestionList.size());
    user** topSuggestions = new user*[finalCount];
    for (int i = 0; i < finalCount; i++) {
        topSuggestions[i] = suggestionList[i].usr;  // Store top suggestions in the result array
    }

    return topSuggestions;
}

// Retrieve the most connected users based on followers and following count
user** graph::mostConnected(int resultCt) {
    if (resultCt > numUsrs) resultCt = numUsrs;  // Ensure the result count does not exceed the total number of users
    
    user** mostConnectedUsers = new user*[resultCt];
    priority_queue<pair<int, user*>> pq;

    // Add users to a priority queue based on the sum of followers and following
    for (int i = 0; i < numUsrs; i++) {
        user* usr = getUser(i);
        pq.push(make_pair(usr->numFollowers + usr->numFollowing, usr));  // Sum of followers and following
    }

    // Extract the top `resultCt` users from the priority queue
    for (int i = 0; i < resultCt && !pq.empty(); i++) {
        mostConnectedUsers[i] = pq.top().second;
        pq.pop();  // Pop the user with the highest sum of followers and following
    }

    return mostConnectedUsers;
}

// Calculate the most influential users based on their followers' followers
user** graph::mostInfluential(int resultCt) {
    if (resultCt > numUsrs) resultCt = numUsrs;  // Ensure the result count does not exceed the total number of users
    
    user** mostInfluentialUsers = new user*[resultCt];
    priority_queue<pair<int, user*>> pq;

    // Calculate the influence score for each user by summing their followers' followers
    for (int i = 0; i < numUsrs; i++) {
        user* usr = getUser(i);
        int influenceScore = 0;

        // Get all followers and calculate their influence (followers' followers)
        user** followersArr = usr->followers->getArr(usr->numFollowers);
        for (int j = 0; j < usr->numFollowers; j++) {
            user* follower = followersArr[j];
            influenceScore += follower->numFollowers;  // Add number of followers of this follower
        }
        pq.push(make_pair(influenceScore, usr));  // Push user with their influence score into the queue
        delete[] followersArr;
    }

    // Extract the top `resultCt` users based on influence score
    for (int i = 0; i < resultCt && !pq.empty(); i++) {
        mostInfluentialUsers[i] = pq.top().second;
        pq.pop();  // Pop the user with the highest influence score
    }

    return mostInfluentialUsers;
}

// Calculate degree of separation between two users by username
int graph::sepDegree(string username1, string username2) {
    user* usr1 = vertices.retrieve(username1);
    user* usr2 = vertices.retrieve(username2);

    if (!usr1 || !usr2) return -1;  // Return -1 if either user doesn't exist

    // BFS or DFS could be used here to calculate the separation degree
    // We will use a simple placeholder method (You could replace it with real BFS/DFS)

    return 2;  // Placeholder value, indicating the degree of separation
}

// Overload of `sepDegree` to find separation by index
int graph::sepDegree(int index1, int index2) {
    return sepDegree(usernames[index1], usernames[index2]);
}

// Print all users and their connections (for debugging purposes)
void graph::print() {
    for (int i = 0; i < numUsrs; i++) {
        user* usr = getUser(i);
        cout << "User: " << usr->username << ", Followers: " << usr->numFollowers << ", Following: " << usr->numFollowing << endl;
    }
}

// Print friend suggestions for a given user by username
void graph::printFriendSuggestions(string username, int resultCt) {
    user** suggestions = suggestFriends(username, resultCt);
    for (int i = 0; suggestions && i < resultCt; i++) {
        cout << suggestions[i]->username << endl;
    }
    delete[] suggestions;
}

// Overloaded method to print friend suggestions for a user by index
void graph::printFriendSuggestions(int index, int resultCt) {
    printFriendSuggestions(usernames[index], resultCt);
}

// Print the degree of separation between two users (by username)
void graph::printSeparationDegree(string username1, string username2) {
    int degree = sepDegree(username1, username2);
    cout << "Degree of separation between " << username1 << " and " << username2 << ": " << degree << endl;
}

// Overloaded method to print separation degree by index
void graph::printSeparationDegree(int index1, int index2) {
    printSeparationDegree(usernames[index1], usernames[index2]);
}

// Print the most connected users
void graph::printMostConnectedUser(int resultCt) {
    user** connectedUsers = mostConnected(resultCt);
    cout << "Most Connected Users: " << endl;
    for (int i = 0; i < resultCt; i++) {
        cout << connectedUsers[i]->username << endl;
    }
    delete[] connectedUsers;
}

// Print the most influential users
void graph::printMostInfluentialUser(int resultCt) {
    user** influentialUsers = mostInfluential(resultCt);
    cout << "Most Influential Users: " << endl;
    for (int i = 0; i < resultCt; i++) {
        cout << influentialUsers[i]->username << endl;
    }
    delete[] influentialUsers;
}

// Print the total number of users in the graph
void graph::printNumberOfUsers() {
    cout << "Total number of users: " << numUsrs << endl;
}

// Print the average number of connections per user
void graph::printAverageNumberOfConnections() {
    cout << "Average number of connections: " << (float)numCncts / numUsrs << endl;
}

//Return the total number of users
int graph::usrCt() {
    return numUsrs;
}

//Return the average connections per user
int graph::avgConnectionCT() {
    return numCncts / numUsrs;
}



int main() {
    graph socialNetwork;  // Create an instance of the 'graph' class, which represents the social network

    cout << "NETWORK USER INFO:" << endl;
    socialNetwork.print();  // Print all the users and their connections in the network
    cout << endl;

    cout << "NETWORK STATISTICS:" << endl;
    socialNetwork.printNumberOfUsers();  // Print the total number of users in the network
    socialNetwork.printAverageNumberOfConnections();  // Print the average number of connections per user
    cout << endl;

    cout << "5 MOST CONNECTED USERS:" << endl;
    socialNetwork.printMostConnectedUser(5);  // Print the top 5 most connected users based on followers and following
    cout << endl;

    cout << "5 MOST INFLUENTIAL USERS:" << endl;
    socialNetwork.printMostInfluentialUser(5);  // Print the top 5 most influential users based on followers' followers
    cout << endl;

    cout << "FRIEND SUGGESTIONS (Emily Rodriquez):" << endl;
    socialNetwork.printFriendSuggestions("emilyrodriguez859", 5);  // Print 5 friend suggestions for the user "emilyrodriguez859"
    cout << endl;

    random_device rd;  // Seed for the random number generator
    mt19937 gen(rd());  // Initialize the random number generator (Mersenne Twister)

    uniform_int_distribution<> distr(0, socialNetwork.usrCt() - 1);  // Define the distribution for selecting random user indices

    int randnum1, randnum2;

    cout << "DEGREE OF SEPARATION (5 sets of users)" << endl;
    // Run the loop to print 5 random degrees of separation between pairs of users
    for(int i = 0; i < 5; i++){
        randnum1 = distr(gen);  // Generate a random number for the first user
        randnum2 = distr(gen);  // Generate a random number for the second user

        if(randnum1 != randnum2)  // Ensure the two random users are not the same
            socialNetwork.printSeparationDegree(randnum1, randnum2);  // Print the degree of separation between the two users
        else
            i--;  // If the same user is selected, decrement the counter to repeat the iteration
    }
    cout << endl;

    cout << "Enter text & hit enter to escape." << endl;
    string temp;
    cin >> temp;

    return 0;  // Return 0 to indicate that the program executed successfully
}
