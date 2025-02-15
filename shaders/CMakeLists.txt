# compile glsl shaders to spir-v
add_custom_command( 
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders/gradient.comp.spv
	COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/gradient.comp -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/gradient.comp.spv
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/gradient.comp
	COMMENT "Compiling shader gradient.comp"
)

if(DEFINED ENV{DEBUG_SHADERS} AND "$ENV{DEBUG_SHADERS}" STREQUAL "true")
	message("Building debug shaders")
	execute_process(COMMAND glslangValidator -g --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_512.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_512.spv)
	execute_process(COMMAND glslangValidator -g --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_1024.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_1024.spv)
	execute_process(COMMAND glslangValidator -g --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_256.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_256.spv)
	execute_process(COMMAND glslangValidator -g --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_subgroup.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_subgroup.spv)
else()
	message("Building release shaders")
	execute_process(COMMAND glslangValidator  --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_512.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_512.spv)
	execute_process(COMMAND glslangValidator  --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_1024.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_1024.spv)
	execute_process(COMMAND glslangValidator  --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_256.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_256.spv)
	execute_process(COMMAND glslangValidator  --target-env vulkan1.3 ${CMAKE_CURRENT_SOURCE_DIR}/heat_shader_subgroup.comp -o ${CMAKE_CURRENT_BINARY_DIR}/heat_shader_subgroup.spv)
endif()
