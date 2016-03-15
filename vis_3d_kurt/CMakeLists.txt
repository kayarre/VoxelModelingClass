cmake_minimum_required(VERSION 3.0)
 
PROJECT(vis_3d)
 
#find_package(VTK REQUIRED)
find_package(CUDA REQUIRED)
find_package(GLEW REQUIRED )
find_package(GLUT)
find_package(OpenGL REQUIRED)

set(CUDA_SEPARABLE_COMPILATION ON)
set(CUDA_PROPAGATE_HOST_FLAGS OFF)

set(SOURCE main.cpp )
set(HEADERS	interactions.h )
  
set(KERNEL_S kernel.cu device_funcs.cu )
set(KERNEL_h kernel.h device_funcs.cuh )

include_directories(${vis_3d_SOURCE_DIR})

include_directories(${CUDA_INCLUDE_DIRS}
                    ${OPENGL_INCLUDE_DIRS} 
                    ${GLUT_INCLUDE_DIRS}
                    $(GLEW_INCLUDE_DIRS)
                    /usr/local/cuda/samples/common/inc
                    )
 
CUDA_ADD_LIBRARY(device_funcs device_funcs.cu)
target_link_libraries(device_funcs ${CUDA_LIBRARIES})
                
CUDA_ADD_LIBRARY(kernel_device kernel.cu device_funcs.cu )
target_link_libraries(kernel_device ${CUDA_LIBRARIES})

include_directories(${kernel_device} ${device_funcs})

cuda_add_executable(vis_3d 
		${SOURCE} ${HEADERS}
		${KERNEL_h}
		) 
 
#include_directories(${kernel_device} ${device_funcs})
target_link_libraries(vis_3d
		#${VTK_LIBRARIES}
		${kernel_device} ${device_funcs}
		${CUDA_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}
		${GLEW_LIBRARIES} -lglut -lGL -lGLU -lGLEW
		)

