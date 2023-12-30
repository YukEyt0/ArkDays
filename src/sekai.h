#pragma once

#include <memory>
#include <vector>
#include <string>

#include "./base/application.h"
#include "./base/camera.h"
#include "./base/skybox.h"
#include "./base/light.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "attack.h"
#include "enemy.h"
#include "myobject.h"
#include "map.h"
#include "operator.h"
#include "material.h"
#include "screenshot.h"

extern std::unique_ptr<ModelLibrary> _model_lib;
extern std::unique_ptr<ShapeLibrary> _shape_lib;
extern std::unique_ptr<TextrueLibrary> _texture_lib;
extern std::unique_ptr<OperatorIconLibrary> _icon_lib;

const std::vector<std::string> skyboxTextureRelPaths = {
    "texture/skybox/Right_Tex.jpg", "texture/skybox/Left_Tex.jpg",  "texture/skybox/Up_Tex.jpg",
    "texture/skybox/Down_Tex.jpg",  "texture/skybox/Front_Tex.jpg", "texture/skybox/Back_Tex.jpg"};

class sekai : public Application {
public:
    sekai(const Options& options);
    ~sekai() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
private:
    struct EnemyOnWorld{
        EnemyOnWorld(std::unique_ptr<Enemy> enemy) : _enemy(std::move(enemy)) {};
        std::unique_ptr<Enemy> _enemy;
        std::unique_ptr<PathLibrary::PathList> _path;
        enum state{
            SUMMON,
            MOVE,
            BLOCK,
            DEAD
        }_state;
        float counter = 0;
        float hp;
    };

    struct OperatorOnWorld {
        OperatorOnWorld() {};
        std::unique_ptr<Operator> _operator;
        enum state{
            READY,
            PLACED,
            DEAD
        }_state;
        int _pos_x;
        int _pos_y;
        std::vector<int> _block_buffer;
        float counter=0;
        float hp;
    };
    
    std::unique_ptr<GLSLProgram> _shader;
    std::unique_ptr<GLSLProgram> _textcoord_shader;
    std::unique_ptr<EnemyLibrary> _enemy_lib;
    std::unique_ptr<OperatorLibrary> _operator_lib;
    std::unique_ptr<SkyBox> _skybox;
    std::unique_ptr<Map> _map;
    std::vector<std::unique_ptr<Camera>> _cameras;
    std::vector<std::unique_ptr<EnemyOnWorld>> _enemies;
    std::vector<std::unique_ptr<OperatorOnWorld>> _operators;
    std::vector<std::unique_ptr<OperatorOnWorld>> _placed_operators;
    std::unique_ptr<ModelOnWorld> _player;
    std::unique_ptr<AmbientLight> _ambientLight;
    std::unique_ptr<DirectionalLight> _directionalLight;
    std::unique_ptr<SpotLight> _spotLight;
    std::unique_ptr<Material> _material;
    std::unique_ptr<HPDrawler> _hpdrawler;
    std::vector<std::unique_ptr<Attack>> _operator_attack;
    std::vector<std::unique_ptr<Attack>> _enemy_attack;
    std::vector<std::unique_ptr<Heal>> _heal;
    int selected_operator = 0;
    std::unique_ptr<GLSLProgram> _select_shader;
    int activeCameraIndex = 0;
    int operator_cursor = 0;
    int selected_object = 0; 
    float _time;

    inline void add_time(float delt) {
        if(_state==PAUSE||_state==EDIT)
            return;
        if(_time<500.0f)
            _time+=delt;
        else
            _time=delt;
    }

    void add_enemy() {
        if(_state==PAUSE||_state==EDIT)
            return;
        std::vector<int> enemy_idx;
        std::vector<int> path_idx;
        if (_map->_summonlist->GetNextEnemy(_time,enemy_idx,path_idx)) {
            for(int i = 0; i<enemy_idx.size();++i) {
                std::unique_ptr<EnemyOnWorld> e;
                e.reset(new EnemyOnWorld(_enemy_lib->getEnemy(enemy_idx.at(i))));
                e->_state = EnemyOnWorld::SUMMON;
                e->_path = std::make_unique<PathLibrary::PathList>(_map->_pathlib->getPathList(path_idx[i]));
                e->_enemy->_model->_transform.position = e->_path->GetSummonPosition();
                e->_enemy->_model->_transform.scale *= glm::vec3(1.0f/3.0f);
                e->_path->CheckPoint(e->_enemy->_model->_transform.position);
                e->_enemy->_model->_transform.lookAt(e->_path->GetMoveDirection());
                e->hp = e->_enemy->_hp;
                _enemies.emplace_back(std::move(e));
            }
        }
    }

    void deal_enemy_move() {
        if(_state==PAUSE||_state==EDIT)
            return;
        for(auto enemy = _enemies.begin(); enemy !=_enemies.end();) {
            auto& ref_enemy = *enemy;
            if(ref_enemy->_state==EnemyOnWorld::MOVE) {
                ref_enemy->_enemy->_model->_transform.position += ref_enemy->_enemy->_model->_transform.getFront() * _deltaTime * ref_enemy->_enemy->_spd * 10.0f;
                if(!ref_enemy->_path->CheckPoint(ref_enemy->_enemy->_model->_transform.position)) {
                    ref_enemy->_state=EnemyOnWorld::DEAD;
                }
                ref_enemy->_enemy->_model->_transform.lookAt(ref_enemy->_path->GetMoveDirection());
                if(CollisionDetection(ref_enemy,enemy-_enemies.begin())) {
                    ref_enemy->_state=EnemyOnWorld::BLOCK;
                }
            }
            if(ref_enemy->_state==EnemyOnWorld::SUMMON) {
                ref_enemy->_state = EnemyOnWorld::MOVE;
            }
            ++enemy;
        }
    }

    void draw_enemies(std::unique_ptr<GLSLProgram>& ref_shader) {
        for(auto enemy = _enemies.begin(); enemy !=_enemies.end();++enemy) {
            auto& ref_enemy = *enemy;
            if(ref_enemy->_state==EnemyOnWorld::DEAD) {
                continue;
            }
            auto& textrue = _texture_lib->getTextureRef(ref_enemy->_enemy->_model->_textrue_idx);
            if(ref_enemy->_enemy->_model->enable_textrue) {
                textrue->bind(0);
                ref_shader->setUniformInt("cubemap",0);
            }
            ref_shader->setUniformVec3("material.ka", ref_enemy->_enemy->_model->_material.ka);
            ref_shader->setUniformVec3("material.ks", ref_enemy->_enemy->_model->_material.ks);
            ref_shader->setUniformVec3("material.kd", ref_enemy->_enemy->_model->_material.kd);
            ref_shader->setUniformFloat("material.ns", ref_enemy->_enemy->_model->_material.ns);
            ref_shader->setUniformMat4("model",ref_enemy->_enemy->_model->_transform.getLocalMatrix());
            ref_enemy->_enemy->_model->draw();
            if(ref_enemy->_enemy->_model->enable_textrue) {
                textrue->unbind();
            }
            ref_enemy->_enemy->_model->is_placed = true;
        }
    }

    void draw_icons() {
        auto offset = glm::vec3(-1.0f,-1.0f,0);
        float scale = (float)_windowHeight / _windowWidth;
        GLfloat _iconscale = 0.3f;
        int idx = 0;
        for(auto& it : _operators) {
            if(it->_state==OperatorOnWorld::READY||OperatorOnWorld::DEAD) {
                auto& icon = it->_operator->_icon;
                icon->_trans.scale =  glm::vec3(_iconscale * scale,_iconscale,1.0f);
                icon->_trans.position = offset;
                if(idx==operator_cursor) 
                    icon->_trans.position += glm::vec3(0,_iconscale/3,0);
                icon->draw();
                offset = glm::vec3((GLfloat)offset.x+_iconscale * scale, (GLfloat)offset.y,0);
                idx++;
            }
        } 
    }

    void first_person_movement_deal();

    enum programstate {
        RUN,
        PAUSE,
        EDIT
    }_state;

    bool processSelection(int xx, int yy, int& o_x, int& o_y) {
        renderSelectFrame();
        GLint viewport[4];
        unsigned char res[4];
        glGetIntegerv(GL_VIEWPORT,viewport);
        glReadPixels(xx,viewport[3]-yy, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,&res);
        o_x = res[0];
        o_y = res[1];
        return !(res[3]==255);
    }

    void renderSelectFrame() {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        _select_shader->use();
        const glm::mat4 projection = _cameras[activeCameraIndex]->getProjectionMatrix();
        const glm::mat4 view = _cameras[activeCameraIndex]->getViewMatrix();
        _select_shader->setUniformMat4("projection", projection);
        _select_shader->setUniformMat4("view", view);
        _map->select_draw(_select_shader);
        _select_shader->unuse();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    }

    enum operatorstate{
        SELECTING,
        PLACING,
        DIRECTING,
    }_operator_state = SELECTING;

    void initOperators() {
        for(auto it : operator_list) {
            auto o = std::make_unique<OperatorOnWorld>();
            o->_operator = std::move(_operator_lib->getOperator(it));
            o->_state = OperatorOnWorld::READY;
            _operators.emplace_back(std::move(o));
        }
    }
    
    inline void inc_cursor() {
        operator_cursor = (_operators.size() + operator_cursor+1)%_operators.size();
    }

    inline void dec_cursor() {
        operator_cursor = (_operators.size() + operator_cursor-1)%_operators.size();
    }

    struct {
        bool lpressed = false;
        bool rpressed = false;
        int x;
        int y;
        bool upressed  = false;
        bool dpressed  = false;
        bool mlpressed = false;
        bool mrpressed = false;
        bool scpressed = false;
        bool ppressed = false;
        bool f2pressed = false;
        bool thirdperson = false;
        int view = 0;
    }_operator_movement_controller;

    void place_operator(); 

    void handle_operator_movement_input();

    void draw_placed_operators(std::unique_ptr<GLSLProgram>& ref_shader);

    void handle_attack();

    void deal_damage();

    void draw_projectile(std::unique_ptr<GLSLProgram>& ref_shader);

    void deal_rearrange();

    void initShaders();

    void draw_thrd_person(std::unique_ptr<GLSLProgram>& ref_shader);

    void handleInput() override;

    void renderFrame() override;

    bool CollisionDetection(std::unique_ptr<sekai::EnemyOnWorld>& ref_enemy,int idx);

    bool PlayerCollisionDetection();

    void draw_hp();

    void update_player_location();

    void draw_object_select();

    bool get_selected_object(int xx, int yy, int &id);

    void renderEditGUI();

    void edit_light();
    
    void edit_object();
};