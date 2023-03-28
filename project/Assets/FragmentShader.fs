#version 330

in vec3 vcolour;
in vec2 texture_uv;

out vec4 fragColour;
uniform sampler2D ourTexture;

void main() {
	//fragColour = vec4(vcolour,1.);
	fragColour = texture(ourTexture, texture_uv);
}
