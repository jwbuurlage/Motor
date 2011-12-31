#pragma once
#include "Vector3.h"

class mat;

namespace Motor {

	class Camera
	{
	public:
		Camera();
		~Camera();

		//Only for initialization. This is the look-at position of the camera.
		void setPosition( const Vector3& CamPos ){ position = CamPos; updateMatrix = true; }
		const Vector3& getPosition() const { return position; }
		float getYaw() const { return yaw; }
		float getPitch() const { return pitch; }

		//To make the camera stay looking at an object smoothly,
		//call setTargetLocation every frame with the object as location
		void setTargetLocation( const Vector3& location, bool FreeYawRotation = false);
		//if you want the camera to stay BEHIND the object, call setCameraYaw every frame
		//with the yaw of the object
		void setCameraYaw(float yaw);
	
		//Smooth camera movement
		void update(float elapsedTime);


		//
		//These should be called by the mouse handler
		//

		//This will give the camera a swing
		void rotateCameraSwing(float yawspeed, float pitchspeed);
		//This will also rotate the camera but will instantly apply the angle and not swing
		void rotateCamera(float Yaw, float Pitch); //Add yaw/pitch to current angle
		void setCameraAngle(float Yaw, float Pitch);

		//Vector3 GetRealCameraPosition();

		//called by Renderer every frame
		//it will generate a new matrix if it changed
		//if nothing changed it will not change outMatrix
		//returns if it updated
		bool updateViewMatrix(mat* outMatrix);
		bool updateMatrix;

		float camZoomSpeed; //Zoomspeed. positive means zooming out
		float camYawSpeed; //Yaw speed in radians per second
		float camPitchSpeed; //Pitch speed in radians per second

	private:
		Vector3 position; //look-at-position. Actual camera is 'zoomed out' from here
		float yaw, pitch;
		float camDist; //Zoom. Higher means further away
		float minCamDist;
		float maxCamDist;

		bool freeYaw; //Free yaw rotation
		Vector3 targetPosition;

		void DeAccelerate(float& Speed, float Acceleration);
	};

}
