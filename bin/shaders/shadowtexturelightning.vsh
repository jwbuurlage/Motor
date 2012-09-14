attribute vec3 normal;
attribute vec4 position;
attribute vec4 color;
attribute vec2 textureCoordinate;

varying vec3 normalVarying;
varying vec3 lightDirVarying;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 lightViewProjMatrix;
uniform vec3 lightPosition;

void main() {
	vec4 pos4 = mMatrix*position;
	//For the normal we set the fourth component to zero to make sure it is not translated
	normalVarying = normalize( mMatrix * vec4(normal, 0.0) ).xyz;
	//Direction from object to light
	lightDirVarying = normalize(lightPosition - pos4.xyz);
	
	gl_FrontColor = color;
	
	//The texture coordinates of this vertex on the shadow map.
	//Still have to divide by w but this is done in the fragment shader
    gl_TexCoord[7] = lightViewProjMatrix * pos4;
    
	gl_TexCoord[0].st = textureCoordinate;
	
	gl_Position = vpMatrix*pos4;
}