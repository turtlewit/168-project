// Defs.h
#pragma once

#include <Godot.hpp>

#define NAMEOF(Object) #Object

#define GET_NODE(Type, Name) cast_to<Type>(get_node(Name))
#define REGISTER_METHOD(Class, Method) register_method(#Method, &Class::Method)
#define REGISTER_PROPERTY_HINT(Class, Type, Var, Default, Hint, HintString) register_property<Class, Type>(#Var, &Class::Var, Default, GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, Hint, HintString)
#define CLASS_INITS(Class) Class::Class() {} Class::~Class() {} void Class::_init() {}
#define PRINT_ERROR(message, function) godot::Godot::print_error(message, function, __FILE__, __LINE__)
#define ADD_INSPECTOR_GROUP(class_name, group_name, group_prefix) \
	register_property<class_name, Variant>(                       \
		group_name,                                               \
		nullptr,                                                  \
		Variant::NIL,                                             \
		GODOT_METHOD_RPC_MODE_DISABLED,                           \
		GODOT_PROPERTY_USAGE_GROUP,                               \
		GODOT_PROPERTY_HINT_NONE,                                 \
		group_prefix);

#define GODOT_HINT_STRING_ARRAY_OF_STRINGS godot::String("4:")
#define GODOT_HINT_STRING_ARRAY_OF_NODE_PATHS godot::String("15:")
