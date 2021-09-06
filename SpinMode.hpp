//
// Created by ccjeff on 2021/9/5.
//

#ifndef INC_15_466_F21_BASE0_SPINMODE_HPP
#define INC_15_466_F21_BASE0_SPINMODE_HPP

#include "ColorTextureProgram.hpp"

#include "Mode.hpp"
#include "GL.hpp"
//#include "Paddle.hpp"
#include "Vertex.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>


struct SpinMode : Mode {
    SpinMode();
    virtual ~SpinMode();

    //functions called by main loop:
    virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
    virtual void update(float elapsed) override;
    virtual void draw(glm::uvec2 const &drawable_size) override;

    //----- game state -----
    glm::vec2 court_radius = glm::vec2(7.0f, 5.0f);
    glm::vec2 paddle_radius = glm::vec2(0.2f, 1.0f);
    glm::vec2 ball_radius = glm::vec2(0.2f, 0.2f);


    float center_radius = 1.0f;
    float inner_radius = 0.75f;
//    float rotateSpeed = 1.0f;
//    const float speedIncrement = 0.02f; // amount to increase speed over time

    int left_force = 5;
    int right_force = 5;

//    float ai_offset = 0.0f;
//    float ai_offset_update = 0.0f;

    std::vector<Ball *> balls; // init will be 2 balls on the paddles
    bool moving_left_paddle = true;
    std::shared_ptr<Paddle> left_paddle;
    std::shared_ptr<Paddle> right_paddle;
    // --- opengl assets / helpers
    //Shader program that draws transformed, vertices tinted with vertex colors:
    ColorTextureProgram color_texture_program;

    //Buffer used to hold vertex data during drawing:
    GLuint vertex_buffer = 0;

    //Vertex Array Object that maps buffer locations to color_texture_program attribute locations:
    GLuint vertex_buffer_for_color_texture_program = 0;

    //Solid white texture:
    GLuint white_tex = 0;

    //matrix that maps from clip coordinates to court-space coordinates:
    glm::mat3x2 clip_to_court = glm::mat3x2(1.0f);
    // computed in draw() as the inverse of OBJECT_TO_CLIP
    // (stored here so that the mouse handling code can use it to position the paddle)
};

#endif //INC_15_466_F21_BASE0_SPINMODE_HPP
