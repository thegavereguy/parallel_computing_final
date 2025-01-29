#include <fmt/format.h>
#include <framework/vk_engine.h>

#include <glm/mat4x4.hpp>

int main(int argc, char **argv) {
  // print matrix
  glm::mat4 matrix(1.0f);
  fmt::print("Matrix: {}\n", matrix[0][0]);

  VulkanEngine engine;

  engine.init();
  engine.run();
  engine.cleanup();

  return 0;
}
