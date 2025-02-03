#include <fmt/format.h>
#include <framework/vk_engine.h>

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <vector>

#include "fmt/base.h"

int main(int argc, char **argv) {
  // print matrix
  glm::mat4 matrix(1.0f);
  fmt::print("Matrix: {}\n", matrix[0][0]);

  VulkanEngine engine;

  std::vector<float> initial_conditions = std::vector<float>(16);
  std::fill(initial_conditions.rbegin(), initial_conditions.rend(), 0);
  initial_conditions.at(0) = 100;
  // initial_conditions.at(5)  = 200;
  initial_conditions.at(15) = 200;

  engine.init(true);
  engine.set_costants(0.01 / 5, ((float)1 / 15), 0.05, 16);
  engine.set_initial_conditions(initial_conditions);
  std::vector<float> output = engine.run_compute(5);
  // engine.set_initial_conditions(output);
  // output = engine.run_compute(1);
  // engine.set_initial_conditions(output);
  // output = engine.run_compute(1);
  engine.cleanup();

  fmt::print("Output data: ");
  for (auto val : output) {
    fmt::print("{} ", val);
  }
  fmt::print("\n");

  return 0;
}
