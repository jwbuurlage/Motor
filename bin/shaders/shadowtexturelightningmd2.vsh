attribute vec3 normal;
attribute vec3 normal_next;
attribute vec4 position;
attribute vec4 position_next;
attribute vec4 color;
attribute vec2 textureCoordinate;

varying vec4 colorVarying;
varying vec4 normalVarying;
varying vec3 lightDirVarying;
varying vec2 textureCoordinateVarying;
varying vec4 shadowCoordinate;

uniform float interpolation;
uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 lightViewProjMatrix;
uniform vec3 lightPosition;

void main() {
    vec3 interpolated_normal = normal + (interpolation * (normal_next - normal));
    vec4 interpolated_position = position + (interpolation * (position_next - position));

	//Calculate vertex position and normal
	//For the normal we set the fourth component to zero to make sure it is not translated
	vec4 pos4 = mMatrix*interpolated_position;
	normalVarying = mMatrix * vec4(interpolated_normal, 0.0);
	
	//Direction from object to light
	lightDirVarying = lightPosition - pos4.xyz;
	
	//The texture coordinates of this vertex on the shadow map.
	//Still have to divide by w but this is done in the fragment shader
    shadowCoordinate = lightViewProjMatrix * pos4;
    
	colorVarying = color;
	textureCoordinateVarying = textureCoordinate;
	
	gl_Position = vpMatrix*pos4;
}