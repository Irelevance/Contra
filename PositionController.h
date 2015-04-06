#ifndef POSITIONCONTROLLER_H_INCLUDED
#define POSITIONCONTROLLER_H_INCLUDED

#include "SFML/Graphics.hpp"
#include "glm/glm.hpp"
#include "ObjectSpaceManager.h"

//objects that other classes can use to keep track of their positions and velocities
//this object is used to that all other objects have a reusable interface when colliding with tiles
///VELOCITY DATA GIVEN TO THIS OBJECT NEEDS TO BE IN METERS PER SECOND
class PositionController {

    public:

        //specify the size of the bounding box
        //and the max velocity the object can reach on each axis
        PositionController(const glm::vec2& sizeWorldSpace, const glm::vec2& gravityWorldSpace, const glm::vec2& terminalVelocitiesWorldSpace, const glm::vec2& objXAxis, const glm::vec2& objYAxis);

        void setVelocities(const glm::vec2& velocityObjectSpace);
        void setVelocities(const float& xVelocityObjectSpace, const float& yVelocityObjectSpace);
        void setPositionWorldSpace(const glm::vec2& position);
        void setPositionObjectSpace(const glm::vec2& position);

        const glm::vec2 getVelocitiesObjectSpace() const;
        const glm::vec2 getVelocitiesWorldSpace() const;
        const sf::FloatRect getBoundingBoxWorldSpace() const;
        const sf::FloatRect getBoundingBoxObjectSpace() const;
        const float getLastDelta() const;
        const ObjectSpaceManager& getObjectSpace() const;

        //update velocities applies gravity to the velocities
        void updateVelocities(const float& delta);

        //returns true if object hit the world bounds and snapped to the edge
        //useful if you want to set velocity to 0 or allow jumping when on top of a surface
        bool moveAlongXAxis(const float& delta, const sf::FloatRect& worldBounds);
        bool moveAlongYAxis(const float& delta, const sf::FloatRect& worldBounds);

    private:

        void capVelocities();

        ObjectSpaceManager object;

        //units are all in meters/second
        //gravity only affects y-axis
        const glm::vec2 gravityObjectSpace;
        glm::vec2 terminalVelocitiesObjectSpace;

        glm::vec2 velocitiesObjectSpace;

        //keep track of the last delta time because tile collision requires dt in order to calculate previous position
        float previousDeltaTime;
};

#endif // POSITIONCONTROLLER_H_INCLUDED
