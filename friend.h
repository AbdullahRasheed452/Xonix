// Group names : M.Abdullah , Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures

#ifndef FRIEND_H
#define FRIEND_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

const int MAX_PLAYERS = 100;
const int HASH_TABLE_SIZE = 101;

class FriendRequest;

struct FriendNode 
{
    string friendID;
    FriendNode* next;

    FriendNode(const string& id) : friendID(id), next(nullptr)
    {}
};

struct RequestNode
{
    string senderID;
    RequestNode* next;

    RequestNode(const string& id) : senderID(id), next(nullptr) 
    {}
};

class Players
{
public:
    string playerID;
    FriendNode* friendsHead;
    RequestNode* requestsHead;
    int numFriends;

    Players(const string& id) : playerID(id), friendsHead(nullptr), requestsHead(nullptr), numFriends(0)
    {}

    ~Players()
    {
        FriendNode* current = friendsHead;
        while (current)
        {
            FriendNode* temp = current;
            current = current->next;
            delete temp;
        }

        RequestNode* req = requestsHead;
        while (req)
        {
            RequestNode* temp = req;
            req = req->next;
            delete temp;
        }
    }

    void addFriend(const string& friendID)
    {
        FriendNode* newFriend = new FriendNode(friendID);
        newFriend->next = friendsHead;
        friendsHead = newFriend;
        numFriends++;
    }

    bool isFriend(const string& friendID) const 
    {
        FriendNode* current = friendsHead;
        while (current) 
        {
            if (current->friendID == friendID)
            {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool hasRequest(const string& senderID) const 
    {
        RequestNode* current = requestsHead;
        while (current)
        {
            if (current->senderID == senderID) 
            {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void addRequest(const string& senderID)
    {
        RequestNode* newRequest = new RequestNode(senderID);
        newRequest->next = requestsHead;
        requestsHead = newRequest;
    }

    void removeRequest(const string& senderID)
    {
        if (!requestsHead)
        {
            return;
        }

        if (requestsHead->senderID == senderID) 
        {
            RequestNode* temp = requestsHead;
            requestsHead = requestsHead->next;
            delete temp;
            return;
        }

        RequestNode* current = requestsHead;
        while (current->next)
        {
            if (current->next->senderID == senderID) 
            {
                RequestNode* temp = current->next;
                current->next = temp->next;
                delete temp;
                return;
            }
            current = current->next;
        }
    }

    void removeFriend(const string& friendID)
    {
        if (!friendsHead)
        {
            return;
        }

        if (friendsHead->friendID == friendID) 
        {
            FriendNode* temp = friendsHead;
            friendsHead = friendsHead->next;
            delete temp;
            numFriends--;
            return;
        }

        FriendNode* current = friendsHead;
        while (current->next) 
        {
            if (current->next->friendID == friendID) 
            {
                FriendNode* temp = current->next;
                current->next = temp->next;
                delete temp;
                numFriends--;
                return;
            }
            current = current->next;
        }
    }

    void viewFriends() const 
    {
        cout << playerID << "'s friends: ";
        if (!friendsHead)
        {
            cout << "None";
        }
        else {
            FriendNode* current = friendsHead;
            bool first = true;
            while (current)
            {
                if (!first)
                {
                    cout << ", ";
                }
                cout << current->friendID;
                first = false;
                current = current->next;
            }
        }
        cout << endl;
    }

    void viewRequests() const
    {
        cout << playerID << "'s pending requests: ";
        if (!requestsHead) 
        {
            cout << "None";
        }
        else 
        {
            RequestNode* current = requestsHead;
            bool first = true;
            while (current) 
            {
                if (!first) cout << ", ";
                cout << current->senderID;
                first = false;
                current = current->next;
            }
        }
        cout << endl;
    }

    string* getRequestsList(int& size) const
    {
        int count = 0;
        RequestNode* current = requestsHead;
        while (current)
        {
            count++;
            current = current->next;
        }

        string* requests = new string[count];
        current = requestsHead;
        int i = 0;
        while (current) 
        {
            requests[i++] = current->senderID;
            current = current->next;
        }
        size = count;
        return requests;
    }

    string* getFriendsList(int& size) const 
    {
        int count = 0;
        FriendNode* current = friendsHead;
        while (current)
        {
            count++;
            current = current->next;
        }

        string* friends = new string[count];
        current = friendsHead;
        int i = 0;
        while (current) {
            friends[i++] = current->friendID;
            current = current->next;
        }
        size = count;
        return friends;
    }
};

class FriendSystem
{
private:
    Players* players[MAX_PLAYERS];
    int numPlayers;
    int playerHashTable[HASH_TABLE_SIZE];
    const string FRIENDS_FILE = "friends_data.txt";
    const string REQUESTS_FILE = "friend_requests.txt";

    int hash(const string& playerID)
    {
        int hashValue = 0;
        for (char ch : playerID) {
            hashValue = (hashValue * 31 + ch) % HASH_TABLE_SIZE;
        }
        return hashValue;
    }

    bool playerExistsInFile(const string& playerID)
    {
        ifstream infile("players.txt");
        string line;

        while (getline(infile, line))
        {
            istringstream iss(line);
            string username;

            if (getline(iss, username, ',')) 
            {
                if (username == playerID)
                {
                    return true;
                }
            }
        }
        return false;
    }

    void loadPlayersFromFile()
    {
        ifstream infile("players.txt");
        if (!infile) {
            cout << "Error opening players.txt" << endl;
            return;
        }

        string line;
        while (getline(infile, line)) 
        {
            if (line.empty())
            {
                continue;
            }

            istringstream iss(line);
            string username;

            if (getline(iss, username, ','))
            {
                if (!getPlayerByID(username)) 
                {
                    addPlayer(username);
                }
            }
        }

        loadFriendsData();
        loadRequestsData();
    }

    void loadFriendsData() 
    {
        ifstream friendsFile(FRIENDS_FILE);
        if (!friendsFile) 
        {
            return;
        }

        string line;
        while (getline(friendsFile, line))
        {
            if (line.empty())
            {
                continue;
            }

            istringstream iss(line);
            string playerID, friendID;

            if (getline(iss, playerID, ':')) 
            {
                Players* player = getPlayerByID(playerID);
                if (!player) 
                {
                    addPlayer(playerID);
                    player = getPlayerByID(playerID);
                }

                string friendsList;
                if (getline(iss, friendsList))
                {
                    istringstream friendsStream(friendsList);
                    string friendID;

                    while (getline(friendsStream, friendID, ','))
                    {
                        if (!friendID.empty() && !player->isFriend(friendID)) 
                        {
                            player->addFriend(friendID);
                        }
                    }
                }
            }
        }
    }

    void loadRequestsData() 
    {
        ifstream requestsFile(REQUESTS_FILE);
        if (!requestsFile) 
        {
            return;
        }

        string line;
        while (getline(requestsFile, line))
        {
            if (line.empty()) continue;

            istringstream iss(line);
            string receiverID, sendersList;

            if (getline(iss, receiverID, ':'))
            {
                Players* receiver = getPlayerByID(receiverID);
                if (!receiver) 
                {
                    addPlayer(receiverID);
                    receiver = getPlayerByID(receiverID);
                }

                if (getline(iss, sendersList))
                {
                    istringstream sendersStream(sendersList);
                    string senderID;

                    while (getline(sendersStream, senderID, ',')) 
                    {
                        if (!senderID.empty() && !receiver->hasRequest(senderID))
                        {
                            receiver->addRequest(senderID);
                        }
                    }
                }
            }
        }
    }

    void saveFriendsData() 
    {
        ofstream friendsFile(FRIENDS_FILE);
        if (!friendsFile) {
            cout << "Error: Could not open friends data file for writing." << endl;
            return;
        }

        for (int i = 0; i < numPlayers; ++i)
        {
            Players* player = players[i];
            int size;
            string* friendsList = player->getFriendsList(size);

            if (size > 0) 
            {
                friendsFile << player->playerID << ":";
                for (int j = 0; j < size; ++j) {
                    if (j > 0) friendsFile << ",";
                    friendsFile << friendsList[j];
                }
                friendsFile << endl;
            }
            delete[] friendsList;
        }
    }

    void saveRequestsData() 
    {
        ofstream requestsFile(REQUESTS_FILE);
        if (!requestsFile) {
            cout << "Could not open friend requests file for writing." << endl;
            return;
        }

        for (int i = 0; i < numPlayers; ++i) 
        {
            Players* player = players[i];
            int size;
            string* requestsList = player->getRequestsList(size);

            if (size > 0)
            {
                requestsFile << player->playerID << ":";
                for (int j = 0; j < size; ++j)
                {
                    if (j > 0) requestsFile << ",";
                    requestsFile << requestsList[j];
                }
                requestsFile << endl;
            }
            delete[] requestsList;
        }
    }

public:
    FriendSystem() : numPlayers(0)
    {
        for (int i = 0; i < HASH_TABLE_SIZE; ++i) 
        {
            playerHashTable[i] = -1;
        }
        loadPlayersFromFile();
    }

    ~FriendSystem() {
        saveFriendsData();
        saveRequestsData();

        for (int i = 0; i < numPlayers; ++i)
        {
            delete players[i];
        }
    }

    void addPlayer(const string& playerID) 
    {
        if (numPlayers >= MAX_PLAYERS)
        {
            cout << "Cannot add more players. Limit reached." << endl;
            return;
        }

        if (getPlayerByID(playerID)) 
        {
            return;
        }

        players[numPlayers] = new Players(playerID);
        int hashValue = hash(playerID);
        while (playerHashTable[hashValue] != -1) 
        {
            hashValue = (hashValue + 1) % HASH_TABLE_SIZE;
        }
        playerHashTable[hashValue] = numPlayers;
        numPlayers++;
    }

    Players* getPlayerByID(const string& playerID)
    {
        int hashValue = hash(playerID);
        int originalHash = hashValue;

        while (playerHashTable[hashValue] != -1)
        {
            if (players[playerHashTable[hashValue]]->playerID == playerID)
            {
                return players[playerHashTable[hashValue]];
            }
            hashValue = (hashValue + 1) % HASH_TABLE_SIZE;
            if (hashValue == originalHash)
            {
                break;
            }
        }
        return nullptr;
    }

    bool sendFriendRequest(const string& senderID, const string& receiverID)
    {
        Players* sender = getPlayerByID(senderID);
        if (!sender) 
        {
            cout << "Sender '" << senderID << "' does not exist!" << endl;
            return false;
        }

        Players* receiver = getPlayerByID(receiverID);
        if (!receiver) 
        {
            if (playerExistsInFile(receiverID)) 
            {
                addPlayer(receiverID);
                receiver = getPlayerByID(receiverID);
            }
            else 
            {
                cout << "Receiver '" << receiverID << "' not found!" << endl;
                return false;
            }
        }

        if (sender->isFriend(receiverID))
        {
            cout << senderID << " and " << receiverID << " are already friends." << endl;
            return false;
        }

        if (receiver->hasRequest(senderID))
        {
            cout << "Friend request already pending." << endl;
            return false;
        }

        receiver->addRequest(senderID);
        cout << "Friend request sent from " << senderID << " to " << receiverID << endl;

        saveRequestsData();

        return true;
    }

    bool acceptFriendRequest(const string& receiverID, const string& senderID)
    {
        Players* receiver = getPlayerByID(receiverID);
        Players* sender = getPlayerByID(senderID);

        if (!receiver || !sender)
        {
            cout << "One or both players not found!" << endl;
            return false;
        }

        if (!receiver->hasRequest(senderID)) 
        {
            cout << "No pending request from " << senderID << endl;
            return false;
        }

        receiver->removeRequest(senderID);
        receiver->addFriend(senderID);
        sender->addFriend(receiverID);

        cout << receiverID << " accepted " << senderID << "'s friend request!" << endl;

        saveFriendsData();
        saveRequestsData();

        return true;
    }

    bool rejectFriendRequest(const string& receiverID, const string& senderID)
    {
        Players* receiver = getPlayerByID(receiverID);

        if (!receiver) 
        {
            cout << "Receiver not found!" << endl;
            return false;
        }

        if (!receiver->hasRequest(senderID)) 
        {
            cout << "No pending request from " << senderID << endl;
            return false;
        }

        receiver->removeRequest(senderID);
        cout << receiverID << " rejected " << senderID << "'s friend request." << endl;

        saveRequestsData();

        return true;
    }

    void viewFriends(const string& playerID) 
    {
        Players* player = getPlayerByID(playerID);
        if (player) 
        {
            player->viewFriends();
        }
        else
        {
            cout << "Player '" << playerID << "' not found!" << endl;
        }
    }

    void viewRequests(const string& playerID) 
    {
        Players* player = getPlayerByID(playerID);
        if (player) 
        {
            player->viewRequests();
        }
        else 
        {
            cout << "Player '" << playerID << "' not found!" << endl;
        }
    }

    bool removeFriend(const string& playerID, const string& friendID)
    {
        Players* player = getPlayerByID(playerID);
        Players* friendPlayer = getPlayerByID(friendID);

        if (!player || !friendPlayer)
        {
            cout << "One or both players not found!" << endl;
            return false;
        }

        player->removeFriend(friendID);
        friendPlayer->removeFriend(playerID);
        cout << playerID << " and " << friendID << " are no longer friends." << endl;

        saveFriendsData();

        return true;
    }

    string* getPendingRequests(const string& playerID, int& size)
    {
        Players* player = getPlayerByID(playerID);
        if (player) {
            return player->getRequestsList(size);
        }
        size = 0;
        return new string[0];
    }

    string* getFriendsList(const string& playerID, int& size) 
    {
        Players* player = getPlayerByID(playerID);
        if (player)
        {
            return player->getFriendsList(size);
        }
        size = 0;
        return new string[0];
    }

    void display_friend(sf::RenderWindow& window, sf::Font& font, Player& loggedInPlayer, bool isLoggedIn)
    {
        cout << "Friends button clicked\n";

        string originalTitle = "main menu";
        window.setTitle("Friends System");

        font.loadFromFile("images1/Emulogic-zrEw.ttf");

        sf::Text inputLabel("Player ID:", font, 20);
        inputLabel.setPosition(10, 10);
        sf::Text inputText("", font, 20);
        inputText.setPosition(230, 10);

        sf::Text friendLabel("Friend ID:", font, 20);
        friendLabel.setPosition(10, 50);
        sf::Text friendText("", font, 20);
        friendText.setPosition(230, 50);

        string currentPlayerID = "";
        string currentFriendID = "";
        bool enteringPlayerID = true;

        sf::RectangleShape sendButton(sf::Vector2f(220, 40));
        sendButton.setPosition(10, 100);
        sendButton.setFillColor(sf::Color::Green);
        sf::Text sendText("Send Request", font, 18);
        sendText.setPosition(10, 110);

        sf::RectangleShape viewFriendsButton(sf::Vector2f(220, 40));
        viewFriendsButton.setPosition(240, 100);
        viewFriendsButton.setFillColor(sf::Color::Blue);
        sf::Text viewFriendsText("View Friends", font, 18);
        viewFriendsText.setPosition(240, 110);

        sf::RectangleShape viewRequestsButton(sf::Vector2f(220, 40));
        viewRequestsButton.setPosition(470, 100);
        viewRequestsButton.setFillColor(sf::Color::Red);
        sf::Text viewRequestsText("View Request", font, 18);
        viewRequestsText.setPosition(470, 110);

        sf::RectangleShape acceptButton(sf::Vector2f(140, 40));
        acceptButton.setPosition(10, 150);
        acceptButton.setFillColor(sf::Color(50, 200, 50));
        sf::Text acceptText("Accept", font, 18);
        acceptText.setPosition(20, 160);

        sf::RectangleShape rejectButton(sf::Vector2f(140, 40));
        rejectButton.setPosition(160, 150);
        rejectButton.setFillColor(sf::Color(200, 50, 50));
        sf::Text rejectText("Reject", font, 18);
        rejectText.setPosition(170, 160);

        sf::RectangleShape removeButton(sf::Vector2f(250, 40));
        removeButton.setPosition(310, 150);
        removeButton.setFillColor(sf::Color::Red);
        sf::Text removeText("Remove Friend", font, 18);
        removeText.setPosition(320, 160);

        sf::Text statusText("", font, 16);
        statusText.setPosition(10, 210);
        statusText.setFillColor(sf::Color::White);

        sf::RectangleShape resultArea(sf::Vector2f(700, 230));
        resultArea.setPosition(10, 250);
        resultArea.setFillColor(sf::Color(30, 30, 30));
        resultArea.setOutlineColor(sf::Color::White);
        resultArea.setOutlineThickness(2);

        sf::Text resultText("", font, 16);
        resultText.setPosition(20, 260);
        resultText.setFillColor(sf::Color::White);

        sf::RectangleShape backButton(sf::Vector2f(100, 30));
        backButton.setPosition(610, 10);
        backButton.setFillColor(sf::Color(100, 100, 100));
        sf::Text backText("Back", font, 16);
        backText.setPosition(625, 15);

        FriendSystem fs;

        if (isLoggedIn)
        {
            currentPlayerID = loggedInPlayer.username;
            inputText.setString(currentPlayerID);
        }

        bool inFriendsSection = true;

        while (inFriendsSection && window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event)) 
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
                    

                else if (event.type == sf::Event::KeyPressed) 
                {
                    if (event.key.code == sf::Keyboard::Escape) 
                    {
                        inFriendsSection = false;
                    }
                }
                else if (event.type == sf::Event::TextEntered)
                {
                    char entered = static_cast<char>(event.text.unicode);
                    if (entered == '\b') 
                    {
                        if (enteringPlayerID && !currentPlayerID.empty())
                            currentPlayerID.pop_back();
                        else if (!enteringPlayerID && !currentFriendID.empty())
                            currentFriendID.pop_back();
                    }
                    else if (isprint(entered)) 
                    {
                        if (enteringPlayerID)
                            currentPlayerID += entered;
                        else
                            currentFriendID += entered;
                    }
                    inputText.setString(currentPlayerID);
                    friendText.setString(currentFriendID);
                }

                else if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left) 
                    {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                        if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) 
                        {
                            inFriendsSection = false;
                        }

                        if (inputLabel.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ||
                            inputText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) 
                        {
                            enteringPlayerID = true;
                        }
                        else if (friendLabel.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)) ||
                            friendText.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                        {
                            enteringPlayerID = false;
                        }

                        if (sendButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) 
                        {
                            if (currentPlayerID.empty() || currentFriendID.empty())
                            {
                                statusText.setString("Please provide both Player ID and Friend ID.");
                            }
                            else 
                            {
                                fs.addPlayer(currentPlayerID);
                                if (fs.sendFriendRequest(currentPlayerID, currentFriendID)) 
                                {
                                    statusText.setString("Friend request sent!");
                                }
                                else
                                {
                                    statusText.setString("Friend request failed.");
                                }
                            }
                        }

                        if (viewFriendsButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) 
                        {
                            if (currentPlayerID.empty()) 
                            {
                                statusText.setString("Please provide a Player ID.");
                            }
                            else 
                            {
                                fs.addPlayer(currentPlayerID);
                                int size;
                                string* friendsList = fs.getFriendsList(currentPlayerID, size);

                                string result = currentPlayerID + "'s friends:\n";
                                if (size == 0)
                                {
                                    result += "None";
                                }
                                else 
                                {
                                    for (int i = 0; i < size; ++i) 
                                    {
                                        result += "- " + friendsList[i] + "\n";
                                    }
                                }
                                delete[] friendsList;
                                resultText.setString(result);
                                statusText.setString("Friends list displayed.");
                            }
                        }

                        if (viewRequestsButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                            if (currentPlayerID.empty()) 
                            {
                                statusText.setString("Please provide a Player ID.");
                            }
                            else 
                            {
                                fs.addPlayer(currentPlayerID);
                                int size;
                                string* requestsList = fs.getPendingRequests(currentPlayerID, size);

                                string result = currentPlayerID + "'s pending requests:\n";
                                if (size == 0)
                                {
                                    result += "None";
                                }
                                else {
                                    for (int i = 0; i < size; ++i) 
                                    {
                                        result += "- " + requestsList[i] + "\n";
                                    }
                                }
                                delete[] requestsList;
                                resultText.setString(result);
                                statusText.setString("Pending requests displayed.");
                            }
                        }

                        if (acceptButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                            if (currentPlayerID.empty() || currentFriendID.empty()) 
                            {
                                statusText.setString("Please provide both Player ID and Friend ID.");
                            }
                            else 
                            {
                                if (fs.acceptFriendRequest(currentPlayerID, currentFriendID)) 
                                {
                                    statusText.setString("Friend request accepted!");
                                }
                                else
                                {
                                    statusText.setString("Failed to accept friend request.");
                                }
                            }
                        }

                        if (rejectButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                            if (currentPlayerID.empty() || currentFriendID.empty())
                            {
                                statusText.setString("Please provide both Player ID and Friend ID.");
                            }
                            else
                            {
                                if (fs.rejectFriendRequest(currentPlayerID, currentFriendID)) 
                                {
                                    statusText.setString("Friend request rejected.");
                                }
                                else 
                                {
                                    statusText.setString("Failed to reject friend request.");
                                }
                            }
                        }

                        if (removeButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                            if (currentPlayerID.empty() || currentFriendID.empty()) 
                            {
                                statusText.setString("Please provide both Player ID and Friend ID.");
                            }
                            else
                            {
                                if (fs.removeFriend(currentPlayerID, currentFriendID))
                                {
                                    statusText.setString("Friend removed.");
                                }
                                else 
                                {
                                    statusText.setString("Friend removal failed.");
                                }
                            }
                        }
                    }
                }
            }

            window.clear();

            window.draw(inputLabel);
            window.draw(friendLabel);
            window.draw(inputText);
            window.draw(friendText);

            window.draw(sendButton);
            window.draw(sendText);
            window.draw(viewFriendsButton);
            window.draw(viewFriendsText);
            window.draw(viewRequestsButton);
            window.draw(viewRequestsText);
            window.draw(acceptButton);
            window.draw(acceptText);
            window.draw(rejectButton);
            window.draw(rejectText);
            window.draw(removeButton);
            window.draw(removeText);
            window.draw(backButton);
            window.draw(backText);

            window.draw(statusText);
            window.draw(resultArea);
            window.draw(resultText);

            window.display();
        }

        window.setTitle(originalTitle);
    }
};

#endif