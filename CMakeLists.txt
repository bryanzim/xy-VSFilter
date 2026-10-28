cmake_minimum_required(VERSION 3.24)

# Static CRT (/MT, /MTd), matching the Visual Studio projects.
cmake_policy(SET CMP0091 NEW)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(xy-VSFilter LANGUAGES CXX RC)

if(NOT WIN32)
    message(FATAL_ERROR "xy-VSFilter builds on Windows.")
endif()

include(CTest)
include(cmake/xy_sources.cmake)
include(cmake/xy_warnings.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

find_program(XY_YASM yasm HINTS "$ENV{TEMP}/xy-yasm" "$ENV{LOCALAPPDATA}/xy-yasm")
if(NOT XY_YASM)
    message(FATAL_ERROR "yasm.exe was not found. Put it on PATH or in %TEMP%\\xy-yasm.")
endif()

set(XY_ROOT "${CMAKE_SOURCE_DIR}")
set(XY_SRC "${XY_ROOT}/src")
set(XY_INCLUDE "${XY_ROOT}/include")
set(XY_VSFILTER "${XY_SRC}/filters/transform/vsfilter")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(XY_WIN64 TRUE)
else()
    set(XY_WIN64 FALSE)
endif()

add_compile_definitions(
    WINVER=0x0601
    _WIN32_WINNT=0x0601
    PSAPI_VERSION=1
    _WINDOWS
    WINDOWS
    _USE_MATH_DEFINES
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Release>:_CRT_SECURE_NO_WARNINGS>
    $<$<CONFIG:Release>:_SCL_SECURE_NO_WARNINGS>)
if(XY_WIN64)
    add_compile_definitions(_WIN64 WIN64)
else()
    add_compile_definitions(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(/arch:SSE2)
    else()
        add_compile_options(/arch:IA32)
    endif()
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions(BOOST_USE_WINDOWS_H)
endif()

function(xy_prefix_sources out_var directory)
    set(prefixed "")
    foreach(name IN LISTS ARGN)
        list(APPEND prefixed "${directory}/${name}")
    endforeach()
    set(${out_var} "${prefixed}" PARENT_SCOPE)
endfunction()

function(xy_skip_pch)
    foreach(path IN LISTS ARGN)
        set_source_files_properties("${path}" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
    endforeach()
endfunction()

function(xy_use_pch target header)
    target_precompile_headers(${target} PRIVATE "${header}")
endfunction()

function(xy_add_yasm target directory)
    if(XY_WIN64)
        set(yasm_format win64)
    else()
        set(yasm_format win32)
    endif()
    foreach(name IN LISTS ARGN)
        get_filename_component(stem "${name}" NAME_WE)
        set(source "${directory}/${name}")
        set(object "${CMAKE_CURRENT_BINARY_DIR}/yasm/${target}/${stem}.obj")
        add_custom_command(
            OUTPUT "${object}"
            COMMAND "${XY_YASM}" -X vc -f ${yasm_format} -I "${directory}/source" -o "${object}" "${source}"
            DEPENDS "${source}"
            VERBATIM)
        target_sources(${target} PRIVATE "${object}")
    endforeach()
endfunction()

function(xy_unicode target)
    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)
endfunction()

# Third-party libraries. These do not use MFC.

xy_prefix_sources(unrar_sources "${XY_SRC}/thirdparty/unrar" ${XY_UNRAR_SOURCES})
add_library(unrar STATIC ${unrar_sources})
target_compile_definitions(unrar PRIVATE _LIB RARDLL UNRAR SILENT _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES _CRT_SECURE_NO_WARNINGS)
xy_third_party_warnings(unrar)
xy_use_pch(unrar "${XY_SRC}/thirdparty/unrar/rar.hpp")
xy_skip_pch("${XY_SRC}/thirdparty/unrar/global.cpp" "${XY_SRC}/thirdparty/unrar/rarpch.cpp")
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(unrar PRIVATE /clang:-fgnuc-version=4.2.1)
endif()

set(XY_KASUMI "${XY_SRC}/thirdparty/VirtualDub/Kasumi")
xy_prefix_sources(kasumi_sources "${XY_KASUMI}" ${XY_KASUMI_SOURCES})
add_library(Kasumi STATIC ${kasumi_sources})
target_include_directories(Kasumi SYSTEM PUBLIC "${XY_KASUMI}/h" "${XY_SRC}/thirdparty/VirtualDub/h")
target_compile_definitions(Kasumi PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
target_compile_features(Kasumi PRIVATE cxx_std_17)
xy_unicode(Kasumi)
xy_third_party_warnings(Kasumi)
xy_use_pch(Kasumi "${XY_KASUMI}/h/stdafx.h")
if(XY_WIN64)
    xy_prefix_sources(kasumi_x64 "${XY_KASUMI}" ${XY_KASUMI_X64_SOURCES})
    target_sources(Kasumi PRIVATE ${kasumi_x64})
    xy_skip_pch(${kasumi_x64})
    xy_add_yasm(Kasumi "${XY_KASUMI}" ${XY_KASUMI_X64_ASM})
else()
    xy_prefix_sources(kasumi_win32 "${XY_KASUMI}" ${XY_KASUMI_WIN32_SOURCES})
    target_sources(Kasumi PRIVATE ${kasumi_win32})
    xy_add_yasm(Kasumi "${XY_KASUMI}" ${XY_KASUMI_WIN32_ASM})
endif()

set(XY_SYSTEM "${XY_SRC}/thirdparty/VirtualDub/system")
xy_prefix_sources(system_sources "${XY_SYSTEM}" ${XY_SYSTEM_SOURCES})
add_library(vd_system STATIC ${system_sources})
target_include_directories(vd_system SYSTEM PUBLIC "${XY_SYSTEM}/h" "${XY_SRC}/thirdparty/VirtualDub/h")
target_compile_definitions(vd_system PRIVATE _LIB NOMINMAX WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS _SCL_SECURE_NO_WARNINGS)
target_compile_features(vd_system PRIVATE cxx_std_17)
target_link_libraries(vd_system PUBLIC winmm)
xy_unicode(vd_system)
xy_third_party_warnings(vd_system)
xy_use_pch(vd_system "${XY_SYSTEM}/h/stdafx.h")
if(XY_WIN64)
    xy_add_yasm(vd_system "${XY_SYSTEM}" ${XY_SYSTEM_X64_ASM})
else()
    xy_add_yasm(vd_system "${XY_SYSTEM}" ${XY_SYSTEM_WIN32_ASM})
endif()
set_source_files_properties("${XY_SYSTEM}/source/stdaccel.cpp" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_sources(vd_system PRIVATE "$<$<CONFIG:Debug>:${XY_SYSTEM}/source/stdaccel.cpp>")

set(XY_LOG4CPLUS "${XY_SRC}/thirdparty/log4cplus/msvc")
xy_prefix_sources(log4cplus_sources "${XY_LOG4CPLUS}" ${XY_LOG4CPLUS_SOURCES})
add_library(log4cplus_static STATIC ${log4cplus_sources})
target_include_directories(log4cplus_static SYSTEM PUBLIC
    "${XY_SRC}/thirdparty/log4cplus/include"
    "${XY_SRC}/thirdparty/log4cplus/threadpool")
target_compile_definitions(log4cplus_static PUBLIC LOG4CPLUS_STATIC)
target_compile_definitions(log4cplus_static PRIVATE
    _LIB INSIDE_LOG4CPLUS _SCL_SECURE_NO_DEPRECATE _CRT_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(log4cplus_static PUBLIC ws2_32)
xy_unicode(log4cplus_static)
xy_third_party_warnings(log4cplus_static)

set(XY_GTEST "${XY_SRC}/thirdparty/gtest")
add_library(gtest STATIC "${XY_GTEST}/src/gtest-all.cc")
target_include_directories(gtest SYSTEM PUBLIC "${XY_GTEST}/include" "${XY_GTEST}")
target_compile_definitions(gtest PRIVATE _LIB)
target_compile_features(gtest PUBLIC cxx_std_17)
xy_unicode(gtest)
xy_third_party_warnings(gtest)

# DirectShow base classes. stdcall, no precompiled header.
set(XY_BASE "${XY_SRC}/filters/BaseClasses")
xy_prefix_sources(strmbase_sources "${XY_BASE}" ${XY_STRMBASE_SOURCES})
add_library(strmbase STATIC ${strmbase_sources})
target_include_directories(strmbase SYSTEM PUBLIC "${XY_BASE}")
target_compile_definitions(strmbase PRIVATE _LIB)
if(NOT XY_WIN64)
    target_compile_definitions(strmbase PRIVATE _WIN32_DCOM)
endif()
target_compile_options(strmbase PRIVATE /Gz)
target_link_libraries(strmbase PUBLIC strmiids)
xy_unicode(strmbase)
xy_third_party_warnings(strmbase)

# Static MFC for the filter libraries, the three DLLs, and the unit test.
set(CMAKE_MFC_FLAG 1)

set(XY_DSUTIL "${XY_SRC}/dsutil")
xy_prefix_sources(dsutil_sources "${XY_DSUTIL}" ${XY_DSUTIL_SOURCES})
add_library(dsutil STATIC ${dsutil_sources})
target_include_directories(dsutil PUBLIC "${XY_DSUTIL}")
target_include_directories(dsutil SYSTEM PUBLIC "${XY_BASE}" "${XY_SRC}/thirdparty/VirtualDub/h")
target_compile_definitions(dsutil PRIVATE _LIB)
target_link_libraries(dsutil PUBLIC strmbase vfw32 version)
xy_unicode(dsutil)
xy_first_party_warnings(dsutil)
xy_use_pch(dsutil "${XY_DSUTIL}/stdafx.h")

set(XY_SUBPIC "${XY_SRC}/subpic")
xy_prefix_sources(subpic_sources "${XY_SUBPIC}" ${XY_SUBPIC_SOURCES})
add_library(subpic STATIC ${subpic_sources})
target_include_directories(subpic PUBLIC
    "${XY_SUBPIC}"
    "${XY_VSFILTER}"
    "${XY_INCLUDE}")
target_include_directories(subpic SYSTEM PUBLIC
    "${XY_INCLUDE}/dx"
    "${XY_INCLUDE}/avisynth"
    "${XY_INCLUDE}/vd2"
    "${XY_INCLUDE}/winddk"
    "${XY_BASE}"
    "${XY_SRC}/thirdparty/log4cplus/include"
    "${XY_SRC}/thirdparty/boost_lib")
target_compile_definitions(subpic PRIVATE _LIB)
target_link_libraries(subpic PUBLIC d3d9 strmbase log4cplus_static)
xy_unicode(subpic)
xy_first_party_warnings(subpic)
xy_use_pch(subpic "${XY_SUBPIC}/stdafx.h")
xy_prefix_sources(subpic_no_pch "${XY_SUBPIC}" ${XY_SUBPIC_NO_PCH})
xy_skip_pch(${subpic_no_pch})

set(XY_SUBTITLES "${XY_SRC}/subtitles")
xy_prefix_sources(subtitles_sources "${XY_SUBTITLES}" ${XY_SUBTITLES_SOURCES})
add_library(subtitles STATIC ${subtitles_sources})
target_include_directories(subtitles PUBLIC
    "${XY_SUBTITLES}"
    "${XY_VSFILTER}"
    "${XY_INCLUDE}")
target_include_directories(subtitles SYSTEM PUBLIC
    "${XY_BASE}"
    "${XY_SRC}/thirdparty/log4cplus/include"
    "${XY_SRC}/thirdparty/boost_lib"
    "${XY_INCLUDE}/unrar"
    "${XY_INCLUDE}/avisynth"
    "${XY_INCLUDE}/vd2"
    "${XY_INCLUDE}/winddk"
    "${XY_SRC}/thirdparty")
target_link_libraries(subtitles PUBLIC strmbase log4cplus_static unrar)
if(XY_WIN64)
    target_link_libraries(subtitles PUBLIC "${XY_ROOT}/lib/libacof64.lib")
else()
    target_link_libraries(subtitles PUBLIC "${XY_ROOT}/lib/libacof32.lib")
endif()
target_compile_definitions(subtitles PRIVATE NTDDI_VERSION=NTDDI_WIN7)
xy_unicode(subtitles)
xy_first_party_warnings(subtitles)
xy_use_pch(subtitles "${XY_SUBTITLES}/stdafx.h")
xy_skip_pch("${XY_SUBTITLES}/xy_malloc.cpp")

set(XY_LIBSSF "${XY_SRC}/subtitles/libssf")
xy_prefix_sources(libssf_sources "${XY_LIBSSF}" ${XY_LIBSSF_SOURCES})
add_library(libssf STATIC ${libssf_sources})
target_include_directories(libssf PUBLIC "${XY_LIBSSF}")
target_include_directories(libssf SYSTEM PUBLIC "${XY_BASE}")
target_compile_definitions(libssf PRIVATE NTDDI_VERSION=NTDDI_WIN7)
target_link_libraries(libssf PUBLIC strmbase)
xy_unicode(libssf)
xy_first_party_warnings(libssf)
xy_use_pch(libssf "${XY_LIBSSF}/stdafx.h")

set(XY_BASEVIDEO "${XY_SRC}/filters/transform/basevideofilter")
add_library(basevideofilter STATIC
    "${XY_BASEVIDEO}/BaseVideoFilter.cpp"
    "${XY_BASEVIDEO}/stdafx.cpp")
target_include_directories(basevideofilter PUBLIC
    "${XY_BASEVIDEO}"
    "${XY_VSFILTER}"
    "${XY_INCLUDE}")
target_include_directories(basevideofilter SYSTEM PUBLIC
    "${XY_BASE}"
    "${XY_SRC}/thirdparty/boost_lib"
    "${XY_SRC}/thirdparty/log4cplus/include"
    "${XY_SRC}/thirdparty/VirtualDub/h"
    "${XY_INCLUDE}/avisynth"
    "${XY_INCLUDE}/vd2"
    "${XY_INCLUDE}/winddk")
target_link_libraries(basevideofilter PUBLIC strmbase subpic)
xy_unicode(basevideofilter)
xy_first_party_warnings(basevideofilter)
xy_use_pch(basevideofilter "${XY_BASEVIDEO}/stdafx.h")

function(xy_add_filter target output_name definition def_file)
    xy_prefix_sources(shared "${XY_VSFILTER}" ${XY_FILTER_SHARED_SOURCES})
    add_library(${target} SHARED ${shared} "${XY_VSFILTER}/${def_file}")
    set_target_properties(${target} PROPERTIES OUTPUT_NAME "${output_name}")
    target_include_directories(${target} PRIVATE
        "${XY_VSFILTER}"
        "${XY_INCLUDE}")
    target_include_directories(${target} SYSTEM PRIVATE
        "${XY_BASE}"
        "${XY_SRC}/thirdparty/boost_lib"
        "${XY_SRC}/thirdparty/log4cplus/include"
        "${XY_SRC}/thirdparty/VirtualDub/h"
        "${XY_INCLUDE}/dx"
        "${XY_INCLUDE}/avisynth"
        "${XY_INCLUDE}/vd2"
        "${XY_INCLUDE}/winddk"
        "${XY_INCLUDE}/unrar")
    target_compile_definitions(${target} PRIVATE
        _AFX_NO_MFC_CONTROLS_IN_DIALOGS
        AFX_RESOURCE_DLL
        AFX_TARG_ENU)
    if(definition)
        target_compile_definitions(${target} PRIVATE "${definition}")
    endif()
    target_link_libraries(${target} PRIVATE
        strmbase dsutil subtitles subpic basevideofilter libssf
        log4cplus_static Kasumi vd_system unrar
        dwmapi mfuuid delayimp)
    target_link_options(${target} PRIVATE "/DELAYLOAD:oleacc.dll")
    xy_unicode(${target})
    xy_first_party_warnings(${target})
    xy_use_pch(${target} "${XY_VSFILTER}/StdAfx.h")
    xy_skip_pch("${XY_VSFILTER}/once_logger.cpp")
endfunction()

xy_add_filter(VSFilter "VSFilter" "" "VSFilter.def")
target_sources(VSFilter PRIVATE
    "${XY_VSFILTER}/Copy.cpp"
    "${XY_VSFILTER}/csriapi.cpp"
    "${XY_VSFILTER}/DirectVobSubFilter.cpp"
    "${XY_VSFILTER}/DirectVobSubFilterDll.cpp"
    "${XY_VSFILTER}/plugins.cpp"
    "${XY_VSFILTER}/Systray.cpp"
    "${XY_VSFILTER}/TextInputPin.cpp"
    "${XY_VSFILTER}/vfr.cpp")

xy_add_filter(XySubFilter "XySubFilter" XY_SUB_FILTER_DLL "xy_sub_filter.def")
target_sources(XySubFilter PRIVATE
    "${XY_VSFILTER}/auto_load_helper.cpp"
    "${XY_VSFILTER}/SubtitleInputPin2.cpp"
    "${XY_VSFILTER}/Systray.cpp"
    "${XY_VSFILTER}/xy_sub_filter.cpp"
    "${XY_VSFILTER}/xy_sub_filter_dll.cpp")

xy_add_filter(xy_sub_filter_consumer "xy_sub_filter_consumer" XY_SUB_FILTER_CONSUMER_DLL "xy_sub_filter_consumer.def")
target_sources(xy_sub_filter_consumer PRIVATE
    "${XY_VSFILTER}/vfr.cpp"
    "${XY_VSFILTER}/xy_sub_filter_consumer.cpp"
    "${XY_VSFILTER}/xy_sub_filter_consumer_dll.cpp")

set(XY_TEST "${XY_ROOT}/test/unit_test")
add_executable(unit_test "${XY_TEST}/main.cpp" "${XY_TEST}/overall_test.cpp")
target_include_directories(unit_test PRIVATE
    "${XY_TEST}"
    "${XY_SUBTITLES}"
    "${XY_SUBPIC}"
    "${XY_VSFILTER}")
target_include_directories(unit_test SYSTEM PRIVATE
    "${XY_GTEST}/include"
    "${XY_SRC}/thirdparty/boost_lib")
target_compile_features(unit_test PRIVATE cxx_std_17)
target_compile_definitions(unit_test PRIVATE _ATL_STATIC_REGISTRY AFX_RESOURCE_DLL AFX_TARG_ENU)
target_link_libraries(unit_test PRIVATE gtest subtitles subpic strmbase VSFilter delayimp atls)
target_link_options(unit_test PRIVATE "/SUBSYSTEM:CONSOLE" "/ENTRY:wmainCRTStartup" "/DELAYLOAD:oleacc.dll")
xy_unicode(unit_test)
xy_first_party_warnings(unit_test)

if(BUILD_TESTING)
    file(GLOB XY_TEST_SCRIPTS "${XY_ROOT}/test/*.ass")
    foreach(script IN LISTS XY_TEST_SCRIPTS)
        get_filename_component(script_name "${script}" NAME_WE)
        add_test(NAME "overall_${script_name}" COMMAND unit_test "${script}")
        set_tests_properties("overall_${script_name}" PROPERTIES
            WORKING_DIRECTORY "${XY_ROOT}/test"
            ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:VSFilter>")
    endforeach()
endif()
