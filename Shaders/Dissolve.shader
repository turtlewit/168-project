shader_type spatial;
render_mode depth_draw_alpha_prepass, cull_disabled;

uniform sampler2D dissolveTexture;
uniform float amount : hint_range(-0.3, 0.3);
uniform vec4 color : hint_color;

void fragment() {
// Input:2
	vec3 n_out2p0 = vec3(UV, 0.0);

// Texture:8
	vec4 dissolveTexture_read = texture(dissolveTexture, n_out2p0.xy);
	vec3 n_out8p0 = dissolveTexture_read.rgb;
	float n_out8p1 = dissolveTexture_read.a;

// ScalarUniform:17
	float n_out17p0 = amount;

// ScalarOp:9
	float n_out9p0 = dot(n_out8p0, vec3(0.333333, 0.333333, 0.333333)) - n_out17p0;

// ScalarFunc:10
	float n_out10p0 = round(n_out9p0);

// ScalarOp:11
	float n_in11p1 = 0.05000;
	float n_out11p0 = n_out9p0 - n_in11p1;

// ScalarOp:12
	float n_in12p0 = 1.00000;
	float n_out12p0 = n_in12p0 - n_out11p0;

// ScalarFunc:13
	float n_out13p0 = round(n_out12p0);

// ColorUniform:18
	vec3 n_out18p0 = color.rgb;
	float n_out18p1 = color.a;

// VectorOp:14
	vec3 n_out14p0 = vec3(n_out13p0) * n_out18p0;

// Output:0
	ALPHA = n_out10p0;
	EMISSION = n_out14p0;

}
