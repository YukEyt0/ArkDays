#pragma once

#include <memory>
#include <list>
#include <string>

#include "enemy.h"
#include "resource.h"
#include "./base/application.h"
#include "./base/glsl_program.h"


struct SummonList 
{
    SummonList();
    bool GetNextEnemy(int i_time, std::vector<int>& o_enemy_index,std::vector<int>& o_path_index);
    void ResetEnemyList();
    struct listnode {
        int summon_time;
        int enemy_idx;
        int enemy_path_idx;
        listnode(int s, int ei, int epi) : summon_time(s), enemy_idx(ei), enemy_path_idx(epi) {};
    };
    std::list<listnode>::iterator _head;
    std::list<listnode>::iterator _next;
    std::list<listnode> _enemylist;
};

struct PathLibrary {
    struct PathList {
        PathList(int path_i);
        struct pathnode {
            int x;
            int y;
            pathnode(int x, int y) :x(x),y(y) {};
        };
        bool CheckPoint(glm::vec3 pos);
        glm::vec3 GetMoveDirection();
        glm::vec3 GetSummonPosition();
        std::list<pathnode>::iterator _head;
        std::list<pathnode>::iterator _next;
        std::list<pathnode> _pathlist;
        const float zero = 0.05f;
    };

    PathLibrary();
    PathList getPathList(int idx);

    std::vector<PathList> _pathlib;
};

class Map {
public:
    Map(std::string pfx);
    ~Map() = default;
    void draw(const std::unique_ptr<GLSLProgram>& ref_shader);
    void select_draw(const std::unique_ptr<GLSLProgram>& ref_shader);

    static glm::vec3 point2WorldPosition(float x, float y);

    std::unique_ptr<SummonList> _summonlist;
    std::unique_ptr<PathLibrary> _pathlib;
    std::vector<std::unique_ptr<ModelOnWorld>> _blocks;
    
};



