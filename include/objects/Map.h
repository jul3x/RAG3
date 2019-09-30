//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_MAP_H
#define RAG3_OBJECTS_MAP_H

#include <list>

#include <objects/Obstacle.h>
#include <objects/AbstractDrawableObject.h>


class Map : public AbstractDrawableObject  {
public:
    Map() = default;

    void loadMap(const std::string &name);

    std::list<Obstacle>& getObstacles();

    void update(float time_elapsed);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::list<Obstacle> obstacles_;
};

#endif // RAG3_OBJECTS_MAP_H