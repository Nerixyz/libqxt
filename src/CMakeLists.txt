add_subdirectory(core)
add_subdirectory(widgets)
add_subdirectory(network)
add_subdirectory(web)
if (DNSSD_FOUND)
    add_subdirectory(zeroconf)
endif()
add_subdirectory(designer)
add_subdirectory(sql)
