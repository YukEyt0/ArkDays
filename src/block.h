#pragma once

#include <memory>

#include "./base/application.h"
#include "./base/gl_utility.h"
#include "./base/glsl_program.h"
#include "./base/texture_cubemap.h"
#include "./base/transform.h"
#include "./base/vertex.h"

#include "basicshapes.h"
#include "material.h"


class Block {
public:
    Block(const std::vector<std::string>& textureFilenames) {
    _cube.reset(new Prism(5,0.5));
    try {
        // init texture
        _texture.reset(new ImageTextureCubemap(textureFilenames));

    } catch (const std::exception&) {
        throw;
    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::stringstream ss;
        ss << "creation failure, (code " << error << ")";
        throw std::runtime_error(ss.str());
    }
    }
    
    ~Block()=default;

    void draw(const std::unique_ptr<GLSLProgram>& ref_shader) {
        glDepthFunc(GL_LEQUAL);
        ref_shader->setUniformInt("cubemap", 0);
        _texture->bind(0);
        _cube->draw();
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void draw() {
        _cube->draw();
    }

private:

    std::unique_ptr<TextureCubemap> _texture;

    std::unique_ptr<MyObject> _cube;

    std::unique_ptr<Material> _material;

};