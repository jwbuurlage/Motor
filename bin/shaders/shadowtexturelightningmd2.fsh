varying vec4 colorVarying;
varying vec4 normalVarying;
varying vec3 lightDirVarying;
varying vec2 textureCoordinateVarying;
varying vec4 shadowCoordinate;

uniform sampler2D tex;
uniform sampler2DShadow shadow;

void main() {    
	float lightFraction = 0.0;
	vec4 shadowCoordinateDivision = shadowCoordinate / shadowCoordinate.w;
	shadowCoordinateDivision.z -= 0.0001;

	vec2 radiusVec = vec2(2.0*shadowCoordinateDivision.x-1.0,2.0*shadowCoordinateDivision.y-1.0);
    
    float radiusSq = dot(radiusVec,radiusVec);
    if( radiusSq < 1.0 ){
        if( shadowCoordinateDivision.z < shadow2D(shadow, shadowCoordinateDivision.xyz).z ){
            lightFraction = (1.0-radiusSq)*max(0.0,dot(normalize(normalVarying.xyz), normalize(lightDirVarying)));
        }
    }
    
    vec4 texColor = texture2D(tex, textureCoordinateVarying);
	gl_FragColor = (lightFraction + 0.2) * texColor * colorVarying;
}