#pragma once

#include <string>
#include <vector>
#include<memory>

#include "./base/bounding_box.h"
#include "./base/gl_utility.h"
#include "./base/transform.h"
#include "./base/vertex.h"

/*
const std::vector<std::string> modle_paths = {
    "obj/knot.obj",
    "obj/bunny.obj",
    "obj/rock.obj",
    "obj/arrow.obj",
    "obj/cube.obj",
    "obj/wolf.obj",
    "obj/cat.obj",
    "obj/vanguard.obj",
    "obj/defender.obj",
    "obj/medic.obj",
    "obj/caster.obj",
    "obj/sniper.obj"
};
*/

const std::vector<std::string> modle_paths = {
    "obj/knot.obj",
    "obj/knot.obj",
    "obj/knot.obj",
    "obj/knot.obj",
    "obj/knot.obj",
    "obj/knot.obj",
    "obj/knot.obj",
    "obj/knot.obj",
    "obj/rock.obj",
    "obj/wolf.obj",
    "obj/cat.obj",
    "obj/bunny.obj",
    "obj/rock.obj",
    "obj/drone.obj"
};
class MyObject {
public:
    MyObject() {};
    MyObject(const std::string& filepath);

    MyObject(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    MyObject(MyObject&& rhs) = delete;

    virtual ~MyObject();

    GLuint getVao() const;

    GLuint getBoundingBoxVao() const;

    size_t getVertexCount() const;

    size_t getFaceCount() const;

    BoundingBox getBoundingBox() const;

    virtual void draw() const;

    virtual void drawBoundingBox() const;

    const std::vector<uint32_t>& getIndices() const {
        return _indices;
    }
    const std::vector<Vertex>& getVertices() const {
        return _vertices;
    }
    const Vertex& getVertex(int i) const {
        return _vertices[i];
    }


protected:
    // vertices of the table represented in MyModel's own coordinate
    std::vector<Vertex> _vertices;
    std::vector<uint32_t> _indices;

    // bounding box
    BoundingBox _boundingBox;

    // opengl objects
    GLuint _vao = 0;
    GLuint _vbo = 0;
    GLuint _ebo = 0;

    GLuint _boxVao = 0;
    GLuint _boxVbo = 0;
    GLuint _boxEbo = 0;

    void computeBoundingBox();

    void initGLResources();

    void initBoxGLResources();

    void cleanup();
};

