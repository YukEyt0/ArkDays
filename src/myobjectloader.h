#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <glm/ext.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "./base/application.h"
#include "./base/glsl_program.h"
#include "./base/vertex.h"

class myobjectloader {
public:
    myobjectloader() {};
    ~myobjectloader() = default;
    bool LoadFile(std::string path) {
        std::ifstream file(path);
        if(!file.is_open())
            return false;
        _vertices.clear();
        _indicies.clear();

        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texCoord;
        
        bool state = false;
        std::string line;
        while (std::getline(file,line)) {
            std::stringstream line_stream(line);
            std::string token;
            line_stream >> token;
            if (token == "v") {
                float x,y,z;
                line_stream >> x >> y >> z;
                positions.emplace_back(x,y,z);
            } else if (token=="vt") {
                float x,y;
                line_stream >> x >> y;
                texCoord.emplace_back(x,y);
            } else if (token=="vn") {
                float x,y,z;
                line_stream >> x >> y >> z;
                normals.emplace_back(x,y,z);
            } else if (token=="f") {
                std::vector<Vertex> verts;
                GetVertices(verts,positions,texCoord,normals,line_stream);
                for(auto it : verts) {
                    _vertices.emplace_back(it);
                }
                std::vector<GLuint> inds;
                Triangluation(inds,verts);
                for(auto it : inds) {
                    auto indnum = (GLuint)(_vertices.size()-verts.size()) + it;
                    _indicies.emplace_back(indnum);
                }
            }
        }
        return true;
    };

    bool DeriveFile(std::string path) {
        return false;
    }

    std::vector<Vertex> getVertices() {
        return _vertices;
    }

    std::vector<GLuint> getIndicies() {
        return _indicies;
    }

private:
    std::vector<Vertex> _vertices;
    std::vector<GLuint> _indicies;
    void GetVertices(std::vector<Vertex>& verts,
        const std::vector<glm::vec3>& poses,
        const std::vector<glm::vec2>& texes,
        const std::vector<glm::vec3>& norms,
        std::stringstream& line) {
            auto getelement = []<typename T>(const std::vector<T> &eles, std::string &index) {
                int idx = std::stoi(index);
                idx = (idx<0)?(int(eles.size())+idx) :--idx;
                return eles[idx];
            };
            bool no_normal = false;
            std::vector<std::string> sface, svert;
            Vertex vert;
            std::string str;
            while (line>>str) {
                sface.push_back(str);
            }
            for (auto it : sface) {
                int type;
                std::string item;
                std::stringstream it_stream(it);
                svert.clear();
                while(std::getline(it_stream,item,'/')) {
                    svert.push_back(item);
                }
                if(svert.size()==3) 
                    type=(svert[1]!="") ? 4 : 3;
                else 
                    type = svert.size();

                switch(type) {
                    case 1:
                        vert.position = getelement(poses,svert[0]);
                        vert.texCoord = glm::vec2(0.0f,0.0f);
                        no_normal = true;
                        verts.push_back(vert);
                        break;
                    case 2:
                        vert.position = getelement(poses,svert[0]);
                        vert.texCoord = getelement(texes,svert[1]);
                        no_normal = true;
                        verts.push_back(vert);
                        break;
                    case 3:
                        vert.position = getelement(poses,svert[0]);
                        vert.texCoord = glm::vec2(0.0f,0.0f);
                        vert.normal = getelement(norms,svert[2]);
                        verts.push_back(vert);
                        break;
                    case 4:
                        vert.position = getelement(poses,svert[0]);
                        vert.texCoord = getelement(texes,svert[1]);
                        vert.normal = getelement(norms,svert[2]);
                        verts.push_back(vert);
                        break;
                    default: break;           
                }
            }
            if(no_normal) {
                auto a = verts[0].position-verts[1].position;
                auto b = verts[2].position-verts[1].position;
                auto norm = glm::cross(a,b);
                for(int i = 0; i<int(verts.size());++i) {
                    verts[i].normal = norm;
                }   
            }
        }

    void Triangluation(std::vector<GLuint>& out_indices,
        const std::vector<Vertex>& in_verts) {
                if(in_verts.size() <3)
                    return;
                else if (in_verts.size()==3) {
                    out_indices.emplace_back(0);
                    out_indices.emplace_back(1);
                    out_indices.emplace_back(2);
                    return;
                }
                std::vector<Vertex> verts = in_verts;
                while (true) {
                    for(int i=0;i<int(verts.size());++i) {
                        int idx1 = (verts.size()+i-1)%verts.size();
                        int idx2 = i;
                        int idx3 = (verts.size()+i+1)%verts.size();
                        Vertex v1 = verts[idx1];
                        Vertex v2 = verts[idx2];
                        Vertex v3 = verts[idx3];
                        if(verts.size()==3) {
                            out_indices.emplace_back(idx2);
                            out_indices.emplace_back(idx1);
                            out_indices.emplace_back(idx3);
                            verts.clear();
                            break;
                        }
                        if(verts.size()==4) {
                            out_indices.emplace_back(idx2);
                            out_indices.emplace_back(idx1);
                            out_indices.emplace_back(idx3);
                            int idxtmp;
                            glm::vec3 tmp;
                            for(int j=0;j<verts.size();++j) {
                                if(j!=idx1&&j!=idx2&&j!=idx3) {
                                        idxtmp = j;
                                        break;
                                    }
                            }
                            out_indices.emplace_back(idx1);
                            out_indices.emplace_back(idx3);
                            out_indices.emplace_back(idxtmp);
                            verts.clear();
                            break;
                        }
                        const auto PI = glm::pi<float>();
                        auto same_side = [](Vertex a, Vertex b, Vertex c, Vertex v) -> bool{
                            auto ab = b.position-a.position;
                            auto ac = c.position-a.position;
                            auto av = v.position-a.position;
                            auto v1 = glm::cross(ab,ac);
                            auto v2 = glm::cross(ab,av);
                            return glm::dot(v1,v2) >=0;
                        };
                        auto in_triangle = [&v1, &v2, &v3,&same_side](Vertex v) -> bool {
                            return same_side(v1,v2,v3,v) &&
                                same_side(v2,v3,v1,v) &&
                                same_side(v3,v1,v2,v);
                        };
                        auto angle = glm::angle(v1.position-v2.position,v3.position-v2.position); 
                        if (angle<=0||angle>=PI)
                            continue;
                        bool is_in;
                        for(int j = 0; j < verts.size();++j) {
                            if(j!=idx1&&j!=idx2&&j!=idx3&&in_triangle(verts[j])) {
                                    is_in = true;
                                    break;
                                }
                        }
                        if(is_in)
                            continue;;
                        out_indices.emplace_back(idx2);
                        out_indices.emplace_back(idx1);
                        out_indices.emplace_back(idx3);
                        verts.erase(verts.begin()+idx2);
                        i -= 1;
                    }
                    if(out_indices.size()==0)
                        break;
                    if(verts.size()==0)
                        break;
                }
            }
};

