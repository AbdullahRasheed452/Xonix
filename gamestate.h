// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
using namespace std;

struct TileNode
{
    int x, y;
    TileNode* next;
    TileNode(int x, int y) : x(x), y(y), next(nullptr) 
    {}
};

struct GameState {
    char saveId[30];
    char playerId[30];
    char timestamp[30];
    int score;
    int playerX, playerY;
    int availablePowerups;
    int nextPowerupScore;
    TileNode* head;

    GameState() : head(nullptr), playerX(0), playerY(0), score(0), availablePowerups(0), nextPowerupScore(50) 
    {
        saveId[0] = playerId[0] = timestamp[0] = '\0';
    }

    void addTile(int x, int y) 
    {
        TileNode* newNode = new TileNode(x, y);
        newNode->next = head;
        head = newNode;
    }

    void clearTiles() 
    {
        while (head) 
        {
            TileNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    ~GameState()
    {
        clearTiles();
    }

    void saveToFile() const
    {
        ofstream out("saves/" + string(saveId) + ".txt", ios::binary);
        if (!out) return;

        out.write(saveId, sizeof(saveId));
        out.write(playerId, sizeof(playerId));
        out.write(timestamp, sizeof(timestamp));
        out.write(reinterpret_cast<const char*>(&score), sizeof(score));
        out.write(reinterpret_cast<const char*>(&playerX), sizeof(playerX));
        out.write(reinterpret_cast<const char*>(&playerY), sizeof(playerY));
        out.write(reinterpret_cast<const char*>(&availablePowerups), sizeof(availablePowerups));
        out.write(reinterpret_cast<const char*>(&nextPowerupScore), sizeof(nextPowerupScore));

        TileNode* curr = head;
        while (curr) 
        {
            int marker = 1;
            out.write(reinterpret_cast<char*>(&marker), sizeof(marker));
            out.write(reinterpret_cast<char*>(&curr->x), sizeof(curr->x));
            out.write(reinterpret_cast<char*>(&curr->y), sizeof(curr->y));
            curr = curr->next;
        }
        int marker = 0;
        out.write(reinterpret_cast<char*>(&marker), sizeof(marker));
    }

    bool loadFromFile(const char* sid)
    {
        ifstream in("saves/" + string(sid) + ".txt", ios::binary);
        if (!in) return false;

        clearTiles();
        in.read(saveId, sizeof(saveId));
        in.read(playerId, sizeof(playerId));
        in.read(timestamp, sizeof(timestamp));

        if (in.peek() != EOF) 
        {
            in.read(reinterpret_cast<char*>(&score), sizeof(score));
        }
        else
        {
            score = 0;
        }

        in.read(reinterpret_cast<char*>(&playerX), sizeof(playerX));
        in.read(reinterpret_cast<char*>(&playerY), sizeof(playerY));

        if (in.peek() != EOF) 
        {
            in.read(reinterpret_cast<char*>(&availablePowerups), sizeof(availablePowerups));
            in.read(reinterpret_cast<char*>(&nextPowerupScore), sizeof(nextPowerupScore));
        }
        else {
            availablePowerups = 0;
            nextPowerupScore = 50;
            int tempScore = score;
            while (tempScore >= nextPowerupScore)
            {
                availablePowerups++;
                if (nextPowerupScore < 100)
                {
                    nextPowerupScore += 20;
                }
                else 
                {
                    nextPowerupScore += 30;
                }
            }
        }

        int marker;
        while (true) {
            if (in.peek() == EOF)
            {
                break;
            }

            in.read(reinterpret_cast<char*>(&marker), sizeof(marker));
            if (marker == 0 || in.eof())
            {
                break;
            }

            int x, y;
            in.read(reinterpret_cast<char*>(&x), sizeof(x));
            in.read(reinterpret_cast<char*>(&y), sizeof(y));
            addTile(x, y);
        }

        return true;
    }

    void generateTimestamp() 
    {
        time_t now = time(0);
        struct tm tmStruct;
        localtime_s(&tmStruct, &now);
        char newTimestamp[30];
        strftime(newTimestamp, sizeof(newTimestamp), "%Y-%m-%d %H:%M:%S", &tmStruct);
        strcpy_s(timestamp, sizeof(timestamp), newTimestamp);
        cout << "Timestamp: " << timestamp << endl;
    }

    void checkPowerupEarned(int oldScore, int newScore) 
    {
        while (oldScore < nextPowerupScore && newScore >= nextPowerupScore)
        {
            availablePowerups++;
            cout << "New power-up earned at score " << nextPowerupScore << "!" << endl;
            if (nextPowerupScore < 100) 
            {
                nextPowerupScore += 20;
            }
            else {
                nextPowerupScore += 30;
            }
        }
    }

    bool usePowerup()
    {
        if (availablePowerups > 0) 
        {
            availablePowerups--;
            return true;
        }
        return false;
    }
};

#endif 
