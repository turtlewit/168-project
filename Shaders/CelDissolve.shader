shader_type spatial;
render_mode diffuse_toon, specular_toon, depth_draw_alpha_prepass, cull_disabled;

uniform sampler2D dissolveTexture;
uniform float amount : hint_range(-0.35, 0.35);
uniform vec4 dissolveColor : hint_color;

uniform sampler2D baseTexture : hint_black_albedo;
uniform bool useColorBlend;
uniform vec4 colorBlend : hint_color;
uniform float rim_amount : hint_range(0f, 1f);
//uniform sampler2D emissionMap : hint_white;
//uniform vec4 emissionColor : hint_color;

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
	vec3 n_out18p0 = dissolveColor.rgb;
	float n_out18p1 = dissolveColor.a;

// VectorOp:14
	vec3 n_out14p0 = vec3(n_out13p0) * n_out18p0;

// Output:0
	ALPHA = n_out10p0;
	EMISSION = n_out14p0;
	METALLIC = 0f;
	ROUGHNESS = 0f;
	SPECULAR = .5f;
	RIM = rim_amount;
	
	vec4 tex = texture(baseTexture, UV);
	ALBEDO = max(float(useColorBlend) * colorBlend, tex).rgb;
}
