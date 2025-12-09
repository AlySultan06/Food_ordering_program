#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cmath>
using namespace std;

// -----------------------------
// Structs
// -----------------------------
struct User
{
    int id;
    string name;
    string email;
    string password;
    string role; // "customer" or "owner"
    string address;
    string phone;

    // For owners
    int OwnerRestrauntCount = 0; // if owner, number of restaurants owned (waht will we use this for?-seif)
};

struct Restaurant
{
    int id;
    int ownerID;
    string name;
    string location;
    int rating;
    int menuItemIDs[50];
    int menuItemCount = 0;
    bool isOpen = true;
};

struct MenuItem
{
    int id;
    int restaurantID;
    string name;
    float price;
};

struct Order
{
    int id;
    int customerID;
    int restaurantID;
    int itemIDs[50];
    int itemCount = 0;
    string status;
    string paymentMethod;
    float totalAmount;
};

// -----------------------------
// Global Arrays AND VARIABLES
// -----------------------------
User users[100];
int userCount = -1;

Restaurant restaurants[50];
int restaurantCount = -1;

MenuItem menuItems[500];
int menuItemCount = -1;

Order orders[500];
int orderCount = -1;

// -----------------------------
// Utility Functions
// -----------------------------
int generateUserID() {  userCount++; return userCount; }
int generateRestaurantID() {  restaurantCount++; return restaurantCount; }
int generateMenuItemID() { menuItemCount++; return menuItemCount; }
int generateOrderID() {  orderCount++; return orderCount; }


// -----------------------------
// File I/O Functions
// -----------------------------
void savefile() {
    //save users
    ofstream file("users.txt");
    file << userCount << endl;
    for (int i = 0; i < userCount; i++) {
        file << users[i].id << endl;
        file << users[i].name << endl;
        file << users[i].email << endl;
        file << users[i].password << endl;
        file << users[i].role << endl;
        file << users[i].address << endl;
        file << users[i].phone << endl;
    }
    file.close();

    //save restaurants
    ofstream file("restaurants.txt");
    file << restaurantCount << endl;
    for (int i = 0; i < restaurantCount; i++) {
        Restaurant &r = restaurants[i];
        file << r.id << endl;
        file << r.ownerID << endl;
        file << r.name << endl;
        file << r.location << endl;
        file << r.rating << endl;
        file << r.menuItemCount << endl;
        for (int j = 0; j < r.menuItemCount; j++) file << r.menuItemIDs[j] << " ";
        file << endl;
        file << r.isOpen << endl;
    }
    file.close();



    //save menu items
    ofstream file("menuItems.txt");
    file << menuItemCount << endl;
    for (int i = 0; i < menuItemCount; i++) {
        file << menuItems[i].id << endl;
        file << menuItems[i].restaurantID << endl;
        file << menuItems[i].name << endl;
        file << menuItems[i].price << endl;
    }
    file.close();



    //save orders
    ofstream file("orders.txt");
    file << orderCount << endl;
    for (int i = 0; i < orderCount; i++) {
        Order &o = orders[i];
        file << o.id << endl;
        file << o.customerID << endl;
        file << o.restaurantID << endl;
        file << o.itemCount << endl;
        for (int j = 0; j < o.itemCount; j++) file << o.itemIDs[j] << " ";
        file << endl;
        file << o.status << endl;
        file << o.paymentMethod << endl;
        file << o.totalAmount << endl;
    }
    file.close();
}


void loadfiles(){
    //load users
    ifstream file("users.txt");
    if (!file) return;
    file >> userCount; file.ignore();
    for (int i = 0; i < userCount; i++) {
        file >> users[i].id; file.ignore();
        getline(file, users[i].name);
        getline(file, users[i].email);
        getline(file, users[i].password);
        getline(file, users[i].role);
        getline(file, users[i].address);
        getline(file, users[i].phone);
        if (users[i].id > userCount) userCount = users[i].id;
    }

    //load restaurants
    ifstream file("restaurants.txt");
    if (!file) return;
    file >> restaurantCount; file.ignore();
    for (int i = 0; i < restaurantCount; i++) {
        Restaurant &r = restaurants[i];
        file >> r.id >> r.ownerID; file.ignore();
        getline(file, r.name);
        getline(file, r.location);
        file >> r.rating >> r.menuItemCount; file.ignore();
        for (int j = 0; j < r.menuItemCount; j++) file >> r.menuItemIDs[j];
        file.ignore();
        file >> r.isOpen; file.ignore();
        if (r.id > restaurantCount) restaurantCount = r.id;
    }

    //load menu items
    ifstream file("menuItems.txt");
    if (!file) return;
    file >> menuItemCount; file.ignore();
    for (int i = 0; i < menuItemCount; i++) {
        file >> menuItems[i].id >> menuItems[i].restaurantID; file.ignore();
        getline(file, menuItems[i].name);
        file >> menuItems[i].price; file.ignore();
        if (menuItems[i].id > menuItemCount) menuItemCount = menuItems[i].id;
    }

    //load orders
    ifstream file("orders.txt");
    if (!file) return;
    file >> orderCount; file.ignore();
    for (int i = 0; i < orderCount; i++) {
        Order &o = orders[i];
        file >> o.id >> o.customerID >> o.restaurantID >> o.itemCount; file.ignore();
        for (int j = 0; j < o.itemCount; j++) file >> o.itemIDs[j];
        file.ignore();
        getline(file, o.status);
        getline(file, o.paymentMethod);
        file >> o.totalAmount; file.ignore();
        if (o.id > orderCount) orderCount = o.id;
    }
}

// -------------
// Dual functions
// -------------

// Registration & Login

void registerUser()
{
    User u;
    u.id = generateUserID();

   std:: cout << "\nEnter name: ";
    cin >> u.name;

    std ::cout << "Enter email: ";
    cin >> u.email;

    std ::cout << "Enter password: ";
    cin >> u.password;

    std ::cout << "Enter address: ";
    cin >> u.address;

    std ::cout << "Role (customer/owner): ";
    cin >> u.role;

    std ::cout << "Enter phone number: ";
    cin >> u.phone;

    users[userCount++] = u;

    std ::cout << "\nRegistration successful!\n";
}

int loginUser()
{
    string email, password;
    std ::cout << "\nEmail: ";
    cin >> email;
    std ::cout << "Password: ";
    cin >> password;

    for (int i = 0; i < userCount; i++)
    {
        if (users[i].email == email && users[i].password == password)
        {
            std ::cout << "\nLogin successful!\n";
            return users[i].id;
        }
    }

    std ::cout << "\nInvalid credentials.\n";
    return -1;
}

// show menu
void showMenu(int restrauntId)
{
    std ::cout << "\n--- Menu ---\n";
    for (int i = 0; i < menuItemCount; i++)
    {
        
        if (menuItems[i].restaurantID == restrauntId)
        {
            std ::cout << menuItems[i].id << ". " << menuItems[i].name << " - $" << menuItems[i].price << "\n";
        }
    }
}

// -----------------------------
// Customer Functions
// -----------------------------
void customerView(int userID)
{
    int choice;
    do
    {
        std ::cout << "\n--- Welcome Dear Customer---\n";
        std ::cout << "1. View Restaurants\n";
        std ::cout << "2. Logout\n";
        std ::cout << "Choose: ";
        cin >> choice;

        if (choice == 1)
        {
            std ::cout << "\n--- Restaurants ---\n";
            for (int i = 0; i < restaurantCount; i++)
            {
                std ::cout << restaurants[i].id << ". "
                     << restaurants[i].name << " ("
                     << restaurants[i].location << ")\n";
            }
            std ::cout << "Choose restaurant ID to view menu: ";
            int resID;
            cin >> resID;
            showMenu(resID);
        }

    } while (choice != 2);
}

 void viewrestraunts(){
    for(int i=0; i<restaurantCount; i++){
        std ::cout << "Restraunt Name: " << restaurants[i].name << endl;
        std ::cout << "Restraunt Location: " << restaurants[i].location << endl;
        std ::cout << "Restraunt Rating: " << restaurants[i].rating << endl;
        std ::cout << "------------------------" << endl;
    }
 }
void displayMenu(string restrauntname){
    int id = -1;
    for(int i=0; i<restaurantCount; i++){
        if(restaurants[i].name == restrauntname){
            id = restaurants[i].id;
            break;
        }
    }
    if(id == -1){
        std ::cout << "Restraunt not found." << endl;
        return;
    }
    std ::cout << "Menu for " << restrauntname << ":" << endl;
    for(int i=0; i<menuItemCount; i++){
        if(menuItems[i].restaurantID == id){
            std ::cout << "Item Name: " << menuItems[i].name << ", Price: " << menuItems[i].price << endl;
        }
}
}
// -----------------------------
// Owner Functions
// -----------------------------

// Create menu items
void createMenu(int restrauntId, int restrauntMenuItemCount){
    int n;
    std ::cout << "How many menu items would you like to add? ";
    cin >> n;
    for (int i = 0; i < n; i++)
    {
       
        MenuItem m;
        m.id = generateMenuItemID();
        m.restaurantID = restrauntId;
        std ::cout << "Menu item name: ";
        cin >> m.name;
        std ::cout << "Price: ";
        cin >> m.price;
        menuItems[menuItemCount++] = m;
        
    }
}


//create restraunt

void createRestraunt(int ownerId){

            Restaurant r;
            r.id = generateRestaurantID();
            r.ownerID = ownerId;

            std ::cout << "\nRestaurant name: ";
            cin >> r.name;

            std ::cout << "Location: ";
            cin >> r.location;

            std ::cout << "Status of restaurant (1 for open, 0 for closed): ";
            cin >> r.isOpen;

            restaurants[restaurantCount++] = r;
            createMenu(r.id, r.menuItemCount);

            

            std ::cout << "\nRestaurant created!\n";


}


//manage profile

void manageProfile(int ownerId){
     std ::cout << "\n-- Manage Profile --\n";
            bool hasRestaurant = false;
            for (int i = 0; i < restaurantCount; i++)
            {
                if (restaurants[i].ownerID == ownerId)
                {
                    hasRestaurant= true;
                    Restaurant r = restaurants[i];
                    std ::cout << r.id << ". " << r.name << " (" << r.location << ")\n";

                    std ::cout << "Choose restaurant ID to view menu: ";
                    int resID;
                    cin >> resID;
                    showMenu(resID);
                }
                if (!hasRestaurant)
    {
        std ::cout << "You have no restaurants.\n";
    }
               
            }
}



void ownerView(int userID)
{
    int choice;
    User owner = users[userID - 1];
    do
    {
        std ::cout << "\n--- Welcome Dear Owner---\n";
        std ::cout << "1. Create Restaurant\n";
        std ::cout << "2. Manage Profile\n";
        std ::cout << "3. Logout\n";
        std ::cout << "Choose: ";
        cin >> choice;

        if (choice == 1)
        {
            createRestraunt(owner.id);
        }

        if (choice == 2)
        {
           manageProfile(owner.id);
        }

    } while (choice != 3);
}

int main()
{
    int choice;
    // main loop
    while (true)
    {
        std ::cout << "\n=== Online Food Ordering System ===\n";
        std ::cout << "1. Register\n";
        std ::cout << "2. Login\n";
        std ::cout << "3. Exit\n";
        std ::cout << "Choose: ";
        cin >> choice;

        if (choice == 1)
        {
            registerUser();
        }

        else if (choice == 2)
        {
            int userID = loginUser();

            if (userID != -1)
            {
                // find the role
                string role;
                for (int i = 0; i < userCount; i++)
                {
                    if (users[i].id == userID)
                        role = users[i].role;
                }

                if (role == "customer")
                {
                    customerView(userID);
                }
                else
                {
                    ownerView(userID);
                }
            }
            else
            {
                std ::cout << "Unsuccesful Login\n";
            }
        }
        else if (choice == 3)
        {
            break;
        }
        else
        {
            std ::cout << "Invalid choice.\n";
        }
    }
}
