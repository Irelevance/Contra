#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include "SFML/Graphics.hpp"
#include "GlobalConstants.h"
#include "ObjectHitbox.h"
#include "HitboxMovementController.h"
#include "TileCollisionHandling.h"
#include "TileMap.h"
#include "Direction.h"
#include <memory>
#include <iostream>

using std::cout;
using std::endl;

class Gun;

struct PlayerKeys {

    sf::Keyboard::Key up = sf::Keyboard::Up;
    sf::Keyboard::Key down = sf::Keyboard::Down;
    sf::Keyboard::Key left = sf::Keyboard::Left;
    sf::Keyboard::Key right = sf::Keyboard::Right;

    sf::Keyboard::Key jump = sf::Keyboard::A;
    sf::Keyboard::Key fire = sf::Keyboard::S;
};

class Player {

    public:

        Player(const PlayerKeys& keyConfiguration = PlayerKeys());

        void handleInputEvents(sf::Event& event, sf::RenderWindow& window);
        void handleKeystate(sf::RenderWindow& window);

        //time should be in seconds
        void update(const float& deltaTime, const sf::FloatRect& worldBounds, TileMap& map);
        void getHit();
        bool checkIsAlive();
        bool checkCanRespawn();
        void respawn(const sf::FloatRect &cameraBounds);//spawns at top of camera

        void draw(sf::RenderWindow& window);

        void setLives(const int &newLives);

        const ObjectHitbox& getHitbox() const;
        const glm::vec2 getPosition() const;
        std::shared_ptr<Gun> &getGun();
        int getLives() const;

    private:

        bool checkCanJump() const;
        bool checkIsJumping() const;
        bool checkIsCrouching() const;

        //check if player can extend his jump by holding the jump button
        bool checkExtendJump() const;

        //calculate the position where the gun produces bullets relative to the player
        //differs whenever player is facing in different directions
        glm::vec2 calculateGunfireOrigin() const;

        //vertical and horizontal tile collisions differ only in terms of the collision handling function they call
        CollisionResponse handleTileCollision(TileMap& map, CollisionResponse(*collisionFunction)(std::shared_ptr<Tile>& tile, HitboxMovementController& object));
        CollisionResponse handleTileCollisionHorizontally(TileMap& map);
        CollisionResponse handleTileCollisionVertically(TileMap& map);

        void determineDirection();
        void jump();

        void die();

        enum LifeState {

            ALIVE, //while player is dying or dead he can't move or jump
            DYING,  //but he can't respawn unless he is dead
            DEAD

        } lifeState;

        //velocity of player when he begins to move
        //measured in meters per second
        //in object space
        const sf::Vector2f MOVEMENT_VELOCITY;

        //seperate flags to keep track of player standing on blocks/ground and tiles
        bool standingOnSolid;
        bool standingOnTile;

        //when user is holding jump he will jump higher
        //it will basically disable gravity until extra jump timer exceeds the max time allowed for user to hold jump
        bool holdingJump;
        sf::Clock extraJumpTimer;
        const sf::Time extraJumpDuration;

        ObjectHitbox hitbox;
        HitboxMovementController hitboxMovementController;

        Direction direction;
        sf::RectangleShape player;
        std::shared_ptr<Gun> gun;

        //the control setup for this player
        PlayerKeys controls;

        int lives;

        sf::Clock respawnInvinsibilityTimer;
        sf::Time respawnInvinsibilityDuration;
};

#endif // PLAYER_H_INCLUDED
