#ifndef GUN_H_INCLUDED
#define GUN_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "ObjectSpaceManager.h"
#include "Direction.h"
#include <vector>
#include <memory>
#include "glm/glm.hpp"

class Bullet;
class TileMap;

//gun object position is relative to top left corner of its owner (heirachal model)
//the gun's initial firing direction is also specified by the owner
//these traits of the gun are all in the space of the object that owns the gun
//thus when the gun is fired it requires transformations from owner object space to world space
class Gun {

    public:

        Gun(const ObjectSpaceManager &userObjectSpace);

        void fire(const glm::vec2 &userPositionWorldSpace, const glm::vec2 &bulletOriginUserSpace, const Direction &fireDirection);
        void update(const float &delta, const sf::FloatRect &worldBounds, TileMap& map);
        void draw(sf::RenderWindow& window);

    private:

        bool checkCanFire() {

            return timeSinceLastFired > fireDelay;
        }

        void createBullet(const glm::vec2 &positionWorldSpace, const glm::vec2 &directionWorldSpace);

        sf::Time timeSinceLastFired;
        sf::Time fireDelay;

        //object space of the user of the gun, used to transform gun's directions
        const ObjectSpaceManager& userObjectSpace;

        std::vector<std::shared_ptr<Bullet> > bullets;
};

#endif // GUN_H_INCLUDED
