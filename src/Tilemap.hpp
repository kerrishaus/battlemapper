#ifndef TILEMAP_HPP
#define TILEMAP_HPP

#include "Tileset.hpp"

#include <SFML/Graphics.hpp>

#include <vector>

class Tilemap : public sf::Drawable, public sf::Transformable
{
public:
    Tilemap();

    bool load(Tileset& tileset, const std::vector<int>& tiles, const sf::Vector2u& mapSize);

    const sf::FloatRect getGlobalBounds() const;

    sf::RectangleShape up;
    sf::RectangleShape down;
    sf::RectangleShape left;
    sf::RectangleShape right;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::VertexArray m_vertices;
    Tileset* m_tileset;
};

#endif // !TILEMAP_HPP