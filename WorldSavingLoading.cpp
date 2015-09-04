#include "WorldSavingLoading.h"
#include "GameWorld.h"
#include "Tile.h"
#include "FileManipulators.h"
#include "StringManipulator.h"
#include "BackgroundManager.h"
#include "SpawnerData.h"

#include <sstream>
#include <vector>
#include <memory>

using std::stringstream;
using std::vector;
using std::shared_ptr;
using std::string;
using std::fstream;

const string savedDataPath("levels/");
const string saveFileExtention(".txt");

const DataTagPair playerSpawnTag("PlayerSpawnBegin", "PlayerSpawnBegin");
const DataTagPair tileMapTag("TileMapBegin", "TileMapEnd");
const DataTagPair backgroundTag("BackgroundBegin", "BackgroundEnd");
const DataTagPair worldBoundsTag("worldBoundsBegin", "worldBoundsEnd");
const DataTagPair worldBoundsBossFightTag("bossWorldBoundsBegin", "bossWorldBoundsEnd");
const DataTagPair nonBossEnemySpawnerTag("nonBossEnemySpawnersBegin", "nonBossEnemySpawnersEnd");
const DataTagPair bossEnemySpawnerTag("bossEnemySpawnersBegin", "bossEnemySpawnersEnd");
const DataTagPair basicEnemySpawnerTag("basicEnemySpawnerBegin", "basicEnemySpawnerEnd");
const DataTagPair turretEnemySpawnerTag("turretSpawnerBegin", "turretSpawnerEnd");
const DataTagPair omnidirectionalTurretSpawnerTag("omnidirectionalTurretSpawnerBegin", "omnidirectionalTurretSpawnerEnd");
const DataTagPair destructibleBlocksTag("destructibleBlockBegin", "destructibleBlockEnd");

void saveWorld(const string& worldName, GameWorld &world) {

    fstream file;

    string fileName = savedDataPath + worldName + saveFileExtention;
    file.open(savedDataPath + worldName + saveFileExtention, std::ios_base::out | std::ios_base::trunc);

    if(!file) {

        cout << "Failed to open level \"" << worldName << "\" ";
        cout << "with file path \"" << fileName << "\".";
        return;
    }

    //save each aspect of the world
    saveWorldBoundsData(file, world.worldBounds, worldBoundsTag);
    saveWorldBoundsData(file, world.worldBoundsBossFight, worldBoundsBossFightTag);
    saveTileMapData(file, world.tileMap);
    saveBackgroundData(file, world.backgrounds);
    saveSpawnerCollection(file, world.nonBossEnemySpawners, nonBossEnemySpawnerTag);
    saveSpawnerCollection(file, world.bossEnemySpawners, bossEnemySpawnerTag);
//    saveDestructibleBlocks(file, world.destructibleBlocks, destructibleBlocksTag);

    file.close();
}

void saveWorldBoundsData(std::fstream &file, sf::FloatRect &bounds, const DataTagPair &tagPair) {

    file << tagPair.first << endl;

    file << bounds.left << " " << bounds.top << " " << bounds.width << " " << bounds.height << endl;

    file << tagPair.second << endl;
}

void saveTileMapData(fstream &file, TileMap &map) {

    file << tileMapTag.first << endl;

    auto tiles = map.getAllTiles();

    //then property of each tile
    for(auto &tile : tiles) {

        sf::FloatRect boundingBox = tile->getBoundingBox();
        TileType type = tile->getType();
        string tileTextureName = tile->getTextureName();
        sf::IntRect textureRect = tile->getTextureRect();

        //save the tile data required for the tilemap to create a tile
        //only needs position, tyletype, texture, and texture rect
        //save order is: x position, y position, type, texture name,
        //texture rect x, texture rect y, texture rect width, texture rect height
        file << boundingBox.left << " " << boundingBox.top << " ";
        file << (int)type << " " << tileTextureName << " ";
        file << textureRect.left << "  " << textureRect.top << "  " << textureRect.width << "  " << textureRect.height << endl;
    }

    //tile data saved
    file << tileMapTag.second << endl;
}

void saveBackgroundData(std::fstream &file, BackgroundManager &manager) {

    file << backgroundTag.first << endl;

    auto backgroundSaveData = manager.getDataForSaving();

    //save all background filenames and their associated distance from view
    for(auto &data : backgroundSaveData) {

        //first in pair is the background filename, second is the distance from view
        file << data.first << " " << data.second << endl;
    }

    file << backgroundTag.second << endl;
}

void saveSpawnerCollection(std::fstream &file, EnemySpawnerCollection &collection, const DataTagPair &spawnerTag) {

    file << spawnerTag.first << endl;

    //for each spawner collection save each spawner type along with its tags
    //start with the normal enemies
    saveSpawnPoints(file, collection.enemySpawnInfo.spawnPoints, basicEnemySpawnerTag);
    saveSpawnPoints(file, collection.turretSpawnInfo.spawnPoints, turretEnemySpawnerTag);
    saveSpawnPoints(file, collection.omnidirectionalTurretSpawnInfo.spawnPoints, omnidirectionalTurretSpawnerTag);

    file << spawnerTag.second << endl;
}

void saveSpawnPoints(std::fstream &file, vector<shared_ptr<SpawnPoint> > &spawnPoints, const DataTagPair &enemyClassificationTag) {

    file << enemyClassificationTag.first << endl;

    for(auto &spawnPoint : spawnPoints) {

        //save data in following order, each peice of data seperated by a space
        //max # of enemies spawned, spawn position, spawn delay in MILLISECONDS, type of enemy spawned
        file << spawnPoint->getMaxEnemiesSpawned() << " ";
        file << spawnPoint->getSpawnPosition().x << " " << spawnPoint->getSpawnPosition().y << " ";
        file << spawnPoint->getSpawnDelayInMilliseconds() << " ";
        file << (int)spawnPoint->getTypeOfEnemySpawned() << endl;
    }

    file << enemyClassificationTag.second << endl;
}

void loadWorld(const std::string &worldName, GameWorld &world) {

    fstream file;

    string fileName = savedDataPath + worldName + saveFileExtention;
    file.open(savedDataPath + worldName + saveFileExtention, std::ios_base::in);

    if(!file) {

        cout << "Failed to open level \"" << worldName << "\" ";
        cout << "with file path \"" << fileName << "\".";
        return;
    }

    loadWorldBoundsData(file, world.worldBounds, worldBoundsTag);
    loadWorldBoundsData(file, world.worldBoundsBossFight, worldBoundsBossFightTag);
    loadTileMapData(file, world.tileMap, glm::vec2(world.worldBounds.width, world.worldBounds.height));
    loadBackgroundData(file, world.backgrounds, world.worldBounds);

    file.close();
}

void loadWorldBoundsData(std::fstream &file, sf::FloatRect &bounds, const DataTagPair &boundsTag) {

    if(!readAfterLine(file, boundsTag.first)) {

        cout << "Failed to find bounds data" << endl;
        return;
    }

    string extractedData;
    getline(file, extractedData);

    //save format is "xPos yPos width height"
    //extract each aspect of the world bounds
    bounds.left = atof(extractFirstWordInString(extractedData).c_str());
    bounds.top = atof(extractFirstWordInString(extractedData).c_str());
    bounds.width = atof(extractFirstWordInString(extractedData).c_str());
    bounds.height = atof(extractFirstWordInString(extractedData).c_str());

    //data is loaded
}

void loadTileMapData(std::fstream &file, TileMap &map, glm::vec2 worldSize) {

    if(!readAfterLine(file, tileMapTag.first)) {

        cout << "Failed to find tilemap data" << endl;
        return;
    }

    //load data about each tile and set the tile using the tilemap
    map.resize(worldSize.x, worldSize.y);

    string extracted;
    getline(file, extracted);

    while(extracted != tileMapTag.second && file) {

        //extract each peice of data from the line and use it to create a tile
        sf::Vector2f position;

        position.x = atoi(extractFirstWordInString(extracted).c_str());

        position.y = atoi(extractFirstWordInString(extracted).c_str());

        TileType type = (TileType)atoi(extractFirstWordInString(extracted).c_str());

        string textureFileName = extractFirstWordInString(extracted);

        sf::IntRect textureRect;

        textureRect.left = atoi(extractFirstWordInString(extracted).c_str());
        textureRect.top = atoi(extractFirstWordInString(extracted).c_str());
        textureRect.width = atoi(extractFirstWordInString(extracted).c_str());
        textureRect.height = atoi(extractFirstWordInString(extracted).c_str());

        map.setTile(position, type, textureFileName, textureRect);

        extracted = "";

        getline(file, extracted);
    }

    map.displayChangedTiles();

    //data is loaded
}

void loadBackgroundData(fstream &file, BackgroundManager &manager, sf::FloatRect worldSize) {

    if(!readAfterLine(file, backgroundTag.first)) {

        cout << "failed to find background data" << endl;
        return;
    }

    manager.clearBackgrounds();

    string extracted;
    getline(file, extracted);

    while(extracted != backgroundTag.second && file) {

        //load each background file name and insert the background
        string backgroundFileName = extractFirstWordInString(extracted);

        float distanceFromView = atof(extractFirstWordInString(extracted).c_str());

        manager.insertBackground(backgroundFileName, distanceFromView, worldSize);

        extracted = "";
        getline(file, extracted);
    }
}
