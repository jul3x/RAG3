//
// Created by jprolejko on 30.09.19.
//


#ifndef RAG3_OBJECTS_MAP_H
#define RAG3_OBJECTS_MAP_H

#include <list>

#include <objects/Obstacle.h>
#include <objects/Decoration.h>
#include <objects/AbstractDrawableObject.h>


class Map : public AbstractDrawableObject  {
public:
    Map() = default;

    void loadMap(const std::string &name);

    std::list<Obstacle>& getObstacles();
    std::list<Obstacle*>& getVisibleObstacles();
    virtual void setVisibility(const sf::View &view);

    void update(float time_elapsed);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::list<Obstacle> obstacles_;
    std::list<Obstacle*> visible_obstacles_;

    std::list<Decoration> decorations_;
};

#endif // RAG3_OBJECTS_MAP_H