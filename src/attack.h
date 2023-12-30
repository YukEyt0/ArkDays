#include "./base/application.h"
#include "./base/glsl_program.h"
#include "./base/transform.h"

#include "mymodel.h"

#include <memory>

struct HPDrawler {
    HPDrawler() {
        const float _verticies[] {
            -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f
        };
        glGenVertexArrays(1, &_vao);
        glGenBuffers(1, &_vbo);

        glBindVertexArray(_vao);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(_verticies), &_verticies, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        glBindVertexArray(0);
        const char *vsCode = 
        "#version 330 core\n"
        "layout(location = 0) in vec3 aPosition;\n"

        "out vec3 worldPosition;\n"

        "uniform mat4 model;\n"
        "uniform mat4 view;\n"
        "uniform mat4 projection;\n"

        "void main() {\n"
        "    worldPosition = vec3(model * vec4(aPosition, 1.0f));\n"
        "    gl_Position = projection * view * vec4(worldPosition, 1.0f);\n"
        "}\n";
        const char *fsCode = 
        "#version 330\n"
        "out vec4 outputF;\n"
        "uniform vec4 color;\n"
        "void main() {\n"
        "   outputF = color;\n"
        "}\n";
        _shader.reset(new GLSLProgram);
        _shader->attachVertexShader(vsCode);
        _shader->attachFragmentShader(fsCode);
        _shader->link();
        
    }

    void draw(glm::mat4 model, float hp, float max_hp) {
        _shader->setUniformMat4("model",glm::scale(model,glm::vec3{hp/max_hp,1.0f,1.0f}));
        _shader->setUniformVec4("color",glm::vec4{1.0f,0.0f,0.0f,1.0f});
        glBindVertexArray(_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    ~HPDrawler() {cleanup();}
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
    GLuint _vao, _vbo;
    std::unique_ptr<GLSLProgram> _shader;
};

struct Projectile{
    enum ProjectStyle {
        SPHERE,
        CONE,
        CUBE,
    };
    Projectile(std::unique_ptr<ModelOnWorld>& target, glm::vec3 pos, Projectile::ProjectStyle style);
    Projectile(const Projectile& rhs);
    ~Projectile() = default;
    void draw();
    bool hit();
    std::unique_ptr<ModelOnWorld>& _target;
    ProjectStyle _style;
    std::unique_ptr<ModelOnWorld> _model; 
};

struct Attack {
    enum AttackStyle {
        MOVE,
        HIT
    };
    Attack(int i, int j) : attacker_idx(i), defender_idx(j){};
    Attack(const Attack& rhs)=delete;
    //: attacker_idx(rhs.attacker_idx), defender_idx(rhs.defender_idx), _proj(std::move(_proj)), _style(rhs._style) {};
    int attacker_idx;
    int defender_idx;
    std::unique_ptr<Projectile> _proj;
    AttackStyle _style = MOVE;
};

struct Heal {
    enum AttackStyle {
        MOVE,
        HIT
    };
    Heal(int i, int j) : attacker_idx(i), defender_idx(j){};
    Heal(const Heal& rhs)=delete;
    //: attacker_idx(rhs.attacker_idx), defender_idx(rhs.defender_idx), _proj(std::move(_proj)), _style(rhs._style) {};
    int attacker_idx;
    int defender_idx;
    std::unique_ptr<Projectile> _proj;
    AttackStyle _style = MOVE;
};