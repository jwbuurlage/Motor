varying vec3 normalVarying;
varying vec3 lightDirVarying;

uniform sampler2D tex;
uniform sampler2DShadow shadow;

void main() {
	float lightFraction = 0.0;
	
	vec3 shadowCoordinateDivision = gl_TexCoord[7].xyz / gl_TexCoord[7].w;
	shadowCoordinateDivision.z -= 0.0001;
	
	vec2 radiusVec = vec2(2.0*shadowCoordinateDivision.x-1.0,2.0*shadowCoordinateDivision.y-1.0);
    float radiusSq = dot(radiusVec,radiusVec);
    if( radiusSq < 1.0 ){
        if( shadowCoordinateDivision.z < shadow2D(shadow, shadowCoordinateDivision.xyz).z ){
            lightFraction = (1.0-radiusSq)*max(0.0,dot(normalize(normalVarying), normalize(lightDirVarying)));;
        }
    }else{
		gl_FragColor = 0.3 * gl_Color * texture2D(tex, gl_TexCoord[0].st);
	}
	
	//Ambient
	lightFraction = min(lightFraction + 0.3, 1.0);
	
	gl_FragColor = lightFraction * gl_Color * texture2D(tex, gl_TexCoord[0].st);
}