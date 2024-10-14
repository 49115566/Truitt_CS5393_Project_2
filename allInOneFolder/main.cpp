#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <random>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>
using namespace std;

struct user;



struct aNode{
    user* val;
    aNode* next;

    aNode(user* person) : val(person), next(nullptr) {}
};



class adjList{
private:
    aNode* head;
public:
    adjList(user* person);
    ~adjList();

    bool add(user* person);
    bool remove(string username);
    user* view(string username) const;
    user* self() const;

    user** getArr(int len);
};



struct user{
    string username;
    string firstname;
    string lastname;
    int numFollowing;
    int numFollowers;

    adjList* following;
    adjList* followers;

    bool follow(user* usr);
    bool unfollow(string uname);

    user(string un, string fn, string ln);
    ~user();
};

user::user(string un, string fn, string ln) : username(un), firstname(fn), lastname(ln), numFollowing(0), numFollowers(0) {
    following = new adjList(this);
    followers = new adjList(this);
}

user::~user(){
    user** temp = following->getArr(numFollowing);
    while(numFollowing){
        unfollow(temp[numFollowing - 1]->username);
    }
    delete[] temp;

    temp = followers->getArr(numFollowers);
    while(numFollowers){
        temp[numFollowers - 1]->unfollow(this->username);
    }
    delete[] temp;

    delete following;
    delete followers;
}

bool user::follow(user* usr){
    bool ret = 1;

    if(following->add(usr))
        numFollowing++;
    else ret = 0;

    if(usr->followers->add(this))
        usr->numFollowers++;
    else ret = 0;

    return ret;
}

bool user::unfollow(string uname){
    bool ret = 1;

    user* usr = following->view(uname);
    if(usr && following->remove(uname))
        numFollowing--;
    else ret = 0;
    
    if(usr && usr->followers->remove(username))
        usr->numFollowers--;
    else ret = 0;

    return ret;
}



adjList::adjList(user* person) {
    head = new aNode(person);
}

adjList::~adjList(){
    aNode* temp;
    while(head){
        temp = head->next;
        delete head;
        head = temp;
    }
}

bool adjList::add(user* person){
    if (person == head->val || view(person->username)) return false;
    aNode* temp = new aNode(person);
    temp->next = head->next;
    head->next = temp;
    return true;
}

bool adjList::remove(string username){
    aNode* cur = head;
    aNode* next = head->next;
    
    while(next && next->val->username != username){
        cur = next;
        next = cur->next;
    }

    if(next){
        cur->next = next->next;
        delete next;
        return 1;
    }
    
    return 0;
}

user* adjList::view(string username) const{
    aNode* cur = head->next;

    while(cur && cur->val->username != username){
        cur = cur->next;
    }

    if(cur) return cur->val;
    return nullptr;
}

user* adjList::self() const{
    return head->val;
}

user** adjList::getArr(int len){
    user** arr = new user*[len];
    aNode* cur = head->next;
    int i;
    for(i = 0; cur; i++){
        arr[i] = cur->val;
        cur = cur->next;
    }

    if(i != len) cout << "WARNING: len = " << len << ", but i = " << i << endl;

    return arr;
}



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



class graph{
private:
    AVL vertices;
    string* usernames;
    int numUsrs;
    int numCncts;

    user* getUser(int index);

    user** suggestFriends(string username, int resultCt);
    user** mostConnected(int resultCt);
    user** mostInfluential(int resultCt);
public:
    graph();
    ~graph();
    
    int usrCt();
    int avgConnectionCT();
    int sepDegree(string username1, string username2);
    int sepDegree(int index1, int index2);

    void print();
    void printFriendSuggestions(string username, int resultCt);
    void printFriendSuggestions(int index, int resultCt);
    void printSeparationDegree(string username1, string username2);
    void printSeparationDegree(int index1, int index2);
    void printMostConnectedUser(int resultCt);
    void printMostInfluentialUser(int resultCt);
    void printNumberOfUsers();
    void printAverageNumberOfConnections();
};

graph::graph(){
    fstream file;
    file.open("user_data.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    string line;
    numUsrs = 0;
    
    while(getline(file, line)) numUsrs++;

    file.close();

    usernames = new string[numUsrs];


    file.open("user_data.csv");
    if (!file.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }
    
    string username, first_name, last_name;

    for(int i = 0; getline(file, line); i++) {
        stringstream row(line);

        getline(row, username, ',');
        getline(row, first_name, ',');
        getline(row, last_name, ',');

        vertices.insert(new user(username, first_name, last_name));
        usernames[i] = username;
    }

    file.close();

    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<> distr(0, numUsrs - 1);

    int randnum1, randnum2;
    numCncts = 0;

    for(int i = 0; i < numUsrs * 30; i++){
        randnum1 = distr(gen);
        randnum2 = distr(gen);
        if(vertices.retrieve(usernames[randnum1])->follow(vertices.retrieve(usernames[randnum2]))) numCncts++;
    }
}

graph::~graph(){
    delete[] usernames;
}

user* graph::getUser(int index) {
    if (index < 0 || index >= numUsrs) return nullptr;
    return vertices.retrieve(usernames[index]);
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

user** graph::suggestFriends(string username, int resultCt) {
    user* usr = vertices.retrieve(username);
    if (!usr) return nullptr;

    // Map to store suggested users and their frequency
    unordered_map<user*, int> suggestionFrequency;

    // Iterate through the user's following list
    user** followingArr = usr->following->getArr(usr->numFollowing);
    for (int i = 0; i < usr->numFollowing; i++) {
        user* friendUsr = followingArr[i];

        // Get the following list of the user's friend (2nd-degree)
        user** friendsOfFriend = friendUsr->following->getArr(friendUsr->numFollowing);
        for (int j = 0; j < friendUsr->numFollowing; j++) {
            user* friendSuggestion = friendsOfFriend[j];

            // Ensure the suggestion is not the user itself or already followed
            if (friendSuggestion != usr && !usr->following->view(friendSuggestion->username)) {
                suggestionFrequency[friendSuggestion]++;
            }
        }
        delete[] friendsOfFriend;
    }
    delete[] followingArr;

    // Convert the map to a vector of UserSuggestion objects
    vector<UserSuggestion> suggestionList;
    for (const auto& entry : suggestionFrequency) {
        suggestionList.push_back({entry.first, entry.second});
    }

    // Sort the suggestions by frequency in descending order
    sort(suggestionList.begin(), suggestionList.end());

    // Allocate memory for the result array
    int finalCount = min(resultCt, (int)suggestionList.size());
    user** topSuggestions = new user*[finalCount];

    // Store the top `resultCt` suggestions in the result array
    for (int i = 0; i < finalCount; i++) {
        topSuggestions[i] = suggestionList[i].usr;
    }

    return topSuggestions;
}

user** graph::mostConnected(int resultCt) {
    if (resultCt > numUsrs) resultCt = numUsrs;
    
    user** mostConnectedUsers = new user*[resultCt];
    priority_queue<pair<int, user*>> pq;

    // Add all users to a priority queue sorted by number of followers
    for (int i = 0; i < numUsrs; i++) {
        user* usr = getUser(i);
        pq.push(make_pair(usr->numFollowers + usr->numFollowing, usr));
    }

    // Extract the top resultCt users
    for (int i = 0; i < resultCt && !pq.empty(); i++) {
        mostConnectedUsers[i] = pq.top().second;
        pq.pop();
    }

    return mostConnectedUsers;
}

user** graph::mostInfluential(int resultCt) {
    if (resultCt > numUsrs) resultCt = numUsrs;
    
    user** mostInfluentialUsers = new user*[resultCt];
    priority_queue<pair<int, user*>> pq;

    // Calculate influence by summing the followers of followers
    for (int i = 0; i < numUsrs; i++) {
        user* usr = getUser(i);
        int influenceScore = 0;

        // Get all followers
        user** followersArr = usr->followers->getArr(usr->numFollowers);
        for (int j = 0; j < usr->numFollowers; j++) {
            user* follower = followersArr[j];
            influenceScore += follower->numFollowers; // The number of followers of this follower
        }
        pq.push(make_pair(influenceScore, usr));
        delete[] followersArr;
    }

    // Extract top resultCt users
    for (int i = 0; i < resultCt && !pq.empty(); i++) {
        mostInfluentialUsers[i] = pq.top().second;
        pq.pop();
    }

    return mostInfluentialUsers;
}

int graph::sepDegree(string username1, string username2) {
    user* usr1 = vertices.retrieve(username1);
    user* usr2 = vertices.retrieve(username2);
    
    if (!usr1 || !usr2) return -1; // Return -1 if either user does not exist
    
    queue<pair<user*, int>> q; // pair<user, degree>
    unordered_set<string> visited;
    
    q.push({usr1, 0});
    visited.insert(usr1->username);

    while (!q.empty()) {
        auto [curUser, degree] = q.front();
        q.pop();

        if (curUser == usr2) return degree; // Found the target user
        
        // Visit all following users (neighbors)
        user** followingArr = curUser->following->getArr(curUser->numFollowing);
        for (int i = 0; i < curUser->numFollowing; i++) {
            if (visited.find(followingArr[i]->username) == visited.end()) {
                visited.insert(followingArr[i]->username);
                q.push({followingArr[i], degree + 1});
            }
        }
        delete[] followingArr;
    }
    
    return -1; // No connection found
}

int graph::sepDegree(int index1, int index2) {
    return sepDegree(usernames[index1], usernames[index2]);
}

void graph::print() {
    for (int i = 0; i < numUsrs; i++) {
        user* usr = getUser(i);
        cout << usr->username << " (" << usr->firstname << " " << usr->lastname << ") is following " << usr->numFollowing << " and has " << usr->numFollowers << " followers.\n";
    }
}

void graph::printFriendSuggestions(string username, int resultCt) {
    user** suggestions = suggestFriends(username, resultCt);
    if (!suggestions) {
        cout << "No suggestions available." << endl;
        return;
    }
    
    cout << "Friend suggestions for " << username << ":" << endl;
    for (int i = 0; i < resultCt && suggestions[i]; i++) {
        cout << suggestions[i]->username << endl;
    }
    delete[] suggestions;
}

void graph::printFriendSuggestions(int index, int resultCt) {
    if (index < 0 || index >= numUsrs) {
        std::cout << "Invalid user index." << std::endl;
        return;
    }
    printFriendSuggestions(usernames[index], resultCt);
}

void graph::printSeparationDegree(string username1, string username2) {
    int degree = sepDegree(username1, username2);
    if (degree == -1) cout << "No connection found between " << username1 << " and " << username2 << endl;
    else cout << "Degree of separation between " << username1 << " and " << username2 << ": " << degree << endl;
}

void graph::printSeparationDegree(int index1, int index2) {
    if (index1 < 0 || index1 >= numUsrs || index2 < 0 || index2 >= numUsrs) {
        std::cout << "Invalid user index." << std::endl;
        return;
    }
    printSeparationDegree(usernames[index1], usernames[index2]);
}

void graph::printMostConnectedUser(int resultCt) {
    user** users = mostConnected(resultCt);
    cout << "Most connected users:" << endl;
    for (int i = 0; i < resultCt; i++) {
        cout << users[i]->username << " with " << users[i]->numFollowers << " followers & following " << users[i]->numFollowing << " others" << endl;
    }
    delete[] users;
}

void graph::printMostInfluentialUser(int resultCt) {
    user** users = mostInfluential(resultCt);
    cout << "Most influential users:" << endl;
    for (int i = 0; i < resultCt; i++) {
        cout << users[i]->username << endl;
    }
    delete[] users;
}

void graph::printNumberOfUsers() {
    cout << "Number of users: " << numUsrs << endl;
}

void graph::printAverageNumberOfConnections() {
    cout << "Average number of connections: " << avgConnectionCT() << endl;
}

int graph::usrCt() {
    return numUsrs;
}

int graph::avgConnectionCT() {
    return numCncts / numUsrs;
}



int main() {
    graph socialNetwork;

    cout << "NETWORK USER INFO:" << endl;
    socialNetwork.print();
    cout << endl;

    cout << "NETWORK STATISTICS:" << endl;
    socialNetwork.printNumberOfUsers();
    socialNetwork.printAverageNumberOfConnections();
    cout << endl;

    cout << "5 MOST CONNECTED USERS:" << endl;
    socialNetwork.printMostConnectedUser(5);
    cout << endl;

    cout << "5 MOST INFLUENTIAL USERS:" << endl;
    socialNetwork.printMostInfluentialUser(5);
    cout << endl;

    cout << "FRIEND SUGGESTIONS:" << endl;
    socialNetwork.printFriendSuggestions("emilyrodriguez859", 5);
    cout << endl;

    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<> distr(0, socialNetwork.usrCt() - 1);

    int randnum1, randnum2;

    cout << "DEGREE OF SEPARATION (5 sets of users)" << endl;
    for(int i = 0; i < 5; i++){
        randnum1 = distr(gen);
        randnum2 = distr(gen);
        if(randnum1 != randnum2) socialNetwork.printSeparationDegree(randnum1, randnum2);
        else i--;
    }
    cout << endl;

    return 0;
}