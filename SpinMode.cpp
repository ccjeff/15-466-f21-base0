//
// Created by ccjeff on 2021/9/5.
//

#include "SpinMode.hpp"
#include "Mode.hpp"
//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <random>
#include <iostream>

SpinMode::SpinMode() {
    //----- allocate OpenGL resources -----
    { //vertex buffer:
        glGenBuffers(1, &vertex_buffer);
        //for now, buffer will be un-filled.

        GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
    }
    { //vertex array mapping buffer for color_texture_program:
        //ask OpenGL to fill vertex_buffer_for_color_texture_program with the name of an unused vertex array object:
        glGenVertexArrays(1, &vertex_buffer_for_color_texture_program);

        //set vertex_buffer_for_color_texture_program as the current vertex array object:
        glBindVertexArray(vertex_buffer_for_color_texture_program);

        //set vertex_buffer as the source of glVertexAttribPointer() commands:
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

        //set up the vertex array object to describe arrays of PongMode::Vertex:
        glVertexAttribPointer(
                color_texture_program.Position_vec4, //attribute
                3, //size
                GL_FLOAT, //type
                GL_FALSE, //normalized
                sizeof(Vertex), //stride
                (GLbyte *)0 + 0 //offset
        );
        glEnableVertexAttribArray(color_texture_program.Position_vec4);
        //[Note that it is okay to bind a vec3 input to a vec4 attribute -- the w component will be filled with 1.0 automatically]

        glVertexAttribPointer(
                color_texture_program.Color_vec4, //attribute
                4, //size
                GL_UNSIGNED_BYTE, //type
                GL_TRUE, //normalized
                sizeof(Vertex), //stride
                (GLbyte *)0 + 4*3 //offset
        );
        glEnableVertexAttribArray(color_texture_program.Color_vec4);

        glVertexAttribPointer(
                color_texture_program.TexCoord_vec2, //attribute
                2, //size
                GL_FLOAT, //type
                GL_FALSE, //normalized
                sizeof(Vertex), //stride
                (GLbyte *)0 + 4*3 + 4*1 //offset
        );
        glEnableVertexAttribArray(color_texture_program.TexCoord_vec2);

        //done referring to vertex_buffer, so unbind it:
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        //done setting up vertex array object, so unbind it:
        glBindVertexArray(0);

        GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
    }
    { //solid white texture:
        //ask OpenGL to fill white_tex with the name of an unused texture object:
        glGenTextures(1, &white_tex);

        //bind that texture object as a GL_TEXTURE_2D-type texture:
        glBindTexture(GL_TEXTURE_2D, white_tex);

        //upload a 1x1 image of solid white to the texture:
        glm::uvec2 size = glm::uvec2(1,1);
        std::vector< glm::u8vec4 > data(size.x*size.y, glm::u8vec4(0xff, 0xff, 0xff, 0xff));
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

        //set filtering and wrapping parameters:
        //(it's a bit silly to mipmap a 1x1 texture, but I'm doing it because you may want to use this code to load different sizes of texture)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //since texture uses a mipmap and we haven't uploaded one, instruct opengl to make one for us:
        glGenerateMipmap(GL_TEXTURE_2D);

        //Okay, texture uploaded, can unbind it:
        glBindTexture(GL_TEXTURE_2D, 0);

        GL_ERRORS(); //PARANOIA: print out any OpenGL errors that may have happened
    }
    // when init set the left & right position for both paddles
    left_paddle = std::make_shared<Paddle> ();
    right_paddle = std::make_shared<Paddle> ();
    this->left_paddle->position = glm::vec2(-court_radius.x + 0.5f, 0.0f);
    this->right_paddle->position = glm::vec2(court_radius.x - 0.5f, 0.0f);

    // Also create the ball objects. At init only two balls
    // the ball should be colliding with the paddle. This is used as a criteria to determine the
    // next ball to be pushed. WARNING: DO NOT CHANGE 0.8
    #define HEX_TO_U8VEC4( HX ) (glm::u8vec4( (HX >> 24) & 0xff, (HX >> 16) & 0xff, (HX >> 8) & 0xff, (HX) & 0xff ))
    this->balls.emplace_back(new Ball(glm::vec2(-court_radius.x + 0.8f, 0.0f), glm::vec2(0.0f, 0.0f), HEX_TO_U8VEC4(0xf2ad9488), true));
    this->balls.emplace_back(new Ball(glm::vec2(court_radius.x - 0.8f, 0.0f), glm::vec2(0.0f, 0.0f), HEX_TO_U8VEC4(0xbacac088), false));
    #undef HEX_TO_U8VEC4
}

SpinMode::~SpinMode() {
    //----- free OpenGL resources -----
    glDeleteBuffers(1, &vertex_buffer);
    vertex_buffer = 0;

    glDeleteVertexArrays(1, &vertex_buffer_for_color_texture_program);
    vertex_buffer_for_color_texture_program = 0;

    glDeleteTextures(1, &white_tex);
    white_tex = 0;
}

bool SpinMode::handle_event(const SDL_Event &evt, const glm::uvec2 &window_size) {
    // use mouse to change the angle && shoot
    // press key to change the force bar
    glm::vec2 clip_mouse;
    switch (evt.type) {
        case SDL_KEYDOWN:
            break;
        case SDL_KEYUP:
            if (evt.key.keysym.sym == SDLK_UP) {
                if (moving_left_paddle) left_force = std::min(++left_force, 10);
                else right_force = std::min(++right_force, 10);
            } else if (evt.key.keysym.sym == SDLK_DOWN) {
                if (moving_left_paddle) left_force = std::max(--left_force, 1);
                else right_force = std::max(--right_force, 1);
            }
            break;
        case SDL_MOUSEMOTION:
            //convert mouse from window pixels (top-left origin, +y is down) to clip space ([-1,1]x[-1,1], +y is up):
            clip_mouse = glm::vec2(
                    (evt.motion.x + 0.5f) / window_size.x * 2.0f - 1.0f,
                    (evt.motion.y + 0.5f) / window_size.y *-2.0f + 1.0f
            );
            if (moving_left_paddle) {
                std::cout << "left mouse_pos is : " << clip_mouse.x << " & " << clip_mouse.y << "\n";
                left_paddle->mouse_pos = clip_to_court * glm::vec3(clip_mouse, 1.0f);
            } else {
                std::cout << "right mouse_pos is : " << clip_mouse.x << " & " << clip_mouse.y << "\n";
                right_paddle->mouse_pos = clip_to_court * glm::vec3(clip_mouse, 1.0f);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            break;
        case SDL_MOUSEBUTTONUP:
            std::cout << "mouse click up \n";
            // push the ball away from paddle with a speed
            clip_mouse = glm::vec2(
                    (evt.button.x + 0.5f) / window_size.x * 2.0f - 1.0f,
                    (evt.button.y + 0.5f) / window_size.y *-2.0f + 1.0f
            );
            if (evt.button.button == SDL_BUTTON_LEFT) {
                std::cout << "mouse click left \n";
                auto paddle = moving_left_paddle ? left_paddle : right_paddle;
                paddle->handle_click(
                        (clip_to_court * glm::vec3(clip_mouse, 1.0f)),
                        true
                );
                for (int i = 0; i < this->balls.size(); ++i) {
                    Ball *b = balls[i];
                    float distance = std::sqrt(std::pow((b->position.x - paddle->position.x), 2) +
                            std::pow((b->position.y - paddle->position.y), 2));
                    if (distance < 1.0f) {
                        // then it is the init ball. Push the ball far and create a new one
                        b->speed.x = moving_left_paddle ? left_force * 0.2 : right_force * 0.2;
                        b->speed.y = moving_left_paddle ? left_force * 0.2 : right_force * 0.2;
                        b->unmoved = false;
                        b->direction = clip_mouse;
                        #define HEX_TO_U8VEC4( HX ) (glm::u8vec4( (HX >> 24) & 0xff, (HX >> 16) & 0xff, (HX >> 8) & 0xff, (HX) & 0xff ))
                        if (moving_left_paddle) {
                            this->balls.emplace_back(new Ball(glm::vec2(-court_radius.x + 0.8f, 0.0f), glm::vec2(0.0f, 0.0f), HEX_TO_U8VEC4(0xf2ad9488), true));
                        } else {
                            this->balls.emplace_back(new Ball(glm::vec2(court_radius.x - 0.8f, 0.0f), glm::vec2(0.0f, 0.0f), HEX_TO_U8VEC4(0xbacac088), false));
                        }
                        #undef HEX_TO_U8VEC4
                        break;
                    }
                }
                moving_left_paddle = !moving_left_paddle;
                return true;
            }
            break;
    }
    return false;
}

void SpinMode::update(float elapsed) {
    static std::mt19937 mt; //mersenne twister pseudo-random number generator
    /*
     * Game update logic: search for ball with non-zero velocity. Move the ball
     * */
    int closest_ball_idx = 0;
    float closest_distance = 0.0f;
    // calculate score, do minor change to the color of the closest ball
//    std::cout << "to update balls \n";
    // -----balls update-----
    for (int i = 0; i < balls.size(); ++i) {
        Ball *b = balls[i];
        b->update(elapsed);
        float distance = std::sqrt(std::pow(b->position.x, 2) + std::pow(b->position.y, 2));
        if (closest_distance > distance) {
            closest_ball_idx = i;
            closest_ball_idx = distance;
        }
        // TODO: update cloest ball color
        // ----Collision Handling----
        // court walls:
        if ((b->position.y > court_radius.y - ball_radius.y) && !b->unmoved) {
            b->speed.y = -b->speed.y;
        }
        if ((b->position.y < -court_radius.y + ball_radius.y) && !b->unmoved) {
            b->speed.y = -b->speed.y;
        }
        if ((b->position.x > court_radius.x - ball_radius.x) && !b->unmoved) {
            b->speed.x = -b->speed.x;
        }
        if ((b->position.x < -court_radius.x + ball_radius.x) && !b->unmoved) {
            b->speed.x = -b->speed.x;
        }
        // collision with other balls:
        int last_i = -1, last_j = -1;
        for (int j = 0; j < balls.size(); ++j) {
            if (b->unmoved || balls[j]->unmoved) break;
            if (j == i) continue;
            if (j == last_j && i == last_i) continue;
            if (std::sqrt(std::pow((b->position.x - balls[j]->position.x),2) + std::pow((b->position.y - balls[j]->position.y),2)) <= 2*ball_radius.x) {
                balls[j]->speed.x += 0.8* b->speed.x;
                balls[j]->speed.y += 0.8* b->speed.y;
                b->speed.x = -b->speed.x;
                b->speed.y = -b->speed.y;
                last_i = i;
                last_j = j;
            }
        }

    }
}

void SpinMode::draw(const glm::uvec2 &drawable_size) {
    //some nice colors from the course web page:
    #define HEX_TO_U8VEC4( HX ) (glm::u8vec4( (HX >> 24) & 0xff, (HX >> 16) & 0xff, (HX >> 8) & 0xff, (HX) & 0xff ))
    const glm::u8vec4 bg_color = HEX_TO_U8VEC4(0x193b59ff);
    const glm::u8vec4 fg_color = HEX_TO_U8VEC4(0xf2d2b6ff);
    const glm::u8vec4 shadow_color = HEX_TO_U8VEC4(0xf2ad94ff);
    const std::vector< glm::u8vec4 > trail_colors = {
            HEX_TO_U8VEC4(0xf2ad9488),
            HEX_TO_U8VEC4(0xf2897288),
            HEX_TO_U8VEC4(0xbacac088),
    };
    #undef HEX_TO_U8VEC4

    //other useful drawing constants:
    const float wall_radius = 0.05f;
    const float shadow_offset = 0.07f;
    const float padding = 0.14f; //padding between outside of walls and edge of window
    //---- compute vertices to draw ----

    //vertices will be accumulated into this list and then uploaded+drawn at the end of this function:
    std::vector< Vertex > vertices;

    //inline helper function for rectangle drawing:
    auto draw_rectangle = [&vertices](glm::vec2 const &center, glm::vec2 const &radius, glm::u8vec4 const &color) {
        //draw rectangle as two CCW-oriented triangles:
        vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x+radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));

        vertices.emplace_back(glm::vec3(center.x-radius.x, center.y-radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x+radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
        vertices.emplace_back(glm::vec3(center.x-radius.x, center.y+radius.y, 0.0f), color, glm::vec2(0.5f, 0.5f));
    };

    // draw circles. Reference: https://github.com/ericeschnei/15-466-f20-base0/blob/HEAD/DefendMode.cpp used with adaption

    const int center_num_segments = 48;
    const int inner_num_segments = 36;
    const glm::u8vec4 center_color = glm::u8vec4(0x99, 0xCC, 0xFF, 0xff);
    const glm::u8vec4 inner_color = glm::u8vec4(0x40, 0x40, 0x40, 0xff);

    auto add_triangle_out = [center_color, &vertices](
            const glm::vec2 &p1, const glm::vec2 &p2
    ) {
        // define CCW tri
        vertices.emplace_back(glm::vec3(p1, 0.0f), center_color, glm::vec2(0.0f, 0.0f));
        vertices.emplace_back(glm::vec3(p2, 0.0f), center_color, glm::vec2(0.0f, 0.0f));
        vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), center_color, glm::vec2(0.0f, 0.0f));
    };

    // draw the outer circle
    glm::vec2 last_pos;
    for (int i = 0; i <= center_num_segments; i++) {
        float angle = ((float) i) / center_num_segments * 6.28319f;
        glm::vec2 pos = glm::vec2(
                center_radius * glm::cos(angle),
                center_radius * glm::sin(angle)
        );

        if (i > 0) {
            add_triangle_out(last_pos, pos);
        }
        last_pos = pos;
    }

    //draw the inner circle
    auto add_triangle_in = [inner_color, &vertices](
            const glm::vec2 &p1, const glm::vec2 &p2
    ) {
        // define CCW tri
        vertices.emplace_back(glm::vec3(p1, 0.0f), inner_color, glm::vec2(0.0f, 0.0f));
        vertices.emplace_back(glm::vec3(p2, 0.0f), inner_color, glm::vec2(0.0f, 0.0f));
        vertices.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), inner_color, glm::vec2(0.0f, 0.0f));
    };

    for (int i = 0; i <= inner_num_segments; i++) {
        float angle = ((float) i) / inner_num_segments * 6.28319f;
        glm::vec2 pos = glm::vec2(
                inner_radius * glm::cos(angle),
                inner_radius * glm::sin(angle)
        );

        if (i > 0) {
            add_triangle_in(last_pos, pos);
        }
        last_pos = pos;
    }


    glm::vec2 s = glm::vec2(0.0f,-shadow_offset);
    //draws the shadow
    draw_rectangle(glm::vec2(-court_radius.x-wall_radius, 0.0f)+s, glm::vec2(wall_radius, court_radius.y + 2.0f * wall_radius), shadow_color);
    draw_rectangle(glm::vec2( court_radius.x+wall_radius, 0.0f)+s, glm::vec2(wall_radius, court_radius.y + 2.0f * wall_radius), shadow_color);
    draw_rectangle(glm::vec2( 0.0f,-court_radius.y-wall_radius)+s, glm::vec2(court_radius.x, wall_radius), shadow_color);
    draw_rectangle(glm::vec2( 0.0f, court_radius.y+wall_radius)+s, glm::vec2(court_radius.x, wall_radius), shadow_color);
//    draw_rectangle(left_paddle+s, paddle_radius, shadow_color);
//    draw_rectangle(right_paddle+s, paddle_radius, shadow_color);
//    draw_rectangle(ball+s, ball_radius, shadow_color);


    //solid objects:

    //walls:
    draw_rectangle(glm::vec2(-court_radius.x-wall_radius, 0.0f), glm::vec2(wall_radius, court_radius.y + 2.0f * wall_radius), fg_color);
    draw_rectangle(glm::vec2( court_radius.x+wall_radius, 0.0f), glm::vec2(wall_radius, court_radius.y + 2.0f * wall_radius), fg_color);
    draw_rectangle(glm::vec2( 0.0f,-court_radius.y-wall_radius), glm::vec2(court_radius.x, wall_radius), fg_color);
    draw_rectangle(glm::vec2( 0.0f, court_radius.y+wall_radius), glm::vec2(court_radius.x, wall_radius), fg_color);

    //paddles:
//    draw_rectangle(left_paddle, paddle_radius, fg_color);
//    draw_rectangle(right_paddle, paddle_radius, fg_color);
    left_paddle->draw(vertices);
    right_paddle->draw(vertices);

    //initially there should be two balls sitting on paddles.

    //ball:
//    draw_rectangle(ball, ball_radius, fg_color);
    for (Ball* b : balls) {
        b->draw(vertices);
    }
    //force bar, changed by key:
    glm::vec2 score_radius = glm::vec2(0.1f, 0.1f);
    for (uint32_t i = 0; i < left_force; ++i) {
        draw_rectangle(glm::vec2( -court_radius.x + (2.0f + 3.0f * i) * score_radius.x, court_radius.y + 2.0f * wall_radius + 2.0f * score_radius.y), score_radius, fg_color);
    }
    for (uint32_t i = 0; i < right_force; ++i) {
        draw_rectangle(glm::vec2( court_radius.x - (2.0f + 3.0f * i) * score_radius.x, court_radius.y + 2.0f * wall_radius + 2.0f * score_radius.y), score_radius, fg_color);
    }
    //------ compute court-to-window transform ------

    //compute area that should be visible:
    glm::vec2 scene_min = glm::vec2(
            -court_radius.x - 2.0f * wall_radius - padding,
            -court_radius.y - 2.0f * wall_radius - padding
    );
    glm::vec2 scene_max = glm::vec2(
            court_radius.x + 2.0f * wall_radius + padding,
            court_radius.y + 2.0f * wall_radius + 3.0f * score_radius.y + padding
    );

    //compute window aspect ratio:
    float aspect = drawable_size.x / float(drawable_size.y);
    //we'll scale the x coordinate by 1.0 / aspect to make sure things stay square.

    //compute scale factor for court given that...
    float scale = std::min(
            (2.0f * aspect) / (scene_max.x - scene_min.x), //... x must fit in [-aspect,aspect] ...
            (2.0f) / (scene_max.y - scene_min.y) //... y must fit in [-1,1].
    );

    glm::vec2 center = 0.5f * (scene_max + scene_min);

    //build matrix that scales and translates appropriately:
    glm::mat4 court_to_clip = glm::mat4(
            glm::vec4(scale / aspect, 0.0f, 0.0f, 0.0f),
            glm::vec4(0.0f, scale, 0.0f, 0.0f),
            glm::vec4(0.0f, 0.0f, 1.0f, 0.0f),
            glm::vec4(-center.x * (scale / aspect), -center.y * scale, 0.0f, 1.0f)
    );
    //NOTE: glm matrices are specified in *Column-Major* order,
    // so each line above is specifying a *column* of the matrix(!)

    //also build the matrix that takes clip coordinates to court coordinates (used for mouse handling):
    clip_to_court = glm::mat3x2(
            glm::vec2(aspect / scale, 0.0f),
            glm::vec2(0.0f, 1.0f / scale),
            glm::vec2(center.x, center.y)
    );

    //---- actual drawing ----

    //clear the color buffer:
    glClearColor(bg_color.r / 255.0f, bg_color.g / 255.0f, bg_color.b / 255.0f, bg_color.a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    //use alpha blending:
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //don't use the depth test:
    glDisable(GL_DEPTH_TEST);

    //upload vertices to vertex_buffer:
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer); //set vertex_buffer as current
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STREAM_DRAW); //upload vertices array
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //set color_texture_program as current program:
    glUseProgram(color_texture_program.program);

    //upload OBJECT_TO_CLIP to the proper uniform location:
    glUniformMatrix4fv(color_texture_program.OBJECT_TO_CLIP_mat4, 1, GL_FALSE, glm::value_ptr(court_to_clip));

    //use the mapping vertex_buffer_for_color_texture_program to fetch vertex data:
    glBindVertexArray(vertex_buffer_for_color_texture_program);

    //bind the solid white texture to location zero so things will be drawn just with their colors:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, white_tex);

    //run the OpenGL pipeline:
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(vertices.size()));

    //unbind the solid white texture:
    glBindTexture(GL_TEXTURE_2D, 0);

    //reset vertex array to none:
    glBindVertexArray(0);

    //reset current program to none:
    glUseProgram(0);


    GL_ERRORS(); //PARANOIA: print errors just in case we did something wrong.


}