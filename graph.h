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

#endif