#pragma once
// Portal.hpp

#include <Area.hpp>
#include <Godot.hpp>
namespace godot {
	
	class Portal : public Area {
		GODOT_CLASS(Portal, Area)

        private:
            Vector3 translation;
        public:
            static void _register_methods();

            Portal();
            ~Portal();
            void _init();

            void _ready();

        private:
            void _on_Portal_body_entered(Node* body);

    };

}  // namespace godot