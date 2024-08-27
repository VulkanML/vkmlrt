#if defined(_MSC_VER)
// no need to ignore any warnings with MSVC
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-variable"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#else
// unknow compiler... just ignore the warnings for yourselves ;)
#endif

#include "../inc/rt.hpp"
#include <vulkan/vulkan.hpp>

std::shared_ptr<rt> rt::_instance = nullptr;
std::shared_ptr<rt> rt::instance() {
  if (_instance == nullptr) {
    _instance = std::shared_ptr<rt>(new rt());
  }
  return _instance;
}
