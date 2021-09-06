//
// Created by ccjeff on 2021/9/5.
//

#include "SpinMode.hpp"
//for the GL_ERRORS() macro:
#include "gl_errors.hpp"

//for glm::value_ptr() :
#include <glm/gtc/type_ptr.hpp>

#include <random>

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

bool SpinMode::handle_event(const SDL_Event &, const glm::uvec2 &window_size) {
    switch (evt.type) {

    }
}

