attribute vec4 position;
attribute float alpha;
attribute vec2 textureCoordinate;

uniform mat4 pMatrix;

void main() {
	gl_Position = pMatrix*position;
	gl_FrontColor = vec4(1.0, 1.0, 1.0, alpha);
	gl_TexCoord[0].st = textureCoordinate;
}