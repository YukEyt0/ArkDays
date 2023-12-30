#pragma once

#include <map>
#include <fstream>
#include <string>


#include "material.h"
#include "basicshapes.h"
#include "resource.h"
#include "./base/transform.h"
#include "./base/texture_cubemap.h"
#include "./base/texture2d.h"
#include "./base/application.h"

static Material _default_material = Material(glm::vec3(0.05f),glm::vec3(1.0f),glm::vec3(1.0f),10.0f);

extern const GLfloat _mapscale;

class ModelOnWorld {
public:
    ModelOnWorld() = delete;
    ModelOnWorld(std::unique_ptr<MyObject>& object, int textrue_idx = 0, Material material = _default_material);
    ModelOnWorld(const ModelOnWorld & rhs) = delete;

    void draw() {
        _object->draw();
        //_object->drawBoundingBox();
    }

    void draw_select() {
        _object->drawBoundingBox();
    }

    int DeriveObjFile(const std::string& dir, const int& offset=0, bool append = false);
    bool CollisionDetection(const std::unique_ptr<ModelOnWorld>& other);
    int _textrue_idx;
    std::unique_ptr<MyObject>& _object;
    Material _material;
    Transform _transform;
    Transform _local_transform;
    int id;
    static int index;
    static std::map<int,std::unique_ptr<ModelOnWorld>&> _model_map;
    bool enable_textrue = false;
    bool is_placed = false;
};

