//
// Created by ccjeff on 2021/9/5.
// reference: https://github.com/ericeschnei/15-466-f20-base0/blob/HEAD/Ball.cpp used with edit
//

#include <cmath>
#include "Ball.hpp"
#define BALL_NUM_TRIANGLES 24

Ball::~Ball() {};

void Ball::draw(std::vector<Vertex> &vertices) {

    auto add_triangle = [this, &vertices](
            const glm::vec2 &p1, const glm::vec2 &p2
    ) {
        // define CCW tri
        vertices.emplace_back(glm::vec3(p1, 0.0f), this->color, glm::vec2(0.0f, 0.0f));
        vertices.emplace_back(glm::vec3(p2, 0.0f), this->color, glm::vec2(0.0f, 0.0f));
        vertices.emplace_back(glm::vec3(this->position, 0.0f), this->color, glm::vec2(0.0f, 0.0f));
    };

    // draw a circle
    glm::vec2 last_pos;
    for (int i = 0; i <= BALL_NUM_TRIANGLES; i++) {
        float angle = ((float) i) / BALL_NUM_TRIANGLES * 6.28319f;
        glm::vec2 pos = glm::vec2(
                this->radius * glm::cos(angle) + this->position.x,
                this->radius * glm::sin(angle) + this->position.y
        );

        if (i > 0) {
            add_triangle(last_pos, pos);
        }
        last_pos = pos;
    }

}

void Ball::update(float elapsed) {
    if (is_from_left) {
//        this->position.x += elapsed * this->speed;
        this->position.x += std::abs(direction.x) * this->speed.x * 0.8;
//        this->position.y += elapsed * this->speed;
        this->position.y += direction.y * this->speed.y * 0.8;
    } else {
//        this->position.x -= elapsed * this->speed;
        this->position.x -= (std::abs(direction.x)) * this->speed.x * 0.8;
//        this->position.y += elapsed * this->speed;
        this->position.y += (direction.y) * this->speed.y * 0.8;
    }

    this->speed *= 0.975; // air fraction
}