#ifndef OBJECTSPACEMANAGER_H_INCLUDED
#define OBJECTSPACEMANAGER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "glm/glm.hpp"

//keeps track of an objects position, and size in object space
//and manages transformations from world space to object space and vice versa
class ObjectSpaceManager{

    public:

        //object's axis in world space, used to transform between spaces
        ObjectSpaceManager(const glm::vec2& xAxis, const glm::vec2& yAxis, const glm::vec2& sizeWorldSpace);

        glm::vec2 convertToObjectSpace(const glm::vec2& worldSpacePosition) const;
        sf::FloatRect convertToObjectSpace(const sf::FloatRect& rect) const;

        //assumes the given coordinate is in this object's coordinate space
        glm::vec2 convertToWorldSpace(const glm::vec2& objectSpacePosition) const;

        void setPositionWorldSpace(const glm::vec2& worldPosition);
        void setPositionObjectSpace(const glm::vec2& objectSpacePosition);
        void move(const glm::vec2& objectSpaceDisplacement);

        glm::vec2 getPositionWorldSpace() const;
        glm::vec2 getPositionObjectSpace() const;
        glm::vec2 getSizeObjectSpace() const;
        glm::vec2 getSizeWorldSpace() const;
        sf::FloatRect getBoundingBoxWorldSpace() const;
        sf::FloatRect getBoundingBoxObjectSpace() const;
        const sf::Transform& getTransform() const;

    private:

        glm::mat2 objectToWorldSpace;
        glm::mat2 worldToObjectSpace;
        glm::vec2 positionInObjectSpace;
        glm::vec2 sizeInObjectSpace;
};

#endif // OBJECTSPACEMANAGER_H_INCLUDED
