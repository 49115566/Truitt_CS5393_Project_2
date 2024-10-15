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
#include <iostream>
#include "adjList.h"
#include "avl.h"
#include "graph.h"
using namespace std;

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

    cout << "FRIEND SUGGESTIONS: (Emily Rodriguez)" << endl;
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

    return 0;  // Return 0 to indicate that the program executed successfully
}
