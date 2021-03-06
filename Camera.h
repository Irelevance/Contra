#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include <vector>
#include <glm/glm.hpp>

//camera will track the position that is directly between all of the given target points
//camera will translate to ensure the center of the target points is at the center of the camera
//and camera will zoom in and out (up to a maximum limit) to ensure that the points aren't too far apart
//all camera transitions should be smooth, so it will transition from one zoom and position to another
//however the camera should move fairly quickly that way if the points are moving the camera doesn't lag behind
class Camera {

    public:

        Camera(sf::RenderWindow &window);

        void setupDefaultProperties(sf::RenderWindow &window);
        void resetZoom();

        //determines new target position and zoom, only resets if the position or zoom has changed
        //otherwise the camera wouldn't smoothly interpolate
        void calculateProperties(const std::vector<glm::vec2> &targets);

        //handles interpolation and ensures camera isn't out of the world bounds
        void update(const float &delta, const sf::FloatRect &worldBounds);
        void applyCamera(sf::RenderWindow &window) const;

        glm::vec2 getDefaultSize() const;
        sf::FloatRect getCameraBounds() const;
        glm::vec2 getCurrentPosition() const;
        sf::Vector2f getViewCenter() const;
        sf::Vector2f getViewTopLeft() const;

        //manually set the camera's target size and positions in order to do some kind of camera transition effect
        void setTargetSize(const glm::vec2 &target);
        void setTargetPosition(const glm::vec2 &target);

    private:

        void handleTransitions(const float &delta);
        void snapToTargetPosition();
        void snaptToTargetSize();
        void finishAllTransitions();

        bool checkShouldSnapProperty(glm::vec2 currentValue, glm::vec2 targetValue) const;
        bool checkShouldSnapPosition() const;
        bool checkShouldSnapSize() const;

        //velocity is in pixel/sec
        //sizeTransitionRate is in pixel/sec
        glm::vec2 calculateVelocity() const;
        glm::vec2 calculateSizeTransitionRate() const;

        glm::vec2 calculateSize(const std::vector<glm::vec2> &targets);
        glm::vec2 calculateMaxSize();
        glm::vec2 calculateCenterPosition(const std::vector<glm::vec2> &targets);

        void calculateView(const sf::FloatRect &worldBounds);

        sf::View view;
        glm::vec2 defaultSize;

        //zoom and position will move towards their target at a constant rate
        glm::vec2 currentSize;
        glm::vec2 targetSize;
        glm::vec2 currentPosition;
        glm::vec2 targetPosition;

        //if the size or position of the camera is within this threshold it will automatially snap to its target
        //in pixels
        float snapThreshold;
};

#endif // CAMERA_H_INCLUDED
