//
// Created by ccjeff on 2021/9/6.
//

// referenced from : https://github.com/ericeschnei/15-466-f20-base0/blob/HEAD/Paddle.cpp
// used with modification

#include <iostream>
#include "Paddle.hpp"

Paddle::Paddle() {}
Paddle::~Paddle() {};

void Paddle::draw(std::vector<Vertex> &vertices) {
    #define HEX_TO_U8VEC4( HX ) (glm::u8vec4( (HX >> 24) & 0xff, (HX >> 16) & 0xff, (HX >> 8) & 0xff, (HX) & 0xff ))
//    const glm::u8vec4 bg_color = HEX_TO_U8VEC4(0x193b59ff);
    const glm::u8vec4 fg_color = HEX_TO_U8VEC4(0xf2d2b6ff);
//    const glm::u8vec4 shadow_color = HEX_TO_U8VEC4(0xf2ad94ff);
    const std::vector< glm::u8vec4 > trail_colors = {
            HEX_TO_U8VEC4(0xf2ad9488),
            HEX_TO_U8VEC4(0xf2897288),
            HEX_TO_U8VEC4(0xbacac088),
    };
    #undef HEX_TO_U8VEC4
    auto draw_rectangle = [&vertices](glm::vec2 const &center, glm::vec2 const &radius, glm::u8vec4 const &color) {
        //draw rectangle as two CCW-oriented triangles:
        vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x+radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));

        vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x-radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
    };

    draw_rectangle(this->position , paddle_radius, fg_color);
}