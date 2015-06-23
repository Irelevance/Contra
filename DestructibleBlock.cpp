#include "DestructibleBlock.h"
#include "CollisionResolution.h"

#include <iostream>

using std::cout;
using std::endl;
using std::shared_ptr;

DestructibleBlock::DestructibleBlock(const glm::vec2 &position) :
    DynamicObject(glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), 1),
    sprite(sf::milliseconds(30))
    {
        setup();
        hitbox.setOrigin(position);
        hitbox.insertHitbox(sf::FloatRect(0, 0, 64, 64));
        hitbox.setActiveHitbox(0);
        sprite.getSprite().setPosition(sf::Vector2f(position.x, position.y));

        updateRendering();
    }

void DestructibleBlock::updateRendering() {

    if(sprite.animate() && sprite.getAnimationState() == DESTRYOING) {

        sprite.setAnimationState(DESTROYED);
        health = 0;
    }

    sprite.getSprite().setPosition(hitbox.getOrigin().x, hitbox.getOrigin().y);
}

bool DestructibleBlock::checkCanGetHit() {

    return sprite.getAnimationState() == SOLID;
}

void DestructibleBlock::getHit(int damage) {

    sprite.setAnimationState(DESTRYOING);
}

void DestructibleBlock::draw(sf::RenderWindow &window) {

    sprite.draw(window);
}

void DestructibleBlock::setup() {

    //load the appropriate texture
    sprite.loadTexture("BrickBreak.png");

    createTextureRects();
}

void DestructibleBlock::createTextureRects() {

    sprite.insertTextureRect(SOLID, sf::IntRect(0, 0, 64, 64));
    sprite.insertTextureRect(DESTROYED, sf::IntRect(0, 0, 0, 0));

    for(int y = 0; y < 3; ++y) {

        for(int x = 0; x < 3; ++x) {

            sprite.insertTextureRect(DESTRYOING, sf::IntRect(x * 64, y * 64, 64, 64));
        }
    }
}
