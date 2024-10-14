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

#endif