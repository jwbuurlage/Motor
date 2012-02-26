#-- Cmake configuration
cmake_minimum_required( VERSION 2.6 )
project (Motor)

#-- Build the library first
SET(MOTOR_NAME "Motor")

SET(
	MOTOR_SOURCES
	"src/matrix.cpp"
	"src/MotorCamera.cpp"
	"src/MotorFilesystem.cpp"
	"src/MotorLight.cpp"
	"src/MotorLogger.cpp"
	"src/MotorMaterialManager.cpp"
	"src/MotorMesh.cpp"
	"src/MotorMeshManager.cpp"
	"src/MotorModelManager.cpp"
	"src/MotorParticleEffect.cpp"
	"src/MotorRenderer.cpp"
	"src/MotorRoot.cpp"
	"src/MotorScene.cpp"
	"src/MotorSceneObject.cpp"
	"src/MotorShaderManager.cpp"
	"src/MotorTextureManager.cpp"
	"src/MotorTimer.cpp"
	"src/MotorWorld.cpp"
	"src/ModelMD2Model.cpp"
	"src/MotorTerrain.cpp"
)


SET(
	MOTOR_INCLUDES
	"include"
)

SET(
	MOTOR_LIBRARIES
	"sfml-graphics"
	"sfml-window"
	"sfml-system"
	"GLEW"
	"GL"
)

INCLUDE_DIRECTORIES( ${MOTOR_INCLUDES} )
ADD_LIBRARY( ${MOTOR_NAME} ${MOTOR_SOURCES} )
TARGET_LINK_LIBRARIES( ${MOTOR_NAME} ${MOTOR_LIBRARIES} )

#-- Then build each samples respectively
add_subdirectory(samples/Demo)
add_subdirectory(samples/DemoServer)
