// SignalManagerPlayer.hpp
#pragma once

#include <Godot.hpp>
#include <Node.hpp>

namespace godot {
	
	class SignalManagerPlayer : public Node {
		GODOT_CLASS(SignalManagerPlayer, Node)

	private:
		static SignalManagerPlayer* singleton;

	public:
		static void _register_methods();

		SignalManagerPlayer();
		~SignalManagerPlayer();
		void _init();

		static SignalManagerPlayer* get_singleton();
	};

}
