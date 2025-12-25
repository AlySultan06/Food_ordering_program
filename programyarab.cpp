#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include "include/raylib.h"
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
    int OwnerRestaurantCount = 0;
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
    Color statusColor;
};

// -----------------------------
// Global Arrays AND VARIABLES
// -----------------------------
User users[100];
int userCount = 0;

Restaurant restaurants[50];
int restaurantCount = 0;

MenuItem menuItems[500];
int menuItemCount = 0;

Order orders[500];
int orderCount = 0;

// variable ids
int nextUserID = 0;
int nextRestaurantID = 0;
int nextMenuItemID = 0;
int nextOrderID = 0;

int currentUserID;
int selectedRestaurantID = -1;

// -----------------------------
// GUI Functions
// -----------------------------
enum GameScreen
{
    SCREEN_LOGIN,
    SCREEN_REGISTER,
    SCREEN_CUSTOMER_DASH,
    SCREEN_RESTAURANT_MENU,
    SCREEN_OWNER_DASH,
    SCREEN_CREATE_RESTAURANT,
    SCREEN_OWNER_ORDERS,
    SCREEN_OWNER_MANAGE_ORDERS_STATUS,
    SCREEN_OWNER_VIEW_RESTAURANTS,
    SCREEN_OWNER_ADD_MENU_ITEM,
    SCREEN_CUSTOMER_ORDERS
};

// Start at Login
GameScreen currentScreen = SCREEN_LOGIN;

// text input Buffers
char nameBuffer[64] = "\0";
char emailBuffer[64] = "\0";
char passBuffer[64] = "\0";
char addressBuffer[64] = "\0";
char phoneBuffer[64] = "\0";
char itemNameBuffer[64] = "\0";
char itemPriceBuffer[64] = "\0";
char restNameBuffer[64] = "\0";
char restLocBuffer[64] = "\0";


//gui variable (would be reset by frame rate if not global variable)
int activeBoxID = 0;
int orderFeedbackTimer = 0;
string lastOrderedItemName = "";
bool viewRestaurants = true;
int cartItemIDs[100];
int cartItemCount = 0;
float cartTotal = 0.0;

// scroll position for each screen
float customerDashScroll = 0;
float restaurantMenuScroll = 0;
float customerOrdersScroll = 0;
float ownerViewRestaurantsScroll = 0;
float ownerOrdersScroll = 0;
float ownerManageOrdersScroll = 0;
// scroll constants
const int ITEM_HEIGHT = 60;
const int SCROLL_SPEED = 10;


// current  role
bool role = false; // true=owner, false =customer

// draw button function
bool GuiButton(Rectangle rec, string text)
{
    bool pressed = false;
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rec);

    if (hover)
    {
        DrawRectangleRec(rec, LIGHTGRAY);
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            pressed = true;
    }
    else
    {
        DrawRectangleRec(rec, GRAY);
    }

    // Center text
    int textWidth = MeasureText(text.c_str(), 20);
    DrawText(text.c_str(), rec.x + (rec.width - textWidth) / 2, rec.y + 10, 20, BLACK);

    return pressed;
}

// A text box that captures typing
void GuiTextBox(Rectangle rec, char *buffer, int bufferSize, int ID, string placeholder)
{
    bool hover = CheckCollisionPointRec(GetMousePosition(), rec);

    // Click to activate
    if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        activeBoxID = ID;

    // Draw Box
    DrawRectangleRec(rec, RAYWHITE);
    if (activeBoxID == ID)
        DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, RED);
    else
        DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, DARKGRAY);

    // Draw Text or Placeholder
    if (strlen(buffer) > 0)
    {
        DrawText(buffer, rec.x + 5, rec.y + 8, 20, BLACK);
    }
    else
    {
        DrawText(placeholder.c_str(), rec.x + 5, rec.y + 8, 20, LIGHTGRAY);
    }

    // Capture Keys if Active
    if (activeBoxID == ID)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            if ((key >= 32) && (key <= 125) && (strlen(buffer) < bufferSize))
            {
                int len = strlen(buffer);
                buffer[len] = (char)key;
                buffer[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE))
        {
            int len = strlen(buffer);
            if (len > 0)
                buffer[len - 1] = '\0';
        }
    }
}

//ID generation functions
int generateUserID() { return nextUserID++; }
int generateRestaurantID() { return nextRestaurantID++; }
int generateMenuItemID() { return nextMenuItemID++; }
int generateOrderID() { return nextOrderID++; }

// -----------------------------
// File I/O Functions
// -----------------------------
void saveAllData()
{
    ofstream file;

    file.open("users.txt");
    file << userCount << endl;
    for (int i = 0; i < userCount; i++)
    {
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
    for (int i = 0; i < restaurantCount; i++)
    {
        Restaurant &r = restaurants[i];
        file << r.id << endl;
        file << r.ownerID << endl;
        file << r.name << endl;
        file << r.location << endl;
        file << r.rating << endl;
        file << r.menuItemCount << endl;
        for (int j = 0; j < r.menuItemCount; j++)
            file << r.menuItemIDs[j] << " ";
        file << endl;
        file << r.isOpen << endl;
    }
    file.close();

    file.open("menuItems.txt");
    file << menuItemCount << endl;
    for (int i = 0; i < menuItemCount; i++)
    {
        file << menuItems[i].id << endl;
        file << menuItems[i].restaurantID << endl;
        file << menuItems[i].name << endl;
        file << menuItems[i].price << endl;
    }
    file.close();

    file.open("orders.txt");
    file << orderCount << endl;
    for (int i = 0; i < orderCount; i++)
    {
        Order &o = orders[i];
        file << o.id << endl;
        file << o.customerID << endl;
        file << o.restaurantID << endl;
        file << o.itemCount << endl;
        for (int j = 0; j < o.itemCount; j++)
            file << o.itemIDs[j] << " ";
        file << endl;
        file << o.status << endl;
        file << o.paymentMethod << endl;
        file << o.totalAmount << endl;
        // Save Color as 4 separate values
        file << (int)o.statusColor.r << " "
             << (int)o.statusColor.g << " "
             << (int)o.statusColor.b << " "
             << (int)o.statusColor.a << endl;
    }
    file.close();
}

void loadAllData()
{
    ifstream file;

    file.open("users.txt");
    if (file)
    {
        file >> userCount;
        file.ignore();
        for (int i = 0; i < userCount; i++)
        {
            file >> users[i].id;
            if (users[i].id >= nextUserID) nextUserID = users[i].id + 1;
            file.ignore();
            getline(file, users[i].name);
            getline(file, users[i].email);
            getline(file, users[i].password);
            getline(file, users[i].role);
            getline(file, users[i].address);
            getline(file, users[i].phone);
        }
        file.close();
    }

    file.open("restaurants.txt");
    if (file)
    {
        file >> restaurantCount;
        file.ignore();
        for (int i = 0; i < restaurantCount; i++)
        {
            Restaurant &r = restaurants[i];
            file >> r.id;
            if (r.id >= nextRestaurantID) nextRestaurantID = r.id + 1;
            file >> r.ownerID;
            file.ignore();
            getline(file, r.name);
            getline(file, r.location);
            file >> r.rating >> r.menuItemCount;
            file.ignore();
            for (int j = 0; j < r.menuItemCount; j++)
                file >> r.menuItemIDs[j];
            file.ignore();
            file >> r.isOpen;
            file.ignore();
        }
        file.close();
    }

    file.open("menuItems.txt");
    if (file)
    {
        file >> menuItemCount;
        file.ignore();
        for (int i = 0; i < menuItemCount; i++)
        {
            file >> menuItems[i].id;
            if (menuItems[i].id >= nextMenuItemID) nextMenuItemID = menuItems[i].id + 1;
            file >> menuItems[i].restaurantID;
            file.ignore();
            getline(file, menuItems[i].name);
            file >> menuItems[i].price;
            file.ignore();
        }
        file.close();
    }

    file.open("orders.txt");
    if (file)
    {
        file >> orderCount;
        file.ignore();
        for (int i = 0; i < orderCount; i++)
        {
            Order &o = orders[i];
            file >> o.id;
            if (o.id >= nextOrderID) nextOrderID = o.id + 1;
            file >> o.customerID >> o.restaurantID >> o.itemCount;
            file.ignore();

            // Read itemIDs
            for (int j = 0; j < o.itemCount; j++)
                file >> o.itemIDs[j];
            file.ignore(numeric_limits<streamsize>::max(), '\n');

            // Then read status and payment
            getline(file, o.status);
            getline(file, o.paymentMethod);

            // Then total amount
            file >> o.totalAmount;

            // Finally read color
            int r, g, b, a;
            file >> r >> g >> b >> a;
            o.statusColor = Color{(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
            file.ignore();
        }
        file.close();
    }
}

int main()
{
    InitWindow(1000, 700, "Food Ordering System");
    SetTargetFPS(60);
    loadAllData();

    // Main Loop
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Global logout Button (except on login/register)
        if (currentScreen != SCREEN_LOGIN && currentScreen != SCREEN_REGISTER)
        {
            if (GuiButton((Rectangle{10, 10, 80, 30}), "Logout"))
            {
                currentScreen = SCREEN_LOGIN;
                activeBoxID = 0;
                role = false;
            }
        }

        switch (currentScreen)
        {
        // ---------------------------------------------------------
        // CASE 1: LOGIN SCREEN
        // ---------------------------------------------------------
        case SCREEN_LOGIN:
        {
            DrawText("LOGIN", 450, 100, 40, DARKGRAY);

            GuiTextBox((Rectangle{350, 200, 300, 40}), emailBuffer, 64, 1, "Email");
            GuiTextBox((Rectangle{350, 260, 300, 40}), passBuffer, 64, 2, "Password");

            // LOGIN BUTTON LOGIC
            if (GuiButton((Rectangle{350, 350, 140, 40}), "Login"))
            {
                string em = emailBuffer;
                string pa = passBuffer;
                bool found = false;

                // Search for user
                for (int i = 0; i < userCount; i++)
                {
                    if (users[i].email == em && users[i].password == pa)
                    {
                        currentUserID = users[i].id;
                        found = true;
                        // Redirect based on role
                        if (users[i].role == "owner")
                            currentScreen = SCREEN_OWNER_DASH;
                        else
                            currentScreen = SCREEN_CUSTOMER_DASH;

                        // Reset buffers for safety
                        memset(emailBuffer, 0, 64);
                        memset(passBuffer, 0, 64);
                        break;
                    }
                }
                if (!found)
                    activeBoxID = -1; // Simple way to trigger "Invalid" text
            }

            // REGISTER NAVIGATION
            if (GuiButton((Rectangle{510, 350, 140, 40}), "Register"))
            {
                currentScreen = SCREEN_REGISTER;
                // Clear buffers
                memset(nameBuffer, 0, 64);
                memset(emailBuffer, 0, 64);
                memset(passBuffer, 0, 64);
                memset(addressBuffer, 0, 64);
                memset(phoneBuffer, 0, 64);
                role = false; // Reset to customer by default
            }

            if (activeBoxID == -1)
                DrawText("Invalid Credentials!", 400, 400, 20, RED);
        }
        break;

        // ---------------------------------------------------------
        // CASE 2: REGISTRATION SCREEN
        // ---------------------------------------------------------
        case SCREEN_REGISTER:
        {
            DrawText("CREATE ACCOUNT", 380, 50, 30, DARKGRAY);

            GuiTextBox((Rectangle{350, 120, 300, 40}), nameBuffer, 64, 1, "Name");
            GuiTextBox((Rectangle{350, 180, 300, 40}), emailBuffer, 64, 2, "Email");
            GuiTextBox((Rectangle{350, 240, 300, 40}), passBuffer, 64, 3, "Password");
            GuiTextBox((Rectangle{350, 300, 300, 40}), addressBuffer, 64, 4, "Address");
            GuiTextBox((Rectangle{350, 360, 300, 40}), phoneBuffer, 64, 5, "Phone");

            // Role Selection using buttons
            DrawText("Select Role:", 350, 420, 20, DARKGRAY);
            
            // Customer button
            if (!role)
            {
                DrawRectangle(350, 450, 140, 40, BLUE);
                DrawText("Customer", 375, 460, 20, WHITE);
            }
            else
            {
                if (GuiButton((Rectangle{350, 450, 140, 40}), "Customer"))
                    role = false;
            }

            // Owner button
            if (role)
            {
                DrawRectangle(510, 450, 140, 40, BLUE);
                DrawText("Owner", 550, 460, 20, WHITE);
            }
            else
            {
                if (GuiButton((Rectangle{510, 450, 140, 40}), "Owner"))
                    role = true;
            }

            if (GuiButton((Rectangle{350, 520, 300, 50}), "Complete Registration"))
            {
                // Check if all fields are filled
                if (strlen(nameBuffer) > 0 && strlen(emailBuffer) > 0 && 
                    strlen(passBuffer) > 0 && strlen(addressBuffer) > 0 && 
                    strlen(phoneBuffer) > 0)
                {
                    User u;
                    u.id = generateUserID();
                    u.name = nameBuffer;
                    u.email = emailBuffer;
                    u.password = passBuffer;
                    u.address = addressBuffer;
                    u.phone = phoneBuffer;
                    u.role = role ? "owner" : "customer";

                    // Add to array if there's space
                    if (userCount < 100)
                    {
                        users[userCount++] = u;
                        currentScreen = SCREEN_LOGIN;
                        
                        // Clear all buffers
                        memset(nameBuffer, 0, 64);
                        memset(emailBuffer, 0, 64);
                        memset(passBuffer, 0, 64);
                        memset(addressBuffer, 0, 64);
                        memset(phoneBuffer, 0, 64);
                        role = false;
                    }
                }
                else
                {
                    activeBoxID = -2; // Show error
                }
            }

            if (activeBoxID == -2)
                DrawText("Please fill all fields!", 400, 580, 20, RED);

            // Cancel button
            if (GuiButton((Rectangle{10, 50, 80, 30}), "Back"))
                currentScreen = SCREEN_LOGIN;
        }
        break;

        // ---------------------------------------------------------
        // CASE 3: CUSTOMER DASHBOARD
        // ---------------------------------------------------------
        case SCREEN_CUSTOMER_DASH:
        {
            DrawText("Select a Restaurant:", 50, 60, 30, DARKGRAY);

            if (GuiButton(Rectangle{450, 50, 200, 40}, "View My Orders"))
                currentScreen = SCREEN_CUSTOMER_ORDERS;

            if (GuiButton(Rectangle{400, 100, 250, 40}, "View/Hide Restaurants"))
                viewRestaurants = !viewRestaurants;

            if (viewRestaurants)
            {
                if (restaurantCount == 0)
                {
                    DrawText("No restaurants available", 50, 150, 20, DARKGRAY);
                }
                else
                {
                    int drawnCount = 0;
                    for (int i = 0; i < restaurantCount; i++)
                    {
                        if (restaurants[i].isOpen)
                        {
                            string btnText = restaurants[i].name + " (" + restaurants[i].location + ")";
                            if (GuiButton((Rectangle{50, (float)(150 + drawnCount * 60), 400, 50}), btnText))
                            {
                                selectedRestaurantID = restaurants[i].id;
                                currentScreen = SCREEN_RESTAURANT_MENU;
                            }
                            drawnCount++;
                        }
                    }
                }
            }
        }
        break;

        // ---------------------------------------------------------
        // CASE 4: MENU VIEW (Ordering)
        // ---------------------------------------------------------
        case SCREEN_RESTAURANT_MENU:
        {
            DrawText("MENU", 50, 100, 30, DARKGRAY);
            if (GuiButton(Rectangle{10, 50, 80, 30}, "Back"))
            {
                currentScreen = SCREEN_CUSTOMER_DASH;
                orderFeedbackTimer = 0;
                cartItemCount = 0;
                cartTotal = 0.0;
            }

            // -----------------------------------------------------
            // LEFT SIDE: THE MENU (Select Items)
            // -----------------------------------------------------
            DrawText("Click items to add to cart:", 50, 140, 20, GRAY);

            int drawnCount = 0;
            for (int i = 0; i < menuItemCount; i++)
            {
                if (menuItems[i].restaurantID == selectedRestaurantID)
                {
                    string itemText = menuItems[i].name + " - $" + to_string(menuItems[i].price).substr(0, 4);

                    if (GuiButton(Rectangle{50, (float)(180 + drawnCount * 60), 300, 50}, itemText))
                    {
                        if (cartItemCount < 50)
                        {
                            cartItemIDs[cartItemCount] = menuItems[i].id;
                            cartItemCount++;
                            cartTotal += menuItems[i].price;
                        }
                    }
                    drawnCount++;
                }
            }

            // -----------------------------------------------------
            // RIGHT SIDE: YOUR ORDER (The Cart)
            // -----------------------------------------------------
            int cartX = 550;
            DrawRectangleLines(cartX - 20, 100, 400, 500, LIGHTGRAY);
            DrawText("YOUR ORDER", cartX, 120, 25, DARKGRAY);

            int cartY = 160;

            for (int i = 0; i < cartItemCount; i++)
            {
                for (int m = 0; m < menuItemCount; m++)
                {
                    if (menuItems[m].id == cartItemIDs[i])
                    {
                        string line = "- " + menuItems[m].name + " ($" + to_string(menuItems[m].price).substr(0, 4) + ")";
                        DrawText(line.c_str(), cartX, cartY, 20, BLACK);
                        cartY += 30;
                        break;
                    }
                }
            }

            // Show Total
            DrawRectangle(cartX - 20, 550, 400, 50, LIGHTGRAY);
            string totalTxt = "Total: $" + to_string(cartTotal).substr(0, 6);
            DrawText(totalTxt.c_str(), cartX + 10, 565, 25, BLACK);

            // -----------------------------------------------------
            // PLACE ORDER BUTTON (Only if cart has items)
            // -----------------------------------------------------
            if (cartItemCount > 0)
            {
                if (GuiButton(Rectangle{(float)cartX, 620, 300, 50}, "Confirm & Place Order"))
                {
                    if (orderCount < 500)
                    {
                        Order o;
                        o.id = generateOrderID();
                        o.customerID = currentUserID;
                        o.restaurantID = selectedRestaurantID;
                        o.itemCount = cartItemCount;
                        o.totalAmount = cartTotal;
                        o.status = "Placed";
                        o.statusColor = ORANGE;
                        o.paymentMethod = "Cash";

                        for (int k = 0; k < cartItemCount; k++)
                            o.itemIDs[k] = cartItemIDs[k];

                        orders[orderCount++] = o;

                        lastOrderedItemName = to_string(cartItemCount) + " Items";
                        orderFeedbackTimer = 120;

                        cartItemCount = 0;
                        cartTotal = 0.0;
                    }
                }
            }

            // --- SUCCESS POPUP ---
            if (orderFeedbackTimer > 0)
            {
                orderFeedbackTimer--;
                DrawRectangle(0, 0, 1000, 700, Fade(BLACK, 0.3f));
                int boxX = 300, boxY = 250;
                DrawRectangle(boxX, boxY, 400, 150, RAYWHITE);
                DrawRectangleLines(boxX, boxY, 400, 150, GREEN);
                DrawText("ORDER PLACED!", boxX + 110, boxY + 30, 20, GREEN);
                DrawText("Wait for confirmation...", boxX + 100, boxY + 70, 20, GRAY);
            }
        }
        break;

        // ---------------------------------------------------------
        // CASE 5: OWNER DASHBOARD
        // ---------------------------------------------------------
        case SCREEN_OWNER_DASH:
        {
            DrawText("OWNER DASHBOARD", 350, 50, 30, DARKGRAY);

            if (GuiButton((Rectangle{300, 200, 400, 60}), "Create New Restaurant"))
            {
                currentScreen = SCREEN_CREATE_RESTAURANT;
                memset(restNameBuffer, 0, 64);
                memset(restLocBuffer, 0, 64);
            }

            if (GuiButton((Rectangle{300, 300, 400, 60}), "View Incoming Orders"))
                currentScreen = SCREEN_OWNER_ORDERS;

            if (GuiButton((Rectangle{300, 400, 400, 60}), "Manage Orders Status"))
                currentScreen = SCREEN_OWNER_MANAGE_ORDERS_STATUS;

            if (GuiButton(Rectangle{300, 500, 400, 60}, "Manage Menus / Add Items"))
                currentScreen = SCREEN_OWNER_VIEW_RESTAURANTS;
        }
        break;

        // ---------------------------------------------------------
        // CASE 6: CREATE RESTAURANT
        // ---------------------------------------------------------
        case SCREEN_CREATE_RESTAURANT:
        {
            DrawText("NEW RESTAURANT", 350, 50, 30, DARKGRAY);

            GuiTextBox((Rectangle{300, 150, 400, 40}), restNameBuffer, 64, 1, "Restaurant Name");
            GuiTextBox((Rectangle{300, 220, 400, 40}), restLocBuffer, 64, 2, "Location");

            if (GuiButton((Rectangle{350, 350, 300, 50}), "Save Restaurant"))
            {
                if (strlen(restNameBuffer) > 0 && strlen(restLocBuffer) > 0)
                {
                    if (restaurantCount < 50)
                    {
                        Restaurant r;
                        r.id = generateRestaurantID();
                        r.ownerID = currentUserID;
                        r.name = restNameBuffer;
                        r.location = restLocBuffer;
                        r.isOpen = true;
                        r.rating = 5;
                        r.menuItemCount = 0;

                        restaurants[restaurantCount++] = r;
                        currentScreen = SCREEN_OWNER_DASH;
                    }
                }
                else
                {
                    activeBoxID = -2;
                }
            }

            if (activeBoxID == -2)
                DrawText("Please enter restaurant name and location!", 250, 420, 20, RED);

            if (GuiButton((Rectangle{10, 50, 80, 30}), "Back"))
                currentScreen = SCREEN_OWNER_DASH;
        }
        break;

        // ---------------------------------------------------------
        // CASE 7: OWNER VIEW INCOMING ORDERS
        // ---------------------------------------------------------
        case SCREEN_OWNER_ORDERS:
        {
            DrawText("INCOMING ORDERS", 350, 50, 30, DARKGRAY);
            if (GuiButton((Rectangle{10, 50, 80, 30}), "Back"))
                currentScreen = SCREEN_OWNER_DASH;

            int orderY = 100;
            bool hasOrders = false;

            for (int i = 0; i < orderCount; i++)
            {
                bool isMyRestaurant = false;
                for (int r = 0; r < restaurantCount; r++)
                {
                    if (restaurants[r].id == orders[i].restaurantID && restaurants[r].ownerID == currentUserID)
                    {
                        isMyRestaurant = true;
                        break;
                    }
                }

                if (isMyRestaurant && orders[i].status == "Placed")
                {
                    hasOrders = true;
                    string orderTxt = "Order #" + to_string(orders[i].id) + " - $" + to_string(orders[i].totalAmount).substr(0, 6);
                    DrawText(orderTxt.c_str(), 50, orderY, 20, BLACK);

                    if (GuiButton((Rectangle{400, (float)orderY, 100, 30}), "Accept"))
                        orders[i].status = "Preparing";
                    
                    if (GuiButton((Rectangle{520, (float)orderY, 100, 30}), "Reject"))
                        orders[i].status = "Rejected";

                    orderY += 50;
                }
            }

            if (!hasOrders)
                DrawText("No incoming orders", 50, 150, 20, GRAY);
        }
        break;

        // ---------------------------------------------------------
        // CASE 7.5: OWNER MANAGE ORDERS STATUS
        // ---------------------------------------------------------
        case SCREEN_OWNER_MANAGE_ORDERS_STATUS:
        {
            DrawText("MANAGE ORDERS STATUS", 350, 50, 30, DARKGRAY);
            if (GuiButton((Rectangle{10, 50, 80, 30}), "Back"))
                currentScreen = SCREEN_OWNER_DASH;

            int orderY = 100;
            bool hasOrders = false;

            for (int i = 0; i < orderCount; i++)
            {
                bool isMyRestaurant = false;
                for (int r = 0; r < restaurantCount; r++)
                {
                    if (restaurants[r].id == orders[i].restaurantID && restaurants[r].ownerID == currentUserID)
                    {
                        isMyRestaurant = true;
                        break;
                    }
                }

                if (isMyRestaurant && orders[i].status == "Preparing")
                {
                    hasOrders = true;
                    string orderTxt = "Order #" + to_string(orders[i].id) + " - $" + to_string(orders[i].totalAmount).substr(0, 6);
                    DrawText(orderTxt.c_str(), 50, orderY, 20, BLACK);

                    if (GuiButton((Rectangle{450, (float)orderY, 200, 30}), "Mark as Ready for delivery"))
                        orders[i].status = "Ready";

                    orderY += 50;
                }

                if (isMyRestaurant && orders[i].status == "Ready")
                {
                    hasOrders = true;
                    string orderTxt = "Order #" + to_string(orders[i].id) + " - $" + to_string(orders[i].totalAmount).substr(0, 6);
                    DrawText(orderTxt.c_str(), 50, orderY, 20, BLACK);

                    if (GuiButton((Rectangle{450, (float)orderY, 200, 30}), "Mark as Delivered"))
                        orders[i].status = "Delivered";

                    orderY += 50;
                }
            }

            if (!hasOrders)
                DrawText("No orders to manage", 50, 150, 20, GRAY);
        }
        break;

        // ---------------------------------------------------------
        // CASE 8: OWNER VIEW THEIR RESTAURANTS
        // ---------------------------------------------------------
        case SCREEN_OWNER_VIEW_RESTAURANTS:
        {
            DrawText("MY RESTAURANTS", 350, 50, 30, DARKGRAY);
            DrawText("Select a restaurant to add items", 320, 90, 20, GRAY);
            
            if (GuiButton(Rectangle{10, 50, 80, 30}, "Back"))
                currentScreen = SCREEN_OWNER_DASH;

            int yPos = 140;
            bool foundAny = false;

            for (int i = 0; i < restaurantCount; i++)
            {
                if (restaurants[i].ownerID == currentUserID)
                {
                    foundAny = true;
                    string btnText = restaurants[i].name + " (" + restaurants[i].location + ")";

                    if (GuiButton(Rectangle{50, (float)yPos, 400, 50}, btnText))
                    {
                        selectedRestaurantID = restaurants[i].id;
                        currentScreen = SCREEN_OWNER_ADD_MENU_ITEM;
                        memset(itemNameBuffer, 0, 64);
                        memset(itemPriceBuffer, 0, 64);
                    }
                    
                    // Toggle Status Button
                    string statusText = restaurants[i].isOpen ? "OPEN" : "CLOSED";
                    Color statusColor = restaurants[i].isOpen ? GREEN : RED;
                    
                    Rectangle statusBtn = {500, (float)yPos, 100, 50};
                    if (GuiButton(statusBtn, statusText))
                        restaurants[i].isOpen = !restaurants[i].isOpen;
                    
                    // Draw status indicator
                    DrawRectangle(statusBtn.x, statusBtn.y, 10, 50, statusColor);

                    yPos += 60;
                }
            }

            if (!foundAny)
                DrawText("You haven't created any restaurants yet.", 50, 150, 20, BLACK);
        }
        break;

        // ---------------------------------------------------------
        // CASE 9: ADD MENU ITEMS
        // ---------------------------------------------------------
        case SCREEN_OWNER_ADD_MENU_ITEM:
        {
            DrawText("ADD MENU ITEM", 380, 50, 30, DARKGRAY);

            // Find restaurant name
            string rName = "Unknown";
            for (int i = 0; i < restaurantCount; i++)
            {
                if (restaurants[i].id == selectedRestaurantID)
                {
                    rName = restaurants[i].name;
                    break;
                }
            }
            DrawText(("Adding to: " + rName).c_str(), 380, 90, 20, GRAY);

            // Input Fields
            GuiTextBox(Rectangle{300, 150, 400, 40}, itemNameBuffer, 64, 1, "Item Name (e.g. Burger)");
            GuiTextBox(Rectangle{300, 220, 400, 40}, itemPriceBuffer, 64, 2, "Price (e.g. 15.50)");

            // Save Button
            if (GuiButton(Rectangle{350, 300, 300, 50}, "Add Item to Menu"))
            {
                if (strlen(itemNameBuffer) > 0 && strlen(itemPriceBuffer) > 0)
                {
                    try
                    {
                        float price = stof(itemPriceBuffer);
                        if (price > 0 && menuItemCount < 500)
                        {
                            MenuItem m;
                            m.id = generateMenuItemID();
                            m.restaurantID = selectedRestaurantID;
                            m.name = itemNameBuffer;
                            m.price = price;

                            menuItems[menuItemCount++] = m;

                            // Update restaurant's menu item list
                            for (int i = 0; i < restaurantCount; i++)
                            {
                                if (restaurants[i].id == selectedRestaurantID)
                                {
                                    if (restaurants[i].menuItemCount < 50)
                                    {
                                        restaurants[i].menuItemIDs[restaurants[i].menuItemCount] = m.id;
                                        restaurants[i].menuItemCount++;
                                    }
                                    break;
                                }
                            }

                            memset(itemNameBuffer, 0, 64);
                            memset(itemPriceBuffer, 0, 64);
                            activeBoxID = 0;
                        }
                    }
                    catch (...)
                    {
                        activeBoxID = -2; // Invalid price
                    }
                }
                else
                {
                    activeBoxID = -2;
                }
            }

            if (activeBoxID == -2)
                DrawText("Invalid item name or price!", 350, 370, 20, RED);

            // Show existing items
            DrawText("Existing Items:", 300, 400, 20, DARKGRAY);
            int displayY = 430;
            int itemCount = 0;
            for (int i = 0; i < menuItemCount && itemCount < 10; i++)
            {
                if (menuItems[i].restaurantID == selectedRestaurantID)
                {
                    string listing = menuItems[i].name + " - $" + to_string(menuItems[i].price).substr(0, 4);
                    DrawText(listing.c_str(), 300, displayY, 20, GRAY);
                   


                   if(GuiButton(Rectangle{600, 430, 80, 30}, "Remove Item")){

                       
                        for (int j = 0; j < menuItemCount; j++)
                        {
                            if (menuItems[j].id == menuItems[i].id)
                            {
                               
                                for (int k = j; k < menuItemCount - 1; k++)
                                    menuItems[k] = menuItems[k + 1];
                                menuItemCount--;
                                break;
                            }
                        }

                   
                        for (int r = 0; r < restaurantCount; r++)
                        {
                            if (restaurants[r].id == selectedRestaurantID)
                            {
                                for (int m = 0; m < restaurants[r].menuItemCount; m++)
                                {
                                    if (restaurants[r].menuItemIDs[m] == menuItems[i].id)
                                    {
                                        
                                        for (int n = m; n < restaurants[r].menuItemCount - 1; n++)
                                            restaurants[r].menuItemIDs[n] = restaurants[r].menuItemIDs[n + 1];
                                        restaurants[r].menuItemCount--;
                                        break;
                                    }
                                }
                                break;
                            }
                        }

                        break; 
                   }



                    displayY += 25;
                    itemCount++;
                }
            }

            if (itemCount == 0)
                DrawText("No items yet", 300, displayY, 20, LIGHTGRAY);

            // Back Button
            if (GuiButton(Rectangle{10, 50, 80, 30}, "Back"))
                currentScreen = SCREEN_OWNER_VIEW_RESTAURANTS;
        }
        break;

        // ---------------------------------------------------------
        // CASE 10: CUSTOMER VIEW ORDERS
        // ---------------------------------------------------------
        case SCREEN_CUSTOMER_ORDERS:
        {
            DrawText("MY ORDERS", 350, 50, 30, DARKGRAY);
            if (GuiButton(Rectangle{10, 50, 80, 30}, "Back"))
                currentScreen = SCREEN_CUSTOMER_DASH;

            int yPos = 100;
            bool foundAny = false;

            // Current Orders
            DrawText("Current Orders", 50, yPos, 20, DARKGRAY);
            yPos += 30;

            for (int i = 0; i < orderCount; i++)
            {
                if (orders[i].customerID == currentUserID &&
                    orders[i].status != "Delivered" && orders[i].status != "Rejected")
                {
                    foundAny = true;

                    // Find Restaurant Name
                    string rName = "Unknown";
                    for (int r = 0; r < restaurantCount; r++)
                    {
                        if (restaurants[r].id == orders[i].restaurantID)
                        {
                            rName = restaurants[r].name;
                            break;
                        }
                    }

                    // Draw Order Box
                    DrawRectangleLines(50, yPos, 500, 80, DARKGRAY);

                    string header = "Order #" + to_string(orders[i].id) + " - " + rName;
                    DrawText(header.c_str(), 60, yPos + 10, 20, BLACK);

                    string details = "Total: $" + to_string(orders[i].totalAmount).substr(0, 6) + 
                                   " | Items: " + to_string(orders[i].itemCount);
                    DrawText(details.c_str(), 60, yPos + 35, 18, GRAY);

                    // Status Color
                    if (orders[i].status == "Placed")
                        orders[i].statusColor = ORANGE;
                    else if (orders[i].status == "Preparing")
                        orders[i].statusColor = BLUE;
                    else if (orders[i].status == "Ready")
                        orders[i].statusColor = PURPLE;
                    else if (orders[i].status == "Delivered")
                        orders[i].statusColor = GREEN;

                    DrawText(("Status: " + orders[i].status).c_str(), 60, yPos + 55, 18, orders[i].statusColor);

                    yPos += 90;
                }
            }

            if (!foundAny)
            {
                DrawText("No current orders", 50, yPos, 20, GRAY);
                yPos += 40;
            }

            // Past Orders
            DrawText("Past History", 50, yPos, 20, DARKGRAY);
            yPos += 30;

            bool foundPast = false;
            for (int i = 0; i < orderCount; i++)
            {
                if (orders[i].customerID == currentUserID &&
                    (orders[i].status == "Delivered" || orders[i].status == "Rejected"))
                {
                    foundPast = true;

                    string rName = "Unknown";
                    for (int r = 0; r < restaurantCount; r++)
                    {
                        if (restaurants[r].id == orders[i].restaurantID)
                        {
                            rName = restaurants[r].name;
                            break;
                        }
                    }

                    DrawRectangleLines(50, yPos, 500, 60, LIGHTGRAY);

                    string txt = "Order #" + to_string(orders[i].id) + " (" + rName + ") - " + orders[i].status;

                    orders[i].statusColor = (orders[i].status == "Delivered") ? GREEN : RED;
                    DrawText(txt.c_str(), 60, yPos + 20, 18, orders[i].statusColor);

                    yPos += 70;
                }
            }

            if (!foundPast)
                DrawText("No past orders", 50, yPos, 20, GRAY);

            if (!foundAny && !foundPast)
                DrawText("You have no order history.", 50, 150, 20, GRAY);
        }
        break;
        }

        EndDrawing();
    }

    saveAllData();
    CloseWindow();

    return 0;
}