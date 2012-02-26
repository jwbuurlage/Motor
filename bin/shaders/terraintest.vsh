attribute vec2 textureCoordinate;

uniform sampler2D heightMap;
uniform sampler2D normalMap;
uniform sampler2D textureMap;

uniform vec2 delta;

//Needed for lighting and shadows and so on:
varying vec3 normalVarying;
varying vec2 texCooVarying;
varying vec3 lightDirVarying;

uniform mat4 vpMatrix;
uniform mat4 mMatrix;
uniform mat4 lightViewProjMatrix;
uniform vec3 lightPosition;

void main() {
	vec4 pos4;
    texCooVarying = textureCoordinate + delta;
    
	pos4.x = texCooVarying.s; 
	pos4.y = texture2D(heightMap, texCooVarying).r;
	pos4.z = texCooVarying.t;
	pos4.w = 1.0;
	vec4 position = mMatrix * pos4;
	
	//The texture coordinates of this vertex on the shadow map.
	//Still have to divide by w but this is done in the fragment shader
    gl_TexCoord[7] = lightViewProjMatrix * position;
	lightDirVarying = normalize(lightPosition - position.xyz);
	
	//THESE ARE CONSTANTS:
    normalVarying = texture2D(normalMap, texCooVarying).rgb;
    
	gl_Position = vpMatrix*position;
	gl_TexCoord[0].st = textureCoordinate;
}