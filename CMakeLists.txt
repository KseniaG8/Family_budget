cmake_minimum_required(VERSION 3.5)

project(BudgetClient VERSION 0.1 LANGUAGES CXX)

option(BUILD_CLIENT "Build Qt client" ON)
option(BUILD_SERVER "Build server" ON)

if(BUILD_CLIENT)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        src/client_main.cpp
        src/mainwindow.cpp
        inc/mainwindow.h
        ui/mainwindow.ui
        src/registrationdialog.cpp
        inc/registrationdialog.h
        ui/registrationdialog.ui
        src/goalsdialog.cpp
        inc/goalsdialog.h
        src/setup2FAdialog.cpp
        inc/setup2FAdialog.h
        src/Verify2FADialog.cpp
        inc/Verify2FADialog.h
        ui/budgetlimitsdialog.ui
        ui/alltransactionsdialog.ui
        src/budgetlimitsdialog.cpp
        inc/budgetlimitsdialog.h
        src/alltransactionsdialog.cpp
        inc/alltransactionsdialog.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BudgetClient
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        inc/goalsdialog.h
        src/goalsdialog.cpp
        ui/goalsdialog.ui

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BudgetClient APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BudgetClient SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BudgetClient
            ${PROJECT_SOURCES}
        )
        target_include_directories(BudgetClient PRIVATE
            ${CMAKE_CURRENT_BINARY_DIR}/BudgetClient_autogen/include
        )
    endif()
endif()

target_link_libraries(BudgetClient PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BudgetClient)
endif()
set_target_properties(BudgetClient PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BudgetClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BudgetClient)
endif()

endif()

if(BUILD_SERVER)

find_package(Boost REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_library(PUGIXML_LIBRARY NAMES pugixml PATHS /opt/homebrew/lib /usr/lib)
find_path(BOTAN_INCLUDE_DIR botan/totp.h
    PATHS
        /opt/homebrew/include/botan-3
        /opt/homebrew/opt/botan/include/botan-3
)

find_library(BOTAN_LIBRARY
    NAMES botan-3 botan-2
    PATHS
        /opt/homebrew/opt/botan/lib
        /usr/lib
        /usr/lib/aarch64-linux-gnu
        /usr/lib/x86_64-linux-gnu
)

add_executable(BudgetServer
    main.cpp
    
    # База данных
    database/Database.cpp
    database/UserDatabase.cpp
    database/TransactionDatabase.cpp
    database/LimitDatabase.cpp
    database/GoalDatabase.cpp
    database/GroupDatabase.cpp
    
    services/CryptoHelper.cpp
    services/CurrencyService.cpp
    services/MLCategorizer.cpp
    services/TransactionService.cpp
    services/UserService.cpp
    
    handlers/TransactionHandler.cpp
    handlers/UserHandler.cpp
    
    server/Server.cpp
    server/Session.cpp
)

target_compile_features(BudgetServer PRIVATE cxx_std_20)

set_target_properties(BudgetServer PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(BudgetServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIRS}
    ${SQLite3_INCLUDE_DIRS}
    ${BOTAN_INCLUDE_DIR}
)

target_link_libraries(BudgetServer PRIVATE
    SQLite::SQLite3
    ${BOTAN_LIBRARY}
    OpenSSL::SSL
    OpenSSL::Crypto
    fmt::fmt
    spdlog::spdlog
    ${PUGIXML_LIBRARY}
    "-framework CoreFoundation"
    "-framework Security"
)

endif()
