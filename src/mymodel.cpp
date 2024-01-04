#include "mymodel.h"
#include <iostream>
#include <string>
#include <fstream>

ModelOnWorld::ModelOnWorld(std::unique_ptr<MyObject>& object, int textrue_idx, Material material):
    _textrue_idx(textrue_idx), _material(material), _object(object){
        auto box = object->getBoundingBox();
        auto min = box.min;
        auto max = box.max;
        auto x_length = max.x-min.x;
        auto y_length = max.y-min.y;
        auto z_length = max.z-min.z;
        auto max_length = std::max(x_length,std::max(y_length,z_length));
        _transform.position -= min;
        _transform.scale *= glm::vec3{_mapscale/max_length,_mapscale/max_length,_mapscale/max_length};
        id = ++index;
}

bool ModelOnWorld::CollisionDetection(const std::unique_ptr<ModelOnWorld>& other) {
    //AABB
    auto box1 = this->_object->getBoundingBox();
    auto box2 = other->_object->getBoundingBox();
    auto matrix1 = this->_transform.getLocalMatrix();
    auto matrix2 = other->_transform.getLocalMatrix();
    auto tmax1 = matrix1 * glm::vec4{box1.max,1.0f};
    auto tmin1 = matrix1 * glm::vec4{box1.min,1.0f};
    auto tmax2 = matrix2 * glm::vec4{box2.max,1.0f};
    auto tmin2 = matrix2 * glm::vec4{box2.min,1.0f};
    
    auto max1 = glm::vec3{std::max(tmax1.x,tmin1.x),std::max(tmax1.y,tmin1.y),std::max(tmax1.z,tmin1.z)};
    auto min1 = glm::vec3{std::min(tmax1.x,tmin1.x),std::min(tmax1.y,tmin1.y),std::min(tmax1.z,tmin1.z)};
    auto max2 = glm::vec3{std::max(tmax2.x,tmin2.x),std::max(tmax2.y,tmin2.y),std::max(tmax2.z,tmin2.z)};
    auto min2 = glm::vec3{std::min(tmax2.x,tmin2.x),std::min(tmax2.y,tmin2.y),std::min(tmax2.z,tmin2.z)};

    auto xaxis = glm::abs(std::max(max1.x,max2.x)-std::min(min1.x,min2.x)) < glm::abs(max1.x+max2.x-min1.x-min2.x);
    auto yaxis = glm::abs(std::max(max1.y,max2.y)-std::min(min1.y,min2.y)) < glm::abs(max1.y+max2.y-min1.y-min2.y);
    auto zaxis = glm::abs(std::max(max1.z,max2.z)-std::min(min1.z,min2.z)) < glm::abs(max1.z+max2.z-min1.z-min2.z);
    return xaxis&&yaxis&&zaxis;
}

int ModelOnWorld::DeriveObjFile(const std::string& dir, const int & offset, bool append) {
    std::ofstream os;
    if(append) 
        os.open(dir,std::ios::app|std::ios::binary);
    else
        os.open(dir,std::ios::out|std::ios::binary);
    if(!os.is_open())
        throw;
    auto vertex_transform = _transform.getLocalMatrix();
    auto normal_transform = glm::transpose(glm::inverse(glm::mat3(vertex_transform)));
    for(auto it : this->_object->getVertices()) {
        auto trans_pos = glm::vec3{vertex_transform*glm::vec4(it.position,1.0f)};
        auto trans_norm = normal_transform * it.normal;
        auto textcoord = it.texCoord;
        os << "v "<< (float)trans_pos.x << " " << (float)trans_pos.y << " " << (float)trans_pos.z << "\n";
        os << "vn " << (float)trans_norm.x << " " << (float)trans_norm.y << " " << (float)trans_norm.z << "\n";
        os << "vt "<< (float)textcoord.x << " " << (float)textcoord.y << "\n";
    }
    auto indices = this->_object->getIndices();
    for(int i = 0; i<indices.size();i+=3) {
        os << "f " <<" "<< indices[i] + 1 + offset << "/" << indices[i] + 1 + offset << "/" << indices[i] + 1 + offset<< " ";
        os << indices[i + 1] + 1 + offset << "/" << indices[i + 1] + 1 + offset<< "/" << indices[i + 1] + 1 + offset<< " ";
        os << indices[i + 2] + 1 + offset<< "/" << indices[i + 2] + 1 + offset<< "/" << indices[i + 2] + 1 + offset<< "\n";
    }
    os.close();
    return _object->getVertexCount();
}