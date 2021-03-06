INCLUDE(${QT_USE_FILE})

include_directories(
    ${FREESTARS_SOURCE_DIR}/tinyxml
    ${FREESTARS_SOURCE_DIR}/Server
)

SET(freestars_SOURCES
    main.cpp
    mainwindow.cpp
    advanced_new_game_wizard.cpp
    race_wizard.cpp
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
    fleet_widget.cpp
    fleet_cargo_widget.cpp
    fleet_composition_widget.cpp
    fleet_waypoints_widget.cpp
    orbiting_widget.cpp
    production_queue_model.cpp
    space_object_sorter.cpp
    map_view.cpp
    research_dialog.cpp
    ship_design_dialog.cpp
    graphics_array.cpp
    slot_widget.cpp
    ship_avatar_widget.cpp
    ship_describer.cpp
)

SET(freestars_HEADERS
    mainwindow.h
    advanced_new_game_wizard.h
    race_wizard.h
    game_view.h
    folding_widget.h
    habitation_bar.h
    mineral_report.h
    scale_widget.h
    cargo_widget.h
    planet_production_widget.h
    fleets_in_orbit_widget.h
    fleet_widget.h
    fleet_cargo_widget.h
    fleet_composition_widget.h
    fleet_waypoints_widget.h
    orbiting_widget.h
    production_queue_model.h
    map_view.h
    research_dialog.h
    ship_design_dialog.h
    slot_widget.h
    ship_avatar_widget.h
    ship_describer.h
)

SET(freestars_FORMS
    angw_page1.ui
    angw_page3.ui
    rw_page1.ui
    rw_page2.ui
    rw_page3.ui
    rw_page4.ui
    rw_page5.ui
    rw_page6.ui
    planet_widget.ui
    planet_report.ui
    fleet_widget.ui
    fleet_cargo_widget.ui
    fleet_composition_widget.ui
    fleet_waypoints_widget.ui
    fleet_report.ui
    planet_production_widget.ui
    message_widget.ui
    fleets_in_orbit_widget.ui
    status_selector.ui
    orbiting_widget.ui
    minerals_on_hand_widget.ui
    planet_status_widget.ui
    starbase_widget.ui
    research_dialog.ui
    ship_design_dialog.ui
)

SET(freestars_RESOURCES application.qrc)

link_directories(${FREESTARS_BINARY_DIR}/tinyxml ${FREESTARS_BINARY_DIR}/Server)

add_definitions(-DTIXML_USE_STL)

QT4_WRAP_CPP(freestars_HEADERS_MOC ${freestars_HEADERS})
QT4_WRAP_UI(freestars_FORMS_HEADERS ${freestars_FORMS})
QT4_ADD_RESOURCES(freestars_RESOURCES_RCC ${freestars_RESOURCES})
QT4_CREATE_TRANSLATION(${freestars_SOURCES} ${freestars_FORMS_HEADERS} freestars.ts)
QT4_ADD_TRANSLATION(messages.qm messages.ts)

add_executable(fs-client-qt ${freestars_SOURCES} ${freestars_HEADERS_MOC} ${freestars_FORMS_HEADERS} ${freestars_RESOURCES_RCC} freestars.qm messages.qm)

target_link_libraries(fs-client-qt fsserver MersenneTwister tinyxml Qt4::QtGui Qt4::QtCore)

