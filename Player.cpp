#include "Player.h"
#include "TileCollisionHandling.h"
#include <iostream>
#include <vector>
#include <memory>
#include "Gun.h"
#include "Tile.h"

using std::vector;
using std::shared_ptr;
using std::cout;
using std::endl;

Player::Player(const PlayerKeys& keyConfiguration):
    MOVEMENT_VELOCITY(4.f, 3.9f),
    canJump(false),
    holdingJump(false),
    extraJumpTimer(),
    extraJumpDuration(sf::milliseconds(220)),
    positionController(glm::vec2(64, 128), glm::vec2(0, GRAVITY), glm::vec2(TERMINAL_VELOCITY, TERMINAL_VELOCITY), glm::vec2(1, 0), glm::vec2(0, 1)),
    direction(),
    player(sf::Vector2f(100, 50)),
    gun(),
    controls(keyConfiguration)
    {
        gun = std::make_shared<Gun>(positionController.getObjectSpace(), glm::vec2(0, 0));

        if(!gun) {

            exit(-1);
        }

        player.setSize(sf::Vector2f(positionController.getBoundingBoxWorldSpace().width, positionController.getBoundingBoxWorldSpace().height));
    }

void Player::handleInputEvents(sf::Event& event, sf::RenderWindow& window) {

}

void Player::handleKeystate(sf::RenderWindow& window) {

    if(sf::Keyboard::isKeyPressed(controls.left)) {

        positionController.setVelocities(-MOVEMENT_VELOCITY.x, positionController.getVelocitiesObjectSpace().y);

    } else if(sf::Keyboard::isKeyPressed(controls.right)) {

        positionController.setVelocities(MOVEMENT_VELOCITY.x, positionController.getVelocitiesObjectSpace().y);

    } else {

        positionController.setVelocities(0, positionController.getVelocitiesObjectSpace().y);
    }

    if(sf::Keyboard::isKeyPressed(controls.jump)) {

        jump();
    }

    if(sf::Keyboard::isKeyPressed(controls.fire)) {

        gun->fire(positionController.getPositionWorldSpace(), direction);
    }

    holdingJump = sf::Keyboard::isKeyPressed(controls.jump);

    determineDirection();
}

void Player::update(const float& deltaTime, const sf::FloatRect& worldBounds, TileMap& map) {

    //since player velocity only changes in the y direction you can prevent gravity from pulling player down
    //when player is holding jump button and trying ot extend his jump height dont let gravity pull player down
    if(!checkExtendJump()) {

        positionController.updateVelocities(deltaTime);
    }

    positionController.moveAlongXAxis(deltaTime, worldBounds);

    handleTileCollisionHorizontally(map);

    if(positionController.moveAlongYAxis(deltaTime, worldBounds)) {

        canJump = true;
        positionController.setVelocities(positionController.getVelocitiesObjectSpace().x, 0);
    }

    handleTileCollisionVertically(map);

    player.setPosition(positionController.getBoundingBoxWorldSpace().left, positionController.getBoundingBoxWorldSpace().top);

    gun->update(deltaTime, worldBounds, map);
}

void Player::draw(sf::RenderWindow& window) {

    gun->draw(window);
    window.draw(player);
}

void Player::handleTileCollision(TileMap& map, bool(*collisionFunction)(std::shared_ptr<Tile>& tile, PositionObject& object)) {

    sf::FloatRect bounding = positionController.getObjectSpace().getBoundingBoxWorldSpace();

    //calculate region encompassed by object
    //extedn the region slightly because slope tiles need extra information about object previous position if he leaves a tile
    glm::vec2 regionTopLeft = glm::vec2(bounding.left, bounding.top) - glm::vec2(TILE_SIZE, TILE_SIZE);
    glm::vec2 regionBottomRight = glm::vec2(bounding.left + bounding.width, bounding.top + bounding.height) + glm::vec2(TILE_SIZE, TILE_SIZE);

    vector<shared_ptr<Tile> > tiles = map.getTilesInRegion(regionTopLeft, regionBottomRight);

    for(unsigned i = 0; i < tiles.size(); ++i) {

        if(collisionFunction(tiles[i], positionController)) {

            canJump = true;
        }
    }
}

void Player::handleTileCollisionHorizontally(TileMap& map) {

    handleTileCollision(map, &handleCollisionHorizontal);
}

void Player::handleTileCollisionVertically(TileMap& map) {

    handleTileCollision(map, &handleCollisionVertical);
}

void Player::determineDirection() {

    if(positionController.getVelocitiesObjectSpace().x < 0) {

        direction.horizontal = HorizontalDirection::LEFT;

    } else if(positionController.getVelocitiesObjectSpace().x > 0) {

        direction.horizontal = HorizontalDirection::RIGHT;
    }

    if(sf::Keyboard::isKeyPressed(controls.up)) {

        direction.vertical = VerticalDirection::UP;

    } else if(sf::Keyboard::isKeyPressed(controls.down)) {

        direction.vertical = VerticalDirection::DOWN;

    } else {

        direction.vertical = VerticalDirection::STRAIGHT;
    }

    direction.isFacingCompletelyVertical = (positionController.getVelocitiesObjectSpace().x == 0 && direction.vertical != VerticalDirection::STRAIGHT);
}

void Player::jump() {

    if(checkCanJump()) {

        extraJumpTimer.restart();
        positionController.setVelocities(positionController.getVelocitiesObjectSpace().x, -MOVEMENT_VELOCITY.y);
        canJump = false;
    }
}
