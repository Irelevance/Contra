#include "Player.h"
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
    lifeState(ALIVE),
    MOVEMENT_VELOCITY(4.f, 4.05f),
    standingOnSolid(false),
    standingOnTile(false),
    standingOnPassablePlatform(false),
    holdingJump(false),
    extraJumpTimer(),
    extraJumpDuration(sf::milliseconds(200)),
    hitbox(),
    hitboxMovementController(glm::vec2(0, GRAVITY), glm::vec2(TERMINAL_VELOCITY, TERMINAL_VELOCITY), &hitbox),
    direction(),
    player(sf::Vector2f(50, 100)),
    gun(),
    controls(keyConfiguration),
    lives(3),
    respawnInvinsibilityTimer(),
    respawnInvinsibilityDuration(sf::seconds(3))
    {
        gun = std::make_shared<Gun>();

        if(!gun) {

            exit(-1);
        }

        hitbox.insertHitbox(sf::FloatRect(0, 0, 50, 100));
        hitbox.insertHitbox(sf::FloatRect(-25, 50, 100, 50));
        hitbox.setActiveHitbox(0);
    }

void Player::handleInputEvents(sf::Event& event, sf::RenderWindow& window) {

}

void Player::handleKeystate(sf::RenderWindow& window) {

    if(!checkIsAlive()) {

        hitboxMovementController.setVelocities(0, hitboxMovementController.getVelocities().y);
        return; //don't handle controls if player is dead
    }

    if(sf::Keyboard::isKeyPressed(controls.left)) {

        hitboxMovementController.setVelocities(-MOVEMENT_VELOCITY.x, hitboxMovementController.getVelocities().y);

    } else if(sf::Keyboard::isKeyPressed(controls.right)) {

        hitboxMovementController.setVelocities(MOVEMENT_VELOCITY.x, hitboxMovementController.getVelocities().y);

    } else {

        hitboxMovementController.setVelocities(0, hitboxMovementController.getVelocities().y);
    }


    if(sf::Keyboard::isKeyPressed(controls.jump) && sf::Keyboard::isKeyPressed(controls.down)) {

        jumpDown();

    }

    //don't put this in an if else statement with the downjumping because if you do then players won't be able to jump while running and holding the down button
    if(sf::Keyboard::isKeyPressed(controls.jump)) {

        jump();
    }

    if(sf::Keyboard::isKeyPressed(controls.fire)) {

        gun->fire(hitbox.getOrigin(), calculateGunfireOrigin(), direction);
    }

    holdingJump = sf::Keyboard::isKeyPressed(controls.jump);

    determineDirection();
}

void Player::update(const float& deltaTime, const sf::FloatRect& worldBounds, TileMap& map) {

    //since player velocity only changes in the y direction you can prevent gravity from pulling player down
    //when player is holding jump button and trying ot extend his jump height dont let gravity pull player down
    if(!checkExtendJump()) {

        hitboxMovementController.updateVelocities(deltaTime);
    }

    hitboxMovementController.moveAlongXAxis(deltaTime, worldBounds);

    CollisionResponse collisionResponseHorizontal = handleTileCollisionHorizontally(map);

    if(hitboxMovementController.moveAlongYAxis(deltaTime, worldBounds)) {

        standingOnSolid = true;
        hitboxMovementController.setVelocities(hitboxMovementController.getVelocities().x, 0);

    } else {

        standingOnSolid = false;
    }

    CollisionResponse collisionResponseVertical = handleTileCollisionVertically(map);

    standingOnTile = (collisionResponseHorizontal.handledVertical || collisionResponseVertical.handledVertical);
    standingOnPassablePlatform = collisionResponseHorizontal.canFallThroughGround || collisionResponseVertical.canFallThroughGround;

    sf::FloatRect activeHitbox = hitbox.getActiveHitboxWorldSpace();
    player.setPosition(activeHitbox.left, activeHitbox.top);
    player.setSize(sf::Vector2f(activeHitbox.width, activeHitbox.height));

    gun->update(deltaTime, worldBounds, map);
}

void Player::getHit() {

    if(respawnInvinsibilityTimer.getElapsedTime() < respawnInvinsibilityDuration || !checkIsAlive()) {

        return;
    }

    ///later this should start a death animation and set lifestate to dying, and once the animation finishes the player should actually die
    ///for now just make player die
    die();
}

bool Player::checkIsAlive() {

    return lifeState == ALIVE;
}

bool Player::checkCanRespawn() {

    return lifeState == DEAD && lives > 0;
}

void Player::respawn(const sf::FloatRect &cameraBounds) {

    lifeState = ALIVE;
    glm::vec2 spawnPosition(cameraBounds.left + hitbox.getActiveHitboxObjectSpace().width, cameraBounds.top);
    hitbox.setOrigin(spawnPosition);

    setLives(lives - 1);

    respawnInvinsibilityTimer.restart();
}

void Player::draw(sf::RenderWindow& window) {

    if(lifeState == DEAD) {

        return;
    }

    gun->draw(window);
    window.draw(player);
}

void Player::setLives(const int &newLives) {

    lives = newLives;
}

const ObjectHitbox& Player::getHitbox() const {

    return hitbox;
}

const glm::vec2 Player::getPosition() const {

    return hitbox.getOrigin();
}

shared_ptr<Gun> &Player::getGun() {

    return gun;
}

int Player::getLives() const {

    return lives;
}

bool Player::checkCanJump() const {

    //if player is falling it means he isn't standing on top of any object because if he was, his velocity would be 0
    //therefore he can't jump if his velocity isn't 0
    return (standingOnSolid || standingOnTile) && (hitboxMovementController.getVelocities().y == 0) && (lifeState == ALIVE);
}

//don't let players jump down while running
bool Player::checkCanJumpDown() const {

    return standingOnPassablePlatform && checkCanJump() && hitboxMovementController.getVelocities().x == 0;
}

bool Player::checkIsJumping() const {

    //player is jumping if he is moving upwards and he isn't able to jump
    return hitboxMovementController.getVelocities().y < 0 && !standingOnSolid;
}

bool Player::checkIsCrouching() const {

    //player is crouching if he is holding down, not moving, and on the ground
    return checkCanJump() && sf::Keyboard::isKeyPressed(controls.down) && hitboxMovementController.getVelocities().x == 0;
}

//check if player can extend his jump by holding the jump button
bool Player::checkExtendJump() const {

    return holdingJump && extraJumpTimer.getElapsedTime() < extraJumpDuration;
}

glm::vec2 Player::calculateGunfireOrigin() const {

    //default facing right so its at the right side of the player
    glm::vec2 gunPosition(player.getGlobalBounds().width, player.getGlobalBounds().height / 2);

    if(direction.isFacingCompletelyVertical && !checkIsCrouching()) {

        gunPosition.x = player.getGlobalBounds().width / 2;

        if(direction.vertical == VerticalDirection::UP) {

            gunPosition.y = 0;

        } else {

            gunPosition.y = player.getGlobalBounds().height;
        }

        return gunPosition;
    }

    if(direction.horizontal == HorizontalDirection::RIGHT) {

        gunPosition.x = player.getGlobalBounds().width;

    } else {

        gunPosition.x = 0;
    }

    if(direction.vertical == VerticalDirection::UP) {

        gunPosition.y = 0;

    } else if(direction.vertical == VerticalDirection::DOWN) {

        gunPosition.y = player.getGlobalBounds().height;

    } else {

        gunPosition.y = player.getGlobalBounds().height / 2;
    }

    if(checkIsCrouching()) {

        gunPosition.y = player.getGlobalBounds().height;
    }

    return gunPosition;
}

CollisionResponse Player::handleTileCollision(TileMap& map, CollisionResponse(*collisionFunction)(std::shared_ptr<Tile>& tile, HitboxMovementController& object)) {

    sf::FloatRect bounding = hitbox.getActiveHitboxWorldSpace();

    //calculate region encompassed by object
    //extedn the region slightly because slope tiles need extra information about object previous position if he leaves a tile
    glm::vec2 regionTopLeft = glm::vec2(bounding.left, bounding.top) - glm::vec2(TILE_SIZE, TILE_SIZE);
    glm::vec2 regionBottomRight = glm::vec2(bounding.left + bounding.width, bounding.top + bounding.height) + glm::vec2(TILE_SIZE, TILE_SIZE);

    vector<shared_ptr<Tile> > tiles = map.getTilesInRegion(regionTopLeft, regionBottomRight);

    CollisionResponse collisionResponse;
    collisionResponse.canFallThroughGround = true;

    for(unsigned i = 0; i < tiles.size(); ++i) {

        CollisionResponse response = collisionFunction(tiles[i], hitboxMovementController);

        if(response.handledVertical) {

            collisionResponse.handledVertical = true;

            //if player ever stands on a solid tile that prevents fall through then don't let him fall through floor
            //this makes it so player can only fall through ground if all checks allow him to fall through ground
            collisionResponse.canFallThroughGround = collisionResponse.canFallThroughGround && response.canFallThroughGround;

        }

        if(response.handledHorizontal) {

            collisionResponse.handledHorizontal = true;
        }
    }

    //if vertical collision was never handled then don't let player call through ground
    collisionResponse.canFallThroughGround = collisionResponse.canFallThroughGround && collisionResponse.handledVertical;

    return collisionResponse;
}

CollisionResponse Player::handleTileCollisionHorizontally(TileMap& map) {

    return handleTileCollision(map, &handleCollisionHorizontal);
}

CollisionResponse Player::handleTileCollisionVertically(TileMap& map) {

    return handleTileCollision(map, &handleCollisionVertical);
}

void Player::determineDirection() {

    if(hitboxMovementController.getVelocities().x < 0) {

        direction.horizontal = HorizontalDirection::LEFT;

    } else if(hitboxMovementController.getVelocities().x > 0) {

        direction.horizontal = HorizontalDirection::RIGHT;
    }

    if(sf::Keyboard::isKeyPressed(controls.up)) {

        direction.vertical = VerticalDirection::UP;

    } else if(sf::Keyboard::isKeyPressed(controls.down)) {

        direction.vertical = VerticalDirection::DOWN;

    } else {

        direction.vertical = VerticalDirection::STRAIGHT;
    }

    if(checkIsCrouching()) {

        direction.vertical = VerticalDirection::STRAIGHT;
        hitbox.setActiveHitbox(1);

    } else {

        hitbox.setActiveHitbox(0);
    }

    direction.isFacingCompletelyVertical = (hitboxMovementController.getVelocities().x == 0 && direction.vertical != VerticalDirection::STRAIGHT);
}

void Player::jump() {

    if(checkCanJump()) {

        extraJumpTimer.restart();
        hitboxMovementController.setVelocities(hitboxMovementController.getVelocities().x, -MOVEMENT_VELOCITY.y);
        stopStandingOnPlatforms();
    }
}

void Player::jumpDown() {

    if(checkCanJumpDown()) {

        stopStandingOnPlatforms();

        //when jumping down just push the player down into the ground since the collision detection code should allow him to pass
        //push him just out of the threshold so collision resolution won't occur
        hitbox.move(glm::vec2(0, fallThroughTopThreshold + 1.f));

        //also set his velocity to go downwards that way it looks a bit smoother
        hitboxMovementController.setVelocities(hitboxMovementController.getVelocities().x, 1.0f);
    }
}

void Player::stopStandingOnPlatforms() {

    standingOnPassablePlatform = false;
    standingOnSolid = false;
    standingOnTile = false;
}

void Player::die() {

    ///later this should stop animations but for now just kill player
    lifeState = DEAD;
}
