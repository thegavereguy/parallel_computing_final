#include <fmt/format.h>
#include <framework/vk_engine.h>

#include <algorithm>
#include <glm/mat4x4.hpp>

#include "fmt/base.h"

int main(int argc, char **argv) {
  // print matrix
  glm::mat4 matrix(1.0f);
  fmt::print("Matrix: {}\n", matrix[0][0]);

  VulkanEngine engine;

  std::vector<float> initial_conditions = std::vector<float>(100);
  std::fill(initial_conditions.rbegin(), initial_conditions.rend(), 1);

  engine.init(true);
  engine.set_costants(0.1, 0.1, 0.05, 1);
  engine.run_compute(initial_conditions, 1);
  engine.cleanup();

  return 0;
}
