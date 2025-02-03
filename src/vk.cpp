#include <fmt/format.h>
#include <framework/vk_engine.h>

#include <algorithm>
#include <glm/mat4x4.hpp>
#include <vector>

#include "fmt/base.h"

int main(int argc, char **argv) {
  const double L       = 1;          // Length of the rod
  const double alpha   = 0.000002;   // Thermal diffusivity
  const double t_final = 1;          // Final time
  const int n_x        = 1024 * 10;  // Number of spatial points
  const int n_t        = 1000;       // Number of time steps

  double dx = L / (n_x - 1);
  double dt = t_final / (n_t - 1);
  fmt::print("dx = {}\n", dx);
  fmt::print("dt = {}\n", dt);

  if (alpha * dt / (dx * dx) > 0.5) {
    fmt::print(
        "Stability condition violated. Reduce dt or increase dx. Current "
        "value: {}\n",
        alpha * dt / (dx * dx));
    return {};
  }

  VulkanEngine engine;

  std::vector<float> initial_conditions = std::vector<float>(n_x);
  // std::fill(initial_conditions.rbegin(), initial_conditions.rend(), 0);
  //  fill with linear values plus some noise
  for (int i = 0; i < n_x; i++) {
    // initial_conditions[i] = 10000 * (1 - i / (n_x - 1.0)) + 200 * (rand() %
    // 100) / 100.0;
  }

  initial_conditions.at(0) = 100;
  // initial_conditions.at(5)  = 200;
  initial_conditions.at(initial_conditions.size() - 1) = 200;

  engine.set_costants(dt, dx, alpha,
                      n_x);  // needed before the initialization as it sets the
                             // size of the buffers
  engine.init(true);
  engine.set_initial_conditions(initial_conditions);

  std::vector<float> output = engine.run_compute(n_t);

  engine.cleanup();

  // fmt::print("Output data: ");
  // for (auto val : output) {
  //   fmt::print("{} ", val);
  // }
  // fmt::print("\n");

  return 0;
}
