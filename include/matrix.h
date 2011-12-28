#pragma once

// 4x4 Matrix class
// Operators:
// A *= B		The result (A) will be the matrix multiplication A*B
// C = A*B		The result (C) will be the matrix multiplication A*B

//IMPORTANT: When applying matrices, OpenGL will multiply the matrix with a COLUMN vector!
//This is the Mathematical standard, but different from DirectX (which multiplies a row vector with the (transposed) matrix)
//      ( x )
//      ( y )
//  M * ( z )
//      ( 1 )
//
//IMPORTANT: When accessing the values with val[a][b] then a will be the colum index and b the row index.
//This is NOT the mathematical notation. OpenGL reads the matrix like this:
//
//	[0][0]	[1][0]	[2][0]	[3][0]
//	[0][1]	[1][1]	[2][1]	[3][1]
//	[0][2]	[1][2]	[2][2]	[3][2]
//	[0][3]	[1][3]	[2][3]	[3][3]
//

class Vector3;

class mat {
public:
	mat();
	mat(const mat& m);
	mat(float initvalue);
	~mat();
	
	//Cast to float*
	//So you can do glUniformXX( .. , matrix );
	operator float*(){ return value; }
	
	//For using Vector3 with matrix:
	//vector2 = matrix1 * vector1;
	Vector3 operator * (const Vector3& vec);
	Vector3 multiplyVecDivideW(const Vector3& vec);
	
	//A *= B means A = (A*B)
	mat& operator *= (const mat& m);
	mat operator * (const mat& m) const;
	
	mat& translate(float x, float y, float z);
	mat& translate(const Vector3& vec);
	mat& setTranslation(float x, float y, float z);
	mat& setTranslation(const Vector3& vec);

	mat& scale(float f);
	
	mat& setRotationX(float pitch);
	mat& setRotationY(float yaw);
	mat& setRotationZ(float roll);
	mat& rotateX(float pitch);
	mat& rotateY(float yaw);
	mat& rotateZ(float roll);
	
	mat& setRotationXYZ(float pitch, float yaw, float roll);
	mat& rotateXYZ(float yaw, float pitch, float roll);
	
	mat& setIdentity();
	mat& setPerspective(float left, float right, float bottom, float top, float near, float far);
	mat& setInversePerspective(float left, float right, float bottom, float top, float near, float far);
	
	union{
		float e[4][4];
		float value[16];
	};

};
