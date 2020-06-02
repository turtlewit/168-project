//DeathPlane.hpp
#pragma once


#include <Godot.hpp>
#include <Area.hpp>
#include <AnimationPlayer.hpp>

namespace godot {

	class DeathPlane : public Area {
		GODOT_CLASS(DeathPlane, Area)

	public:

	private:
		
	public:
		static void _register_methods();

		DeathPlane();
		~DeathPlane();
		void _init();

		void _ready();

	private:
		void _on_Deathplane_body_entered(Node* body);
	};

}
