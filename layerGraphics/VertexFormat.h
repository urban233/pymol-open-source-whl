#pragma once

#include <cstddef>

enum class VertexFormat {
  // 8 bit
  Byte,
  Byte2,
  Byte3,
  Byte4,
  ByteNorm,
  Byte2Norm,
  Byte3Norm,
  Byte4Norm,
  UByte,
  UByte2,
  UByte3,
  UByte4,
  UByteNorm,
  UByte2Norm,
  UByte3Norm,
  UByte4Norm,

  // Single Precision
  Float,
  Float2,
  Float3,
  Float4,

  // Double Precision
  Float64,
  Float64_2,
  Float64_3,
  Float64_4,

  // 32 bit
  Int,
  Int2,
  Int3,
  Int4,
  UInt,
  UInt2,
  UInt3,
  UInt4,
};

enum class VertexFormatBaseType { Byte, UByte, Float, Int, UInt };

/**
 * @param format Vertex format
 * @return The base type of the vertex format
*/
VertexFormatBaseType GetVertexFormatBaseType(VertexFormat format);


/**
 * @param format Vertex format
 * @return Whether the vertex format is a normalized type
*/
bool VertexFormatIsNormalized(VertexFormat format);

/**
 * @param format Vertex format
 * @return The size of the vertex format in bytes
*/
std::size_t GetSizeOfVertexFormat(VertexFormat format);
