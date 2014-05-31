include_directories(
    /usr/include/qt4
    /usr/include/qt4/QtGui
    /usr/include/qt4/QtCore
    ${FREESTARS_SOURCE_DIR}/tinyxml
    ${FREESTARS_SOURCE_DIR}/Server
)

SET(freestars_SOURCES main.cpp mainwindow.cpp game_view.cpp folding_widget.cpp vertical_flow_layout.cpp message_formatter.cpp habitation_bar.cpp)
SET(freestars_HEADERS mainwindow.h game_view.h folding_widget.h habitation_bar.h)
SET(freestars_FORMS planet_report.ui planet_production_widget.ui message_widget.ui fleets_in_orbit_widget.ui status_selector.ui)
SET(freestars_RESOURCES application.qrc)

link_directories(${FREESTARS_BINARY_DIR}/tinyxml ${FREESTARS_BINARY_DIR}/Server)

add_definitions(-DTIXML_USE_STL)

QT4_WRAP_CPP(freestars_HEADERS_MOC ${freestars_HEADERS})
QT4_WRAP_UI(freestars_FORMS_HEADERS ${freestars_FORMS})
QT4_ADD_RESOURCES(freestars_RESOURCES_RCC ${freestars_RESOURCES})

add_executable(fs-client-qt ${freestars_SOURCES} ${freestars_HEADERS_MOC} ${freestars_FORMS_HEADERS} ${freestars_RESOURCES_RCC})

target_link_libraries(fs-client-qt fsserver MersenneTwister tinyxml QtGui QtCore)
