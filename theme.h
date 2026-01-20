// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef THEME_H
#define THEME_H

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

using namespace std;

struct Theme
{
    int themeID;
    char themeName[100];
    char description[200];
    char preview[100];
    char backgroundPath[200];

    Theme()
    {
        themeID = 0;
        themeName[0] = '\0';
        description[0] = '\0';
        preview[0] = '\0';
        backgroundPath[0] = '\0';
    }

    Theme(int id, const char* name, const char* desc, const char* previewCode, const char* bgPath)
    {
        themeID = id;
        int i = 0;
        while (name[i] != '\0' && i < 99) 
        {
            themeName[i] = name[i];
            ++i;
        }
        themeName[i] = '\0';

        i = 0;
        while (desc[i] != '\0' && i < 199)
        {
            description[i] = desc[i];
            ++i;
        }
        description[i] = '\0';

        i = 0;
        while (previewCode[i] != '\0' && i < 99) 
        {
            preview[i] = previewCode[i];
            ++i;
        }
        preview[i] = '\0';

        i = 0;
        while (bgPath[i] != '\0' && i < 199) 
        {
            backgroundPath[i] = bgPath[i];
            ++i;
        }
        backgroundPath[i] = '\0';
    }

    string getBackgroundPath() const
    {
        return string(backgroundPath);
    }
};

struct AVLNode 
{
    Theme theme;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(const Theme& t) : theme(t), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree
{
private:
    AVLNode* root;

    int height(AVLNode* node)
    {
        return node == nullptr ? 0 : node->height;
    }

    int balanceFactor(AVLNode* node) 
    {
        return height(node->left) - height(node->right);
    }

    int max(int a, int b) 
    {
        return (a > b) ? a : b;
    }

    AVLNode* rotateRight(AVLNode* y) 
    {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    AVLNode* rotateLeft(AVLNode* x) 
    {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    AVLNode* balance(AVLNode* node)
    {
        int bf = balanceFactor(node);
        if (bf > 1) 
        {
            if (balanceFactor(node->left) < 0) 
            {
                node->left = rotateLeft(node->left);
            }
            return rotateRight(node);
        }
        if (bf < -1)
        {
            if (balanceFactor(node->right) > 0)
            {
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node);
        }
        return node;
    }

    AVLNode* insert(AVLNode* node, const Theme& theme) 
    {
        if (node == nullptr)
        {
            return new AVLNode(theme);
        }

        if (theme.themeID < node->theme.themeID) 
        {
            node->left = insert(node->left, theme);
        }
        else if (theme.themeID > node->theme.themeID) 
        {
            node->right = insert(node->right, theme);
        }
        else 
        {
            return node;
        }

        node->height = max(height(node->left), height(node->right)) + 1;
        return balance(node);
    }

    AVLNode* searchByID(AVLNode* node, int themeID) 
    {
        if (node == nullptr || node->theme.themeID == themeID)
        {
            return node;
        }
        if (themeID < node->theme.themeID)
        {
            return searchByID(node->left, themeID);
        }
        return searchByID(node->right, themeID);
    }

    void inOrderTraversal(AVLNode* node)
    {
        if (!node)
        {
            return;
        }
        inOrderTraversal(node->left);
        cout << "Theme ID: " << node->theme.themeID << ", Name: " << node->theme.themeName
            << ", Description: " << node->theme.description << ", Preview: " << node->theme.preview << endl;
        inOrderTraversal(node->right);
    }

    void destroyTree(AVLNode* node) 
    {
        if (!node)
        {
            return;
        }
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

public:
    AVLTree() : root(nullptr) {}

    ~AVLTree() 
    {
        destroyTree(root); 
    }

    void insert(const Theme& theme)
    {
        root = insert(root, theme);
    }

    Theme* searchByID(int themeID)
    {
        AVLNode* node = searchByID(root, themeID);
        return node ? &node->theme : nullptr;
    }

    void displayAllThemes() 
    {
        inOrderTraversal(root);
    }

    void loadThemesFromFile(const char* filename) 
    {
        ifstream file(filename);
        char line[512];
        while (file.getline(line, 512))
        {
            int themeID = 0;
            char name[100], desc[200], preview[100], bgPath[200];

            int i = 0, j = 0;
            while (line[i] >= '0' && line[i] <= '9')
            {
                themeID = themeID * 10 + (line[i++] - '0');
            }
            while (line[i] == ' ' || line[i] == ',')
            {
                i++;
            }

            j = 0;
            while (line[i] != ',' && line[i] != '\0')
            {
                name[j++] = line[i++];
            }
            name[j] = '\0';
            if (line[i] == ',')
            {
                i++;
            }

            j = 0;
            while (line[i] != ',' && line[i] != '\0')
            {
                desc[j++] = line[i++];
            }
            desc[j] = '\0';
            if (line[i] == ',')
            {
                i++;
            }

            j = 0;
            while (line[i] != ',' && line[i] != '\0')
            {
                preview[j++] = line[i++];
            }
            preview[j] = '\0';
            if (line[i] == ',')
            {
                i++;
            }

            j = 0;
            while (line[i] != '\0')
            {
                bgPath[j++] = line[i++];
            }
            bgPath[j] = '\0';

            Theme t(themeID, name, desc, preview, bgPath);
            insert(t);
        }
    }

    void saveThemePreferences(const char* filename, int themeID)
    {
        ofstream file(filename, ios::app);
        Theme* selected = searchByID(themeID);
        if (selected) 
        {
            file << "Preferred Theme ID: " << selected->themeID
                << ", Name: " << selected->themeName << endl;
        }
        else 
        {
            cout << "Theme not found." << endl;
        }
    }

    int getCurrentThemeID()
    {
        ifstream file("theme_pref.txt");
        if (!file.is_open()) 
        {
            return 1;
        }

        char line[512];
        int lastThemeID = 1;

        while (file.getline(line, 512))
        {
            const char* idMarker = strstr(line, "Preferred Theme ID:");
            if (idMarker != nullptr) 
            {

                idMarker += strlen("Preferred Theme ID:");


                while (*idMarker != '\0' && *idMarker == ' ') 
                {
                    idMarker++;
                }


                int id = 0;
                while (*idMarker != '\0' && *idMarker >= '0' && *idMarker <= '9')
                {
                    id = id * 10 + (*idMarker - '0');
                    idMarker++;
                }


                if (id > 0) 
                {
                    lastThemeID = id;
                }
            }
        }

        return lastThemeID;
    }
};

#endif