#include"map.h"
extern std::unique_ptr<ModelLibrary> _model_lib;
extern std::unique_ptr<ShapeLibrary> _shape_lib;
extern std::unique_ptr<TextrueLibrary> _texture_lib;

    Map::Map(std::string pfx) {
        _summonlist.reset(new SummonList());
        _pathlib.reset(new PathLibrary());
        GLfloat x,y,z;
        y = 0.0f;
        for(int i=0;i<7;i++) {
            x = ((float)i+0.5) * _mapscale;
            for(int j=0;j<11;j++) {
                z = ((float)j+0.5)*_mapscale;
                auto type = bitmap[i][j];
                std::unique_ptr<ModelOnWorld> block;
                if(type == 2) {
                    block.reset(new ModelOnWorld{_shape_lib->getShape(0),type});
                }
                else if (type == 6)
                    block.reset(new ModelOnWorld{_shape_lib->getShape(1),type});
                else
                    block.reset(new ModelOnWorld{_shape_lib->getShape(0),type});
                block->_transform.position += glm::vec3(x, y, z);
                if(type == 0 || type == 1)
                    block->_transform.position += glm::vec3(0, -_mapscale, 0);
                _blocks.emplace_back(std::move(block));
            }
        }
        for(auto &it : _blocks) {
            it->enable_textrue = true;
            ModelOnWorld::_model_map.emplace(it->id,it);
        }
    };

    void Map::select_draw(const std::unique_ptr<GLSLProgram>& ref_shader) {
        for(int i = 0; i < 7; i++) {
            for(int j = 0;j<11;j++) {
                auto& block = _blocks.at(i*11+j);
                ref_shader->setUniformMat4("model",block->_transform.getLocalMatrix());
                ref_shader->setUniformInt("x",i);
                ref_shader->setUniformInt("y",j);
                block->draw();
            }
        }
    }

    void Map::draw(const std::unique_ptr<GLSLProgram>& ref_shader) {
        for(int i = 0; i < 7; i++) {
            for(int j = 0;j<11;j++) {
                auto& block = _blocks.at(i*11+j);
                ref_shader->setUniformVec3("material.ka", block->_material.ka);
                ref_shader->setUniformVec3("material.ks", block->_material.ks);
                ref_shader->setUniformVec3("material.kd", block->_material.kd);
                ref_shader->setUniformFloat("material.ns", block->_material.ns);
                auto model = block->_transform.getLocalMatrix()*block->_local_transform.getLocalMatrix();
                ref_shader->setUniformMat4("model",model);
                auto& textrue = _texture_lib->getTextureRef(block->_textrue_idx);
                if(block->enable_textrue) {
                    textrue->bind(0);
                    ref_shader->setUniformInt("cubemap",0);
                }
                block->draw();
                if(block->enable_textrue) {
                    textrue->unbind();
                }
                block->is_placed = true;
            }
        }
    }

    glm::vec3 Map::point2WorldPosition(float x, float y) {
        return glm::vec3{_mapscale*(x+0.5),0,_mapscale*(y+0.5)};
    }


    SummonList::SummonList() {
        for(auto it : summonlist) {
            _enemylist.emplace_back(it[0],it[1],it[2]);
        };
        _head = _enemylist.begin();
        _next = _head;
    }
    bool SummonList::GetNextEnemy(int i_time, std::vector<int>& o_enemy_index,std::vector<int>& o_path_index) {
        if(_next==_enemylist.end()) {
            return false;
        }
        while (i_time>_next->summon_time&&_next!=_enemylist.end()) {
            o_enemy_index.emplace_back(_next->enemy_idx);
            o_path_index.emplace_back(_next->enemy_path_idx);
            ++_next;
        }
        return true;
    }
    void SummonList::ResetEnemyList() {
        _next = _head;
    }

    PathLibrary::PathLibrary() {
        for(int i = 0; i< 3;++i) {
            _pathlib.emplace_back(i);
        }
    }
    PathLibrary::PathList PathLibrary::getPathList(int idx) {
        return _pathlib[idx];
    }

    PathLibrary::PathList::PathList(int path_i) {
            for(auto it : path[path_i]) {
                _pathlist.emplace_back((int)it[0],(int)it[1]);
            }
            _pathlist.emplace_back(-1,-1);
            _head = _pathlist.begin();
            _next = _head;
    }

    bool PathLibrary::PathList::CheckPoint(glm::vec3 pos) {
            if(_next == _pathlist.end())
                return false;
            auto relcheckpoint = Map::point2WorldPosition(_next->x,_next->y);
            //std::cout << pos.x << ", " << pos.z << std::endl;
            if(abs(glm::length(relcheckpoint-pos))<zero) {
                ++_next;
                if(_next->x == -1&&_next->y==-1) {
                    _next = _pathlist.end();
                    return false;
                }
                //std::cout << _next->x << ", " << _next->y << std::endl;
            }
            return true;
        }
    glm::vec3 PathLibrary::PathList::GetMoveDirection() {
            glm::vec3 relcheckpoint = Map::point2WorldPosition(_next->x,_next->y);
            return relcheckpoint;
        };
    glm::vec3 PathLibrary::PathList::GetSummonPosition() {
            return Map::point2WorldPosition(_head->x,_head->y);
        }