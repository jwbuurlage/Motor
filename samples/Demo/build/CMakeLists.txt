CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET(
	PROJECT_NAME
	"Demo"
)

SET(
	PROJECT_SOURCES
	"../src/DemoGame.cpp"
	"../src/DemoMain.cpp"
)

SET(
	PROJECT_INCLUDES
	"../../../include"
	"../include"
)

SET(
	PROJECT_LIBRARIES
	"motor"
	"sfml-network"
	"sfml-system"
)

#----------- DO NOT EDIT BELOW --------------

INCLUDE_DIRECTORIES( ${PROJECT_INCLUDES} )
ADD_EXECUTABLE( ${PROJECT_NAME} ${PROJECT_SOURCES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} ${PROJECT_LIBRARIES} )

