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

        void draw(sf::RenderWindow& window);

        const glm::vec2 getPosition() const;
        std::shared_ptr<Gun> &getGun();

    private:

        bool checkCanJump() const {

            //if player is falling it means he isn't standing on top of any object because if he was, his velocity would be 0
            //therefore he can't jump if his velocity isn't 0
            return (standingOnSolid || standingOnTile) && hitboxMovementController.getVelocities().y == 0;
        }

        bool checkIsJumping() const {

            //player is jumping if he is moving upwards and he isn't able to jump
            return hitboxMovementController.getVelocities().y < 0 && !standingOnSolid;
        }

        bool checkIsCrouching() const {

            //player is crouching if he is holding down, not moving, and on the ground
            return checkCanJump() && sf::Keyboard::isKeyPressed(controls.down) && hitboxMovementController.getVelocities().x == 0;
        }

        //check if player can extend his jump by holding the jump button
        bool checkExtendJump() const {

            return holdingJump && extraJumpTimer.getElapsedTime() < extraJumpDuration;
        }

        //calculate the position where the gun produces bullets relative to the player
        //differs whenever player is facing in different directions
        glm::vec2 calculateGunfireOrigin() const;

        //vertical and horizontal tile collisions differ only in terms of the collision handling function they call
        CollisionResponse handleTileCollision(TileMap& map, CollisionResponse(*collisionFunction)(std::shared_ptr<Tile>& tile, HitboxMovementController& object));
        CollisionResponse handleTileCollisionHorizontally(TileMap& map);
        CollisionResponse handleTileCollisionVertically(TileMap& map);

        void determineDirection();
        void jump();

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
};

#endif // PLAYER_H_INCLUDED
