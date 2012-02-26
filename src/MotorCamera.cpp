#include "MotorCamera.h"
#include "matrix.h"

const float PI = 3.14159265358979323846264338327950288f;

namespace Motor {

	Camera::Camera(){
		updateMatrix = true;
		camZoomSpeed = 0.0f;
		camYawSpeed = 0.0f;
		camPitchSpeed = 0.0f;
		camDist = 5.0f;
		minCamDist = 3.0f;
		maxCamDist = 100.0f;
		freeYaw = false;
		position.x = position.y = position.z = 0.0f;
		yaw = 0.0f;
		pitch = 0.0f;
	}

	Camera::~Camera(){
	}

	void Camera::update(float elapsedTime){
		//---------------------------------------------
		//TODO: Camera shouldn't move beneath the floor
		//		so we need to check with a heightmap or
		//		for now probably just the y = -20 plane.
		//---------------------------------------------

		Vector3 ToMove = targetPosition - position;
		float dist = ToMove.length();
    
		//1. Camera position
		//We only need to move if the camera is not at the player
		if( dist > 0 ){
			//If the camera is really far away OR really close to the object and the object is not moving
			//then just teleport the camera to the exact object position
			if( dist > 100.0f ){
				position = targetPosition;
			}else if( dist > 0.1f ){ //We are at a normal distance from the object. Let camera fly with normal speed
				//ToMove already has a length, so if the camera is further away from the player,
				//then the camera will go faster.
				//When the object is moving in a single direction then the camera will be (PlayerSpeed/2.0f) units behind the object
				position += ToMove * 2.0f * elapsedTime; //speed is proportional to distance
			}else{ //Really close
				Vector3 newMove = ToMove.normalisedCopy();
				newMove *= 3.0f * elapsedTime; //constant speed of 3.0f units per second
				if( newMove.length() >= dist ){
					position = targetPosition;
				}else{
					position += newMove;
				}
			}
			updateMatrix = true;
		}

		//2. Camera rotation
		//For yaw rotation we have a free rotation bool. Pitch rotation is always free
		if( freeYaw ){
			if( camYawSpeed ){
				if( camYawSpeed > 1.5f*PI ) camYawSpeed = 1.5f*PI;
				if( camYawSpeed < -1.5f*PI ) camYawSpeed = -1.5f*PI;
				yaw += camYawSpeed * elapsedTime;
				updateMatrix = true;
				DeAccelerate(camYawSpeed, 12.0f * elapsedTime); //Deacceleration of 3.5 radians per second per second
			}
		}
		//Free pitch rotation
		if( camPitchSpeed ){
			if( camPitchSpeed > PI ) camPitchSpeed = PI;
			else if( camPitchSpeed < -PI ) camPitchSpeed = -PI;
			pitch += camPitchSpeed * elapsedTime;
			updateMatrix = true;
			//Decrease the speed (which causes a natural 'slow down')
			DeAccelerate(camPitchSpeed, 12.0f * elapsedTime); //Deacceleration of 3.5 radians per second per second
		}

		//3. Camera zoom
		if( camZoomSpeed ){
			if( camZoomSpeed > 40 ) camZoomSpeed = 40;
			else if( camZoomSpeed < -40 ) camZoomSpeed = -40;
			//Adjust the zoom with the speed
			camDist += camZoomSpeed * elapsedTime;
			updateMatrix = true;
			//Make sure it does not zoom in or out too far
			if( camDist < minCamDist ){ camDist = minCamDist; camZoomSpeed = 0; }
			else if( camDist > maxCamDist ){ camDist = maxCamDist; camZoomSpeed = 0; }
			//Decrease the speed (which causes a natural 'slow down')
			DeAccelerate(camZoomSpeed, 40 * elapsedTime); //Deacceleration of 40 units per second per second
		}
		return;
	}

	void Camera::DeAccelerate(float& Speed, float Acceleration){
		if( Speed > 0 ){
			if( Speed < Acceleration ) Speed = 0;
			else Speed -= Acceleration;
		}else{
			if( Speed > -Acceleration ) Speed = 0;
			else Speed += Acceleration;
		}
	}
    
    void Camera::setTargetLocation(const Vector3 &location, bool FreeYawRotation) {
        targetPosition = location;
        freeYaw = FreeYawRotation;
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