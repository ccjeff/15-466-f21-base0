//
// Created by ccjeff on 2021/9/5.
//

#ifndef INC_15_466_F21_BASE0_BALL_HPP
#define INC_15_466_F21_BASE0_BALL_HPP

#include <vector>
#include <glm/glm.hpp>
#include <memory>
#include "Vertex.hpp"

// reference: https://github.com/ericeschnei/15-466-f20-base0/blob/HEAD/Ball.hpp used with edit

struct Ball {
    Ball(const glm::vec2 &startPos, const glm::vec2 &initSpeed, const glm::u8vec4& color, bool isLeft) : position(std::move(startPos)), speed(std::move(initSpeed)), color(std::move(color)), is_from_left(isLeft){};
    ~Ball();
    void update(float elapsed);
    void draw(std::vector<Vertex> &vertices);

    glm::vec2 position;
    glm::vec2 direction;
    glm::vec2 speed;
    glm::u8vec4 color;
    bool is_from_left;
    bool unmoved = true;
    const float radius = 0.25f;
};


#endif //INC_15_466_F21_BASE0_BALL_HPP
