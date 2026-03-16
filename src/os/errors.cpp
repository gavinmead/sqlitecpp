#include "sqlite/os/errors.hpp"

namespace sqlite::os {

    // `loc` is NOT defaulted here — the default argument lives in the header so
    // that `std::source_location::current()` is evaluated at each call site rather
    // than here inside the function body.
    Error make_error(ErrorCode code, std::string_view message, std::source_location loc) {
        return {code, std::string(message), loc};
    }

} // namespace sqlite::os
