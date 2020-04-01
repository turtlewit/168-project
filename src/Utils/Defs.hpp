// Defs.h
#ifndef __GD_DEFS_H
#define __GD_DEFS_H

#include <Godot.hpp>

#define NAMEOF(Object) #Object

#define GET_NODE(Type, Name) cast_to<Type>(get_node(Name))
#define REGISTER_METHOD(Class, Method) register_method(#Method, &Class::Method)
#define REGISTER_PROPERTY_HINT(Class, Type, Var, Default, Hint, HintString) register_property<Class, Type>(#Var, &Class::Var, Default, GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, Hint, HintString)
#define CLASS_INITS(Class) Class::Class() {} Class::~Class() {} void Class::_init() {}

#endif // __GD_DEFS_H
