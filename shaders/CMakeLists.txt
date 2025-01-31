# compile glsl shaders to spir-v
add_custom_command( 
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders/gradient.comp.spv
	COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/shaders/gradient.comp -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/gradient.comp.spv
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/shaders/gradient.comp
	COMMENT "Compiling shader gradient.comp"
)
