old program ignore



#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include <windows.h>
#include <conio.h>
using namespace std;


struct user{
    string name;
    string email;
    string password;
    string role;
    string address;
    int userID;
};
user usersarr[100];
 
struct resteraunt{
    string name;
    string location;
    int menuitems[100];
    float rating;
    int resterauntID;
    int ownerID;
    bool isOpen;

};

struct menuitem{
    string itemName;
    string description;
    int itemID;
    float price;
    int resterauntID;
    bool isAvailable;
};
struct order{
    int orderID;
    int userID;
    int resterauntID;
    int numofitems[100];
    float totalprice;
    string status;
    time_t ordertime;
    time_t deliverytime;
    bool cash;
    bool card;

};

struct review{
    int reviewID;
    int userID;
    int resterauntID;
    float rating;
    string comment;
};

int lastreviewID = 0;
int lastorderID = 0;
int lastmenuitemID = 0;
int lastresterauntID = 0;
int lastuserID = 0;
int generateUserID(){
    lastuserID++;
    return lastuserID;
}

void saveAllData() {
    ofstream userFile("users.txt");
    
    if (userFile.is_open()) {
        for (int i = 0; i < lastuserID; i++) {
            userFile << usersarr[i].userID << " " ;
            userFile << usersarr[i].name << " " ;
            userFile << usersarr[i].email << " " ;
            userFile << usersarr[i].password << " " ;
            userFile << usersarr[i].role << " " ;
            userFile << usersarr[i].address << endl; 
        }
        userFile.close();
        cout << "Users saved." << endl;
    }
}

void registerowner(){
    user newowner;
    newowner.userID = generateUserID();
    cout << "Enter your name: ";
    cin >> newowner.name;
    cout << "Enter your email: ";
    cin >> newowner.email;
    cout << "Enter your password: ";
    cin >> newowner.password;
    cout << "Enter your address: ";
    cin >> newowner.address;
    newowner.role = "owner";
    // Save owner to database (file or in-memory structure)
    cout << "Owner registered successfully with ID: " << newowner.userID << endl;
    usersarr[lastuserID] = newowner;
}

void loadAllData() {
    // --- 1. LOAD USERS ---
    ifstream userFile("users.txt");
    user tempUser;
    if (userFile.is_open()) {
        // "While we can successfully read an ID from the file..."
        while (userFile >>lastuserID) {
            userFile >> tempUser.name;
            userFile >> tempUser.email;
            userFile >> tempUser.password;
            userFile >> tempUser.role;
            userFile >> tempUser.address;
            userFile >> tempUser.userID;
            lastuserID++;

            if (tempUser.userID > lastuserID) {
                lastuserID = tempUser.userID;
            }
        }
        userFile.close();
    } else {
        cout << "No user database found. Starting fresh." << endl;
    }
}

// bool loginowner(string email, string password){
//     for(int i=0; i<lastuserID; i++){
//         if(users[i].email == email && users[i].password == password && users[i].role == "owner"){
//             cout << "Login successful. Welcome, " << users[i].name << "!" << endl;
//             return true;
//         }
//     }
//     cout << "Login failed. Please check your credentials." << endl;
//     return false;

// }
int main(){
cout << "Current number of users: " << lastuserID << endl;
loadAllData();
registerowner();
registerowner();
saveAllData();
cout << "Current number of users: " << lastuserID << endl;




    return 0;
}
