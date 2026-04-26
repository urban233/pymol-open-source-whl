#pragma once

enum class MemoryUsageProperty {
  GpuOnly,  ///< Keep in GPU Memory
  CpuToGpu, ///< Usual Memcpy from CPU to GPU
  GpuToCpu, ///< Readback from GPU to CPU
};

enum class VertexBufferLayout {
  Separate,   // multiple vbos
  Sequential, // single vbo
  Interleaved // single vbo
};

enum class GPUPrimitiveTopology
{
  Triangles,
  TriangleStrip,
  TriangleFan,
  Lines,
  LinesStrip,
  LineLoop,
  Points,
  Quads // depreated
};
