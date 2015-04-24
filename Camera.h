#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include <vector>
#include <glm/glm.hpp>

//camera will track the position that is directly between all of the given target points
//camera will translate to ensure the center of the target points is at the center of the camera
//and camera will zoom in and out (up to a maximum limit) to ensure that the points aren't too far apart
//all camera transitions should be smooth, so it will interpolate from its pevious zoom/position to its target
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

        sf::FloatRect getCameraBounds() const;

    private:

        void updateInterpolationParameters(const float &delta);
        void finishAllInterpolation();
        void finishInterpolatingPosition();
        void finishInterpolatingSize();

        glm::vec2 calculateSize(const std::vector<glm::vec2> &targets);
        glm::vec2 calculateMaxSize();
        glm::vec2 calculateCenterPosition(const std::vector<glm::vec2> &targets);

        void calculateView(const sf::FloatRect &worldBounds);

        sf::View view;
        glm::vec2 defaultSize;

        //zoom and positions interpolate from its initial and final points
        glm::vec2 initialSize;
        glm::vec2 finalSize;
        glm::vec2 initialPosition;
        glm::vec2 finalPosition;

        //timeElapsed is the time since the interpolation began
        //and time is how much time needs to elapse before position and zoom are interpolated to their final value
        ///all values are in seconds
        float positionInterpolationTimeElapsed;
        float positionInterpolationTime;

        float sizeInterpolationTimeElapsed;
        float sizeInterpolationTime;
};

#endif // CAMERA_H_INCLUDED
