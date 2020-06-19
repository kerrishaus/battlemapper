#ifndef TILESET_HPP
#define TILESET_HPP

#include <SFML/Graphics.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class Tileset : public sf::Texture
{
public:
    sf::Vector2u tileSize;

    void loadFromDirectory(const std::string& directory, const sf::Vector2u& tileSize)
    {
        this->tileSize = tileSize;

        std::vector<sf::Image> images;

        std::string path = "./battlemapper_bin/resources/tiles";
        for (const auto& entry : fs::directory_iterator(path))
        {
            std::cout << entry.path() << std::endl;

            std::string path = entry.path();
            std::string filename = path.substr(path.find_last_of('/'), path.length());
            std::string extension = path.substr(path.find_last_of('.'), path.length());

            if (extension != ".png")
                continue;

            sf::Image image;
            image.loadFromFile("./battlemapper_bin/resources/tiles/" + filename);

            images.push_back(image);

            std::cout << "loaded " << filename << std::endl;
        }

        sf::Image textureatlas;
        textureatlas.create(images.size() * tileSize.x, tileSize.y);

        for (size_t i = 0; i < images.size(); i++)
        {
            for (size_t j = 0; j < tileSize.y; j++)
            {
                for (size_t k = 0; k < tileSize.x; k++)
                {
                    textureatlas.setPixel((i * 32) + k, j, images[i].getPixel(k, j));
                }
            }
        }

        this->loadFromImage(textureatlas);
    }
};

#endif // !TILESET_HPP