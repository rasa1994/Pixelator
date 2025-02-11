include ("../cmake/LoadImageHandler.cmake")

add_library(imageextracter STATIC ImageExtracter.ixx)

target_link_libraries(imageextracter PUBLIC imagehandler)
target_link_directories(imageextracter PUBLIC ${CMAKE_SOURCE_DIR} ${imagehandler_SOURCE_DIR})