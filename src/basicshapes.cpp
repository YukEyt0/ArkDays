#include "basicshapes.h"

Cube::Cube() {
    _vertices.emplace_back(Vertex({-1.0f, -1.0f, -1.0f},{ 0.0f,  0.0f, -1.0f},{0.0f,  0.0f}));
    _vertices.emplace_back(Vertex({ 1.0f, -1.0f, -1.0f},{ 0.0f,  0.0f, -1.0f},{1.0f,  0.0f}));
    _vertices.emplace_back(Vertex({ 1.0f,  1.0f, -1.0f},{ 0.0f,  0.0f, -1.0f},{1.0f,  1.0f}));
    _vertices.emplace_back(Vertex({-1.0f,  1.0f, -1.0f},{ 0.0f,  0.0f, -1.0f},{0.0f,  1.0f}));

    _indices.emplace_back(0);
    _indices.emplace_back(1);
    _indices.emplace_back(2);
    _indices.emplace_back(2);
    _indices.emplace_back(3);
    _indices.emplace_back(0);

    _vertices.emplace_back(Vertex({-1.0f, -1.0f,  1.0f},{ 0.0f,  0.0f, 1.0f},{1.0f,  0.0f}));
    _vertices.emplace_back(Vertex({ 1.0f, -1.0f,  1.0f},{ 0.0f,  0.0f, 1.0f},{1.0f,  0.0f}));
    _vertices.emplace_back(Vertex({ 1.0f,  1.0f,  1.0f},{ 0.0f,  0.0f, 1.0f},{1.0f,  1.0f}));
    _vertices.emplace_back(Vertex({-1.0f,  1.0f,  1.0f},{ 0.0f,  0.0f, 1.0f},{0.0f,  0.0f}));

    _indices.emplace_back(4);
    _indices.emplace_back(5);
    _indices.emplace_back(6);
    _indices.emplace_back(6);
    _indices.emplace_back(7);
    _indices.emplace_back(4);

    _vertices.emplace_back(Vertex({-1.0f,  1.0f,  1.0f},{-1.0f,  0.0f,  0.0f},{0.0f,  0.0f}));
    _vertices.emplace_back(Vertex({-1.0f,  1.0f, -1.0f},{-1.0f,  0.0f,  0.0f},{1.0f,  1.0f}));
    _vertices.emplace_back(Vertex({-1.0f, -1.0f, -1.0f},{-1.0f,  0.0f,  0.0f},{0.0f,  1.0f}));
    _vertices.emplace_back(Vertex({-1.0f, -1.0f,  1.0f},{-1.0f,  0.0f,  0.0f},{1.0f,  0.0f}));

    _indices.emplace_back(8);
    _indices.emplace_back(9);
    _indices.emplace_back(10);
    _indices.emplace_back(10);
    _indices.emplace_back(11);
    _indices.emplace_back(8);

    _vertices.emplace_back(Vertex({1.0f,  1.0f,  1.0f},{1.0f,  0.0f,  0.0f},{0.0f,  0.0f}));
    _vertices.emplace_back(Vertex({1.0f,  1.0f, -1.0f},{1.0f,  0.0f,  0.0f},{1.0f,  1.0f}));
    _vertices.emplace_back(Vertex({1.0f, -1.0f, -1.0f},{1.0f,  0.0f,  0.0f},{0.0f,  1.0f}));
    _vertices.emplace_back(Vertex({1.0f, -1.0f,  1.0f},{1.0f,  0.0f,  0.0f},{1.0f,  0.0f}));

    _indices.emplace_back(12);
    _indices.emplace_back(13);
    _indices.emplace_back(14);
    _indices.emplace_back(14);
    _indices.emplace_back(15);
    _indices.emplace_back(12);

    _vertices.emplace_back(Vertex({-1.0f, -1.0f, -1.0f},{ 0.0f, -1.0f, 0.0f},{0.0f,  1.0f}));
    _vertices.emplace_back(Vertex({ 1.0f, -1.0f, -1.0f},{ 0.0f, -1.0f, 0.0f},{1.0f,  1.0f}));
    _vertices.emplace_back(Vertex({ 1.0f, -1.0f,  1.0f},{ 0.0f, -1.0f, 0.0f},{1.0f,  0.0f}));
    _vertices.emplace_back(Vertex({-1.0f, -1.0f,  1.0f},{ 0.0f, -1.0f, 0.0f},{0.0f,  0.0f}));

    _indices.emplace_back(16);
    _indices.emplace_back(17);
    _indices.emplace_back(18);
    _indices.emplace_back(18);
    _indices.emplace_back(19);
    _indices.emplace_back(16);

    _vertices.emplace_back(Vertex({-1.0f, 1.0f, -1.0f},{ 0.0f, 1.0f, 0.0f},{0.0f,  1.0f}));
    _vertices.emplace_back(Vertex({ 1.0f, 1.0f, -1.0f},{ 0.0f, 1.0f, 0.0f},{1.0f,  1.0f}));
    _vertices.emplace_back(Vertex({ 1.0f, 1.0f,  1.0f},{ 0.0f, 1.0f, 0.0f},{1.0f,  0.0f}));
    _vertices.emplace_back(Vertex({-1.0f, 1.0f,  1.0f},{ 0.0f, 1.0f, 0.0f},{0.0f,  0.0f}));

    _indices.emplace_back(20);
    _indices.emplace_back(21);
    _indices.emplace_back(22);
    _indices.emplace_back(22);
    _indices.emplace_back(23);
    _indices.emplace_back(20);

    computeBoundingBox();

    initGLResources();

    initBoxGLResources();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cleanup();
        throw std::runtime_error("OpenGL Error: " + std::to_string(error));
    }
};

Sphere::Sphere(int prec = 48) {

    auto numVertices = (prec+1)*(prec+1);
    auto numIndices = prec * prec * 6;
    _vertices.resize(numVertices);
    _indices.resize(numIndices);
    for(int i = 0; i<prec+1;i++) {
        for(int j=0; j<prec+1;j++) {
            float y = glm::cos(glm::radians(180.0f-i*180.0f/prec));
            float x = -glm::cos(glm::radians((j*360.0f/prec)))*glm::abs(glm::cos(glm::asin(y)));
            float z = glm::sin(glm::radians((j*360.0f/prec)))*glm::abs(glm::cos(glm::asin(y)));
            _vertices.at(i*(prec+1)+j) = Vertex(glm::vec3(x,y,z),glm::vec3(x,y,z),glm::vec2(float(j)/prec,float(i)/prec));
        }
    }
    for(int i = 0; i< prec; i++) {
        for(int j = 0; j<prec; j++) {
            _indices.at(6*(i*prec+j)+0) = i * (prec+1) + j;
            _indices.at(6*(i*prec+j)+1) = i * (prec+1) + j+1;
            _indices.at(6*(i*prec+j)+2) = (i+1) * (prec+1) + j;
            _indices.at(6*(i*prec+j)+3) = i * (prec+1) + j + 1;
            _indices.at(6*(i*prec+j)+4) = (i+1) * (prec+1) + j+1;
            _indices.at(6*(i*prec+j)+5) = (i+1) * (prec+1) + j;
        }
    }

    computeBoundingBox();

    initGLResources();

    initBoxGLResources();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cleanup();
        throw std::runtime_error("OpenGL Error: " + std::to_string(error));
    }
};

Cylinder::Cylinder(int prec=40) {
    auto numVertices = (prec+1)*4+2;
    auto numIndices = prec * 3 * 4;
    _vertices.resize(numVertices);
    _indices.resize(numIndices);

    _vertices[0].position = glm::vec3(0.0f);
    _vertices[0].normal = glm::vec3(0.0f,0.0f,-1.0f);
    _vertices[0].texCoord = glm::vec2(0.5f,1.0f);
    _vertices[numVertices-1].position = glm::vec3(0.0f,0.0f,1.0f);
    _vertices[numVertices-1].normal = glm::vec3(0.0f,0.0f,1.0f);
    _vertices[numVertices-1].texCoord = glm::vec2(0.5f,1.0f);

    for(int i = 0; i<=prec;++i) {
        auto x = glm::cos(glm::radians(i*360.0f/prec));
        auto y = -glm::sin(glm::radians(i*360.0f/prec));
        auto z = float(0);
        _vertices[i+1] = Vertex(glm::vec3(x,y,z),glm::vec3(0,0,-1),glm::vec2(float(i)/prec,0));
        _vertices[i+2+prec] = Vertex(glm::vec3(x,y,z),glm::vec3(x,y,0),glm::vec2(float(i)/prec,0));
        _vertices[i+1+2*(prec+1)] = Vertex(_vertices[i+2+prec]);
        _vertices[i+1+2*(prec+1)].position.z = 1.0f;
        _vertices[i+1+2*(prec+1)].texCoord.y = 1.0f;
        _vertices[i+1+3*(prec+1)] = Vertex(_vertices[i+1]);
        _vertices[i+1+3*(prec+1)].position.z = 1.0f;
        _vertices[i+1+3*(prec+1)].normal = glm::vec3(0.0f,0.0f,1.0f);
        if(i!=prec) {
            _indices[i*12] = 0;
            _indices[i*12+1] = i+1;
            _indices[i*12+2] = i+2;

            _indices[i*12+3] = numVertices-1;
            _indices[i*12+4] = i+2+3*(prec+1);
            _indices[i*12+5] = i+1+3*(prec+1);

            _indices[i*12+6] = i+2+(prec+1);
            _indices[i*12+7] = i+1+(prec+1);
            _indices[i*12+8] = i+1+2*(prec+1);

            _indices[i*12+9]=i+2+(prec+1);
            _indices[i*12+10] = i+1+2*(prec+1);
            _indices[i*12+11] = i+2+2*(prec+1);
        }
    }

    computeBoundingBox();

    initGLResources();

    initBoxGLResources();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cleanup();
        throw std::runtime_error("OpenGL Error: " + std::to_string(error));
    }
}

Cone::Cone(int prec=40) {
    auto numVertices = (prec+1)*3+1;
    auto numIndices = prec * 2 * 3;
    _vertices.resize(numVertices);
    _indices.resize(numIndices);

    _vertices[0].position = glm::vec3(0.0f);
    _vertices[0].normal = glm::vec3(0.0f,0.0f,-1.0f);
    _vertices[0].texCoord = glm::vec2(0.5f,1.0f);
    float normalize = 0.5f;

    for(int i = 0; i<=prec;++i) {
        auto x = glm::cos(glm::radians(i*360.0f/prec));
        auto y = -glm::sin(glm::radians(i*360.0f/prec));
        auto z = float(0);
        _vertices[i+1] = Vertex(glm::vec3(x,y,z),glm::vec3(0,0,-1),glm::vec2(float(i)/prec,0));
        _vertices[i+2+prec] = Vertex(glm::vec3(x,y,z),glm::vec3(x,y,0),glm::vec2(float(i)/prec,0));
        _vertices[i+1+2*(prec+1)] = Vertex(glm::vec3(0.0f,0.0f,1.0f),glm::vec3(normalize*x,normalize*y,0.5f),glm::vec2(0.5f,0.5f));
        if(i!=prec) {
            _indices[i*6] = 0;
            _indices[i*6+1] = i+1;
            _indices[i*6+2] = i+2;

            _indices[i*6+3] = i+1+2*(prec+1);
            _indices[i*6+4] = i+2+(prec+1);
            _indices[i*6+5] = i+1+(prec+1);
        }
    }

    computeBoundingBox();

    initGLResources();

    initBoxGLResources();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cleanup();
        throw std::runtime_error("OpenGL Error: " + std::to_string(error));
    }
}

Prism::Prism(int prec=3, float scale=1.0f) {
        auto numVertices = (prec+1)*4+2;
    auto numIndices = prec * 3 * 4;
    _vertices.resize(numVertices);
    _indices.resize(numIndices);

    auto height = scale==1.0f?1.0f:(1/1-scale);
    auto radius = 1.0f;
    float normalize  = height * height / (height*height+radius*radius);

    _vertices[0].position = glm::vec3(0.0f);
    _vertices[0].normal = glm::vec3(0.0f,0.0f,-1.0f);
    _vertices[0].texCoord = glm::vec2(0.5f,1.0f);
    _vertices[numVertices-1].position = glm::vec3(0.0f,0.0f,1.0f);
    _vertices[numVertices-1].normal = glm::vec3(0.0f,0.0f,1.0f);
    _vertices[numVertices-1].texCoord = glm::vec2(0.5f,1.0f);

    for(int i = 0; i<=prec;++i) {
        auto x = glm::cos(glm::radians(i*360.0f/prec));
        auto y = -glm::sin(glm::radians(i*360.0f/prec));
        auto z = float(0);
        _vertices[i+1] = Vertex(glm::vec3(x,y,z),glm::vec3(0,0,-1),glm::vec2(float(i)/prec,0));
        _vertices[i+2+prec] = Vertex(glm::vec3(x,y,z),glm::vec3(normalize*x,normalize*y,radius*height/(height*height+radius*radius)),glm::vec2(float(i)/prec,0));
        _vertices[i+1+2*(prec+1)] = Vertex(_vertices[i+2+prec]);
        _vertices[i+1+2*(prec+1)].position = glm::vec3(x*1.0f*scale,y*1.0f*scale,1.0f);
        _vertices[i+1+2*(prec+1)].texCoord.y = 1.0f;
        _vertices[i+1+3*(prec+1)] = Vertex(_vertices[i+1+2*(prec+1)]);
        _vertices[i+1+3*(prec+1)].normal = glm::vec3(0.0f,0.0f,1.0f);
        _vertices[i+1+3*(prec+1)].texCoord.y = 0.0f;
        if(i!=prec) {
            _indices[i*12] = 0;
            _indices[i*12+1] = i+1;
            _indices[i*12+2] = i+2;

            _indices[i*12+3] = numVertices-1;
            _indices[i*12+4] = i+2+3*(prec+1);
            _indices[i*12+5] = i+1+3*(prec+1);

            _indices[i*12+6] = i+2+(prec+1);
            _indices[i*12+7] = i+1+(prec+1);
            _indices[i*12+8] = i+1+2*(prec+1);

            _indices[i*12+9]=i+2+(prec+1);
            _indices[i*12+10] = i+1+2*(prec+1);
            _indices[i*12+11] = i+2+2*(prec+1);
        }
    }

    computeBoundingBox();

    initGLResources();

    initBoxGLResources();

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        cleanup();
        throw std::runtime_error("OpenGL Error: " + std::to_string(error));
    }
}