#pragma once

#include <cstddef>

namespace pymol
{
template <typename T> class span
{
public:
  span() = default;

  span(T* data, std::size_t size)
      : data_(data)
      , size_(size)
  {
  }

  template <typename RangeT>
  span(RangeT& range)
      : data_(range.data())
      , size_(range.size())
  {
  }

  template <typename RangeT>
  span(const RangeT& range)
      : data_(range.data())
      , size_(range.size())
  {
  }

  T& operator[](std::size_t index) { return data_[index]; }

  std::size_t size() const { return size_; }

  T* data() { return data_; }

  const T* data() const { return data_; }

  using value_type = T;

private:
  T* data_{};
  std::size_t size_{};
};

template <typename RangeT> span(RangeT&) -> span<typename RangeT::value_type>;

template <typename RangeT>
span(const RangeT&) -> span<const typename RangeT::value_type>;

template <typename RangeT> span<const std::byte> as_bytes(RangeT& range)
{
  return span<const std::byte>(reinterpret_cast<const std::byte*>(range.data()),
      range.size() * sizeof(typename RangeT::value_type));
}

} // namespace pymol
