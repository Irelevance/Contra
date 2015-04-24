#include "Camera.h"
#include "glm/gtx/compatibility.hpp"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;

Camera::Camera(sf::RenderWindow &window) :
    view(),
    defaultSize(1024, 768),
    initialSize(1024, 768),
    finalSize(1024, 768),
    initialPosition(0, 0),
    finalPosition(0, 0),
    positionInterpolationTimeElapsed(0),
    positionInterpolationTime(0.001),
    sizeInterpolationTimeElapsed(0),
    sizeInterpolationTime(0.001)
    {
        setupDefaultProperties(window);
    }

void Camera::setupDefaultProperties(sf::RenderWindow &window) {

    //default view size is the size of the window
    sf::Vector2u windowSize = window.getSize();

    defaultSize = glm::vec2(windowSize.x, windowSize.y);
    resetZoom();
}

void Camera::resetZoom() {

    initialSize = defaultSize;
    finalSize = defaultSize;

    sizeInterpolationTimeElapsed = 0;
}

void Camera::calculateProperties(const vector<glm::vec2> &targets) {

    if(targets.size() == 0) {

        finishAllInterpolation();
        return;
    }

    glm::vec2 centerPos = calculateCenterPosition(targets);
    finalPosition = centerPos;

    glm::vec2 size = calculateSize(targets);
    finalSize = size;
}

void Camera::update(const float &delta, const sf::FloatRect &worldBounds) {

    updateInterpolationParameters(delta);
    calculateView(worldBounds);
}

void Camera::applyCamera(sf::RenderWindow &window) const {

    window.setView(view);
}

sf::FloatRect Camera::getCameraBounds() const {

    sf::Vector2f size = view.getSize();
    sf::Vector2f center = view.getCenter();

    sf::Vector2f topLeft = center - size / 2.f;

    return sf::FloatRect(topLeft.x, topLeft.y, size.x, size.y);
}

void Camera::updateInterpolationParameters(const float &delta) {

    positionInterpolationTimeElapsed += delta;

    if(positionInterpolationTimeElapsed > positionInterpolationTime) {

        finishInterpolatingPosition();
    }

    sizeInterpolationTimeElapsed += delta;

    if(sizeInterpolationTimeElapsed > sizeInterpolationTime) {

        finishInterpolatingSize();
    }
}

void Camera::finishInterpolatingPosition() {

    initialPosition = finalPosition;
    positionInterpolationTimeElapsed = 0;
}

void Camera::finishInterpolatingSize() {

    initialSize = finalSize;
    sizeInterpolationTimeElapsed = 0;
}

void Camera::finishAllInterpolation() {

    finishInterpolatingPosition();
    finishInterpolatingSize();
}

glm::vec2 Camera::calculateSize(const vector<glm::vec2> &targets) {

    //determine how far apart all targets are
    //find the maximum differece on either axis
    //use the maximum difference to determine size of viewing region
    glm::vec2 minimumPosition(99999, 99999);
    glm::vec2 maximumPosition(-99999, -99999);

    for(unsigned  i = 0; i < targets.size(); ++i) {

        minimumPosition.x = glm::min(minimumPosition.x, targets[i].x);
        minimumPosition.y = glm::min(minimumPosition.y, targets[i].y);
        maximumPosition.x = glm::max(maximumPosition.x, targets[i].x);
        maximumPosition.y = glm::max(maximumPosition.y, targets[i].y);
    }

    float horizontalDifference = glm::abs(minimumPosition.x - maximumPosition.x);
    float verticalDifference = glm::abs(minimumPosition.y - maximumPosition.y);

    //the camera needs to zoom uniformly on both axis
    //so calculate on which axis you need to zoom out the furthest
    float horizontalRatio = horizontalDifference / defaultSize.x;
    float verticalRatio = verticalDifference / defaultSize.y;

    //use the larger ratio to scale the default size
    //however scale the size by a bit more than the actual ratio
    //this way the targets aren't at the edge of the camera
    float defaultSizeScale = 1.35;
    defaultSizeScale *= glm::max(horizontalRatio, verticalRatio);

    glm::vec2 size = defaultSize * defaultSizeScale;

    //make sure the size is within the bounds
    //only need to check on one axis since the aspect ratio is maintained
    if(size.x < defaultSize.x) {

        size = defaultSize;
    }

    if(size.x > calculateMaxSize().x) {

        size = calculateMaxSize();
    }

    return size;
}

glm::vec2 Camera::calculateMaxSize() {

    float maxScale = 2.2;

    return defaultSize * maxScale;
}

glm::vec2 Camera::calculateCenterPosition(const vector<glm::vec2> &targets) {

    glm::vec2 averagePosition(0, 0);

    for(unsigned i = 0; i < targets.size(); ++i) {

        averagePosition += targets[i];
    }

    averagePosition /= targets.size();
    return averagePosition;
}

void Camera::calculateView(const sf::FloatRect &worldBounds) {

    //set view size and position and make sure its within bounds
    glm::vec2 currentPosition = glm::lerp(initialPosition, finalPosition, positionInterpolationTimeElapsed / positionInterpolationTime);
    glm::vec2 currentSize = glm::lerp(initialSize, finalSize, sizeInterpolationTimeElapsed / sizeInterpolationTime);

    glm::vec2 centerPosition = currentPosition;

    centerPosition.x = glm::clamp(centerPosition.x, worldBounds.left + currentSize.x / 2, worldBounds.left + worldBounds.width - currentSize.x / 2);
    centerPosition.y = glm::clamp(centerPosition.y, worldBounds.top + currentSize.y / 2, worldBounds.top + worldBounds.height - currentSize.y / 2);

    view.setSize(currentSize.x, currentSize.y);
    view.setCenter(centerPosition.x, centerPosition.y);
}
