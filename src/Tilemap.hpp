#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include <SFML/Graphics.hpp>

#include <vector>

class Tilemap : public sf::Drawable, public sf::Transformable
{
public:
    bool load(const std::string& tileset, sf::Vector2u tileSize, const std::vector<int>& tiles, unsigned int width, unsigned int height);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};

#endif // !TILEMAP_HPP