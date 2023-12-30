#pragma once

#include "myobject.h"
#include "mymodel.h"
#include "map.h"

#include <memory>
#include <map>


extern std::unique_ptr<ModelLibrary> _model_lib;
extern std::unique_ptr<ShapeLibrary> _shape_lib;
extern std::unique_ptr<TextrueLibrary> _texture_lib;

/*
* TODO:
* 
* 实现被阻挡
*/

class Enemy {
public:
    Enemy(float hp, float atk, float def, float spd, float cool_down, std::unique_ptr<MyObject>& model, int textrue_idx) 
    : _hp(hp), _atk(atk), _def(def), _spd(spd), _cool_down(cool_down), _model(new ModelOnWorld(model,textrue_idx)) {
        ModelOnWorld::_model_map.emplace(_model->id,_model);
    };

    Enemy(const Enemy& rhs)
    : _hp(rhs._hp), _atk(rhs._atk), _def(rhs._def), _spd(rhs._spd), _cool_down(rhs._cool_down),_model(new ModelOnWorld(rhs._model->_object,rhs._model->_textrue_idx)) {
        ModelOnWorld::_model_map.emplace(_model->id,_model);
        ModelOnWorld::_model_map.erase(rhs._model->id);
    };
    
    float _hp;
    float _atk;
    float _def;
    float _spd;
    float _cool_down;
    std::unique_ptr<ModelOnWorld> _model;
};

class EnemyLibrary {
public:
    EnemyLibrary() {
        _enemies.emplace_back(Enemy(1050.0f,185.0f,0.0f,1.0f,1.7f,_shape_lib->getShape(3),0));
        _enemies.emplace_back(Enemy(1650.0f,200.0f,100.0f,1.1f,2.0f,_shape_lib->getShape(2),0));
        _enemies.emplace_back(Enemy(5000.0f,500.0f,50.0f,0.8,3.0f,_model_lib->getModel(2),0));
        _enemies.emplace_back(Enemy(1550.0f,180.0f,50.0f,1.0f,2.7f,_shape_lib->getShape(4),0));
        _enemies.emplace_back(Enemy(2050.0f,240.0f,250.0f,1.0f,2.0f,_shape_lib->getShape(1),0));
        _enemies.emplace_back(Enemy(1700.0f,250.0f,50.0f,1.1f,2.0f,_shape_lib->getShape(0),0));
    }
    std::unique_ptr<Enemy> getEnemy(int idx) {
        return std::make_unique<Enemy>(_enemies.at(idx));
    }
private:
    std::vector<Enemy> _enemies;
};


