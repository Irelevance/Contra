#ifndef ENTITYBASE_H_INCLUDED
#define ENTITYBASE_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include "TileMap.h"
#include "Tile.h"
#include "TileCollisionHandling.h"
#include "ObjectHitbox.h"
#include "CollisionResponse.h"
#include "AnimatedSprite.h"
#include "HitboxMovementController.h"

#include "glm/glm.hpp"
#include <memory>
#include <vector>

class GameWorld;

class EntityBase {

    public:

        EntityBase(const glm::vec2 &gravity, const glm::vec2 &movementVelocity, const glm::vec2 &terminalVelocity, const unsigned &initialHealth);

        virtual ~EntityBase() {

        }

        virtual void updatePhysics(const float& deltaTime, const sf::FloatRect& worldBounds, TileMap& map) = 0;
        virtual void draw(sf::RenderWindow &window);
        virtual bool checkIsAlive();
        virtual bool checkCanGetHit();
        virtual void getHit(int damage = 1);
        void scale(const float &xFactor, const float& yFactor) {

            //scale all hitboxes and sprite
            hurtbox.scale(xFactor, yFactor);
            hitbox.scale(xFactor, yFactor);
            sprite.getSprite().scale(xFactor, yFactor);

            this->scaleComponents(xFactor, yFactor);
        }

        //this one should be called
        virtual void respondToCollision(const CollisionResponse &collisionResponse) {

            matchHitboxPosition();
        }

        ObjectHitbox& getHurtbox();
        ObjectHitbox& getHitbox();
        HitboxMovementController& getMovementController();
        const glm::vec2 getPosition() const;

        void setHealth(const int &newVal);

    protected:

        //get all tiles that are covered by the surface area of this object
        //add padding to the area of this object incase you need extra tiles that are around the object but aren't touching the object
        //you might need to use padding for things like handling collision with slopes, since slopes require surounding tiles, not just colliding tiles
        std::vector<std::shared_ptr<Tile> > getSurroundingTiles(const TileMap &map, const glm::vec2 &areaPadding);

        //leave this virtual since different objects have different collision handling
        virtual CollisionResponse handleTileCollision(TileMap &map, CollisionResponse(*collisionFunction)(std::shared_ptr<Tile>& tile, HitboxMovementController& object));
        CollisionResponse handleTileCollisionHorizontally(TileMap& map);
        CollisionResponse handleTileCollisionVertically(TileMap& map);

        //set the position of all components to the hitbox's position
        void matchHitboxPosition();

        //derived classes can override this function to scale other paramters
        virtual void scaleComponents(const float &xFactor, const float &yFactor) {

        }

        template<class Data>
        void loadBase(const Data &data);

        void setState(const unsigned &state);
        void setPosition(const glm::vec2 &position);

        //hitbox collides with environment and is generally static
        ObjectHitbox hurtbox;

        //hurt box collides with attacks and stuff and can vary every frame
        ObjectHitbox hitbox;

         //all entities have a default hitbox state, which is the hitbox they use for almost all animation states when colliding with the environment
         //this is defined so that when an object is first born it has a state to use
        unsigned defaultHitboxState;

        HitboxMovementController hitboxMovementController;
        glm::vec2 MOVEMENT_VELOCITY; //measured in meters per second
        unsigned health;

        sf::RectangleShape entity;

        unsigned currentState;
        AnimatedSprite sprite;
};

template<class Data>
void EntityBase::loadBase(const Data &data) {

    hurtbox.clearHitboxes();
    sprite.clearAnimation();

    health = data.health;

    sprite.getSprite().setScale(data.scale, data.scale);
    sprite.loadTexture(data.textureFilename);
    sprite.setNextFrameTime(data.animationNextFrameTime);

    defaultHitboxState = data.defaultHitboxState;

    for(auto it = data.animationTextureRects.begin(); it != data.animationTextureRects.end(); ++it) {

        for(auto vt = it->second.begin(); vt != it->second.end(); ++vt) {

            sprite.insertTextureRect(it->first, *vt);
        }
    }

    for(auto it = data.hurtboxes.begin(); it != data.hurtboxes.end(); ++it) {

        for(auto vt = it->second.begin(); vt != it->second.end(); ++vt) {

            sf::FloatRect bounds(vt->left, vt->top, vt->width, vt->height);

            hurtbox.insertHitbox(bounds, it->first);
        }
    }

    for(auto it = data.hitboxes.begin(); it != data.hitboxes.end(); ++it) {

        for(auto vt = it->second.begin(); vt != it->second.end(); ++vt) {

            sf::FloatRect bounds(vt->left, vt->top, vt->width, vt->height);

            hitbox.insertHitbox(bounds, it->first);
        }
    }

    hitbox.setActiveHitbox(0, defaultHitboxState);
}

#endif // ENTITYBASE_H_INCLUDED
