#include "Direction.h"

glm::vec2 getDirectionVector(const Direction& direction) {

    if(direction.isFacingCompletelyVertical && direction.vertical != VerticalDirection::STRAIGHT) {

        if(direction.vertical == VerticalDirection::UP) {

            return glm::vec2(0, -1);

        } else if(direction.vertical == VerticalDirection::DOWN) {

            return glm::vec2(0, 1);
        }
    }

    if(direction.horizontal == HorizontalDirection::LEFT) {

        if(direction.vertical == VerticalDirection::STRAIGHT) {

            return glm::vec2(-1, 0);

        } else if(direction.vertical == VerticalDirection::UP) {

            return glm::vec2(-0.707, -0.707);

        } else if(direction.vertical == VerticalDirection::DOWN) {

            return glm::vec2(-0.707, 0.707);
        }

    } else if(direction.horizontal == HorizontalDirection::RIGHT) {

        if(direction.vertical == VerticalDirection::STRAIGHT) {

            return glm::vec2(1, 0);

        } else if(direction.vertical == VerticalDirection::UP) {

            return glm::vec2(0.707, -0.707);

        } else if(direction.vertical == VerticalDirection::DOWN) {

            return glm::vec2(0.707, 0.707);
        }
    }

    return glm::vec2(0, 0);
}
