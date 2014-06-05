include_directories( ${FREESTARS_SOURCE_DIR}/MersenneTwister
		     ${FREESTARS_SOURCE_DIR}/tinyxml)

link_directories( ${FREESTARS_BINARY_DIR}/MersenneTwister
		  ${FREESTARS_BINARY_DIR}/tinyxml)

add_definitions(-DTIXML_USE_STL)

SET(fsserver_SOURCES 
	Battle.cpp
	BattlePlan.cpp
	Bombing.cpp
	CargoHolder.cpp
	Component.cpp
	Cost.cpp
	Creation.cpp
	Fleet.cpp
	FSServer.cpp
	Galaxy.cpp
	Game.cpp
	Hull.cpp
	Invasion.cpp
	Location.cpp
	Message.cpp
	MineField.cpp
	MineFieldType.cpp
	Order.cpp
	Packet.cpp
	Planet.cpp
	Player.cpp
	ProdOrder.cpp
	Race.cpp
	RacialTrait.cpp
	Rules.cpp
	Salvage.cpp
	Ship.cpp
	Slot.cpp
	SpaceObject.cpp
	Stack.cpp
	TempFleet.cpp
	TinyXmlPlus.cpp
    VictoryConditions.cpp
	WayOrder.cpp
	WayOrderList.cpp
    Wormhole.cpp
)

add_executable(fs-server Main.cpp)

add_library(fsserver Library.cpp ${fsserver_SOURCES})

target_link_libraries(fs-server fsserver MersenneTwister tinyxml)
