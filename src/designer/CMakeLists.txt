set(DESIGNER_SOURCES
    qxtbasespinboxplugin.cpp
    qxtbasespinboxplugin.h
    qxtcheckcomboboxplugin.cpp
    qxtcheckcomboboxplugin.h
    qxtcountrycomboboxplugin.cpp
    qxtcountrycomboboxplugin.h
    qxtdesignerplugin.cpp
    qxtdesignerplugin.h
    qxtdesignerplugins.cpp
    qxtdesignerplugins.h
    qxtflowviewplugin.cpp
    qxtflowviewplugin.h
    qxtgroupboxplugin.cpp
    qxtgroupboxplugin.h
    qxtlabelplugin.cpp
    qxtlabelplugin.h
    qxtlanguagecomboboxplugin.cpp
    qxtlanguagecomboboxplugin.h
    qxtletterboxwidgetplugin.cpp
    qxtletterboxwidgetplugin.h
    qxtlineeditplugin.cpp
    qxtlineeditplugin.h
    qxtlistwidgetplugin.cpp
    qxtlistwidgetplugin.h
    qxtprogresslabelplugin.cpp
    qxtprogresslabelplugin.h
    qxtpushbuttonplugin.cpp
    qxtpushbuttonplugin.h
    qxtspansliderplugin.cpp
    qxtspansliderplugin.h
    qxtstarsplugin.cpp
    qxtstarsplugin.h
    qxtstringspinboxplugin.cpp
    qxtstringspinboxplugin.h
    qxttablewidgetplugin.cpp
    qxttablewidgetplugin.h
    qxttreewidgetplugin.cpp
    qxttreewidgetplugin.h

    resources.qrc
)

add_library(libqxtdesigner ${QXT_LIB_TYPE} ${DESIGNER_SOURCES})
target_link_libraries(libqxtdesigner
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Gui
    Qt${QT_MAJOR_VERSION}::Widgets
    Qt${QT_MAJOR_VERSION}::Designer
    libqxtcore
    libqxtwidgets
)
target_include_directories(libqxtdesigner PUBLIC ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(libqxtdesigner
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
)
target_compile_definitions(libqxtdesigner PRIVATE BUILD_QXT_DESIGNER)

if (QXT_STATIC)
    target_compile_definitions(libqxtdesigner PRIVATE QXT_STATIC)
endif()
