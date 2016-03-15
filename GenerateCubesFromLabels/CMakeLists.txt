cmake_minimum_required(VERSION 2.8)
 
PROJECT(GenerateCubesFromLabels)
 
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
 
add_executable(GenerateCubesFromLabels MACOSX_BUNDLE GenerateCubesFromLabels)
 
if(VTK_LIBRARIES)
  target_link_libraries(GenerateCubesFromLabels ${VTK_LIBRARIES})
else()
  target_link_libraries(GenerateCubesFromLabels vtkHybrid vtkWidgets)
endif()
