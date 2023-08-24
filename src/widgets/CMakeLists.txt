set(WIDGETS_SOURCES
    qxtapplication_p.h
    qxtapplication.cpp
    qxtapplication.h
    qxtbasespinbox.cpp
    qxtbasespinbox.h
    qxtcheckcombobox_p.h
    qxtcheckcombobox.cpp
    qxtcheckcombobox.h
    qxtconfigdialog_p.h
    qxtconfigdialog.cpp
    qxtconfigdialog.h
    qxtconfigwidget_p.h
    qxtconfigwidget.cpp
    qxtconfigwidget.h
    qxtconfirmationmessage.cpp
    qxtconfirmationmessage.h
    qxtcountrycombobox_p.h
    qxtcountrycombobox.cpp
    qxtcountrycombobox.h
    qxtcountrymodel_p.h
    qxtcountrymodel.cpp
    qxtcountrymodel.h
    qxtcrumbview_p.h
    qxtcrumbview.cpp
    qxtcrumbview.h
    qxtfilterdialog_p.h
    qxtfilterdialog.cpp
    qxtfilterdialog.h
    qxtflowview_p.cpp
    qxtflowview_p.h
    qxtflowview.cpp
    qxtflowview.h
    qxtglobalshortcut_p.h
    qxtglobalshortcut.cpp
    qxtglobalshortcut.h
    qxtgroupbox.cpp
    qxtgroupbox.h
    qxtgui.h
    qxtheaderview.cpp
    qxtheaderview.h
    qxtitemdelegate_p.h
    qxtitemdelegate.cpp
    qxtitemdelegate.h
    qxtitemeditorcreator.h
    qxtitemeditorcreatorbase.h
    qxtlabel.cpp
    qxtlabel.h
    qxtlanguagecombobox_p.h
    qxtlanguagecombobox.cpp
    qxtlanguagecombobox.h
    qxtletterboxwidget_p.h
    qxtletterboxwidget.cpp
    qxtletterboxwidget.h
    qxtlineedit.cpp
    qxtlineedit.h
    qxtlistwidget_p.h
    qxtlistwidget.cpp
    qxtlistwidget.h
    qxtlistwidgetitem.cpp
    qxtlistwidgetitem.h
    qxtlookuplineedit_p.h
    qxtlookuplineedit.cpp
    qxtlookuplineedit.h
    qxtmultirootproxymodel_p.h
    qxtmultirootproxymodel.cpp
    qxtmultirootproxymodel.h
    qxtnativeeventfilter.h
    qxtprogresslabel.cpp
    qxtprogresslabel.h
    qxtproxystyle.cpp
    qxtproxystyle.h
    qxtpushbutton.cpp
    qxtpushbutton.h
    qxtscheduleheaderwidget.cpp
    qxtscheduleheaderwidget.h
    qxtscheduleitemdelegate.cpp
    qxtscheduleitemdelegate.h
    qxtscheduleview_p.cpp
    qxtscheduleview_p.h
    qxtscheduleview.cpp
    qxtscheduleview.h
    qxtscheduleviewheadermodel_p.cpp
    qxtscheduleviewheadermodel_p.h
    qxtscreen_p.h
    qxtscreen.cpp
    qxtscreen.h
    qxtsortfilterproxymodel.cpp
    qxtsortfilterproxymodel.h
    qxtspanslider_p.h
    qxtspanslider.cpp
    qxtspanslider.h
    qxtstandarditemeditorcreator.h
    qxtstars.cpp
    qxtstars.h
    qxtstringspinbox.cpp
    qxtstringspinbox.h
    qxtstringvalidator_p.h
    qxtstringvalidator.cpp
    qxtstringvalidator.h
    qxtstyleoptionscheduleviewitem.cpp
    qxtstyleoptionscheduleviewitem.h
    qxttablewidget_p.h
    qxttablewidget.cpp
    qxttablewidget.h
    qxttablewidgetitem.cpp
    qxttablewidgetitem.h
    qxttabwidget_p.h
    qxttabwidget.cpp
    qxttabwidget.h
    qxttooltip_p.h
    qxttooltip.cpp
    qxttooltip.h
    qxttreewidget_p.h
    qxttreewidget.cpp
    qxttreewidget.h
    qxttreewidgetitem.cpp
    qxttreewidgetitem.h
    qxtwindowsystem.cpp
    qxtwindowsystem.h

    resources.qrc
)

if(WIN32)
    list(APPEND WIDGETS_SOURCES
        win/qxtapplication_win.cpp
        win/qxtglobalshortcut_win.cpp
        win/qxtscreen_win.cpp
        win/qxtwindowsystem_win.cpp
    )
elseif(LINUX)
    list(APPEND WIDGETS_SOURCES
        x11/qxtapplication_x11.cpp
        x11/qxtglobalshortcut_x11.cpp
        x11/qxtscreen_x11.cpp
        x11/qxtwindowsystem_x11.cpp
    )
elseif(APPLE)
    list(APPEND WIDGETS_SOURCES
        mac/qxtapplication_mac.cpp
        mac/qxtglobalshortcut_mac.cpp
        mac/qxtwindowsystem_mac.cpp
        mac/qxtwindowsystem_mac.h
    )
endif()

add_library(libqxtwidgets ${QXT_LIB_TYPE} ${WIDGETS_SOURCES})
target_link_libraries(libqxtwidgets Qt${QT_MAJOR_VERSION}::Core Qt${QT_MAJOR_VERSION}::Widgets libqxtcore)
target_include_directories(libqxtwidgets PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libqxtwidgets PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/QxtWidgets)
set_target_properties(libqxtwidgets
    PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)
target_compile_definitions(libqxtwidgets PRIVATE BUILD_QXT_GUI)

if (QXT_STATIC)
    target_compile_definitions(libqxtwidgets PRIVATE QXT_STATIC)
endif()
