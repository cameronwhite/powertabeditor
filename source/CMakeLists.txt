cmake_minimum_required(VERSION 2.8.8)

add_subdirectory(actions)
add_subdirectory(app)
add_subdirectory(audio)
add_subdirectory(dialogs)
add_subdirectory(formats)
add_subdirectory(painters)
add_subdirectory(powertabdocument)
add_subdirectory(score)
add_subdirectory(sigfwd)
add_subdirectory(widgets)

qt5_add_resources(RESOURCES build/resources.qrc)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(RC_FILE build/build.rc)
endif()

add_executable(powertabeditor WIN32
    build/main.cpp
    ${RC_FILE}
    ${RESOURCES}
)

qt5_use_modules(powertabeditor Widgets)

target_link_libraries(powertabeditor
    pteapp
    ptewidgets
    ptedialogs
    pteactions
    pteaudio
    rtmidi
    ptepainters
    sigfwd
    pteformats
    ptedocument
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Link against the ALSA library.
    target_link_libraries(powertabeditor ${ALSA_LIBRARY} pthread)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Link against winmm and QtMain
    target_link_libraries(powertabeditor winmm Qt5::WinMain)
endif()

# Copy the tuning database and the skins to the build directory.
file(COPY data skins DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copy the Qt dlls to the build folder on Windows.
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(DEBUG_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/Debug)
    set(RELEASE_BIN_DIR ${CMAKE_CURRENT_BINARY_DIR}/Release)
    set(QT_BIN_DIR ${Qt5Core_DIR}/../../../bin)

    add_custom_command(TARGET powertabeditor POST_BUILD
	    COMMAND ${CMAKE_COMMAND} -E make_directory ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Widgetsd.dll ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Guid.dll ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Cored.dll ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libEGLd.dll ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libGLESv2d.dll ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/icuin51.dll ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/icuuc51.dll ${DEBUG_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/icudt51.dll ${DEBUG_BIN_DIR}

	    COMMAND ${CMAKE_COMMAND} -E make_directory ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Widgets.dll ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Gui.dll ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/Qt5Core.dll ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libEGL.dll ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/libGLESv2.dll ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/icuin51.dll ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/icuuc51.dll ${RELEASE_BIN_DIR}
	    COMMAND ${CMAKE_COMMAND} -E copy ${QT_BIN_DIR}/icudt51.dll ${RELEASE_BIN_DIR}
    )
endif()