
module mann.exceptions;

namespace {

struct NormalizedErrorCategory : std::error_category {
    auto name() const noexcept -> const char* override {
        return "NormalizedError";
    }

    auto message(int condition) const -> std::string override {
        switch (static_cast<::mann::NormalizedError>(condition)) {
            using enum ::mann::NormalizedError;
            case InputInvalid:
                return "InputInvalid";
            case WindowSize:
                return "WindowSize";
            case Unknown:
                return "UnknownError";
            case IllegalAction:
                return "IllegalAction";
            default:
                return "Unknown NormalizedError";
        }
    }
};

const NormalizedErrorCategory normalized_error_category;
}  // namespace

namespace mann {

std::error_condition make_error_condition(NormalizedError e) {
    return std::error_condition{static_cast<i32>(e), normalized_error_category};
}

}