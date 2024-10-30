#ifndef ULMON_UTILS_EXCEPTIONS_H
#define ULMON_UTILS_EXCEPTIONS_H

#include <stdexcept>

namespace lemon {

namespace utils {

class NotSupportedError : public std::logic_error {
 public:
  NotSupportedError()
      : std::logic_error("This is not supported.") {};
};

};  // namespace utils

};  // namespace lemon

#endif
