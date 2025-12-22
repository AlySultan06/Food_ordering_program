#include <iostream>
#include <string>
#include <cstring>
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
    Color statusColor;
};

// -----------------------------
// Global Arrays AND VARIABLES
// -----------------------------
User users[100];
int userCount = -1;

Restaurant restaurants[50];
int restaurantCount = 0;

MenuItem menuItems[500];
int menuItemCount = 0;

Order orders[500];
int orderCount = 0;

// Track last IDs for file I/O
int lastUserID = 0;
int lastRestaurantID = 0;
int lastMenuItemID = 0;
int lastOrderID = 0;

int currentUserID;
// for owners
int ownerreterauntids[50];
// for customers
int selectedRestaurantID = -1;

// -----------------------------
// Utility Functions
// -----------------------------
int generateUserID()
{
    userCount++;
    return userCount;
}
int generateRestaurantID()
{
  
    return restaurantCount;
}
int generateMenuItemID()
{
    
    return menuItemCount;
}
int generateOrderID()
{
    return orderCount;
}

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

// input Buffers
char nameBuffer[64] = "\0";
char emailBuffer[64] = "\0";
char passBuffer[64] = "\0";
char addressBuffer[64] = "\0";
char roleBuffer[64] = "\0"; // Type "customer" or "owner"
char phoneBuffer[64] = "\0";
char itemNameBuffer[64] = "\0";
char itemPriceBuffer[64] = "\0";

// Restaurant Creation Buffers
char restNameBuffer[64] = "\0";
char restLocBuffer[64] = "\0";

// State Trackers + global vars(the ones that were removed from loop or functions)
int activeBoxID = 0;
int orderFeedbackTimer = 0;
string lastOrderedItemName = "";
bool viewresteraunts = true;
int cartItemIDs[50];
int cartItemCount = 0;
float cartTotal = 0.0;

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
// ID: A unique number for this box so we know which one is active
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
            file.ignore();
            getline(file, users[i].name);
            getline(file, users[i].email);
            getline(file, users[i].password);
            getline(file, users[i].role);
            getline(file, users[i].address);
            getline(file, users[i].phone);
            if (users[i].id > lastUserID)
                lastUserID = users[i].id;
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
            file >> r.id >> r.ownerID;
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
            if (r.id > lastRestaurantID)
                lastRestaurantID = r.id;
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
            file >> menuItems[i].id >> menuItems[i].restaurantID;
            file.ignore();
            getline(file, menuItems[i].name);
            file >> menuItems[i].price;
            file.ignore();
            if (menuItems[i].id > lastMenuItemID)
                lastMenuItemID = menuItems[i].id;
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
            file >> o.id >> o.customerID >> o.restaurantID >> o.itemCount;
            file.ignore();

            // Read itemIDs first
            for (int j = 0; j < o.itemCount; j++)
                file >> o.itemIDs[j];
            file.ignore(numeric_limits<streamsize>::max(), '\n'); // ← Skip to end of line

            // Then read status and payment
            getline(file, o.status);
            getline(file, o.paymentMethod);

            // Then total amount
            file >> o.totalAmount;

            // Finally read color (last, as saved)
            int r, g, b, a;
            file >> r >> g >> b >> a;
            o.statusColor = (Color){(unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a};
            file.ignore();

            if (o.id > lastOrderID)
                lastOrderID = o.id;
        }
        file.close();
    }
}

int main()
{
    InitWindow(1000, 700, "Food Ordering System"); // create program window
    SetTargetFPS(60);
    loadAllData();

    // Main Loop
    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Global exit Button (except on login)
        if (currentScreen != SCREEN_LOGIN && currentScreen != SCREEN_REGISTER)
        {
            if (GuiButton((Rectangle{10, 10, 80, 30}), "Logout"))
            {
                currentScreen = SCREEN_LOGIN;
                activeBoxID = 0;
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
                memset(roleBuffer, 0, 64);
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
            GuiTextBox((Rectangle{350, 420, 300, 40}), roleBuffer, 64, 6, "Role (owner/customer)");

            if (GuiButton((Rectangle{350, 500, 300, 50}), "Complete Registration"))
            {
                // Logic from old registerUser()
                User u;
                u.id = generateUserID();
                u.name = nameBuffer;
                u.email = emailBuffer;
                u.password = passBuffer;
                u.address = addressBuffer;
                u.phone = phoneBuffer;
                u.role = roleBuffer; // "owner" or "customer"

                users[userCount++] = u;       // Add to array
                currentScreen = SCREEN_LOGIN; // Go back to login
            }

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

            if (GuiButton(Rectangle{350, 50, 200, 40}, "View My Orders"))
            {
                currentScreen = SCREEN_CUSTOMER_ORDERS;
            }

            if (GuiButton(Rectangle{350, 100, 200, 40}, "view/hide restraunts")){
                viewresteraunts = !viewresteraunts;
            
            }
           




            if (viewresteraunts)
            {
                // Loop through restaurants and draw a button for each
                for (int i = 0; i < restaurantCount; i++)
                {
                    // Draw each button lower down the screen (y = 120 + i*60)
                    string btnText = restaurants[i].name + " (" + restaurants[i].location + ")";
                    if (GuiButton((Rectangle{50, (float)(150 + i * 60), 400, 50}), btnText))
                    {
                        selectedRestaurantID = restaurants[i].id;
                        currentScreen = SCREEN_RESTAURANT_MENU;
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
            DrawText("MENU", 50, 60, 30, DARKGRAY);
            if (GuiButton(Rectangle{10, 50, 80, 30}, "Back"))
            {
                currentScreen = SCREEN_CUSTOMER_DASH;
                orderFeedbackTimer = 0; // Reset timer when leaving
                cartItemCount = 0;
                cartTotal = 0.0;
            }

            // -----------------------------------------------------
            // LEFT SIDE: THE MENU (Select Items)
            // -----------------------------------------------------
            DrawText("Click items to add to cart:", 50, 100, 20, GRAY);

            int drawnCount = 0;
            for (int i = 0; i < menuItemCount; i++)
            {
                if (menuItems[i].restaurantID == selectedRestaurantID)
                {

                    string itemText = menuItems[i].name + " - $" + to_string((int)menuItems[i].price);

                    // ITEM BUTTON
                    if (GuiButton(Rectangle{50, (float)(140 + drawnCount * 60), 300, 50}, itemText))
                    {
                        // Add to Cart (Limit 50 items)
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
            int cartX = 550;                                          // Start drawing cart on the right half
            DrawRectangleLines(cartX - 20, 100, 400, 500, LIGHTGRAY); // Visual Border
            DrawText("YOUR ORDER", cartX, 120, 25, DARKGRAY);

            int cartY = 160;

            // Loop through cart items to display them
            for (int i = 0; i < cartItemCount; i++)
            {
                // We have the ID, we need to find the Name/Price
                for (int m = 0; m < menuItemCount; m++)
                {
                    if (menuItems[m].id == cartItemIDs[i])
                    {
                        string line = "- " + menuItems[m].name + " ($" + to_string((int)menuItems[m].price) + ")";
                        DrawText(line.c_str(), cartX, cartY, 20, BLACK);
                        cartY += 30;
                    }
                }
            }

            // Show Total
            DrawRectangle(cartX - 20, 550, 400, 50, LIGHTGRAY); // Background for total
            string totalTxt = "Total: $" + to_string((int)cartTotal);
            DrawText(totalTxt.c_str(), cartX + 10, 565, 25, BLACK);

            // -----------------------------------------------------
            // PLACE ORDER BUTTON (Only if cart has items)
            // -----------------------------------------------------
            if (cartItemCount > 0)
            {
                if (GuiButton(Rectangle{(float)cartX, 620, 300, 50}, "Confirm & Place Order"))
                {
                    // 1. Create the Final Order
                    Order o;
                    o.id = generateOrderID();
                    o.customerID = currentUserID;
                    o.restaurantID = selectedRestaurantID;
                    o.itemCount = cartItemCount;
                    o.totalAmount = cartTotal;
                    o.status = "Placed";
                    o.statusColor = ORANGE;
                    o.paymentMethod = "Cash";

                    // Copy items from cart to order
                    for (int k = 0; k < cartItemCount; k++)
                    {
                        o.itemIDs[k] = cartItemIDs[k];
                    }

                    orders[orderCount] = o;
                    orderCount++;

                    // 2. Visual Feedback
                    lastOrderedItemName = to_string(cartItemCount) + " Items"; // Update feedback text
                    orderFeedbackTimer = 120;                                  // Show popup

                    // 3. Clear Cart
                    cartItemCount = 0;
                    cartTotal = 0.0;
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
            {
                currentScreen = SCREEN_OWNER_ORDERS;
            }

            if (GuiButton((Rectangle{300, 400, 400, 60}), "Manage Orders Status"))
            {
                currentScreen = SCREEN_OWNER_MANAGE_ORDERS_STATUS;
            }

            if (GuiButton(Rectangle{300, 500, 400, 60}, "Manage Menus / Add Items"))
            {
                currentScreen = SCREEN_OWNER_VIEW_RESTAURANTS;
            }
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
                Restaurant r;
                r.id = generateRestaurantID();
                r.ownerID = currentUserID;
                r.name = restNameBuffer;
                r.location = restLocBuffer;
                r.isOpen = true;
                r.rating = 5;

                restaurants[restaurantCount++] = r;
                currentScreen = SCREEN_OWNER_DASH;
            }

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
            for (int i = 0; i < orderCount; i++)
            {
                bool isMyRestaurant = false;
                for (int r = 0; r < restaurantCount; r++)
                {
                    if (restaurants[r].id == orders[i].restaurantID && restaurants[r].ownerID == currentUserID)
                    {
                        isMyRestaurant = true;
                    }
                }

                if (isMyRestaurant && orders[i].status == "Placed")
                {
                    string orderTxt = "Order #" + to_string(orders[i].id) + " - $" + to_string((int)orders[i].totalAmount);
                    DrawText(orderTxt.c_str(), 50, orderY, 20, BLACK);

                    // Accept Button
                    if (GuiButton((Rectangle{400, (float)orderY, 100, 30}), "Accept"))
                    {
                        orders[i].status = "Preparing";
                    }
                    // Reject Button
                    if (GuiButton((Rectangle{520, (float)orderY, 100, 30}), "Reject"))
                    {
                        orders[i].status = "Rejected";
                    }

                    orderY += 50;
                }
            }
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
            for (int i = 0; i < orderCount; i++)
            {
                bool isMyRestaurant = false;
                for (int r = 0; r < restaurantCount; r++)
                {
                    if (restaurants[r].id == orders[i].restaurantID && restaurants[r].ownerID == currentUserID)
                    {
                        isMyRestaurant = true;
                    }
                }

                if (isMyRestaurant && orders[i].status == "Preparing")
                {
                    string orderTxt = "Order #" + to_string(orders[i].id) + " - $" + to_string((int)orders[i].totalAmount);
                    DrawText(orderTxt.c_str(), 50, orderY, 20, BLACK);

                    // Upfate status to Ready Button
                    if (GuiButton((Rectangle{400, (float)orderY, 100, 30}), "Mark as Ready"))
                    {
                        orders[i].status = "Ready";
                    }

                    orderY += 50;
                }

                if (isMyRestaurant && orders[i].status == "Ready")
                {
                    string orderTxt = "Order #" + to_string(orders[i].id) + " - $" + to_string((int)orders[i].totalAmount);
                    DrawText(orderTxt.c_str(), 50, orderY, 20, BLACK);

                    // Update status to Delivered Button
                    if (GuiButton((Rectangle{400, (float)orderY, 100, 30}), "Mark as Delivered"))
                    {
                        orders[i].status = "Delivered";
                    }

                    orderY += 50;
                }
            }
        }
        break;

        // ---------------------------------------------------------
        // CASE 8: OWNER VIEW THEIR RESTAURANTS
        // ---------------------------------------------------------
        case SCREEN_OWNER_VIEW_RESTAURANTS:
        {
            DrawText("MY RESTAURANTS", 350, 50, 30, DARKGRAY);
            DrawText("Select a restaurant to add items", 320, 90, 20, GRAY);
            // Back Button
            if (GuiButton(Rectangle{10, 50, 80, 30}, "Back"))
                currentScreen = SCREEN_OWNER_DASH;

            int yPos = 140;
            bool foundAny = false;

            for (int i = 0; i < restaurantCount; i++)
            {
                // Only show restaurants owned by the current user
                if (restaurants[i].ownerID == currentUserID)
                {
                    foundAny = true;
                    string btnText = restaurants[i].name + " (" + restaurants[i].location + ")";

                    if (GuiButton(Rectangle{50, (float)yPos, 400, 50}, btnText))
                    {
                        selectedRestaurantID = restaurants[i].id; // Remember which one we picked
                        currentScreen = SCREEN_OWNER_ADD_MENU_ITEM;
                        // Clear buffers for the next screen
                        memset(itemNameBuffer, 0, 64);
                        memset(itemPriceBuffer, 0, 64);
                    }
                    yPos += 60;
                }
            }

            if (!foundAny)
            {
                DrawText("You haven't created any restaurants yet.", 50, 150, 20, BLACK);
            }
        }
        break;
        // ---------------------------------------------------------
        // CASE 9: ADD MENU ITEMS
        // ---------------------------------------------------------
        case SCREEN_OWNER_ADD_MENU_ITEM:
        {
            DrawText("ADD MENU ITEM", 380, 50, 30, DARKGRAY);

            // Find restaurant name for display purposes
            string rName = "Unknown";
            for (int i = 0; i < restaurantCount; i++)
            {
                if (restaurants[i].id == selectedRestaurantID)
                    rName = restaurants[i].name;
            }
            DrawText(("Adding to: " + rName).c_str(), 380, 90, 20, GRAY);

            // Input Fields
            GuiTextBox(Rectangle{300, 150, 400, 40}, itemNameBuffer, 64, 1, "Item Name (e.g. Burger)");
            GuiTextBox(Rectangle{300, 220, 400, 40}, itemPriceBuffer, 64, 2, "Price (e.g. 15.50)");

            // Save Button
            if (GuiButton(Rectangle{350, 300, 300, 50}, "Add Item to Menu"))
            {
                // 1. Create the new item
                MenuItem m;
                m.id = generateMenuItemID();
                m.restaurantID = selectedRestaurantID;
                m.name = itemNameBuffer;

                // Convert price string to float (simple check)

                m.price = stof(itemPriceBuffer);

                // 2. Add to global menu array
                menuItems[menuItemCount] = m; 
                menuItemCount++;

                // 3. Update the Restaurant's internal list (Required for data consistency)
                for (int i = 0; i < restaurantCount; i++)
                {
                    if (restaurants[i].id == selectedRestaurantID)
                    {
                        if (restaurants[i].menuItemCount < 50)
                        {
                            restaurants[i].menuItemIDs[restaurants[i].menuItemCount] = m.id;
                      
                        }
                        break;
                    }
                }

                // 4. Feedback / Reset
                memset(itemNameBuffer, 0, 64);
                memset(itemPriceBuffer, 0, 64);
                activeBoxID = 0;
            }

            // Show existing items briefly below
            DrawText("Existing Items:", 300, 380, 20, DARKGRAY);
            int displayY = 410;
            for (int i = 0; i < menuItemCount; i++)
            {
                if (menuItems[i].restaurantID == selectedRestaurantID)
                {


                    string listing = menuItems[i].name + " - $" + to_string((int)menuItems[i].price);
                    DrawText(listing.c_str(), 300, displayY, 20, GRAY);
                    displayY += 25;
                    if (displayY > 650)
                        break; // Stop drawing if we run off screen
                }
            }

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

            // Headers
            DrawText("Current Orders", 50, yPos, 20, DARKGRAY);
            yPos += 30;

            // 1. SHOW CURRENT ORDERS (Preparing, Ready,Delivered)
            for (int i = 0; i < orderCount; i++)
            {
                if (orders[i].customerID == currentUserID &&
                    (orders[i].status != "Delivered" && orders[i].status != "Rejected"))
                {

                    foundAny = true;

                    // Find Restaurant Name
                    string rName = "Unknown";
                    for (int r = 0; r < restaurantCount; r++)
                    {
                        if (restaurants[r].id == orders[i].restaurantID)
                            rName = restaurants[r].name;
                    }

                    // Draw Order Box
                    DrawRectangleLines(50, yPos, 500, 80, DARKGRAY);

                    string header = "Order #" + to_string(orders[i].id) + " - " + rName;
                    DrawText(header.c_str(), 60, yPos + 10, 20, BLACK);

                    string details = "Total: $" + to_string((int)orders[i].totalAmount) + " | Items: " + to_string(orders[i].itemCount);
                    DrawText(details.c_str(), 60, yPos + 35, 18, GRAY);

                    // Status Color
                    orders[i].statusColor = ORANGE; // Default for "Placed"
                    if (orders[i].status == "Preparing")
                        orders[i].statusColor = BLUE;
                    if (orders[i].status == "Ready")
                        orders[i].statusColor = PURPLE;
                    if (orders[i].status == "Delivered")
                        orders[i].statusColor = GREEN;

                    DrawText(("Status: " + orders[i].status).c_str(), 60, yPos + 55, 18, orders[i].statusColor);

                    yPos += 90;
                }
            }

            yPos += 20;
            DrawText("Past History", 50, yPos, 20, DARKGRAY);
            yPos += 30;

            // 2. SHOW PAST ORDERS (Delivered, Rejected)
            for (int i = 0; i < orderCount; i++)
            {
                if (orders[i].customerID == currentUserID &&
                    (orders[i].status == "Delivered" || orders[i].status == "Rejected"))
                {

                    foundAny = true;

                    // Find Restaurant Name
                    string rName = "Unknown";
                    for (int r = 0; r < restaurantCount; r++)
                    {
                        if (restaurants[r].id == orders[i].restaurantID)
                            rName = restaurants[r].name;
                    }

                    DrawRectangleLines(50, yPos, 500, 60, LIGHTGRAY);

                    string txt = "Order #" + to_string(orders[i].id) + " (" + rName + ") - " + orders[i].status;

                    orders[i].statusColor = (orders[i].status == "Delivered") ? GREEN : RED;
                    DrawText(txt.c_str(), 60, yPos + 20, 18, orders[i].statusColor);

                    yPos += 70;
                }
            }

            if (!foundAny)
            {
                DrawText("You have no order history.", 50, 150, 20, GRAY);
            }
        }
        break;
        }

        EndDrawing();
    }

    saveAllData();
    CloseWindow();

    return 0;
}
