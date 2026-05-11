cmake_minimum_required(VERSION 3.16)
project(task3 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core Gui Qml Quick Multimedia Sql LinguistTools Test
)

qt_standard_project_setup()

set(TS_FILES apptask3_en.ts apptask3_ru.ts apptask3_by.ts)

qt_add_executable(apptask3
    main.cpp
    playercontroller.h
    playercontroller.cpp
    Track.h
    ${TS_FILES}
)

qt_add_qml_module(apptask3
    URI App.Player
    VERSION 1.0
    QML_FILES Main.qml
)

qt_add_executable(player_tests
    tst_player.cpp
    playercontroller.cpp
    playercontroller.h
    Track.h
)

target_link_libraries(apptask3 PRIVATE
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Multimedia Qt6::Sql
)

target_link_libraries(player_tests PRIVATE
    Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Multimedia Qt6::Sql Qt6::Test
)

qt_add_translations(apptask3 TS_FILES ${TS_FILES})
enable_testing()
add_test(NAME player_tests COMMAND player_tests)