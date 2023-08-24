set(CORE_SOURCES
    qxtabstractconnectionmanager.cpp
    qxtabstractconnectionmanager.h
    qxtabstractfileloggerengine.cpp
    qxtabstractfileloggerengine.h
    qxtabstractiologgerengine.cpp
    qxtabstractiologgerengine.h
    qxtabstractsignalserializer.h
    qxtalgorithms.h
    qxtbasicfileloggerengine.cpp
    qxtbasicfileloggerengine.h
    qxtbasicstdloggerengine.cpp
    qxtbasicstdloggerengine.h
    qxtboundcfunction.h
    qxtboundfunction.h
    qxtboundfunctionbase.h
    qxtcommandoptions.cpp
    qxtcommandoptions.h
    qxtcore.h
    qxtcsvmodel.cpp
    qxtcsvmodel.h
    qxtcurrency.cpp
    qxtcurrency.h
    qxtdaemon.cpp
    qxtdaemon.h
    qxtdatastreamsignalserializer.cpp
    qxtdatastreamsignalserializer.h
    qxtdeplex_p.h
    qxtdeplex.cpp
    qxtdeplex.h
    qxterror.cpp
    qxterror.h
    qxtfifo.cpp
    qxtfifo.h
    qxtfilelock_p.h
    qxtfilelock.cpp
    qxtfilelock.h
    qxtglobal.cpp
    qxtglobal.h
    qxthmac.cpp
    qxthmac.h
    qxtjob_p.h
    qxtjob.cpp
    qxtjob.h
    qxtjson.cpp
    qxtjson.h
    qxtlinesocket_p.h
    qxtlinesocket.cpp
    qxtlinesocket.h
    qxtlinkedtree.cpp
    qxtlinkedtree.h
    qxtlocale_data_p.h
    qxtlocale.cpp
    qxtlocale.h
    qxtlogger_p.h
    qxtlogger.cpp
    qxtlogger.h
    qxtloggerengine.cpp
    qxtloggerengine.h
    qxtlogstream_p.h
    qxtlogstream.cpp
    qxtlogstream.h
    qxtmetaobject.cpp
    qxtmetaobject.h
    qxtmetatype.h
    qxtmodelserializer.cpp
    qxtmodelserializer.h
    qxtmultisignalwaiter.cpp
    qxtmultisignalwaiter.h
    qxtnamespace.h
    qxtnull.cpp
    qxtnull.h
    qxtnullable.h
    qxtpairlist.h
    qxtpimpl.h
    qxtpipe_p.h
    qxtpipe.cpp
    qxtpipe.h
    qxtpointerlist.cpp
    qxtpointerlist.h
    qxtrpcservice_p.h
    qxtrpcservice.cpp
    qxtrpcservice.h
    qxtsharedprivate.h
    qxtsignalgroup.cpp
    qxtsignalgroup.h
    qxtsignalwaiter.cpp
    qxtsignalwaiter.h
    qxtslotjob_p.h
    qxtslotjob.cpp
    qxtslotjob.h
    qxtslotmapper.cpp
    qxtslotmapper.h
    qxtstdio_p.h
    qxtstdio.cpp
    qxtstdio.h
    qxtstdstreambufdevice.cpp
    qxtstdstreambufdevice.h
    qxttemporarydir_p.h
    qxttemporarydir.cpp
    qxttemporarydir.h
    qxttimer.cpp
    qxttimer.h
    qxttypelist.h
    qxtxmlfileloggerengine.cpp
    qxtxmlfileloggerengine.h
)

if(WIN32)
    list(APPEND CORE_SOURCES
        win/qxtfilelock_win.cpp
        win/qxttemporarydir_win.cpp
    )
elseif(UNIX)
    list(APPEND CORE_SOURCES
        qxtfilelock_unix.cpp
        qxtserialdevice_p.h
        qxtserialdevice_unix.cpp
        qxtserialdevice.cpp
        qxtserialdevice.h
        qxttemporarydir_unix.cpp
    )
endif()

add_library(libqxtcore ${QXT_LIB_TYPE} ${CORE_SOURCES})
target_link_libraries(libqxtcore Qt${QT_MAJOR_VERSION}::Core)
target_include_directories(libqxtcore PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(libqxtcore PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../../include/QxtCore)
set_target_properties(libqxtcore
    PROPERTIES
    AUTOMOC ON
    AUTORCC ON
)
target_compile_definitions(libqxtcore PRIVATE BUILD_QXT_CORE)

if (QXT_STATIC)
    target_compile_definitions(libqxtcore PRIVATE QXT_STATIC)
endif()
