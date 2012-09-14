#-- Cmake configuration
cmake_minimum_required( VERSION 2.6 )
project (Motor)

#-- Where to search for libraries
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/../bin)
#-- Where to output libraries and applications
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
#-- Display warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

#-- Build the library first
SET(LIB_NAME "motor")

SET(
	LIB_SOURCES
	"../src/matrix.cpp"
	"../src/ModelMD2Model.cpp"
	"../src/MotorCamera.cpp"
#	"../src/MotorColladaLoader.cpp"
	"../src/MotorColladaModel.cpp"
	"../src/MotorFilesystem.cpp"
	"../src/MotorLight.cpp"
	"../src/MotorLogger.cpp"
	"../src/MotorMaterialManager.cpp"
	"../src/MotorMesh.cpp"
	"../src/MotorMeshManager.cpp"
	"../src/MotorModelManager.cpp"
	"../src/MotorParticleEffect.cpp"
	"../src/MotorRenderer.cpp"
	"../src/MotorRoot.cpp"
	"../src/MotorScene.cpp"
	"../src/MotorSceneObject.cpp"
	"../src/MotorShaderManager.cpp"
	"../src/MotorTerrain.cpp"
	"../src/MotorTextureManager.cpp"
	"../src/MotorTimer.cpp"
	"../src/MotorWorld.cpp"
)

SET(
	LIB_INCLUDES
	"../include"
)

SET(
	LIB_LIBRARIES
	"sfml-graphics"
	"sfml-window"
	"sfml-system"
	"GLEW"
	"GL"
)

INCLUDE_DIRECTORIES( ${LIB_INCLUDES} )
ADD_LIBRARY( ${LIB_NAME} STATIC ${LIB_SOURCES} )
TARGET_LINK_LIBRARIES( ${LIB_NAME} ${LIB_LIBRARIES} )

#-- Then build each sample respectively
add_subdirectory(../samples/Demo/build ../samples/Demo/build)
add_subdirectory(../samples/DemoServer/build ../samples/DemoServer/build)
