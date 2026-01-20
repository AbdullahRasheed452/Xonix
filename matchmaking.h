// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef MATCHMAKING_H
#define MATCHMAKING_H

#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

struct QueuePlayer 
{
    string username;
    string nickname;
    int score;

    QueuePlayer(const string& uname, const string& nname, int s)
        : username(uname), nickname(nname), score(s)
    {
    }

    QueuePlayer() : username(""), nickname(""), score(0) {}


    QueuePlayer(const QueuePlayer& other)
        : username(other.username), nickname(other.nickname), score(other.score) 
    {
    }


    QueuePlayer& operator=(const QueuePlayer& other)
    {
        if (this != &other) 
        {
            username = other.username;
            nickname = other.nickname;
            score = other.score;
        }
        return *this;
    }

    bool operator<(const QueuePlayer& other) const
    {
        return score > other.score;
    }
};

struct Match {
    QueuePlayer player1;
    QueuePlayer player2;

    Match(const QueuePlayer& p1, const QueuePlayer& p2) : player1(p1), player2(p2) {}
    Match() : player1(), player2() {}


    Match(const Match& other) : player1(other.player1), player2(other.player2) {}


    Match& operator=(const Match& other)
    {
        if (this != &other) {
            player1 = other.player1;
            player2 = other.player2;
        }
        return *this;
    }
};

class PlayerPriorityQueue 
{
private:
    struct Node {
        QueuePlayer data;
        Node* next;
        Node(const QueuePlayer& value) : data(value), next(nullptr) {}
    };

    Node* head;
    int size_;

public:
    PlayerPriorityQueue() : head(nullptr), size_(0) {}


    PlayerPriorityQueue(const PlayerPriorityQueue& other) : head(nullptr), size_(0) 
    {
        *this = other;
    }

    ~PlayerPriorityQueue()
    {
        while (!empty()) 
        {
            pop();
        }
    }

    void push(const QueuePlayer& value) 
    {
        Node* newNode = new Node(value);

        if (head == nullptr || !(head->data < value)) 
        {
            newNode->next = head;
            head = newNode;
        }
        else {
            Node* current = head;
            while (current->next != nullptr && current->next->data < value)
            {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
        size_++;
    }

    void pop() {
        if (empty()) return;

        Node* temp = head;
        head = head->next;
        delete temp;
        size_--;
    }

    QueuePlayer top() const 
    {
        if (empty()) 
        {
            return QueuePlayer("", "", 0);
        }
        return head->data;
    }

    bool empty() const 
    {
        return head == nullptr;
    }

    int size() const 
    {
        return size_;
    }

    PlayerPriorityQueue& operator=(const PlayerPriorityQueue& other) 
    {
        if (this == &other)
        {
            return *this;
        }


        while (!empty()) 
        {
            pop();
        }


        Node* current = other.head;
        if (current == nullptr) 
        {
            return *this;
        }


        head = new Node(current->data);
        Node* thisCurrent = head;
        current = current->next;


        while (current != nullptr) 
        {
            thisCurrent->next = new Node(current->data);
            thisCurrent = thisCurrent->next;
            current = current->next;
        }

        size_ = other.size_;
        return *this;
    }
};

class MatchVector 
{
private:
    Match* data;
    int capacity_;
    int size_;


    void copyData(Match* dest, const Match* src, int count) 
    {
        for (int i = 0; i < count; ++i) 
        {
            dest[i] = src[i];
        }
    }

public:
    MatchVector() : data(nullptr), capacity_(0), size_(0) {}


    MatchVector(const MatchVector& other) : data(nullptr), capacity_(0), size_(0) 
    {
        if (other.size_ > 0) 
        {
            data = new Match[other.capacity_];
            capacity_ = other.capacity_;
            size_ = other.size_;

            copyData(data, other.data, size_);
        }
    }

    ~MatchVector() 
    {
        delete[] data;
    }

    void push_back(const Match& value)
    {
        if (size_ >= capacity_) 
        {
            int newCapacity = capacity_ == 0 ? 1 : capacity_ * 2;
            Match* newData = new Match[newCapacity];


            if (data != nullptr && size_ > 0) 
            {
                copyData(newData, data, size_);
            }


            if (data != nullptr) 
            {
                delete[] data;
            }

            data = newData;
            capacity_ = newCapacity;
        }


        if (size_ < capacity_) 
        {
            data[size_] = value;
            size_++;
        }
    }

    void clear() 
    {
        size_ = 0;
    }

    Match& operator[](int index)
    {
        if (index >= 0 && index < size_) 
        {
            return data[index];
        }
        static Match dummy;
        return dummy;
    }

    const Match& operator[](int index) const 
    {
        if (index >= 0 && index < size_) 
        {
            return data[index];
        }
        static Match dummy;
        return dummy;
    }

    int size() const
    {
        return size_;
    }

    bool empty() const 
    {
        return size_ == 0;
    }


    MatchVector& operator=(const MatchVector& other)
    {
        if (this == &other) return *this;


        delete[] data;
        data = nullptr;


        if (other.size_ > 0)
        {
            data = new Match[other.capacity_];
            capacity_ = other.capacity_;
            size_ = other.size_;

            copyData(data, other.data, size_);
        }
        else 
        {
            capacity_ = 0;
            size_ = 0;
        }

        return *this;
    }
};

class MatchMakingSystem
{
private:
    PlayerPriorityQueue playerQueue;
    MatchVector currentMatches;

    void loadPlayersFromFile(const string& filename) 
    {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Unable to open file " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) 
        {
            istringstream iss(line);
            string username, password, nickname, something, date, gamesPlayedStr, scoreStr;

            if (getline(iss, username, ',') &&
                getline(iss, password, ',') &&
                getline(iss, nickname, ',') &&
                getline(iss, something, ',') &&
                getline(iss, date, ',') &&
                getline(iss, gamesPlayedStr, ',') &&
                getline(iss, scoreStr)) {

                try {
                    int score = stoi(scoreStr);
                    playerQueue.push(QueuePlayer(username, nickname, score));
                }
                catch (const exception& e) {
                    cout << "Error parsing score for player " << username << ": " << e.what() << endl;
                }
            }
        }

        file.close();
    }

public:
    MatchMakingSystem()
    {

    }

    void initialize(const string& playersFile)
    {
        loadPlayersFromFile(playersFile);
    }

    void createMatches() 
    {
        currentMatches.clear();

        PlayerPriorityQueue tempQueue;

        while (playerQueue.size() >= 2) 
        {
            QueuePlayer player1 = playerQueue.top();
            playerQueue.pop();

            QueuePlayer player2 = playerQueue.top();
            playerQueue.pop();

            currentMatches.push_back(Match(player1, player2));
        }


        if (!playerQueue.empty())
        {
            tempQueue.push(playerQueue.top());
            playerQueue.pop();
        }

        playerQueue = tempQueue;
    }

    void displayMatchmaking(sf::RenderWindow& window, sf::Font& font)
    {

        (void)window;

        if (playerQueue.empty() && currentMatches.empty()) 
        {
            initialize("players.txt");
            createMatches();
        }

        sf::RenderWindow matchWindow(sf::VideoMode(800, 600), "Matchmaking Queue");

        sf::Texture backgroundTexture;
        if (!backgroundTexture.loadFromFile("images1/matchmaking.jpg"))
        {
            if (!backgroundTexture.loadFromFile("images1/bg.jpg"))
            {
                cout << "Failed to load background image." << endl;
            }
        }

        sf::Sprite backgroundSprite(backgroundTexture);

        sf::Vector2u textureSize = backgroundTexture.getSize();
        sf::Vector2u windowSize = matchWindow.getSize();
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;
        backgroundSprite.setScale(scaleX, scaleY);

        sf::Text titleText("MATCHMAKING", font, 40);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(180, 20);

        sf::RectangleShape backButton(sf::Vector2f(100, 30));
        backButton.setPosition(650, 550);
        backButton.setFillColor(sf::Color::White);

        sf::Text backText("Back", font, 15);
        backText.setFillColor(sf::Color::Black);
        backText.setPosition(675, 555);

        sf::RectangleShape refreshButton(sf::Vector2f(150, 30));
        refreshButton.setPosition(50, 550);
        refreshButton.setFillColor(sf::Color::White);

        sf::Text refreshText("Refresh", font, 15);
        refreshText.setFillColor(sf::Color::Black);
        refreshText.setPosition(75, 555);

        while (matchWindow.isOpen()) {
            sf::Event event;
            while (matchWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed ||
                    (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
                {
                    matchWindow.close();
                }

                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) 
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(matchWindow);

                    if (backButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) 
                    {
                        matchWindow.close();
                    }

                    if (refreshButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) 
                    {
                        while (!playerQueue.empty()) playerQueue.pop();
                        currentMatches.clear();
                        initialize("players.txt");
                        createMatches();
                    }
                }
            }

            matchWindow.clear();
            matchWindow.draw(backgroundSprite);
            matchWindow.draw(titleText);
            matchWindow.draw(backButton);
            matchWindow.draw(backText);
            matchWindow.draw(refreshButton);
            matchWindow.draw(refreshText);

            sf::Text matchesHeader("GAME ROOM - MATCHES", font, 30);
            matchesHeader.setFillColor(sf::Color::Yellow);
            matchesHeader.setPosition(140, 100);
            matchWindow.draw(matchesHeader);

            int yPos = 150;
            for (int i = 0; i < currentMatches.size(); ++i) 
            {
                sf::RectangleShape matchRect(sf::Vector2f(600, 60));
                matchRect.setPosition(100, yPos);
                matchRect.setFillColor(sf::Color(50, 50, 50, 200));
                matchRect.setOutlineThickness(2);
                matchRect.setOutlineColor(sf::Color(200, 200, 0));
                matchWindow.draw(matchRect);

                sf::Text matchNumText("MATCH " + to_string(i + 1), font, 18);
                matchNumText.setFillColor(sf::Color::Yellow);
                matchNumText.setPosition(110, yPos + 5);
                matchWindow.draw(matchNumText);

                sf::Text player1Text(currentMatches[i].player1.username, font, 20);
                player1Text.setFillColor(sf::Color::Cyan);
                player1Text.setPosition(150, yPos + 30);
                matchWindow.draw(player1Text);

                sf::Text score1Text(to_string(currentMatches[i].player1.score), font, 16);
                score1Text.setFillColor(sf::Color::White);
                score1Text.setPosition(150, yPos + 30);
                float player1Width = player1Text.getGlobalBounds().width;
                score1Text.setPosition(150 + player1Width + 10, yPos + 32);
                matchWindow.draw(score1Text);

                sf::Text vsText("VS", font, 22);
                vsText.setFillColor(sf::Color::Red);
                vsText.setPosition(410 - vsText.getGlobalBounds().width / 2, yPos + 25);
                matchWindow.draw(vsText);

                sf::Text player2Text(currentMatches[i].player2.username, font, 20);
                player2Text.setFillColor(sf::Color::Green);
                player2Text.setPosition(450, yPos + 30);
                matchWindow.draw(player2Text);

                sf::Text score2Text(to_string(currentMatches[i].player2.score), font, 16);
                score2Text.setFillColor(sf::Color::White);
                float player2Width = player2Text.getGlobalBounds().width;
                score2Text.setPosition(430 + player2Width + 30, yPos + 32);
                matchWindow.draw(score2Text);

                yPos += 70;
            }

            if (!playerQueue.empty()) 
            {
                sf::Text waitingHeader("PLAYERS WAITING", font, 30);
                waitingHeader.setFillColor(sf::Color::Yellow);
                waitingHeader.setPosition(170, yPos + 20);
                matchWindow.draw(waitingHeader);

                sf::RectangleShape waitingBg(sf::Vector2f(600, 40 + 30 * playerQueue.size()));
                waitingBg.setPosition(100, yPos + 60);
                waitingBg.setFillColor(sf::Color(50, 50, 50, 200));
                waitingBg.setOutlineThickness(2);
                waitingBg.setOutlineColor(sf::Color(200, 0, 0));
                matchWindow.draw(waitingBg);

                PlayerPriorityQueue tempQueue = playerQueue;
                yPos += 70;
                int waitingIndex = 1;

                while (!tempQueue.empty()) 
                {
                    QueuePlayer player = tempQueue.top();
                    tempQueue.pop();

                    sf::Text waitingNumText(to_string(waitingIndex) + ".", font, 16);
                    waitingNumText.setFillColor(sf::Color::Red);
                    waitingNumText.setPosition(120, yPos + 10);
                    matchWindow.draw(waitingNumText);

                    sf::Text playerNameText(player.username, font, 18);
                    playerNameText.setFillColor(sf::Color::Magenta);
                    playerNameText.setPosition(150, yPos + 10);
                    matchWindow.draw(playerNameText);

                    sf::Text playerScoreText(to_string(player.score), font, 16);
                    playerScoreText.setFillColor(sf::Color::White);
                    playerScoreText.setPosition(450, yPos + 10);
                    matchWindow.draw(playerScoreText);

                    yPos += 30;
                    waitingIndex++;
                }
            }

            matchWindow.display();
        }
    }
};

#endif