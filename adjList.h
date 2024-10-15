#ifndef _ADJLIST_H_
#define _ADJLIST_H_
/*
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
*/
#include <string>
using namespace std;

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

#endif