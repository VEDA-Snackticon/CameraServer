cmake_minimum_required(VERSION 3.5)
project(Veda4Backend_test CXX)




file(GLOB SRC_FILES
        ${CMAKE_SOURCE_DIR}/models/*
        ${CMAKE_SOURCE_DIR}/controllers/*
)

add_executable(${PROJECT_NAME} test_main.cc
        CameraInfoControllerTest.cpp
)
message(${SRC_FILES})
target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/models)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/test)

#message(${CTL_SRC})
message(${CMAKE_SOURCE_DIR}/${CTL_SRC})
target_sources(${PROJECT_NAME}
        PRIVATE
        ${SRC_DIR}
        ${SRC_FILES}
)


#target_link_libraries(${PROJECT_NAME} PRIVATE Veda4Backend)

# ##############################################################################
# If you include the drogon source code locally in your project, use this method
# to add drogon
# target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
#
# and comment out the following lines
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

ParseAndAddDrogonTests(${PROJECT_NAME})
