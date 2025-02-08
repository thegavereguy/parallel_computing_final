#include <dlfcn.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <framework/vk_engine.h>
// #include <renderdoc_app.h>
#include <time.h>

#include <ctime>
#include <glm/mat4x4.hpp>
#include <vector>

// RENDERDOC_API_1_1_2 *rdoc_api = NULL;

int main(int argc, char **argv) {
  const double L       = 1;          // Length of the rod
  const double alpha   = 0.0000002;  // Thermal diffusivity
  const double t_final = 0.1;        // Final time
  const int n_x        = 1024 * 8;   // Number of spatial points
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

  initial_conditions.at(0) = 100;
  // initial_conditions.at(5)  = 200;
  initial_conditions.at(n_x - 1) = 200;

  engine.set_costants(dt, dx, alpha,
                      n_x);  // needed before the initialization as it sets the
                             // size of the buffers
  // if (void *mod = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD)) {
  //   pRENDERDOC_GetAPI RENDERDOC_GetAPI =
  //       (pRENDERDOC_GetAPI)dlsym(mod, "RENDERDOC_GetAPI");
  //   int ret =
  //       RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
  //   fmt::print("RenderDoc API version: {}\n", ret);
  //   assert(ret == 1);
  // } else {
  //   fmt::print("RenderDoc not found\n");
  // }
  //
  // if (rdoc_api) rdoc_api->StartFrameCapture(NULL, NULL);
  //
  engine.init(true);
  engine.set_initial_conditions(initial_conditions);

  struct timespec start, end;
  clock_gettime(CLOCK_MONOTONIC, &start);

  std::vector<float> output = engine.run_compute(
      n_t, n_x / 1024);  // the group show be a multiple of subgroupSize

  clock_gettime(CLOCK_MONOTONIC, &end);

  engine.cleanup();
  // if (rdoc_api) rdoc_api->EndFrameCapture(NULL, NULL);

  fmt::print("Output data: ");
  // for (auto val : output) {
  //   fmt::print("{} ", val);
  // }
  fmt::println("(2)\t{}", output.at(1));
  fmt::println("(n_x/2)\t{}", output.at(n_x / 2));
  fmt::println("(n_x-1)\t{}", output.at(n_x - 2));
  fmt::print("\n");

  long seconds_ts              = end.tv_sec - start.tv_sec;
  long nanoseconds_ts          = end.tv_nsec - start.tv_nsec;
  double elapsed_clock_gettime = seconds_ts + nanoseconds_ts * 1e-9;

  fmt::print("Elapsed time using clock_gettime: {} seconds\n",
             elapsed_clock_gettime);

  return 0;
}
