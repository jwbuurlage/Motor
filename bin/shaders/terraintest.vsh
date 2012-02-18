attribute vec2 textureCoordinate;

uniform sampler2D heightMap;
uniform float textureDelta;

//Needed for lighting and shadows and so on:
varying vec3 normalVarying;
varying vec3 lightDirVarying;

uniform mat4 vpMatrix;
uniform mat4 mMatrix;
uniform mat4 lightViewProjMatrix;
uniform vec3 lightPosition;

void main() {
	vec4 pos4;
	pos4.x = 2.0*(textureCoordinate.s-0.5);
	pos4.y = texture2D(heightMap, textureCoordinate).r;
	pos4.z = 2.0*(textureCoordinate.t-0.5);
	pos4.w = 1.0;
	vec4 position = mMatrix * pos4;
	
	//The texture coordinates of this vertex on the shadow map.
	//Still have to divide by w but this is done in the fragment shader
    gl_TexCoord[7] = lightViewProjMatrix * position;
	lightDirVarying = normalize(lightPosition - position.xyz);
	
	//THESE ARE CONSTANTS:
	//We should give them through a uniform instead of calculating them every time.
	float xScale = (mMatrix * vec4(1.0,0.0,0.0,0.0)).x;
	float yScale = (mMatrix * vec4(0.0,1.0,0.0,0.0)).y;
	float zScale = (mMatrix * vec4(0.0,0.0,1.0,0.0)).z;
	
	float A = texture2D(heightMap, textureCoordinate + vec2(0.0,textureDelta)).r;
	float B = texture2D(heightMap, textureCoordinate + vec2(textureDelta,0.0)).r;
	float C = texture2D(heightMap, textureCoordinate + vec2(0.0,-textureDelta)).r;
	float D = texture2D(heightMap, textureCoordinate + vec2(-textureDelta,0.0)).r;
	normalVarying = normalize(vec3(zScale*(D-B), 2.0/yScale, xScale*(C-A)));
	
	gl_Position = vpMatrix*position;
	gl_TexCoord[0].st = textureCoordinate;
}