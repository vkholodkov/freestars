INCLUDE(${QT_USE_FILE})

include_directories(
    ${FREESTARS_SOURCE_DIR}/tinyxml
    ${FREESTARS_SOURCE_DIR}/Server
)

SET(freestars_SOURCES
    main.cpp
    mainwindow.cpp
    advanced_new_game_wizard.cpp
    game_view.cpp
    folding_widget.cpp
    vertical_flow_layout.cpp
    message_formatter.cpp
    habitation_bar.cpp
    mineral_report.cpp
    scale_widget.cpp
    cargo_widget.cpp
    diamond_button.cpp
    planet_production_widget.cpp
    fleets_in_orbit_widget.cpp
    production_queue_model.cpp
    space_object_sorter.cpp
    map_view.cpp
)

SET(freestars_HEADERS
    mainwindow.h
    advanced_new_game_wizard.h
    game_view.h
    folding_widget.h
    habitation_bar.h
    mineral_report.h
    scale_widget.h
    cargo_widget.h
    planet_production_widget.h
    fleets_in_orbit_widget.h
    production_queue_model.h
    map_view.h
)

SET(freestars_FORMS
    angw_page1.ui
    angw_page3.ui
    planet_widget.ui
    planet_report.ui
    fleet_report.ui
    planet_production_widget.ui
    message_widget.ui
    fleets_in_orbit_widget.ui
    status_selector.ui
    minerals_on_hand_widget.ui
    planet_status_widget.ui
    starbase_widget.ui
)

SET(freestars_RESOURCES application.qrc)

link_directories(${FREESTARS_BINARY_DIR}/tinyxml ${FREESTARS_BINARY_DIR}/Server)

add_definitions(-DTIXML_USE_STL)

QT4_WRAP_CPP(freestars_HEADERS_MOC ${freestars_HEADERS})
QT4_WRAP_UI(freestars_FORMS_HEADERS ${freestars_FORMS})
QT4_ADD_RESOURCES(freestars_RESOURCES_RCC ${freestars_RESOURCES})
QT4_CREATE_TRANSLATION(messages.qm)

add_executable(fs-client-qt ${freestars_SOURCES} ${freestars_HEADERS_MOC} ${freestars_FORMS_HEADERS} ${freestars_RESOURCES_RCC})

target_link_libraries(fs-client-qt fsserver MersenneTwister tinyxml Qt4::QtGui Qt4::QtCore)

