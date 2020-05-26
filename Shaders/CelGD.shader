shader_type spatial;
render_mode diffuse_toon, specular_toon;

uniform sampler2D baseTexture : hint_black_albedo;
uniform bool useColorBlend;
uniform vec4 colorBlend : hint_color;
uniform float rim_amount : hint_range(0f, 1f);

uniform sampler2D emissionMap : hint_white;
uniform vec4 emissionColor : hint_color;

uniform float metallic : hint_range(0f, 1f) = 0f;
uniform float roughness : hint_range(0f, 1f) = 0f;
uniform float specular : hint_range(0f, 1f) = 0f;

void fragment() {
	METALLIC = metallic;
	ROUGHNESS = roughness;
	SPECULAR = specular;
	RIM = rim_amount;
	
	vec4 emissionTex = texture(emissionMap, UV);
	EMISSION = emissionTex.rgb * emissionColor.rgb;
	
	vec4 tex = texture(baseTexture, UV);
	ALBEDO = max(float(useColorBlend) * colorBlend, tex).rgb;
}
