#pragma once

#include <string>
#include <functional> // For std::function
#include <utility>    // For std::forward
#include <ostream>    // For std::ostream

namespace pymol
{

class Error
{
public:
  enum Code {
    DEFAULT,
    QUIET,
    MEMORY,
    INCENTIVE_ONLY,
  };

  Error() = default;

  /**
   * @brief Construct an error with a specific code.
   * @param code The error code to set.
   */
  explicit Error(Code code) : m_code(code) {}

  /**
   * @brief Get the error message, formatting it if necessary.
   * @return The formatted error message.
   */
  const std::string& what() const noexcept;

  /**
   * @return The error code of this error instance.
   */
  Code code() const noexcept { return m_code; }

  /**
   * @brief Make an error instance by capturing the arguments for later formatting.
   */
  template <Code C, typename... PrintableTs>
  static Error make(PrintableTs&&... ts)
  {
    Error error(C);
    // This lambda captures the arguments and calls our lightweight helper.
    // It is cheap to create and store.
    error.m_formatter = [=](std::ostream& os) {
      (os << ... << ts);
    };
    return error;
  }

private:
  Code m_code = DEFAULT;
  std::function<void(std::ostream&)> m_formatter;

  // Caching for the generated string
  mutable std::string m_cachedErrMsg;
  mutable bool m_isCached = false;
};

/**
 * @brief Creates Error object by capturing arguments.
 * @tparam PrintableTs Types that can be printed to an ostream.
 * @param ts Arguments to be included in the error message.
 * @return An Error object with the captured message.
 */
template <typename... PrintableTs>
Error make_error(PrintableTs&&... ts)
{
  return Error::make<Error::DEFAULT>(std::forward<PrintableTs>(ts)...);
}

} // namespace pymol
