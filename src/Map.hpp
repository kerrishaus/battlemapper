#ifndef MAP_HPP
#define MAP_HPP

#include "Tilemap.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

class Map
{
public:
    void load(const std::string& name)
    {
        std::ifstream readMap("./battlemapper_bin/" + name + ".dat", std::ios::in | std::ios::binary);

        std::string revision, applicationrevision, width, height;
        std::getline(readMap, this->name);
        if (name != this->name)
            std::cerr << "WARNING: mapname and filename do not match!" << std::endl;
        std::getline(readMap, author);
        std::getline(readMap, revision);
        this->revision = std::stoi(revision);
        std::getline(readMap, applicationrevision);
        this->applicationrevision = std::stoi(applicationrevision);
        std::getline(readMap, width);
        std::getline(readMap, height);

        size.x = std::stoi(width);
        size.y = std::stoi(height);

        std::cout << "Creating map of size: " << size.x << "x" << size.y << std::endl;

        tiledata.clear();
        tiledata.resize(size.x * size.y);

        for (size_t i = 0; i < size.y; i++)
        {
            std::string line;
            std::getline(readMap, line);

            for (size_t j = 0; j < size.x; j++)
            {
                size_t pos = line.find_first_of(',');
                std::string ids = line.substr(0, pos);
                int id = std::stoi(ids);

                tiledata[j + (size.x * i)] = id;
    //			tilesInformation.emplace(i, tiles[level[i]]);

                line.erase(0, pos + 1);
            }
        }

        visual.load("./battlemapper_bin/resources/tileset.png", sf::Vector2u(32, 32), tiledata, size.x, size.y);
    }

    void save()
    {
        std::cout << "saving map" << std::endl;

        std::ofstream saveStream("./battlemapper_bin/" + this->name + ".dat", std::ios::out | std::ios::binary | std::ios::trunc);

        if (!saveStream.good())
            abort();

        saveStream << name << std::endl;
        saveStream << author << std::endl;
        saveStream << revision++ << std::endl;
        saveStream << applicationrevision << std::endl;
        saveStream << size.x << std::endl;
        saveStream << size.y << std::endl;

        for (size_t i = 0; i < size.y; i++)
        {
            for (size_t j = 0; j < size.x; j++)
                saveStream << tiledata[j + (size.x * i)] << ",";

            saveStream << std::endl;
        }
    }

    void setTile(const sf::Vector2i& tilePosition, const int tileType)
    {
        if (tilePosition.x >= size.x)
            return;
        if (tilePosition.x < 0)
            return;

        tiledata[tilePosition.x + (size.x * tilePosition.y)] = tileType;
        visual.load("./battlemapper_bin/resources/tileset.png", sf::Vector2u(32, 32), tiledata, size.x, size.y);
    }

    int getTile(const sf::Vector2i& tilePosition) const
    {
        return tiledata[tilePosition.x + (size.x * tilePosition.y)];
    }

    std::string name;
    std::string author;
    
    int revision = -1;
    int applicationrevision = -1;

    sf::Vector2i size;

    Tilemap visual;

private:
    std::vector<int> tiledata;
};

#endif // !MAP_HPP