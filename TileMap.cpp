#include "TileMap.h"
#include "glm/glm.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::make_shared;

TileMap::TileMap() :
    tiles(),
    gridWidth(0),
    gridHeight(0),
    areas(),
    areaSize(glm::min(1024, (int)sf::Texture::getMaximumSize())),
    areaCountHorizontal(0),
    areaCountVertical(0)
    {

    }

TileMap::TileMap(const unsigned &mapWidth, const unsigned &mapHeight):
    tiles(),
    gridWidth(0),
    gridHeight(0),
    areas(),
    areaSize(glm::min(1024, (int)sf::Texture::getMaximumSize())),
    areaCountHorizontal(0),
    areaCountVertical(0)
    {
        resize(mapWidth, mapHeight);
    }

void TileMap::resize(const unsigned &mapWidth, const unsigned &mapHeight) {

    if(mapWidth < 0 || mapHeight < 0) {

        return;
    }

    gridWidth = glm::ceil(mapWidth / static_cast<float>(TILE_SIZE));
    gridHeight = glm::ceil(mapHeight / static_cast<float>(TILE_SIZE));

    tiles.clear();

    for(unsigned y = 0; y < gridHeight; ++y) {

        for(unsigned x = 0; x < gridWidth; ++x) {

            shared_ptr<Tile> tile = make_shared<Tile>(glm::vec2(x * TILE_SIZE, y * TILE_SIZE), TileType::EMPTY);
            tiles.push_back(tile);
        }
    }

    areaCountHorizontal = glm::ceil(mapWidth / (float)areaSize);
    areaCountVertical = glm::ceil(mapHeight / (float)areaSize);

    createRenderedAreas();
}

void TileMap::setTile(const sf::Vector2f& position, const TileType& type, const std::string &textureFilename, const sf::IntRect &textureRect) {

    glm::i32vec2 gridPosition = convertToGridPosition(glm::vec2(position.x, position.y));

    if(!checkValidGridPosition(gridPosition)) {

        return;
    }

    int index = gridPosition.y * gridWidth + gridPosition.x;

    if(tiles.size() <= index) {

        return;
    }

    tiles[index]->setType(type);
    tiles[index]->setTexture(textureFilename);
    tiles[index]->setTextureRect(textureRect);
}

vector<shared_ptr<Tile> > TileMap::getTilesInRegion(const glm::vec2 &upperLeft, const glm::vec2& bottomRight) const {

    glm::i32vec2 gridTopLeft = convertToGridPosition(upperLeft);
    glm::i32vec2 gridBottomRight = convertToGridPosition(bottomRight);

    if(gridTopLeft.x > gridBottomRight.x || gridTopLeft.y > gridBottomRight.y) {

        return vector<shared_ptr<Tile> > ();
    }

    gridTopLeft.x = glm::clamp(gridTopLeft.x, 0, gridWidth - 1);
    gridTopLeft.y = glm::clamp(gridTopLeft.y, 0, gridHeight - 1);
    gridBottomRight.x = glm::clamp(gridBottomRight.x, 0, gridWidth - 1);
    gridBottomRight.y = glm::clamp(gridBottomRight.y, 0, gridHeight - 1);

    vector<shared_ptr<Tile> > tilesInRegion;

    for(unsigned y = gridTopLeft.y; y <= gridBottomRight.y; ++y) {

        for(unsigned x = gridTopLeft.x; x <= gridBottomRight.x; ++x) {

            int index = y * gridWidth + x;
            tilesInRegion.push_back(tiles[index]);
        }
    }

    return tilesInRegion;
}

const vector<shared_ptr<Tile> > &TileMap::getAllTiles() {

    return tiles;
}

void TileMap::draw(sf::RenderWindow& window, const glm::vec2 &upperLeft, const glm::vec2 &lowerRight) {

    //convert positions to area positions
    glm::i32vec2 areaUpperLeft(upperLeft.x / areaSize, upperLeft.y / areaSize);
    glm::i32vec2 areaLowerRight(lowerRight.x / areaSize, lowerRight.y / areaSize);

    //make sure positions are valid
    if(areaUpperLeft.x > areaLowerRight.x || areaUpperLeft.y > areaLowerRight.y) {

        return;
    }

    //clamp all values to edge of grid
    areaUpperLeft.x = glm::clamp(areaUpperLeft.x, 0, areaCountHorizontal - 1);
    areaUpperLeft.y = glm::clamp(areaUpperLeft.y, 0, areaCountVertical - 1);
    areaLowerRight.x = glm::clamp(areaLowerRight.x, 0, areaCountHorizontal - 1);
    areaLowerRight.y = glm::clamp(areaLowerRight.y, 0, areaCountVertical - 1);

    //loop through all areas and render them
    for(int y = areaUpperLeft.y; y <= areaLowerRight.y; ++y) {

        for(int x = areaUpperLeft.x; x <= areaLowerRight.x; ++x) {

            int index = y * areaCountHorizontal + x;
            sf::Sprite& toDraw = areas[index]->sprite;

            window.draw(toDraw);
        }
    }
}

void TileMap::drawTiles(sf::RenderTarget& window, const glm::vec2 &upperLeft, const glm::vec2 &lowerRight) {

    vector<shared_ptr<Tile> > tilesInRegion = getTilesInRegion(upperLeft, lowerRight);

    for(unsigned i = 0; i < tilesInRegion.size(); ++i) {

        tilesInRegion[i]->draw(window);
    }
}

void TileMap::drawTilesDebug(sf::RenderTarget& window, const glm::vec2 &upperLeft, const glm::vec2 &lowerRight) {

    vector<shared_ptr<Tile> > tilesInRegion = getTilesInRegion(upperLeft, lowerRight);

    for(unsigned i = 0; i < tilesInRegion.size(); ++i) {

        tilesInRegion[i]->drawDebug(window);
    }
}

glm::i32vec2 TileMap::convertToGridPosition(const glm::vec2 &worldPosition) const {

    return glm::i32vec2(worldPosition.x / TILE_SIZE, worldPosition.y / TILE_SIZE);
}

bool TileMap::checkValidGridPosition(const glm::i32vec2 &gridPosition) const {

    if(gridPosition.x < 0 || gridPosition.x >= gridWidth || gridPosition.y < 0 || gridPosition.y >= gridHeight) {

        return false;
    }

    return true;
}

void TileMap::createRenderedAreas() {

    areas.clear();

    for(int y = 0; y < areaCountVertical; ++y) {

        for(int x = 0; x < areaCountHorizontal; ++x) {

            shared_ptr<RenderedArea> area = make_shared<RenderedArea>();
            area->texture.create(areaSize, areaSize);
            area->texture.clear(sf::Color(0, 0, 0, 0));

            //determine the offset of the topleft tile in this area, and use thsi position to create the view that way tiles that are offscreen are still drawn
            int left = x * areaSize;
            int top = y * areaSize;

            sf::FloatRect viewArea(left, top, areaSize, areaSize);
            sf::View view(viewArea);

            area->texture.setView(view);

            //render tiles to the texture
            drawTilesDebug(area->texture, glm::vec2(left, top), glm::vec2(left + areaSize, top + areaSize));
            area->texture.display();

            //create sprite from texture
            area->sprite.setTexture(area->texture.getTexture());
            area->sprite.setPosition(left, top);

            areas.push_back(area);
        }
    }
}
