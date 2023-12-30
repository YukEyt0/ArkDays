#pragma once

#include "./base/application.h"

struct Material {
    Material(glm::vec3 _ka = glm::vec3(0.05f), glm::vec3 _kd=glm::vec3(1.0f), glm::vec3 _ks=glm::vec3(1.0f), float _ns=10.0f)
    :ka(_ka),kd(_kd),ks(_ks),ns(_ns){};
    glm::vec3 ka;
    glm::vec3 kd;
    glm::vec3 ks;
    float ns;
};