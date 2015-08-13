#ifndef OMNIDIRECTIONALTURRET_H_INCLUDED
#define OMNIDIRECTIONALTURRET_H_INCLUDED

#include "ShootingEntity.h"
#include "PreloadedData.h"
#include "glm/glm.hpp"

class OmniDirectionalTurret : public ShootingEntity {

    public:

        OmniDirectionalTurret(const glm::vec2 &position, const int initialHealth = 8);

        virtual ~OmniDirectionalTurret() {

        }

        virtual void updatePhysics(const float& deltaTime, const sf::FloatRect& worldBounds, TileMap& map);
        void updateRendering();

        virtual bool checkCanGetHit();
        virtual void draw(sf::RenderWindow &window);

        void load(PreloadedOmniDirectionalTurretData &data);

    private:

        virtual CollisionResponse handleTileCollision(TileMap &map, CollisionResponse(*collisionFunction)(std::shared_ptr<Tile>& tile, HitboxMovementController& object)) {

            //empty for now because this turret doesn't move so it doesn't collide with anything
            return CollisionResponse();
        }

        bool checkCanShoot();
        void beginShootingAnimation();
        void shoot();

        sf::Clock hiddenStateTimer;
        sf::Clock exposedStateTimer;
        sf::Clock shootingTimer;

        sf::Time hiddenStateDuration;
        sf::Time exposedStateDuration;
        sf::Time shootingDelay;

        unsigned STATE_HIDING;
        unsigned STATE_COMING_OUT_OF_HIDING;
        unsigned STATE_GOING_INTO_HIDING;
        unsigned STATE_EXPOSED;
        unsigned STATE_SHOOTING;
};

#endif // OMNIDIRECTIONALTURRET_H_INCLUDED
