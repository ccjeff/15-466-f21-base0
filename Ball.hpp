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
    Ball(const glm::vec2 &startPos, float initSpeed) : position(std::move(startPos)), speed(initSpeed) {};
    ~Ball();
    void update(float elapsed);
    void draw(std::vector<Vertex> &vertices);

    glm::vec2 position;
    float speed;
    #define HEX_TO_U8VEC4( HX ) (glm::u8vec4( (HX >> 24) & 0xff, (HX >> 16) & 0xff, (HX >> 8) & 0xff, (HX) & 0xff ))
    glm::u8vec4 color = HEX_TO_U8VEC4(0xf2d2b6ff);
    #undef HEX_TO_U8VEC4
    const float radius = 0.25f;
};


#endif //INC_15_466_F21_BASE0_BALL_HPP
