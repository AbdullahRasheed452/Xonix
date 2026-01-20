// Group names : M.Abdullah, Abdullah Rasheed
// Roll no's : 23i-0509 , 23i-0646
// Project title : xonix game data structures


#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include "theme.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

class ThemeManager 
{
private:
    static ThemeManager* instance;
    AVLTree themeTree;
    int currentThemeID;
    bool hasSelectedTheme;

    ThemeManager() 
    {
        themeTree.loadThemesFromFile("themes.txt");
        currentThemeID = 0;
        cout << "Current theme ID: " << currentThemeID << endl;
        hasSelectedTheme = (currentThemeID > 0);
    }

public:
    static ThemeManager* getInstance() 
    {
        if (instance == nullptr) 
        {
            instance = new ThemeManager();
        }
        return instance;
    }

    ~ThemeManager() {}

    AVLTree& getThemeTree() 
    {
        return themeTree;
    }

    void setCurrentTheme(int themeID)
    {
        currentThemeID = themeID;
        hasSelectedTheme = true;
        themeTree.saveThemePreferences("theme_pref.txt", themeID);
    }

    int getCurrentThemeID() const 
    {
        return currentThemeID;
    }

    bool hasThemeSelected() const 
    {
        return hasSelectedTheme;
    }

    Theme* getCurrentTheme()
    {
        if (!hasSelectedTheme) 
        {
            return nullptr;
        }
        return themeTree.searchByID(currentThemeID);
    }

    bool loadThemeBackgroundTexture(sf::Texture& texture) 
    {
        if (!hasSelectedTheme) 
        {
            return false;
        }
        Theme* currentTheme = getCurrentTheme();
        if (!currentTheme) 
        {
            return false;
        }
        if (!texture.loadFromFile(currentTheme->getBackgroundPath()))
        {
            cout << "Failed to load theme background: " << currentTheme->getBackgroundPath() << endl;
            return false;
        }
        return true;
    }

    bool applyThemeBackground(sf::Sprite& sprite) 
    {
        if (!hasSelectedTheme) 
        {
            return false;
        }
        sf::Texture texture;
        if (!loadThemeBackgroundTexture(texture))
        {
            return false;
        }
        sprite.setTexture(texture);
        return true;
    }

    string getCurrentBackgroundPath()
    {
        if (!hasSelectedTheme) 
        {
            return "";
        }
        Theme* currentTheme = getCurrentTheme();
        return currentTheme->getBackgroundPath();
    }
};

ThemeManager* ThemeManager::instance = nullptr;

#endif
