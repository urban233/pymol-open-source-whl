#pragma once

#include <cstddef>
// #include <span>
#include "pymol/span.h"
#include <vector>

#include "GenericBuffer.h"

struct BufferAndOffsets {
  std::size_t bufferIdx{};
  std::size_t offset{};
};

class VertexBuffer : public GPUBuffer
{
public:

  /**
   * @return Underlying buffers Image IDs
   */
  virtual std::vector<std::uint64_t> getBufferIDs() const = 0;

  /**
   * @return Underlying buffers and offsets
   */
  virtual std::vector<BufferAndOffsets> getBufferOffsets() const = 0;

  /**
   * @brief Copies data from external source to this buffer
   * @param bufferAndOffets Buffer index and offset
   * @param data The data to copy to the buffer
   */
  virtual void copyFrom(const BufferAndOffsets& bufferAndOffsets,
      pymol::span<const std::byte> data) = 0;
};
