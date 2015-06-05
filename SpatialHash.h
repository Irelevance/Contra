#ifndef SPATIALHASH_H_INCLUDED
#define SPATIALHASH_H_INCLUDED

#include "SpatialHashEntry.h"
#include "glm/glm.hpp"
#include <vector>
#include <unordered_map>
#include <memory>
#include <boost/functional/hash.hpp>


//the key-value pair for spatial hash should be <ivec2, map<unsigned, hashentry> >
//this way when you get the grid position of an object you can use the object's id to insert/remove it from the grid without using a linear search
//for now use a vector because i think using a map wouldn't give much benefits

typedef std::vector<std::shared_ptr<SpatialHashEntry> > EntryContainer;

struct Hasher {

    size_t operator()(const glm::ivec2 &point) const {

        size_t seed = 0;
        boost::hash_combine(seed, point.x);
        boost::hash_combine(seed, point.y);

        return seed;
    }
};

class SpatialHash {

    public:

        SpatialHash(const unsigned &hashCellWidth, const unsigned &hashCellHeight);

        //if its known that an objects arealdy been inserted into the hash its better to call updatePosition
        //instead of using insert
        //insert assumes object isn't inside the grid
        void insert(std::shared_ptr<SpatialHashEntry> &entry);
        void remove(std::shared_ptr<SpatialHashEntry> &entry);
        void updateLocation(std::shared_ptr<SpatialHashEntry> &entry);

        EntryContainer getSurroundingEntites(const sf::FloatRect boundingBox);

    private:

        //insert the entry from the given grid bounds
        void insert(std::shared_ptr<SpatialHashEntry> &entry, const sf::IntRect &gridBounds);

        //remove an entry from the given grid bounds
        void remove(std::shared_ptr<SpatialHashEntry> &entry, const sf::IntRect &gridBounds);
        sf::IntRect calculateGridBounds(const sf::FloatRect &entryBoundingBox);

        std::unordered_map<glm::ivec2, EntryContainer, Hasher> hash;

        unsigned cellWidth;
        unsigned cellHeight;
};

#endif // SPATIALHASH_H_INCLUDED
