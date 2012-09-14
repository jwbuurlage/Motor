attribute vec3 normal;
attribute vec4 position;
attribute vec2 textureCoordinate;
attribute vec4 indices;
attribute vec4 weights;

varying vec3 normalVarying;
varying vec3 lightDirVarying;

uniform mat4 mMatrix;
uniform mat4 vpMatrix;
uniform mat4 lightViewProjMatrix;
uniform vec3 lightPosition;

uniform mat4 boneMatrices[50];

void main(){
	//The program will make sure that the first index is always set. It might be 0, and boneMatrices[0] is always identity
	mat4 skinMatrix = weights.x * boneMatrices[int(indices.x)];
	if( indices.y > 0.5 ){ skinMatrix += weights.y * boneMatrices[int(indices.y)]; }
	if( indices.z > 0.5 ){ skinMatrix += weights.z * boneMatrices[int(indices.z)]; }
	if( indices.w > 0.5 ){ skinMatrix += weights.w * boneMatrices[int(indices.w)]; }
	
	vec4 pos4 = mMatrix*skinMatrix*position;
	
	//For the normal we set the fourth component to zero to make sure it is not translated
	normalVarying = normalize( mMatrix * skinMatrix * vec4(normal, 0.0) ).xyz;
	//Direction from object to light
	lightDirVarying = normalize(lightPosition - pos4.xyz);
	
	gl_FrontColor = vec4(1.0,1.0,1.0,1.0);
	
	//The texture coordinates of this vertex on the shadow map.
	//Still have to divide by w but this is done in the fragment shader
    gl_TexCoord[7] = lightViewProjMatrix * pos4;
    
	gl_TexCoord[0].st = textureCoordinate;
	
	gl_Position = vpMatrix*pos4;
}