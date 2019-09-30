//
// Created by jprolejko on 06.03.19.
//

#ifndef RAG3_PHYSICS_PHYSICS_H
#define RAG3_PHYSICS_PHYSICS_H


class Physics {

public:
    explicit Physics();

    Physics(const Physics&) = delete;
    Physics& operator=(const Physics&) = delete;

    void update(float time_elapsed);

private:

};


#endif //RAG3_PHYSICS_PHYSICS_H
