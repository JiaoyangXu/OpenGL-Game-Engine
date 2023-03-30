#version 330

in vec3 vcolour;
in vec2 texture_uv;
uniform sampler2D ourTexture;

out vec4 fragColour;

void main() {
	if (texture_uv == vec2(0.0, 0.0)) {
		fragColour = texture(ourTexture, texture_uv);
	}
	fragColour = vec4(vcolour,1.0);

}
