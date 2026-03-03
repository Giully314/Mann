#ifndef INCLUDED_ERROR_MACROS_HPP
#define INCLUDED_ERROR_MACROS_HPP

// PURPOSE: define macros to simplify the addition of custom error_code.
//
// MACROS:
//  CUSTOM_ERROR_CODE_DECLARATION: declare a custom error_code function.
//  CUSTOM_ERROR_CODE_DEFINITION:   define a custom error_code function.
//
// DESCRIPTION:
//  This file defines some macros to simplify the definitions of custom errors in a package.
//
// USAGE:
//
// in the .hpp file.
// #include <system_error>
// #include <type_traits>
// namespace packagename {
// enum class ParserError {
//     // no 0.
//     // Bad input.
//     Input = 1,
//     // Bad brackets match.
//     BadBrackets,
// };
// } // end namespace
// CUSTOM_ERROR_CODE_DECLARATION(:::packagename::ParserError);
//
// // in the .cpp file.
// namespace {
// struct ParserErrorCategory : std::error_category {
//     // Name of the category
//     auto name() const noexcept -> const char* override {
//         return "ParserErrorCategory";
//     }
//
//     // Message associated to an enum value.
//     auto message(int condition) const -> std::string override {
//         switch (static_cast<:::package_name::ParserError>(condition)) {
//         using enum :::package_name::ParserError;
//             case Input:
//                 // You can also return a string with additional error,
//                 // you don't need to return only the name of the enum value.
//                 return "Input";
//             case BadBrackets:
//                 return "BadBrackets";
//             default:
//                 return "Unknown error";
//         }
//     }
//
//     // Map the custom error to the normalized error.
//     // Note: you must define the right mapping from your defined errors to the normalized.
//     std::error_condition default_error_condition(int cond) const noexcept override {
//         switch (static_cast<:::package_name::ParserError>(cond)) {
//             using enum :::package_name::ParserError;
//             using namespace :error;
//             case Input:
//                 return NormalizedError::InputInvalid;
//             case BadBrackets:
//                 return NormalizedError::InputBadExpression;
//             default:
//                 return NormalizedError::UnknownError;
//         }
//     }
// };
//
// // This must be the name passed to the macro CUSTOM_ERROR_CODE_DEFINITION.
// const ParserErrorCategory parser_error_category;
// } // namespace anonymous
//
// CUSTOM_ERROR_CODE_DEFINITION(ParserError, parser_error_category);

// End of usage.

// Copy this template (the whole namespace) and change the relevant stuff to create your
// error_category.
// This must be defined in the cpp file.
// namespace {
// struct MyCategoryName : std::error_category {
//     // Name of the category
//     auto name() const noexcept -> const char* override {
//         return "MyCategoryName";
//     }
//
//     // Message associated to an enum value.
//     auto message(int condition) const -> std::string override {
//         switch (static_cast<:::package_name::EnumError>(condition)) {
//         using enum :::package_name::EnumError;
//             case MyError1:
//                 // You can also return a string with additional error,
//                 // you don't need to return only the name of the enum value.
//                 return "MyError1";
//             default:
//                 return "Unknown error";
//         }
//     }
//
//     // Map the custom error to the normalized error.
//     std::error_condition default_error_condition(int cond) const noexcept override {
//         switch (static_cast<:::package_name::EnumError>(cond)) {
//             using enum :::package_name::EnumError;
//             using namespace :error;
//             case MyError1:
//                 return NormalizedError::BestMapping;
//             default:
//                 return NormalizedError::UnknownError;
//         }
//     }
// };
//
// // This must be the name passed to the macro CUSTOM_ERROR_CODE_DEFINITION.
// const MyCategoryName my_category_name_category;
// } // namespace anonymous

// use this in the cpp after the definition of the category.
// ENUM_NAME is the name of the enum.
// CATEGORY_NAME is the name of the instance of the category associated to this error_code,
// if you use the template from above this corresponds to my_category_name_category.
#define CUSTOM_ERROR_CODE_DEFINITION(NAMESPACE_NAME, ENUM_NAME, CATEGORY_NAME) \
    namespace NAMESPACE_NAME {                                                         \
    std::error_code make_error_code(ENUM_NAME e) {                                     \
        return std::error_code{static_cast<int>(e), CATEGORY_NAME};                    \
    }                                                                                  \
    }

// This must be defined after the associated enum in the hpp file.
#define CUSTOM_ERROR_CODE_DECLARATION(NAMESPACE_NAME, ENUM_NAME)   \
    export namespace NAMESPACE_NAME {                                             \
    std::error_code make_error_code(ENUM_NAME e);                          \
    }                                                                      \
    namespace std {                                                        \
    template <>                                                            \
    struct is_error_code_enum<::NAMESPACE_NAME::ENUM_NAME> : true_type {}; \
    }  // namespace std

#endif
