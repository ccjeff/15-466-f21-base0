//
// Created by ccjeff on 2021/9/6.
//

#ifndef INC_15_466_F21_BASE0_PADDLE_HPP
#define INC_15_466_F21_BASE0_PADDLE_HPP
#include "GL.hpp"
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.hpp"
#include "Ball.hpp"
//#include "SpinMode.hpp"
// reference from https://github.com/ericeschnei/15-466-f20-base0/blob/HEAD/Paddle.hpp
// referenced for structure, used with modification

struct Paddle {
    Paddle();
    ~Paddle();
    void draw(std::vector<Vertex> &vertices);

    // paddle color settings
    const glm::u8vec4 color = glm::u8vec4(0x37, 0x71, 0x8E, 0xFF);
    const glm::u8vec4 line_color = glm::u8vec4(0x8E, 0xE3, 0xEF, 0xFF);
    const glm::vec2   paddle_radius       = glm::vec2(0.2f, 1.0f);
    glm::vec2   mouse_pos;
    // paddle identifier, left paddle will move first
    glm::vec2   position            = glm::vec2(0.0f, 0.0f);
};


#endif //INC_15_466_F21_BASE0_PADDLE_HPP
