#pragma once

#include "GenericBuffer.h"

#include <cstdint>

class IndexBuffer : public GPUBuffer
{
public:
  [[nodiscard]] virtual std::uint64_t getBufferID() const = 0;

  virtual void copyFrom(pymol::span<const std::uint32_t> data) = 0;
};
