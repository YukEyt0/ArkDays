#include "./operator.h"

extern std::unique_ptr<ModelLibrary> _model_lib;
extern std::unique_ptr<ShapeLibrary> _shape_lib;
extern std::unique_ptr<TextrueLibrary> _texture_lib;
extern std::unique_ptr<OperatorIconLibrary> _icon_lib;

Operator::Operator(float hp, float atk, float def, float cool_down, int block, int model_idx, int pic_idx, float range, OperatorType type, float reset_time, int cost)
    :_hp(hp), _atk(atk), _def(def), _cool_down(cool_down), _block(block), _type(type), _model(new ModelOnWorld(_model_lib->getModel(model_idx),0)),
     _model_idx(model_idx), _pic_idx(pic_idx), _reset_time(reset_time),_cost(cost), _range(range), _icon(_icon_lib->getIconRef(pic_idx)) {
        ModelOnWorld::_model_map.emplace(_model->id,_model);
     };

Operator::Operator(const Operator &rhs)
    :_hp(rhs._hp), _atk(rhs._atk), _def(rhs._def), _cool_down(rhs._cool_down), _block(rhs._block), _type(rhs._type),
     _model_idx(rhs._model_idx), _pic_idx(rhs._pic_idx), _reset_time(rhs._reset_time),_cost(rhs._cost), _range(rhs._range),
     _model(new ModelOnWorld(rhs._model->_object,rhs._model->_textrue_idx)), _icon(rhs._icon) {
        ModelOnWorld::_model_map.erase(rhs._model->id);
        ModelOnWorld::_model_map.emplace(_model->id,_model);
     };

