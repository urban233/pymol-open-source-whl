#pragma once

#include "Error.h"

#include <cassert>

namespace pymol
{
struct Void {
};

template <typename ResultT=Void> class Result
{

public:
  using type = ResultT;

  Result() = default;

  /**
   * @brief Constructor alternative that allows for convertible types
   * @param r result returned from function with a type convertible to ResultT
   */
  Result(type r) : m_result(std::move(r)) {}

  /**
   * Constructor alternative that takes in pymol::Error. Value of expected type
   * should not be taken at this point.
   * @param e error object to express why value should not be used
   */
  Result(Error e) : m_error{std::move(e)}, m_valid{false} {}

  /**
   * @brief Construct from error code.
   */
  Result(Error::Code code)
      : m_error(code)
      , m_valid{false}
  {
  }

  /**
   * @brief Determines whether the value of the expected type can be used.
   * @return true if the value can be used, false if an error occurred
   */
  explicit operator bool() const noexcept { return m_valid; }

  /**
   * @brief Retrieves the underlying error object
   * @return The underlying error object
   */
  const Error& error() const noexcept { return m_error; }

  /**
   * @return Rvalue reference to the underlying error object
   */
  Error&& error_move() noexcept
  {
    assert(!m_valid);
    return std::move(m_error);
  }

  /**
   * @result the value of the expected object
   */
  ResultT& result() { return m_result; }

  /**
   * @return the value of the expected object
   */
  const ResultT& result() const { return m_result; }

  /**
   * @return pointer to the expected object. Never nullptr. Call is invalid if this
   * instance is in error state.
   */
  ResultT* operator->()
  {
    assert(m_valid);
    return &m_result;
  }

  /**
   * @return Reference to the expected object. Behavior is undefined if this
   * instance is in error state.
   */
  ResultT& operator*() &
  {
    assert(m_valid);
    return m_result;
  }

  /**
   * @return Rvalue reference to the expected object. Behavior is undefined if this
   * instance is in error state.
   */
  ResultT&& operator*() &&
  {
    assert(m_valid);
    return std::move(m_result);
  }

  /**
   * @return Const reference to the expected object. Behavior is undefined if this
   * instance is in error state.
   */
  const ResultT& operator*() const &
  {
    assert(m_valid);
    return m_result;
  }

  /**
   * @return Const rvalue reference to the expected object. Behavior is undefined if this
   * instance is in error state.
   */
  const ResultT&& operator*() const &&
  {
    assert(m_valid);
    return std::move(m_result);
  }

private:
  ResultT m_result;
  Error m_error;
  bool m_valid{true};
};

} // namespace pymol

/**
 * If `res` is in error state, return from the calling scope with `res.error()`.
 * @param res Expression of type pymol::Result
 * @note Inspired by `g_return_val_if_fail` from glib, except that the check
 * will always be performed, there is nothing like `G_DISABLE_CHECKS`.
 */
#define p_return_if_error(res)                                                 \
  {                                                                            \
    auto&& _res_evaluated_ = res;                                              \
    if (!_res_evaluated_)                                                      \
      return _res_evaluated_.error_move();                                     \
  }

/**
 * Like p_return_if_error but add a prefix to the error message.
 */
#define p_return_if_error_prefixed(res, prefix)                                \
  {                                                                            \
    auto&& _res_evaluated_ = res;                                              \
    if (!_res_evaluated_)                                                      \
      return pymol::make_error(prefix, _res_evaluated_.error().what());        \
  }

/**
 * If `expr` evaluates to false, return from the calling scope with `val`.
 * @note Inspired by `g_return_val_if_fail` from glib, except that the check
 * will always be performed, there is nothing like `G_DISABLE_CHECKS`.
 */
#define p_return_val_if_fail(expr, val)                                        \
  {                                                                            \
    if (!(expr))                                                               \
      return val;                                                              \
  }
