#ifndef _GRAPH_H_
#define _GRAPH_H_

/*
Graph:
-AVL tree of all nodes
-Nodes store all edges
-Edges store nodes
-AVL tree performs memory management
-String array to allow for indexing of users
-Integers to store total number of users and total number of connections
-Methods added to allow for computations
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include "adjList.h"
#include "avl.h"
using namespace std;

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

#endif