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

// Track last IDs for file I/O
int lastUserID = 0;
int lastRestaurantID = 0;
int lastMenuItemID = 0;
int lastOrderID = 0;

int currentUserID;
//for owners
int ownerreterauntids[50];
//for customers
int selectedrestaurantid;

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
void saveAllData() {
    ofstream file;

    file.open("users.txt");
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

    file.open("restaurants.txt");
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

    file.open("menuItems.txt");
    file << menuItemCount << endl;
    for (int i = 0; i < menuItemCount; i++) {
        file << menuItems[i].id << endl;
        file << menuItems[i].restaurantID << endl;
        file << menuItems[i].name << endl;
        file << menuItems[i].price << endl;
    }
    file.close();

    file.open("orders.txt");
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

void loadAllData() {
    ifstream file;

    file.open("users.txt");
    if (file) {
        file >> userCount; file.ignore();
        for (int i = 0; i < userCount; i++) {
            file >> users[i].id; file.ignore();
            getline(file, users[i].name);
            getline(file, users[i].email);
            getline(file, users[i].password);
            getline(file, users[i].role);
            getline(file, users[i].address);
            getline(file, users[i].phone);
            if (users[i].id > lastUserID) lastUserID = users[i].id;
        }
        file.close();
    }

    file.open("restaurants.txt");
    if (file) {
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
            if (r.id > lastRestaurantID) lastRestaurantID = r.id;
        }
        file.close();
    }

    file.open("menuItems.txt");
    if (file) {
        file >> menuItemCount; file.ignore();
        for (int i = 0; i < menuItemCount; i++) {
            file >> menuItems[i].id >> menuItems[i].restaurantID; file.ignore();
            getline(file, menuItems[i].name);
            file >> menuItems[i].price; file.ignore();
            if (menuItems[i].id > lastMenuItemID) lastMenuItemID = menuItems[i].id;
        }
        file.close();
    }

    file.open("orders.txt");
    if (file) {
        file >> orderCount; file.ignore();
        for (int i = 0; i < orderCount; i++) {
            Order &o = orders[i];
            file >> o.id >> o.customerID >> o.restaurantID >> o.itemCount; file.ignore();
            for (int j = 0; j < o.itemCount; j++) file >> o.itemIDs[j];
            file.ignore();
            getline(file, o.status);
            getline(file, o.paymentMethod);
            file >> o.totalAmount; file.ignore();
            if (o.id > lastOrderID) lastOrderID = o.id;
        }
        file.close();
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

    cout << "\nEnter name: ";
    cin >> u.name;

    cout << "Enter email: ";
    cin >> u.email;

    cout << "Enter password: ";
    cin >> u.password;

    cout << "Enter address: ";
    cin >> u.address;

    cout << "Role (customer/owner): ";
    cin >> u.role;

    cout << "Enter phone number: ";
    cin >> u.phone;

    users[userCount++] = u;

    cout << "\nRegistration successful!\n";
}

int loginUser()
{
    string email, password;
    cout << "\nEmail: ";
    cin >> email;
    cout << "Password: ";
    cin >> password;

    for (int i = 0; i < userCount; i++)
    {
        if (users[i].email == email && users[i].password == password)
        {
            cout << "\nLogin successful!\n";
            return users[i].id;
            currentUserID = users[i].id;
        }
    }

    cout << "\nInvalid credentials.\n";
    return -1;
}

// show menu
void showMenu(int restrauntId)
{
    cout << "\n--- Menu ---\n";
    for (int i = 0; i < menuItemCount; i++)
    {
        
        if (menuItems[i].restaurantID == restrauntId)
        {
            cout << menuItems[i].id << ". " << menuItems[i].name << " - $" << menuItems[i].price << "\n";
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
        cout << "\n--- Welcome Dear Customer---\n";
        cout << "1. View Restaurants\n";
        cout << "2. Logout\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice == 1)
        {
            cout << "\n--- Restaurants ---\n";
            for (int i = 0; i < restaurantCount; i++)
            {
                cout << restaurants[i].id << ". "
                     << restaurants[i].name << " ("
                     << restaurants[i].location << ")\n";
            }
            cout << "Choose restaurant ID to view menu: ";
            int resID;
            cin >> resID;
            showMenu(resID);
        }

    } while (choice != 2);
}

 void viewrestraunts(){
    for(int i=0; i<restaurantCount; i++){
        cout << "Restraunt Name: " << restaurants[i].name << endl;
        cout << "Restraunt Location: " << restaurants[i].location << endl;
        cout << "Restraunt Rating: " << restaurants[i].rating << endl;
        cout << "------------------------" << endl;
    }
 }
void displayMenu(string restrauntname){
    int id = -1;
    for(int i=0; i<restaurantCount; i++){
        if(restaurants[i].name == restrauntname){
            id = restaurants[i].id;
            selectedrestaurantid = id;
            break;
        }
    }
    if(id == -1){
        cout << "Restraunt not found." << endl;
        return;
    }
    cout << "Menu for " << restrauntname << ":" << endl;
    for(int i=0; i<menuItemCount; i++){
        if(menuItems[i].restaurantID == id){
            cout << "Item Name: " << menuItems[i].name << ", Price: " << menuItems[i].price << endl;
        }
}
}


 void placeorder(){
    Order o;
    o.id = generateOrderID();
    o.customerID = currentUserID;
    o.restaurantID = selectedrestaurantid;
    o.itemCount = 0;
    o.totalAmount = 0.0;

    string itemname;
    char more;
    do{
        cout << "Enter item name to add to order: ";
        cin >> itemname;
        bool found = false;
        for(int i=0; i<menuItemCount; i++){
            if(menuItems[i].name == itemname && menuItems[i].restaurantID == selectedrestaurantid){
                o.itemIDs[o.itemCount++] = menuItems[i].id;
                o.totalAmount += menuItems[i].price;
                found = true;
                break;
            }
        }
        if(!found){
            cout << "Item not found in the selected restaurant." << endl;
        }
        cout << "Add more items? (y/n): ";
        cin >> more;
    }while(more == 'y' || more == 'Y');

    o.status = "Placed";
    orders[orderCount++] = o;
    cout << "Order placed successfully! Total amount: $" << o.totalAmount << endl;
    cout << "payment method (Cash/Card): ";
    cin >> o.paymentMethod;
 }
// -----------------------------
// Owner Functions
// -----------------------------
// for owner to view pending orders
void pendingorders(){
    cout << "Pending Orders:" << endl;
    //shows all pending orders for the owner's restaurants
    for(int i=0; i<orderCount; i++){
        if(orders[i].status == "Placed"){
            for(int j=0; j<restaurantCount; j++){
                if(restaurants[j].id == orders[i].restaurantID && restaurants[j].ownerID == currentUserID){
                    cout << "Order ID: " << orders[i].id << ", Customer ID: " << orders[i].customerID << ", Total Amount: $" << orders[i].totalAmount << endl;
                    cout <<"will you accept the order? (y/n): ";
                    // accept or reject order
                    char choice;
                    cin >> choice;
                    if(choice == 'y'){
                        orders[i].status = "preparing";
                        cout << "Order accepted." << endl;
                    } else {
                        orders[i].status = "rejected";
                        cout << "Order rejected." << endl;
                        //me7tageen taree2a ne3mel notify lel customer en el order etrefed
                    }
                }
            }
        }
    }
    //shows all orders that are being prepared for delivery
    cout << "display orders waiting for delivery(y/n)" << endl;
    char choice;
    cin >> choice;
    if(choice == 'y'){
        for(int i=0; i<orderCount; i++){
            if(orders[i].status == "preparing"){
                for(int j=0; j<restaurantCount; j++){
                    if(restaurants[j].id == orders[i].restaurantID && restaurants[j].ownerID == currentUserID){
                        cout << "Order ID: " << orders[i].id << ", Customer ID: " << orders[i].customerID << ", Total Amount: $" << orders[i].totalAmount << endl;
                        cout <<"is the order read? (y/n): ";
                        char delivery;
                        cin >> delivery;
                        // mark order as prepared and out for delivery
                        if(delivery == 'y'){
                            orders[i].status = "in delivery";
                            cout << "Order marked as delivered." << endl;
                        }
                    }
                }
            }
        }
    }
}

// Create menu items
void createMenu(int restrauntId, int restrauntMenuItemCount){
    int n;
    cout << "How many menu items would you like to add? ";
    cin >> n;
    for (int i = 0; i < n; i++)
    {
       
        MenuItem m;
        m.id = generateMenuItemID();
        m.restaurantID = restrauntId;
        cout << "Menu item name: ";
        cin >> m.name;
        cout << "Price: ";
        cin >> m.price;
        menuItems[menuItemCount++] = m;
        
    }
}


//create restraunt

void createRestraunt(int ownerId){

            Restaurant r;
            r.id = generateRestaurantID();
            r.ownerID = ownerId;

            cout << "\nRestaurant name: ";
            cin >> r.name;

            cout << "Location: ";
            cin >> r.location;

            cout << "Status of restaurant (1 for open, 0 for closed): ";
            cin >> r.isOpen;

            restaurants[restaurantCount++] = r;
            createMenu(r.id, r.menuItemCount);

            

            cout << "\nRestaurant created!\n";


}


//manage profile

void manageProfile(int ownerId){
     cout << "\n-- Manage Profile --\n";
            bool hasRestaurant = false;
            for (int i = 0; i < restaurantCount; i++)
            {
                if (restaurants[i].ownerID == ownerId)
                {
                    hasRestaurant= true;
                    Restaurant r = restaurants[i];
                    cout << r.id << ". " << r.name << " (" << r.location << ")\n";

                    cout << "Choose restaurant ID to view menu: ";
                    int resID;
                    cin >> resID;
                    showMenu(resID);
                }
                if (!hasRestaurant)
    {
        cout << "You have no restaurants.\n";
    }
               
            }
}



void ownerView(int userID)
{
    int choice;
    User owner = users[userID - 1];
    do
    {
        cout << "\n--- Welcome Dear Owner---\n";
        cout << "1. Create Restaurant\n";
        cout << "2. Manage Profile\n";
        cout << "3. Logout\n";
        cout << "Choose: ";
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
loadAllData();
// registerUser();
cout << users[0].name << endl;
cout << users[1].name << endl;
    //int choice;
    // main loop
    // while (true)
    // {
    //     cout << "\n=== Online Food Ordering System ===\n";
    //     cout << "1. Register\n";
    //     cout << "2. Login\n";
    //     cout << "3. Exit\n";
    //     cout << "Choose: ";
    //     cin >> choice;

    //     if (choice == 1)
    //     {
    //         registerUser();
    //     }

    //     else if (choice == 2)
    //     {
    //         int userID = loginUser();

    //         if (userID != -1)
    //         {
    //             // find the role
    //             string role;
    //             for (int i = 0; i < userCount; i++)
    //             {
    //                 if (users[i].id == userID)
    //                     role = users[i].role;
    //             }

    //             if (role == "customer")
    //             {
    //                 customerView(userID);
    //             }
    //             else
    //             {
    //                 ownerView(userID);
    //             }
    //         }
    //         else
    //         {
    //             cout << "Unsuccesful Login\n";
    //         }
    //     }
    //     else if (choice == 3)
    //     {
    //         break;
    //     }
    //     else
    //     {
    //         cout << "Invalid choice.\n";
    //     }
    // }
saveAllData();
}
