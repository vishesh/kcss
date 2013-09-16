cmake_minimum_required(VERSION 2.8)
project (kcss)

find_package(KDE4 REQUIRED)

include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

set(kcss_SRCS 
    main.cpp
    )

kde4_add_executable(kcss ${kcss_SRCS})
target_link_libraries(kcss ${KDE4_KDEUI_LIBS} ${KDE4_KDECORE_LIBS} )

install(TARGETS kcss  ${INSTALL_TARGETS_DEFAULT_ARGS})

