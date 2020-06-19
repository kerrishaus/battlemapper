#include "Tilemap.hpp"

#include <string>
#include <vector>
#include <iostream>

namespace fs = std::experimental::filesystem;

Tilemap::Tilemap()
{
    up.setSize(sf::Vector2f(8, 8));
    down.setSize(sf::Vector2f(8, 8));
    left.setSize(sf::Vector2f(8, 8));
    right.setSize(sf::Vector2f(8, 8));

    up.setFillColor(sf::Color(80, 80, 80));
    down.setFillColor(sf::Color(80, 80, 80));
    left.setFillColor(sf::Color(80, 80, 80));
    right.setFillColor(sf::Color(80, 80, 80));

    up.setOutlineColor(sf::Color(120, 120, 120));
    down.setOutlineColor(sf::Color(120, 120, 120));
    left.setOutlineColor(sf::Color(120, 120, 120));
    right.setOutlineColor(sf::Color(120, 120, 120));

    up.setOutlineThickness(2.0f);
    down.setOutlineThickness(2.0f);
    left.setOutlineThickness(2.0f);
    right.setOutlineThickness(2.0f);
}

bool Tilemap::load(Tileset& tileset, const std::vector<int>& tiles, const sf::Vector2u& mapSize)
{
    m_tileset = &tileset;

    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(mapSize.x * mapSize.y * 4);

    // populate the vertex array, with one quad per tile
    for (unsigned int i = 0; i < mapSize.x; ++i)
    {
        for (unsigned int j = 0; j < mapSize.y; ++j)
        {
            // get the current tile number
            int tileNumber = tiles[i + j * mapSize.x];

            // find its position in the tileset texture
            int tu = tileNumber % (m_tileset->getSize().x / m_tileset->tileSize.x);
            int tv = tileNumber / (m_tileset->getSize().x / m_tileset->tileSize.x);

            // get a pointer to the current tile's quad
            sf::Vertex* quad = &m_vertices[(i + j * mapSize.x) * 4];

            // define its 4 corners
            quad[0].position = sf::Vector2f(i * m_tileset->tileSize.x, j * m_tileset->tileSize.y);
            quad[1].position = sf::Vector2f((i + 1) * m_tileset->tileSize.x, j * m_tileset->tileSize.y);
            quad[2].position = sf::Vector2f((i + 1) * m_tileset->tileSize.x, (j + 1) * m_tileset->tileSize.y);
            quad[3].position = sf::Vector2f(i * m_tileset->tileSize.x, (j + 1) * m_tileset->tileSize.y);

            // define its 4 texture coordinates
            quad[0].texCoords = sf::Vector2f(tu * m_tileset->tileSize.x, tv * m_tileset->tileSize.y);
            quad[1].texCoords = sf::Vector2f((tu + 1) * m_tileset->tileSize.x, tv * m_tileset->tileSize.y);
            quad[2].texCoords = sf::Vector2f((tu + 1) * m_tileset->tileSize.x, (tv + 1) * m_tileset->tileSize.y);
            quad[3].texCoords = sf::Vector2f(tu * m_tileset->tileSize.x, (tv + 1) * m_tileset->tileSize.y);
        }
    }

    up.setPosition(sf::Vector2f(getPosition().x + ((m_tileset->tileSize.x * (mapSize.x)) / 2) - (up.getSize().x / 2), getPosition().y - up.getSize().y));
    down.setPosition(sf::Vector2f(getPosition().x + ((m_tileset->tileSize.x * (mapSize.x)) / 2) - (down.getSize().x / 2), getPosition().y + (m_tileset->tileSize.x * mapSize.y)));
    left.setPosition(sf::Vector2f(getPosition().x - left.getSize().x, getPosition().y + ((m_tileset->tileSize.x * mapSize.y) / 2) - (left.getSize().y / 2)));
    right.setPosition(sf::Vector2f(getPosition().x + (m_tileset->tileSize.x * mapSize.x), getPosition().y + ((m_tileset->tileSize.x * mapSize.y) / 2) - (right.getSize().y / 2)));

    return true;
}

const sf::FloatRect Tilemap::getGlobalBounds() const
{
    return m_vertices.getBounds();
}

void Tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= getTransform();
    states.texture = m_tileset;

	target.draw(up);
	target.draw(down);
	target.draw(left);
	target.draw(right);

    target.draw(m_vertices, states);
}