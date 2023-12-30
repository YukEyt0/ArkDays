#pragma once

#include "mymodel.h"

#include "./base/texture_cubemap.h"
#include "./base/texture2d.h"

const unsigned short bitmap[7][11] = {
    {3,3,3,3,3,3,3,3,3,3,3},
    {3,2,2,2,0,0,0,0,0,1,4},
    {3,2,0,0,0,2,2,2,2,3,3},
    {5,0,0,6,0,0,0,0,0,1,4},
    {3,2,0,0,0,2,2,2,2,3,3},
    {3,2,2,2,0,0,0,0,0,1,4},
    {3,3,3,3,3,3,3,3,3,3,3}
};

const short path[3][7][2] = {
    {{1,10},{1,4},{2,4},{2,2},{3,2},{3,0}},
    {{3,10},{3,4},{2,4},{2,2},{3,2},{3,0}},
    {{5,10},{5,4},{4,4},{4,2},{3,2},{3,0}}
};

const int summonlist[][3] = {
    {6,0,0},
    {12,0,2},
    {13,0,2},
    {18,0,1},
    {19,0,1},
    {24,0,1},
    {23,0,1},
    {32,5,0},
    {35,1,2},
    {39,5,0},
    {42,1,2},
    {50,4,0},
    {50,4,2},
    {53,1,1},
    {56,1,1},
    {65,5,0},
    {65,5,1},
    {71,1,0},
    {71,5,2},
    {75,0,1},
    {76,0,1},
    {77,0,1},
    {78,0,1},
    {79,0,1},
    {80,0,1},
    {81,0,1},
    {82,0,1},
    {83,0,1},
    {84,0,1},
    {86,1,0},
    {86,1,2},
    {94,4,0},
    {94,4,2},
    {99,3,1},
    {107,0,2},
    {108,0,2},
    {108,3,1},
    {109,0,2},
    {116,0,0},
    {117,0,0},
    {118,0,0},
};

const std::vector<std::string> grassTextureRelPaths = {
    "texture/Grass.bmp", "texture/Grass.bmp",  "texture/Grass.bmp",
    "texture/Grass.bmp",  "texture/Grass.bmp", "texture/Grass.bmp"};

const std::vector<std::string> woodTextureRelPaths = {
    "texture/wood.png","texture/wood.png","texture/wood.png",
    "texture/wood.png","texture/wood.png","texture/wood.png"
};

const std::vector<std::string> rockTextureRelPaths = {
    "texture/rock.png","texture/rock.png","texture/rock.png",
    "texture/rock.png","texture/rock.png","texture/rock.png"
};

const std::vector<std::string> stoneTextureRelPaths = {
    "texture/stone.bmp","texture/stone.bmp","texture/stone.bmp",
    "texture/stone.bmp","texture/stone.bmp","texture/stone.bmp"
};

const std::vector<std::string> redzoneTextureRelPaths = {
    "texture/red_zone.png","texture/red_zone.png","texture/red_zone.png",
    "texture/red_zone.png","texture/red_zone.png","texture/red_zone.png"
};

const std::vector<std::string> bluezoneTextureRelPaths = {
    "texture/blue_zone.png","texture/blue_zone.png","texture/blue_zone.png",
    "texture/blue_zone.png","texture/blue_zone.png","texture/blue_zone.png"
};

const std::vector<std::string> nuclearTextureRelPaths = {
    "texture/nuclear.png","texture/nuclear.png","texture/nuclear.png",
    "texture/nuclear.png","texture/nuclear.png","texture/nuclear.png"
};

class ModelLibrary {
public:
    ModelLibrary(std::string prefix_path) {
        _models.resize(modle_paths.size());
        for(int i  = 0; i < _models.size();++i) {
            _models[i].reset(new MyObject(prefix_path+modle_paths[i]));
            std::cout << "("<< i+1 << "/" << _models.size() << ")" << "load " << prefix_path+modle_paths[i] << "success!" << std::endl;
        }
    }

    std::unique_ptr<MyObject>& getModel(int index) {
        return _models.at(index);
    }
private:
    std::vector<std::unique_ptr<MyObject>> _models;
};



class ShapeLibrary {
public:
    ShapeLibrary() {
        _shapes.resize(4);
        _shapes[0].reset(new Cube());
        _shapes[1].reset(new Sphere(40));
        _shapes[2].reset(new Cone(40));
        _shapes[3].reset(new Cylinder(40));
    }

    int AddPrism(int edge, float scale) {
        std::unique_ptr<MyObject> o;
        o.reset(new Prism(edge,scale));
        _shapes.push_back(std::move(o));
        return _shapes.size()-1;
    }

    std::unique_ptr<MyObject>& getShape(int idx) {
        return _shapes.at(idx);
    }

private:
    std::vector<std::unique_ptr<MyObject>> _shapes;
};



class TextrueLibrary {
public:
    TextrueLibrary(std::string prefix_path) {
        _textures.resize(7);
        std::vector<std::string> grassTextureFullPaths;
        for (size_t i = 0; i < grassTextureRelPaths.size(); ++i) {
            grassTextureFullPaths.push_back(prefix_path+grassTextureRelPaths[i]);
        }
        _textures[0].reset(new ImageTextureCubemap(grassTextureFullPaths));

        std::vector<std::string> stoneTextureFullPaths;
        for (size_t i = 0; i < stoneTextureRelPaths.size(); ++i) {
            stoneTextureFullPaths.push_back(prefix_path+stoneTextureRelPaths[i]);
        }
        _textures[1].reset(new ImageTextureCubemap(stoneTextureFullPaths));

        std::vector<std::string> woodTextureFullPaths;
        for (size_t i = 0; i < woodTextureRelPaths.size(); ++i) {
            woodTextureFullPaths.push_back(prefix_path+woodTextureRelPaths[i]);
        }
        _textures[2].reset(new ImageTextureCubemap(woodTextureFullPaths));

        std::vector<std::string> rockTextureFullPaths;
        for (size_t i = 0; i < rockTextureRelPaths.size(); ++i) {
            rockTextureFullPaths.push_back(prefix_path+rockTextureRelPaths[i]);
        }
        _textures[3].reset(new ImageTextureCubemap(rockTextureFullPaths));

        std::vector<std::string> redzoneTextureFullPaths;
        for (size_t i = 0; i < redzoneTextureRelPaths.size(); ++i) {
            redzoneTextureFullPaths.push_back(prefix_path+redzoneTextureRelPaths[i]);
        }
        _textures[4].reset(new ImageTextureCubemap(redzoneTextureFullPaths));

        std::vector<std::string> bluezoneTextureFullPaths;
        for (size_t i = 0; i < bluezoneTextureRelPaths.size(); ++i) {
            bluezoneTextureFullPaths.push_back(prefix_path+bluezoneTextureRelPaths[i]);
        }
        _textures[5].reset(new ImageTextureCubemap(bluezoneTextureFullPaths));

        std::vector<std::string> nuclearTextureFullPaths;
        for (size_t i = 0; i < grassTextureRelPaths.size(); ++i) {
            nuclearTextureFullPaths.push_back(prefix_path+nuclearTextureRelPaths[i]);
        }
        _textures[6].reset(new ImageTextureCubemap(nuclearTextureFullPaths));
    }

    std::unique_ptr<TextureCubemap>& getTextureRef(int index) {
        return _textures.at(index);
    }
private:
    std::vector<std::unique_ptr<TextureCubemap>> _textures;
};

