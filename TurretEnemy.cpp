#include "TurretEnemy.h"
#include "EntityAnimationStates.h"
#include "GlobalConstants.h"
#include <cmath>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;
using std::make_shared;

TurretEnemy::TurretEnemy(const glm::vec2 &position, const int initialHealth) :
    ShootingEntity(glm::vec2(0, 0), glm::vec2(0, 0), glm::vec2(0, 0), initialHealth),
    hiddenStateTimer(),
    exposedStateTimer(),
    hiddenStateDuration(),
    exposedStateDuration()
    {
        setPosition(position);

        gun = make_shared<Gun>();
    }

void TurretEnemy::updatePhysics(const float& deltaTime, const sf::FloatRect& worldBounds, TileMap& map, const vector<glm::vec2> &targetPositions) {

    //update previous bullets
    gun->updatePhysics(deltaTime, worldBounds, map);

    unsigned frame = sprite.getFrame();

    if(currentState != TurretEnemyEnums::STATE_SHOOTING) {

        //enemy isn't shooting so no need to fire new bullets
        //the gun needs to update though so keep this after the gun physics update
        //when not shooting the animation frame adn the hitbox match up
        //set the hitbox according to the current frame of animation
        hurtbox.setActiveHitbox(frame, currentState);
        return;
    }

    //when shooting there is only one hitbox
    hurtbox.setActiveHitbox(0, currentState);

    //start shooting at the closest player
    unsigned idClosestTarget = getIdOfClosestTarget(targetPositions);

    if(idClosestTarget < 0) {

        return;
    }

    glm::vec2 gunfireOrigin = calculateGunfireOrigin(targetPositions[idClosestTarget]);
    gun->fire(hurtbox.getOrigin(), gunfireOrigin, direction);

    matchHitboxPosition();
}

void TurretEnemy::updateRendering() {

    gun->updateRendering();

    //don't animate when entity is shooting because the shooting frames aren't animated
    //use the sprites animation state instead of the enemies because if the enemy state changes but the sprite doesn't then the sprite won't draw the correct state
    if(sprite.animate() && sprite.getAnimationState() != TurretEnemyEnums::STATE_SHOOTING) {

        //animation finished so if its the transition animation then complete transitions
        //don't restart state timers if the current state isn't one of these
        //because turrets also animate while hiding and if you restart state timers at the end up the hiding animation
        //then the timers will be reset every frame so turret will stay hiding forever
        if(currentState == TurretEnemyEnums::STATE_COMING_OUT_OF_HIDING) {

            setState(TurretEnemyEnums::STATE_SHOOTING);
            restartStateDurationTimers();

        } else if(currentState == TurretEnemyEnums::STATE_GOING_INTO_HIDING) {

            setState(TurretEnemyEnums::STATE_HIDING);
            restartStateDurationTimers();
        }
    }

    //turret only really from hiding or shooting
    //determine if turret needs to start a transition animation
    //and if so which animation
    if(currentState == TurretEnemyEnums::STATE_HIDING && hiddenStateTimer.getElapsedTime() > hiddenStateDuration) {

        setState(TurretEnemyEnums::STATE_COMING_OUT_OF_HIDING);
        restartStateDurationTimers();

    } else if(currentState == TurretEnemyEnums::STATE_SHOOTING && exposedStateTimer.getElapsedTime() > exposedStateDuration) {

        setState(TurretEnemyEnums::STATE_GOING_INTO_HIDING);
        restartStateDurationTimers();

    } else if(currentState == TurretEnemyEnums::STATE_SHOOTING) {

        if(direction.vertical == VerticalDirection::STRAIGHT) {

            if(direction.horizontal == HorizontalDirection::LEFT) {

                sprite.setFrame(TurretEnemyEnums::LEFT);

            }  else {

                sprite.setFrame(TurretEnemyEnums::RIGHT);
            }

        } else if(direction.vertical == VerticalDirection::UP) {

            if(direction.isFacingCompletelyVertical) {

                sprite.setFrame(TurretEnemyEnums::UP);

            } else if(direction.horizontal == HorizontalDirection::LEFT) {

                sprite.setFrame(TurretEnemyEnums::UP_LEFT);

            } else {

                sprite.setFrame(TurretEnemyEnums::UP_RIGHT);
            }

        } else if(direction.vertical == VerticalDirection::DOWN) {

            if(direction.isFacingCompletelyVertical) {

                sprite.setFrame(TurretEnemyEnums::DOWN);

            } else if(direction.horizontal == HorizontalDirection::LEFT) {

                sprite.setFrame(TurretEnemyEnums::DOWN_LEFT);

            } else {

                sprite.setFrame(TurretEnemyEnums::DOWN_RIGHT);
            }
        }

    } else {

        //going into hiding or coming out of hiding so use the correct hitbox every frame
        hurtbox.setActiveHitbox(sprite.getFrame(), currentState);
    }

    matchHitboxPosition();

    glm::vec2 position = hurtbox.getOrigin();
    sprite.getSprite().setPosition(position.x, position.y);
}

bool TurretEnemy::checkCanGetHit() {

    return checkIsAlive() && currentState != TurretEnemyEnums::STATE_HIDING;
}

void TurretEnemy::draw(sf::RenderWindow &window) {

    gun->draw(window);
    sprite.draw(window);
}

void TurretEnemy::load(const PreloadedTurretData &data) {

    loadBase(data);
    loadShootingEntityData(data);
    scale(data.scale, data.scale);

    hiddenStateDuration = data.hiddenStateDuration;
    exposedStateDuration = data.exposedStateDuration;

    setState(TurretEnemyEnums::STATE_COMING_OUT_OF_HIDING);
    restartStateDurationTimers();
}

void TurretEnemy::createHitboxes(const vector<sf::FloatRect> &hitboxes) {

    hurtbox.clearHitboxes();

    for(unsigned i = 0; i < hitboxes.size(); ++i) {

        hurtbox.insertHitbox(hitboxes[i]);
    }
}

unsigned TurretEnemy::getIdOfClosestTarget(const vector<glm::vec2> &targetPositions) const {

    if(targetPositions.size() == 0) {

        return -1;
    }

    unsigned closestId = 0;
    float distanceToClosest = glm::dot(targetPositions[0] - hurtbox.getOrigin(), targetPositions[0] - hurtbox.getOrigin());

    for(unsigned i = 1; i < targetPositions.size(); ++i) {

        float distanceToTarget = glm::dot(targetPositions[i] - hurtbox.getOrigin(), targetPositions[i] - hurtbox.getOrigin());

        if(distanceToTarget < distanceToClosest) {

            distanceToClosest = distanceToTarget;
            closestId = i;
        }
    }

    return closestId;
}

glm::vec2 TurretEnemy::calculateGunfireOrigin(const glm::vec2 &targetPosition) {

    determineDirection(targetPosition);

    return bulletOriginForState[convertToCombinedAxis(direction)];
}

void TurretEnemy::determineDirection(const glm::vec2 &targetPosition) {

    //determine angle to target
    float x = targetPosition.x - hurtbox.getOrigin().x;
    float y = targetPosition.y - hurtbox.getOrigin().y;

    float angleRadians = atan2(y, x);

    //angle goes positive in clockwise direction when tis hould be counter clockwise so negate it
    float angleDegrees = angleRadians * RAD_TO_DEG_RATIO * -1;

    direction.isFacingCompletelyVertical = false;

    if(angleDegrees <= 60 && angleDegrees >= -60) {

        direction.horizontal = HorizontalDirection::RIGHT;

    } else if(angleDegrees >= 120 || angleDegrees <= -120) {

        direction.horizontal = HorizontalDirection::LEFT;

    } else {

        direction.isFacingCompletelyVertical = true;
    }

    if(angleDegrees <= 150 && angleDegrees >= 30) {

        direction.vertical = VerticalDirection::UP;

    } else if(angleDegrees <= -30 && angleDegrees >= -150) {

        direction.vertical = VerticalDirection::DOWN;

    } else {

        direction.vertical = VerticalDirection::STRAIGHT;
    }
}

void TurretEnemy::restartStateDurationTimers() {

    hiddenStateTimer.restart();
    exposedStateTimer.restart();
}
