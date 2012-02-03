CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )

SET(
	PROJECT_NAME
	"Demo"
)

SET(
	PROJECT_SOURCES
	"../../src/matrix.cpp"
	"../../src/MotorCamera.cpp"
	"../../src/MotorFilesystem.cpp"
	"../../src/MotorLight.cpp"
	"../../src/MotorLogger.cpp"
	"../../src/MotorMaterialManager.cpp"
	"../../src/MotorMesh.cpp"
	"../../src/MotorMeshManager.cpp"
	"../../src/MotorRenderer.cpp"
	"../../src/MotorRoot.cpp"
	"../../src/MotorScene.cpp"
	"../../src/MotorSceneObject.cpp"
	"../../src/MotorShaderManager.cpp"
	"../../src/MotorTextureManager.cpp"
	"../../src/MotorTimer.cpp"
	"../../src/MotorWorld.cpp"
	
	"src/DemoGame.cpp"
	"src/DemoMain.cpp"
)

SET(
	PROJECT_INCLUDES
	"../../include"
	"include"
)

SET(
	PROJECT_LIBRARIES
	"sfml-graphics"
	"sfml-window"
	"sfml-system"
	"GLEW"
	"GL"
)

#----------- DO NOT EDIT BELOW --------------

INCLUDE_DIRECTORIES( ${PROJECT_INCLUDES} )
ADD_EXECUTABLE( ${PROJECT_NAME} ${PROJECT_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARIES} )
