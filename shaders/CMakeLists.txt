# compile glsl shaders to spir-v
add_custom_command( 
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders/gradient.comp.spv
	COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/gradient.comp -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/gradient.comp.spv
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/gradient.comp
	COMMENT "Compiling shader gradient.comp"
)

if(DEFINED ENV{DEBUG_SHADER} AND "$ENV{DEBUG_SHADER}" STREQUAL "true")
	execute_process(COMMAND glslangValidator -g --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader.spv)
elseif()
	execute_process(COMMAND glslangValidator  --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader.spv)
endif()
