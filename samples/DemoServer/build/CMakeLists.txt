CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )

SET(
	PROJECT_NAME
	"DemoServer"
)

SET(
	PROJECT_SOURCES
	"../src/DemoServerMain.cpp"
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

