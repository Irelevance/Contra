#include "ObjectTypeToFilename.h"
#include "Player.h"

using std::string;

const std::string enemyDataPath = "Data/EnemyData/";
const std::string playerDataPath = "Data/PlayerData/";
const std::string bulletDataPath = "Data/BulletData/";
const std::string blockDataPath = "Data/BlockData/";
const std::string gunDataPath = "Data/GunData/";
const std::string powerUpDataPath = "Data/PowerUpData/";

string getFilenameForData(const EnemyType &enemyType) {

    switch(enemyType) {

        case EnemyType::ENEMY_GOOMBA:
            return enemyDataPath + "goomba.txt";

        case EnemyType::ENEMY_PIRANHA:
            return enemyDataPath + "piranha.txt";

        case EnemyType::ENEMY_MUSHROOM:
            return enemyDataPath + "mushroom.txt";

        default:
            return "";
    }
}

string getFilenameForData(const int &playerNumber) {

    switch(playerNumber) {

        case (int)PlayerNumber::PLAYER_1:
            return playerDataPath + "player1_data.txt";

        case (int)PlayerNumber::PLAYER_2:
            return playerDataPath + "player2_data.txt";

        case (int)PlayerNumber::PLAYER_3:
            return playerDataPath + "player3_data.txt";

        case (int)PlayerNumber::PLAYER_4:
            return playerDataPath + "player4_data.txt";

        default:
            return "";
    }
}

string getFilenameForData(const BulletType &bulletType) {

    switch(bulletType) {

        case BulletType::BULLET_SLOW:
            return bulletDataPath + "slow_bullet.txt";

        case BulletType::BULLET_FAST:
            return bulletDataPath + "fast_bullet.txt";

        case BulletType::BULLET_MEDIUM:
            return bulletDataPath + "medium_bullet.txt";

        default:
            return "";
    }
}

string getFilenameForData(const DestructibleBlockType &blockType) {

    switch(blockType) {

        case DestructibleBlockType::BLOCK_BRICK:
            return blockDataPath + "brick.txt";

        default:
            return "";
    }
}

string getFilenameForData(const GunType &gunType) {

    switch(gunType) {

        case GunType::GUN_BASIC:
            return gunDataPath + "basic_gun.txt";

        default:
            return "";
    }
}

string getFilenameForData(const PowerUpType &powerUpType) {

    ///all powerup data is stored on one file
    ///a single powerup will have the animation stuff for all powerup types
    switch(powerUpType) {

        default:
            return powerUpDataPath + "powerup.txt";
    }
}
