#pragma once
#include "./base/vertex.h"
#include "./base/application.h"

#include "myobject.h"

#include <vector>
#include <memory>


class Cube : public MyObject{
public:
    Cube();
};

class Sphere : public MyObject{
public:
    Sphere(int prec);
};

class Cylinder : public MyObject{
public:
    Cylinder(int prec);
};

class Cone : public MyObject {
public:
    Cone(int prec);
};

class Prism : public MyObject {
public:
    Prism(int edge, float scale);
};
