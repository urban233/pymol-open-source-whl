#pragma once

#include "GPUEnums.h"
#include "VertexFormat.h"

#include <cstddef>
#include <cstdint>
#include <optional>
// #include <span>
#include "pymol/span.h"
#include <string_view>
#include <variant>
#include <vector>

// -----------------------------------------------------------------------------
// DESCRIPTORS
// -----------------------------------------------------------------------------
// Describes a single array held in the vbo
struct BufferDesc {
  BufferDesc(std::string_view _attr_name, VertexFormat _format,
      std::size_t _data_size = 0, const void* _data_ptr = nullptr,
      std::uint32_t offset = 0)
      : attr_name(_attr_name)
      , m_format(_format)
      , data_size(_data_size)
      , data_ptr(_data_ptr)
      , offset(offset)
  {
  }

  std::string_view attr_name;
  VertexFormat m_format{VertexFormat::Float};
  std::size_t data_size{};
  const void* data_ptr{nullptr};
  std::uint32_t offset{};
};

/**
 * @brief Describes the vertex buffer data
 */
struct BufferDataDesc {

  /**
   * @brief Default constructor
   */
  BufferDataDesc() = default;

  /**
   * @brief Constructor
   * @param _descs The buffer descriptors
   */
  BufferDataDesc(std::vector<BufferDesc> _descs)
      : descs(std::move(_descs))
  {
  }
  std::vector<BufferDesc> descs;
  GPUPrimitiveTopology topology{GPUPrimitiveTopology::Triangles};
  std::optional<std::size_t> stride;
};

using VertexBufferDataDesc = BufferDataDesc;
using VertexBufferDesc = BufferDesc;
