#include "matrix.h"
#include "Vector3.h"

#define _USE_MATH_DEFINES
#include <math.h>

mat::mat(){ setIdentity(); }
mat::mat(const mat& m){ for( unsigned int i = 0; i < 16; ++i ) value[i] = m.value[i]; }
mat::mat(float initvalue){ for( unsigned int i = 0; i < 16; ++i ) value[i] = initvalue; }
mat::~mat() { }

//------------
//MATRIX CLASS
//------------
//overloaded operators

//A *= B means A = (A*B)
mat& mat::operator *= (const mat& m){
    //returns (this*m) and makes this the result
    mat temp(*this);
    for(int i = 0; i < 4; ++i ){
        for(int j = 0; j < 4; ++j ){
            e[i][j] = 0;
            for(int k = 0; k < 4; ++k ){
                e[i][j] += temp.e[k][j] * m.e[i][k];
            }
        }
    }
    return *this;
}

mat mat::operator * (const mat& m) const{
	//returns (this*m) without changing this
    mat newmat(0.0f);
    for(int i = 0; i < 4; ++i ){
        for(int j = 0; j < 4; ++j ){
            for(int k = 0; k < 4; ++k ){
                newmat.e[i][j] += e[k][j] * m.e[i][k];
            }
        }
    }
    return newmat;
}

//For using Vector3 with our own matrix code
Vector3 mat::operator * (const Vector3& vec){
	Vector3 result;
	result.x = vec.x * e[0][0] + vec.y * e[1][0] + vec.z * e[2][0] + e[3][0];
	result.y = vec.x * e[0][1] + vec.y * e[1][1] + vec.z * e[2][1] + e[3][1];
	result.z = vec.x * e[0][2] + vec.y * e[1][2] + vec.z * e[2][2] + e[3][2];
	return result;
}

Vector3 mat::multiplyVecDivideW(const Vector3& vec){
	Vector3 result;
	float invW = 1.0f/(vec.x * e[0][3] + vec.y * e[1][3] + vec.z * e[2][3] + e[3][3]);
	result.x = invW * ( vec.x * e[0][0] + vec.y * e[1][0] + vec.z * e[2][0] + e[3][0] );
	result.y = invW * ( vec.x * e[0][1] + vec.y * e[1][1] + vec.z * e[2][1] + e[3][1] );
	result.z = invW * ( vec.x * e[0][2] + vec.y * e[1][2] + vec.z * e[2][2] + e[3][2] );
	return result;
}

//mat& mat::scale(float f) {
//	for(int i = 0; i < 3; ++i){
//		for( int j = 0; j < 3; ++j ){
//			e[i][j] *= f;
//		}
//	}
//}
//
//mat& mat::setScale(float f) {
//    setIdentity();
//    e[0][0] = f;
//    e[1][1] = f;
//    e[2][2] = f;
//}

mat& mat::translate(float x, float y, float z) {
    e[3][0] += x;
    e[3][1] += y;
    e[3][2] += z;
	return *this;
}

mat& mat::setTranslation(float x, float y, float z) {
    setIdentity();
    e[3][0] = x;
    e[3][1] = y;
    e[3][2] = z;
	return *this;
}

mat& mat::setTranslation(const Vector3& vec){
	return setTranslation(vec.x, vec.y, vec.z);
}

//------------------------
//ROTATION METHODS (X,Y,Z)
//------------------------
mat& mat::rotateXYZ(float pitch, float yaw, float roll) {
    mat rotatexyz; 
    rotatexyz.setRotationXYZ(pitch, yaw, roll);
	*this = rotatexyz * *this;
	return *this;
}

mat& mat::setRotationXYZ(float pitch, float yaw, float roll) {
    setIdentity();
	float cosPitch = cos(pitch), cosYaw = cos(yaw), sinPitch = sin(pitch), sinYaw = sin(yaw);
    e[0][0] = cosYaw;
    e[1][0] = 0.0;
    e[2][0] = sinYaw;
    e[0][1] = sinPitch*sinYaw;
    e[1][1] = cosPitch;
    e[2][1] = -sinPitch*cosYaw; 
    e[0][2] = -cosPitch*sinYaw;
    e[1][2] = sinPitch;
    e[2][2] = cosPitch*cosYaw;
	return *this;
}

mat& mat::setRotationX(float pitch){
	e[1][1] = cos(pitch);
	e[2][1] = -sin(pitch);
	e[1][2] = -e[2][1];
	e[2][2] = e[1][1];
	return *this;
}

mat& mat::setRotationY(float yaw){
	e[0][0] = cos(yaw);
	e[2][0] = sin(yaw);
	e[0][2] = -e[2][0];
	e[2][2] = e[0][0];
	return *this;
}

mat& mat::setRotationZ(float roll){
	e[0][0] = cos(roll);
	e[0][1] = sin(roll);
	e[1][0] = -e[0][1];
	e[1][1] = e[0][0];
	return *this;
}

mat& mat::rotateX(float pitch){
	mat rot;
	*this = rot.setRotationX(pitch) * *this;
	return *this;
}

mat& mat::rotateY(float yaw){
	mat rot;
	*this = rot.setRotationY(yaw) * *this;
	return *this;
}

mat& mat::rotateZ(float roll){
	mat rot;
	*this = rot.setRotationZ(roll) * *this;
	return *this;
}

mat& mat::setIdentity() {
    //first we set all values to 0
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            e[i][j] = 0.0;
        }
        //every diagonal value gets to be 1
        e[i][i] = 1.0;
    }
	return *this;
}

mat& mat::setPerspective(float left, float right, float bottom, float top, float near, float far) {
    //first we set all values to 0
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            e[i][j] = 0.0;
        }
    }
    
    e[0][0] = (2*near) / (right - left);
    e[2][0] = (right+left) / (right - left);
    e[1][1] = (2*near) / (top - bottom);
    e[2][1] = (top+bottom) / ( top - bottom);
    e[2][2] = -(near + far)/(far - near);
    e[3][2] = -(2*far*near)/(far - near);
    e[2][3] = -1.0;
	return *this;
}

mat& mat::setInversePerspective(float left, float right, float bottom, float top, float near, float far) {
    //first we set all values to 0
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            e[i][j] = 0.0;
        }
    }
    
    e[0][0] = (right-left)/(2*near);
    e[3][0] = (right+left)/(2*near);
    e[1][1] = (top-bottom)/(2*near);
    e[3][1] = (top+bottom)/(2*near);
    e[3][3] = (near+far)/(2*far*near);
    e[2][3] = (near-far)/(2*far*near);
    e[3][2] = -1.0;
	return *this;
}
