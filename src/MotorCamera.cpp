#include "MotorCamera.h"
#include "matrix.h"

namespace Motor {

	Camera::Camera(){
		updateMatrix = true;
		camZoomSpeed = 0.0f;
		camYawSpeed = 0.0f;
		camPitchSpeed = 0.0f;
		camDist = 5.0f;
		minCamDist = 3.0f;
		maxCamDist = 50.0f;
		freeYaw = false;
		yaw = 0.0f;
		pitch = 0.0f;
	}

	Camera::~Camera(){
	}

	void Camera::rotateCamera(float Yaw, float Pitch){
		yaw += Yaw, 
		pitch += Pitch;
		updateMatrix = true;
	}

	bool Camera::updateViewMatrix(mat* outMatrix){
		if( updateMatrix ){
			mat zoomMat, rotateMat, translateMat;
			zoomMat.setTranslation(0, 0, -camDist);
			rotateMat.setRotationXYZ( -pitch, -yaw, 0 );
			translateMat.setTranslation(-position);
			*outMatrix = zoomMat * rotateMat * translateMat;
			//if( inverseViewMatrix ){
			//	mat zoomMat, rotateMat, translateMat;
			//	zoomMat.setTranslation(0, 0, mCamDist);
			//	rotateMat.setRotationX(pitch);
			//	rotateMat.rotateY(yaw);
			//	translateMat.setTranslation(position);
			//	*inverseViewMatrix = translateMat * rotateMat * zoomMat;
			//}
			updateMatrix = false;
			return true;
		}
		return false;
	}

}