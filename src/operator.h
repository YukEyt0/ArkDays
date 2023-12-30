#pragma once

#include<list>
#include<memory>
#include<vector>

#include "./base/texture2d.h"
#include "./base/application.h"
#include "./base/glsl_program.h"
#include "./base/transform.h"

#include "mymodel.h"

const std::vector<std::string> img_paths = {
    "texture/class/class_vanguard.png",
    "texture/class/class_defender.png",
    "texture/class/class_medic.png",
    "texture/class/class_caster.png",
    "texture/class/class_sniper.png",
};
/*
struct Range {
    Range() {};
    Range(std::vector<int> range_x, std::vector<int> range_y) : _range_x(range_x), _range_y(range_y){};
    std::vector<int> _range_x;
    std::vector<int> _range_y;
};
*/
class OperatorIcon {
public:
    OperatorIcon(std::string icon_path) {
        GLfloat vertices[] = {
            0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f,

            1.0f, 1.0f,  1.0f, 0.0f,  0.0f, 0.0f
        };
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

        glBindVertexArray(0);

        try {
        // init texture
        _texture.reset(new ImageTexture2D(icon_path));

        const char* vsCode =
            "#version 330 core\n"
            "layout(location = 0) in vec2 aPosition;\n"
            "out vec2 texCoord;\n"
            "uniform mat4 model;\n"
            "void main() {\n"
            "   texCoord = aPosition;\n"
            "   gl_Position = model * vec4(aPosition,0.0,1.0);\n"
            "}\n";

        const char* fsCode =
            "#version 330 core\n"
            "out vec4 color;\n"
            "in vec2 texCoord;\n"
            "uniform sampler2D tdmap;\n"
            "void main() {\n"
            "   color = texture(tdmap, texCoord);\n"
            "}\n";

        _shader.reset(new GLSLProgram);
        _shader->attachVertexShader(vsCode);
        _shader->attachFragmentShader(fsCode);
        _shader->link();
        } catch (const std::exception&) {
            cleanup();
            throw;
        }

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::stringstream ss;
            ss << "icon creation failure, (code " << error << ")";
            cleanup();
            throw std::runtime_error(ss.str());
        }
    }

    void draw() {
        _shader->use();
        _shader->setUniformMat4("model",_trans.getLocalMatrix());
        glBindVertexArray(_vao);
        _texture->bind(0);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }
    Transform _trans;
private:
    std::unique_ptr<GLSLProgram> _shader;
    std::unique_ptr<ImageTexture2D> _texture;
    GLuint _vao = 0;
    GLuint _vbo = 0;
    

    void cleanup() {
    if (_vbo != 0) {
        glDeleteBuffers(1, &_vbo);
        _vbo = 0;
    }

    if (_vao != 0) {
        glDeleteVertexArrays(1, &_vao);
        _vao = 0;
    }
}
};

class OperatorIconLibrary{
public:
    OperatorIconLibrary(std::string assetRootDir) {
        _icons.resize(img_paths.size());
        for(int i = 0; i<img_paths.size();++i) {
            _icons[i].reset(new OperatorIcon(assetRootDir+img_paths[i]));
        }
    }
    std::unique_ptr<OperatorIcon>& getIconRef(int idx) {
        return _icons[idx];
    }
private:
    std::vector<std::unique_ptr<OperatorIcon>> _icons;
};

const std::vector<int> operator_list = {0,0,0,1,1,2,2,3,3,4,4};
/*
struct RangeLibrary {
    RangeLibrary() {
        _ranges.emplace_back(
            std::vector<int>{-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1}, std::vector<int>{0,0,0,1,1,1,2,2,2,3,3,3,4,4,4}
        );
        _ranges.emplace_back(
            std::vector<int>{0,0}, std::vector<int>{0,1}
        );
        _ranges.emplace_back(
            std::vector<int>{-1,0,1,-1,0,1,-1,0,1,-1,0,1,0}, std::vector<int>{0,0,0,1,1,1,2,2,2,3,3,3,4}
        );
        _ranges.emplace_back(
            std::vector<int>{int(0)}, std::vector<int>{int(0)}
        );
    }
    std::vector<Range> _ranges;
    Range getRange(int index) {return _ranges.at(index);}
};
*/
enum OperatorType {
    VANGUARD,
    DEFENDER,
    MEDIC,
    CASTER,
    SNIPER
};

struct Operator {
    Operator(float hp, float atk, float def, float cool_down, int block, int model_idx, int pic_idx, float range, OperatorType type, float reset_time=0, int cost=0);
    Operator(const Operator &rhs);
    float _atk;
    float _def;
    float _hp;
    float _cool_down;
    float _reset_time=0;
    int _block;
    int _cost=0;    
    float _range;
    std::unique_ptr<ModelOnWorld> _model;
    std::unique_ptr<OperatorIcon>& _icon;
    OperatorType _type;
private:
    int _model_idx;
    int _pic_idx;
};

class OperatorLibrary {
public:
    OperatorLibrary () {
        _operators.emplace_back(1165,309,278,1.05,2,7,0,1,VANGUARD);
        _operators.emplace_back(1865,285,534,1.2,3,8,1,0,DEFENDER);
        _operators.emplace_back(1080,338,102,2.85,1,9,2,4,MEDIC);
        _operators.emplace_back(1050,456,84,1.6,1,10,3,4,CASTER);
        _operators.emplace_back(980,401,115,1.0,1,11,4,4,SNIPER);
    }
    std::unique_ptr<Operator> getOperator(int idx) {
        Operator o = _operators.at(idx);
        return std::make_unique<Operator>(o);
    }
private:
    std::vector<Operator> _operators;
};