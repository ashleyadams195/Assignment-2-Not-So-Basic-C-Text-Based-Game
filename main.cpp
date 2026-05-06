#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

// ==================== COLORS ====================
const string RESET = "\033[0m";
const string BOLD  = "\033[1m";
const string DIM   = "\033[2m";
const string RED   = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string CYAN  = "\033[36m";
const string MAGENTA = "\033[35m";

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    cout << "\033[2J\033[1;1H";
#endif
}

void enableVirtualTerminal() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

// ==================== GAME STRUCTURES ====================
struct Room {
    string name;
    map<string, int> exits;
    vector<string> items;
};

struct Player {
    int currentRoom = 0;
    vector<string> inventory;
    string lastThought = "";
};

bool hasItem(const Player& player, const string& item) {
    return find(player.inventory.begin(), player.inventory.end(), item) != player.inventory.end();
}

// ==================== CHARACTER THOUGHT ====================
string getCharacterThought(const Room& room, const Player& player) {
    if (room.name.find("Outside") != string::npos) return "Alright... time to sneak in. Stay quiet.";
    if (room.name.find("Foyer") != string::npos) return "Such fancy.";
    if (room.name.find("Long Hallway") != string::npos) return "Daaang, this hallway is long...";
    if (room.name.find("Living Room") != string::npos) {
        return hasItem(player, "key") ? "The key worked!" : "Drat... this door is locked.";
    }
    // Office - Special logic
    if (room.name.find("Office") != string::npos) {
        if (hasItem(player, "flashlight")) {
            return "Look, a birthday card!";
        } else {
            return "It's so dark in here...";
        }
    }
    
    if (room.name.find("Servers") != string::npos) {
        return hasItem(player, "flashlight") ? "There's the computer! I only have one shot at this. I need a four digit code to login..." : "It's still so dark in here... I can't see anything.";
    }
    return "...";
}

// ==================== MAP ====================
void drawMap(int currentRoom) {
    cout << "\n" << CYAN << BOLD << "                          MAP" << RESET << "\n\n";
    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                     |   OUTSIDE       |" << RESET 
         << (currentRoom == 0 ? "   " + RED + "X" + RESET : "") << "\n";
    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                              |" << RESET << "\n";

    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                     |   FOYER         |" << RESET 
         << (currentRoom == 1 ? "   " + RED + "X" + RESET : "") << "\n";
    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                              |" << RESET << "\n";

    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                     | LONG HALLWAY    |" << RESET 
         << (currentRoom == 2 ? "   " + RED + "X" + RESET : "") << "\n";
    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                              |" << RESET << "\n";

    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                     |   LIVING ROOM   |" << RESET 
         << (currentRoom == 3 ? "   " + RED + "X" + RESET : "") << "\n";
    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                              |" << RESET << "\n";

    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                     |   OFFICE        |" << RESET 
         << (currentRoom == 4 ? "   " + RED + "X" + RESET : "") << "\n";
    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                              |" << RESET << "\n";

    cout << CYAN << "                     +-----------------+" << RESET << "\n";
    cout << CYAN << "                     |   SERVERS       |" << RESET 
         << (currentRoom == 5 ? "   " + RED + "X" + RESET : "") << "\n";
    cout << CYAN << "                     +-----------------+" << RESET << "\n\n";
}

// ==================== DISPLAY ====================
void displayRoom(const Room& room, const Player& player) {
    clearScreen();

    cout << RED << BOLD << "Your Location: " << RESET
         <<  room.name << string(40 - room.name.length(), ' ')
         << "\n\n";

    cout << MAGENTA << BOLD << "Thought: " << RESET 
         << (player.lastThought.empty() ? getCharacterThought(room, player) : player.lastThought) 
         << "\n\n";
    

    // can't see nothing when no flashlight
    cout << GREEN << BOLD << "You see: " << RESET;
    
    if (room.name.find("Office") != string::npos) {
        if (hasItem(player, "flashlight")) {
            cout << "birthday card";
        } else {
            cout << "(nothing)";
        }
    }
    else if (room.name.find("Servers") != string::npos) {
        if (hasItem(player, "flashlight")) {
            cout << "computer";
        } else {
            cout << "(nothing)";
        }
    } 
    else if (room.items.empty()) {
        cout << "(nothing)";
    } else {
        for (const string& item : room.items) cout << item << "  ";
    }
    cout << "\n\n";

    // inventory 
    cout << GREEN << BOLD << "Inventory: " << RESET;
    if (player.inventory.empty()) cout << "(empty)";
    else for (const string& item : player.inventory) cout << item << "  ";
    cout << "\n\n";

    drawMap(player.currentRoom);

    cout << DIM << "W=Up  S=Down  D=Pickup All  A=Interact  Q=Quit\n" << RESET;
    cout << MAGENTA << "> " << RESET;
}

// ==================== MAIN ====================
int main() {
    enableVirtualTerminal();

    cout << GREEN << BOLD << "\n=== Mansion Infiltration ===\nPress ENTER to advance line by line...\nPress SPACE to skip the intro and start the game." << RESET << endl;

    string introLines[] = {
        "Hey, Rookie!",
        "You ready for your mission?",
        "Break into the mansion and steal the critical data from the servers.",
        "Be smart. Be quiet. Don't get caught.",
        "Good luck, Rookie."
    };

    for (const string& line : introLines) {
        cout << line << endl;
        char ch = getchar();
        if (ch == ' ' || ch == '\r') {
            cout << "\nSkipping intro...\n";
            break;
        }
    }

    cout << GREEN << "\nYou are standing outside a beautiful mansion...\n" << RESET;
    cout << "Press ENTER to begin...\n";
    cin.get();

    vector<Room> rooms(6);

    rooms[0].name = "Outside the Mansion";
    rooms[0].exits = {{"down", 1}};

    rooms[1].name = "Foyer";
    rooms[1].exits = {{"up", 0}, {"down", 2}};
    rooms[1].items = {"flashlight"};

    rooms[2].name = "Long Hallway";
    rooms[2].exits = {{"up", 1}, {"down", 3}};
    rooms[2].items = {"key"};

    rooms[3].name = "Living Room";
    rooms[3].exits = {{"up", 2}, {"down", 4}};

    rooms[4].name = "Office";
    rooms[4].exits = {{"up", 3}, {"down", 5}};

    rooms[5].name = "Servers";
    rooms[5].exits = {{"up", 4}};

    Player player{0};

    while (true) {
        Room& current = rooms[player.currentRoom];
        displayRoom(current, player);

        string input;
        getline(cin, input);
        if (input.empty()) continue;

        for (char& c : input) c = tolower(c);

        bool actionTaken = false;
        string newThought = "";

        if (input == "w" || input == "up") {
            if (current.exits.count("up") && current.exits["up"] != -1) {
                player.currentRoom = current.exits["up"];
                actionTaken = true;
                player.lastThought = "";
            }
        }
        else if (input == "s" || input == "down") {
            if (current.exits.count("down") && current.exits["down"] != -1) {
                bool canEnter = true;

                if (player.currentRoom == 2 && !hasItem(player, "key")) {
                    newThought = "Drat... this door is locked.";
                    canEnter = false;
                }

                if (canEnter) {
                    player.currentRoom = current.exits["down"];
                    actionTaken = true;
                    player.lastThought = "";
                } else {
                    player.lastThought = newThought;
                }
            }
        }

        else if (input == "d") {
            if (!current.items.empty()) {
                for (const string& item : current.items) {
                    player.inventory.push_back(item);
                    cout << GREEN << "\nYou picked up the " << item << ".\n" << RESET;
                }
                current.items.clear();
                actionTaken = true;
            }
        }

        else if (input == "a") {
            if (player.currentRoom == 4) {                    // Office
                cout << GREEN << "\n" << RESET;
                player.lastThought = "\n";
            }
            else if (player.currentRoom == 5) {               // Server Room
                if (!hasItem(player, "flashlight")) {
                    cout << RED << "\nIt's still so dark in here... I can't see anything.\n" << RESET;
                } else {
                    cout << YELLOW << "\nEnter 4-digit passkey: " << RESET;
                    string code;
                    getline(cin, code);
                    if (code == "1986") {
                        cout << GREEN << BOLD << "\n*** ACCESS GRANTED! Mission Successful! ***\n" << RESET;
                        break;
                    } else {
                        cout << RED << BOLD << "\nINCORRECT CODE! Security system triggered!\n" << RESET;
                        cout << RED << "You were caught!\n" << RESET;
                        break;
                    }
                }
            }
            else {
                cout << YELLOW << "\nThere's nothing to interact with here.\n" << RESET;
            }
            actionTaken = true;
        }

        else if (input == "q" || input == "quit") {
            cout << RED << "\nThanks for playing!\n" << RESET;
            break;
        }

        if (!actionTaken) {
            cout << YELLOW << "\nUnknown command.\n" << RESET;
        }
    }

    return 0;
}
