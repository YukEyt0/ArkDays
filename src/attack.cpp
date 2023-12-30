#include "attack.h"

#include "./base/application.h"

#include "basicshapes.h"

extern std::unique_ptr<ShapeLibrary> _shape_lib;
extern std::unique_ptr<TextrueLibrary> _texture_lib;

Projectile::Projectile(std::unique_ptr<ModelOnWorld>& target, glm::vec3 pos, Projectile::ProjectStyle style)
: _target(target), _style(style) {
    std::unique_ptr<ModelOnWorld> model;
    switch(style) {
        case Projectile::CUBE :
            model.reset(new ModelOnWorld(_shape_lib->getShape(0),0));
            break;
        case Projectile::CONE :
            model.reset(new ModelOnWorld(_shape_lib->getShape(2),0));
            break;
        case Projectile::SPHERE :
            model.reset(new ModelOnWorld(_shape_lib->getShape(1),0));
            break;
        default:
            model.reset(new ModelOnWorld(_shape_lib->getShape(3),0));
            break;
    }
    _model = std::move(model);
    _model->_transform.position = pos;
    _model->_transform.lookBack(target->_transform.position);
}
Projectile::Projectile(const Projectile& rhs)
: _target(rhs._target), _style(rhs._style) {
    _model->_transform = rhs._model->_transform;
    std::unique_ptr<ModelOnWorld> model;
    model.reset(new ModelOnWorld(rhs._model->_object,rhs._model->_textrue_idx));
    _model = std::move(model);
}
void Projectile::draw() {
    _model->draw();
}
bool Projectile::hit() {
    return _model->CollisionDetection(_target);
};