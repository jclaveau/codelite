# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
 
# Our project is called 'codelite' this is how it will be called in
# visual studio, and in our makefiles. 
project(libcodelite)

# It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'.
find_package(wxWidgets COMPONENTS std stc REQUIRED)

# wxWidgets include (this will do all the magic to configure everything)
include( "${wxWidgets_USE_FILE}" )

# Flex files
if (UNIX AND NOT APPLE)
    if(WITH_FLEX)
        message("-- Adding flex support")
        find_package(FLEX REQUIRED)
        
        set(CxxFlexOutput ${CMAKE_SOURCE_DIR}/CodeLite/CxxLexer.cpp)
        set(CxxFlexInput ${CMAKE_SOURCE_DIR}/CodeLite/CxxScanner.l)
        
        set(PhpFlexOutput ${CMAKE_SOURCE_DIR}/CodeLite/PhpLexer.cpp)
        set(PhpFlexInput ${CMAKE_SOURCE_DIR}/CodeLite/PhpLexer.l)
        
        set(JSFlexOutput ${CMAKE_SOURCE_DIR}/CodeLite/JSLexer.cpp)
        set(JSFlexInput ${CMAKE_SOURCE_DIR}/CodeLite/JSLexer.l)
        
        if(FLEX_FOUND)
            add_custom_command(
                OUTPUT ${CxxFlexOutput}
                COMMAND ${FLEX_EXECUTABLE} --noline --yylineno --batch --outfile=${CxxFlexOutput} ${CxxFlexInput}
                DEPENDS ${CxxFlexInput}
                COMMENT "Generating ${CxxFlexOutput}")
            add_custom_command(
                OUTPUT ${PhpFlexOutput}
                COMMAND ${FLEX_EXECUTABLE} -Pphp --yylineno --noline --batch --outfile=${PhpFlexOutput} ${PhpFlexInput}
                DEPENDS ${PhpFlexInput}
                COMMENT "Generating ${PhpFlexOutput}")
            add_custom_command(
                OUTPUT ${JSFlexOutput}
                COMMAND ${FLEX_EXECUTABLE} -Pjs --yylineno --noline --batch --outfile=${JSFlexOutput} ${JSFlexInput}
                DEPENDS ${JSFlexInput}
                COMMENT "Generating ${JSFlexOutput}")
        endif()
    else()
        message("-- Flex support is disabled")
    endif()
endif()

# Include paths
include_directories("${CL_SRC_ROOT}/sdk/wxsqlite3/include" 
                    "${CL_SRC_ROOT}/sdk/codelite_indexer/network" 
                    "${CL_SRC_ROOT}/CodeLite" 
                    "${CL_SRC_ROOT}/PCH"
                    "${CL_SRC_ROOT}/Interfaces")

set( ADDITIONAL_LIBRARIES "" )

if (UNIX)
    if ( IS_FREEBSD )
        set(ADDITIONAL_LIBRARIES "-lkvm")
    elseif ( UNIX AND NOT APPLE )
        set(ADDITIONAL_LIBRARIES "-ldl -lutil")
    else ( )
        set(ADDITIONAL_LIBRARIES "-ldl")
    endif ( )
else (UNIX)
    set(ADDITIONAL_LIBRARIES "-lws2_32")
endif (UNIX)

## Macros
if(WIN32)
    add_definitions(-DWXMAKINGDLL_CL)
endif(WIN32)

if ( USE_PCH )
    add_definitions(-include "${CL_PCH_FILE}")
    add_definitions(-Winvalid-pch)
endif ( USE_PCH )

if (UNIX AND NOT APPLE)
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )
endif()

if ( APPLE )
    add_definitions(-fPIC)
endif()

add_definitions(-DWXUSINGDLL_WXSQLITE3)

# Add RPATH
if (NOT MINGW)
if ( WXC_APP )
    string( REPLACE "codelite" "wxcrafter" WXC_LIBS_DIR ${PLUGINS_DIR}) 
    set (LINKER_OPTIONS -Wl,-rpath,"${WXC_LIBS_DIR}:${PLUGINS_DIR}")
    message( "-- libcodelite.so is using RPATH set to ${WXC_LIBS_DIR}:${PLUGINS_DIR}")
else ( WXC_APP )
    set (LINKER_OPTIONS -Wl,-rpath,"${PLUGINS_DIR}")
    message( "-- libcodelite.so is using RPATH set to ${PLUGINS_DIR}")
endif ( WXC_APP )
endif (NOT MINGW)
FILE(GLOB SRCS "*.cpp" "../sdk/codelite_indexer/network/*.cpp" "SocketAPI/*.cpp")

# Define the output
add_library(libcodelite SHARED ${SRCS})
if (UNIX AND NOT APPLE )
    target_link_libraries(libcodelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" -lsqlite3lib -lwxsqlite3 ${LIBSSH_LIB} ${ADDITIONAL_LIBRARIES})
else (UNIX AND NOT APPLE)
    target_link_libraries(libcodelite ${LINKER_OPTIONS} ${wxWidgets_LIBRARIES} -L"${CL_LIBPATH}" -lsqlite3lib -lwxsqlite3 ${LIBSSH_LIB} ${ADDITIONAL_LIBRARIES} -lz -lcrypto)
endif ( UNIX AND NOT APPLE )

if (NOT MINGW)
install(TARGETS libcodelite DESTINATION ${PLUGINS_DIR})
else ()
install(TARGETS libcodelite RUNTIME DESTINATION ${CL_PREFIX}/bin LIBRARY DESTINATION ${CL_PREFIX}/lib ARCHIVE DESTINATION ${CL_PREFIX}/lib)
endif()
