uniform sampler2D heightMap;
uniform sampler2D textureMap;
uniform sampler2DShadow shadowMap;

varying vec2 texCooVarying;
varying vec3 normalVarying;
varying vec3 lightDirVarying;

uniform vec3 colorUniform;

void main() {
	float lightFraction = 0.0;
	
	vec3 shadowCoordinateDivision = gl_TexCoord[7].xyz / gl_TexCoord[7].w;
	shadowCoordinateDivision.z -= 0.0001;
	
	vec2 radiusVec = vec2(2.0*shadowCoordinateDivision.x-1.0,2.0*shadowCoordinateDivision.y-1.0);
    float radiusSq = dot(radiusVec,radiusVec);
    if( radiusSq < 1.0 ){
        if( shadowCoordinateDivision.z < shadow2D(shadowMap, shadowCoordinateDivision.xyz).z ){
            lightFraction = (1.0-radiusSq)*max(0.0,dot(normalize(normalVarying), normalize(lightDirVarying)));
        }
	}
	//lightFraction = max(0.0,dot(normalize(normalVarying), normalize(lightDirVarying)));
	
	//Ambient
	lightFraction = min(lightFraction + 0.25, 1.0);
	
	vec2 texCooGrass = mod(7.0*texCooVarying, 1.0);
    
	gl_FragColor = lightFraction * vec4(colorUniform, 1.0);
}