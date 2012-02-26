attribute vec4 position;
attribute vec4 position_next;

uniform float interpolation;

uniform mat4 mvpMatrix;
void main() {	
    vec4 position_interpolated = position + interpolation * (position_next - position);
	gl_Position = mvpMatrix*position_interpolated;
}