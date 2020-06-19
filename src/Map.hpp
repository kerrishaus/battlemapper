#ifndef MAP_HPP
#define MAP_HPP

#include "Tilemap.hpp"
#include "Tileset.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <deque>

class Map : public sf::Drawable
{
public:
    struct Layer
    {
        Tilemap visual;
        std::vector<int> tiledata;
    };

    void load(const std::string& name, Tileset& tileset)
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

/*
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
*/

        std::string layerdata;
        int layerCount = 0;
        while(std::getline(readMap, layerdata))
        {
            std::cout << "loading layer " << layerCount << std::endl;

            Layer layer;

            layer.tiledata.clear();
            layer.tiledata.resize(size.x * size.y);

            for (size_t i = 0; i < size.y; i++)
            {   
                std::string line = layerdata.substr(0, layerdata.find_first_of(':'));
                for (size_t j = 0; j < size.x; j++)
                {
                    size_t pos = line.find_first_of(',');
                    std::string ids = line.substr(0, pos);
                    int id = std::stoi(ids);

                    layer.tiledata[j + (size.x * i)] = id;

                    line.erase(0, pos + 1);
                }
                layerdata.erase(0, layerdata.find_first_of(':') + 1);
            }

            layer.visual.load(tileset, layer.tiledata, size);
            layers.push_back(layer);

            layerCount += 1;
        }
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

        for (const Layer& layer : layers)
        {
            for (size_t i = 0; i < size.y; i++)
            {
                for (size_t j = 0; j < size.x; j++)
                    saveStream << layer.tiledata[j + (size.x * i)] << (j == size.x ? "" : ",");

                saveStream << ":";
            }

            std::cout << std::endl;
        }
    }

    void move(const sf::Vector2f& offset)
    {
        for (Layer& layer : layers)
            layer.visual.move(offset);

        this->position += offset;
    }

    void setPosition(const sf::Vector2f& position)
    {
        for (Layer& layer : layers)
            layer.visual.setPosition(position);

        this->position = position;
    }

    const sf::Vector2f& getPosition() const
    {
        return position;
    }

    void setTile(const sf::Vector2i& tilePosition, const int tileType, Tileset& tileset)
    {
        if (tilePosition.x >= size.x)
            return;
        if (tilePosition.x < 0)
            return;

        if (tilePosition.y >= size.y)
            return;
        if (tilePosition.y < 0)
            return;

        layers[currentLayer].tiledata[tilePosition.x + (size.x * tilePosition.y)] = tileType;
        layers[currentLayer].visual.load(tileset, layers[currentLayer].tiledata, size);
    }

    int getTile(const sf::Vector2i& tilePosition) const
    {
        return layers[currentLayer].tiledata[tilePosition.x + (size.x * tilePosition.y)];
    }

    void increaseSizeUp(unsigned int amount, Tileset& tileset)
    {
        size.y += amount;

        for (int i = 0; i < amount; i++)
            for (int j = 0; j < size.x; j++)
                layers[currentLayer].tiledata.insert(layers[currentLayer].tiledata.begin(), 0);

        layers[currentLayer].visual.load(tileset, layers[currentLayer].tiledata, size);
    }

    void increaseSizeDown(unsigned int amount, Tileset& tileset)
    {
        size.y += amount;

        for (int i = 0; i < amount; i++)
            for (int j = 0; j < size.x; j++)
                layers[currentLayer].tiledata.push_back(0);

        layers[currentLayer].visual.load(tileset, layers[currentLayer].tiledata, size);
    }

    void increaseSizeLeft(unsigned int amount, Tileset& tileset)
    {
        size.x += amount;

        for (int i = 0; i < amount; i++)
            for (int j = 0; j < size.y; j++)
                layers[currentLayer].tiledata.insert(layers[currentLayer].tiledata.begin() + (j * size.x), 0);

        layers[currentLayer].visual.load(tileset, layers[currentLayer].tiledata, size);
    }

    void increaseSizeRight(unsigned int amount, Tileset& tileset)
    {
        size.x += amount;

        for (int i = 0; i < amount; i++)
            for (int j = 0; j < size.y; j++)
                layers[currentLayer].tiledata.insert(layers[currentLayer].tiledata.begin() + (j * size.x), 0);

        layers[currentLayer].visual.load(tileset, layers[currentLayer].tiledata, size);
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        for (const Layer& layer : layers)
            target.draw(layer.visual);
    }

    std::string name;
    std::string author;
    
    int revision = -1;
    int applicationrevision = -1;

    sf::Vector2u size;

    std::deque<Layer> layers;
    int currentLayer = 0;

private:
    sf::Vector2f position;
};

#endif // !MAP_HPP