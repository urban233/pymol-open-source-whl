#include "GLVertexBuffer.h"

#include "GraphicsUtil.h"

#include "pymol/algorithm.h"

#include <cstring>

static bool GLGenBuffer(GLuint& id, GLuint bufferType, pymol::span<const std::byte> data)
{
  glGenBuffers(1, &id);
  if (!CheckGLErrorOK(nullptr, "GenericBuffer::genBuffer failed\n"))
    return false;
  glBindBuffer(bufferType, id);
  if (!CheckGLErrorOK(nullptr, "GenericBuffer::bindBuffer failed\n"))
    return false;
  glBufferData(bufferType, data.size(), data.data(), GL_STATIC_DRAW);
  return CheckGLErrorOK(nullptr, "GenericBuffer::bufferData failed\n");
}

/**
 * @brief Converts raw data (void*) to a byte span
 * @param data The data
 * @param size The size of the data in bytes
 * @return The byte span
 */
static pymol::span<const std::byte> RawDataToByteSpan(
    const void* data, std::size_t size)
{
  return {static_cast<const std::byte*>(data), size};
}

std::vector<std::uint64_t> VertexBufferGL::getBufferIDs() const
{
  std::vector<std::uint64_t> bufferIDs;
  for (const auto& glID : desc_glIDs) {
    bufferIDs.push_back(glID);
  }
  return bufferIDs;
}

std::vector<BufferAndOffsets> VertexBufferGL::getBufferOffsets() const
{
  std::vector<BufferAndOffsets> bufferOffsets;

  return bufferOffsets;
}

void VertexBufferGL::copyFrom(
    const BufferAndOffsets& bufferAndOffsets, pymol::span<const std::byte> data)
{
}

void VertexBufferGL::bufferReplaceData(std::size_t offset, pymol::span<const std::byte> data)
{
  glBindBuffer(bufferType(), m_interleavedID);
  glBufferSubData(bufferType(), offset, data.size(), data.data());
}

void VertexBufferGL::bind_attrib(GLuint prg, const BufferDesc& d, GLuint glID)
{
  GLint loc = glGetAttribLocation(prg, d.attr_name.data());
  auto type_dim = VertexFormatToGLSize(d.m_format);
  auto type = VertexFormatToGLType(d.m_format);
  auto data_norm = VertexFormatToGLNormalized(d.m_format);
  bool masked = false;
  for (GLint lid : m_attribmask)
    if (lid == loc)
      masked = true;
  if (loc >= 0)
    m_locs.push_back(loc);
  if (loc >= 0 && !masked) {
    if (!isInterleaved() && glID)
      glBindBuffer(bufferType(), glID);
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, type_dim, type, data_norm, m_stride,
        reinterpret_cast<const void*>(d.offset));
  }
};

VertexBufferGL::VertexBufferGL(
    VertexBufferLayout layout, MemoryUsageProperty memProperty)
    : m_layout{layout}
    , m_memProperty{memProperty}
{
}

void VertexBufferGL::bind() const
{
  // we shouldn't use this one
  if (isInterleaved())
    glBindBuffer(bufferType(), m_interleavedID);
}

void VertexBufferGL::bind(GLuint prg, int index)
{
  auto& descs = m_desc.descs;
  if (index >= 0) {
    glBindBuffer(bufferType(), m_interleavedID);
    bind_attrib(prg, descs[index], desc_glIDs[index]);
  } else {
    if (isInterleaved() && m_interleavedID)
      glBindBuffer(bufferType(), m_interleavedID);
    for (auto i = 0; i < descs.size(); ++i) {
      const auto& d = descs[i];
      auto glID = desc_glIDs[i];
      bind_attrib(prg, d, glID);
    }
    m_attribmask.clear();
  }
}

void VertexBufferGL::unbind()
{
  for (auto& d : m_locs) {
    glDisableVertexAttribArray(d);
  }
  m_locs.clear();
  glBindBuffer(bufferType(), 0);
}

bool VertexBufferGL::isInterleaved() const noexcept
{
  return m_layout == VertexBufferLayout::Interleaved;
}

void VertexBufferGL::maskAttributes(std::vector<GLint> attrib_locs)
{
  m_attribmask = std::move(attrib_locs);
}

void VertexBufferGL::maskAttribute(GLint attrib_loc)
{
  m_attribmask.push_back(attrib_loc);
}

std::uint32_t VertexBufferGL::bufferType() const
{
  return GL_ARRAY_BUFFER;
}

bool VertexBufferGL::sepBufferData()
{
  auto& descs = m_desc.descs;
  for (auto i = 0; i < descs.size(); ++i) {
    // If the specified size is 0 but we have a valid pointer
    // then we are going to glVertexAttribXfv X in {1,2,3,4}
    const auto& d = descs[i];
    auto& glID = desc_glIDs[i];
    if (d.data_ptr && (m_memProperty == MemoryUsageProperty::GpuOnly)) {
      if (d.data_size) {
        auto data = RawDataToByteSpan(d.data_ptr, d.data_size);
        if (!GLGenBuffer(glID, bufferType(), data)) {
          return false;
        }
      }
    }
  }
  return true;
}

bool VertexBufferGL::seqBufferData() {
  // this is only going to use a single opengl vbo
  m_layout = VertexBufferLayout::Interleaved;

  auto& descs = m_desc.descs;
  size_t buffer_size { 0 };
  for ( auto & d : descs ) {
    buffer_size += d.data_size;
  }

  std::vector<std::byte> buffer_data(buffer_size);
  auto data_ptr = buffer_data.data();
  size_t offset = 0;

  for ( auto & d : descs ) {
    d.offset = offset;
    if (d.data_ptr)
      memcpy(data_ptr, d.data_ptr, d.data_size);
    else
      memset(data_ptr, 0, d.data_size);
    data_ptr += d.data_size;
    offset += d.data_size;
  }

  return GLGenBuffer(m_interleavedID, bufferType(), pymol::span{buffer_data});
}

bool VertexBufferGL::interleaveBufferData()
{
  auto& descs = m_desc.descs;
  const std::size_t bufferCount = descs.size();
  std::size_t stride = 0;
  std::vector<const uint8_t*> data_table(bufferCount);
  std::vector<const uint8_t*> ptr_table(bufferCount);
  std::vector<std::size_t> size_table(bufferCount);
  std::size_t count =
      descs[0].data_size / GetSizeOfVertexFormat(descs[0].m_format);

  // Maybe assert that all pointers in d_desc are valid?
  for (size_t i = 0; i < bufferCount; ++i) {
    auto& d = descs[i];
    // offset is the current stride
    d.offset = stride;

    // These must come after so that offset starts at 0
    // Size of 3 normals or whatever the current type is
    size_table[i] = GetSizeOfVertexFormat(d.m_format);

    // Increase our current estimate of the stride by this amount
    stride += size_table[i];

    // Does the addition of that previous stride leave us on a word boundry?
    int m = stride % 4;
    stride = (m ? (stride + (4 - m)) : stride);

    // data_table a pointer to the begining of each array
    data_table[i] = static_cast<const std::uint8_t*>(d.data_ptr);

    // We will move these pointers along by the values in the size table
    ptr_table[i] = data_table[i];
  }

  m_stride = stride;

  std::size_t interleavedSize = count * stride;

  std::vector<std::byte> interleavedData(interleavedSize);
  auto iPtr = interleavedData.data();

  while (iPtr != (interleavedData.data() + interleavedSize)) {
    for (size_t i = 0; i < bufferCount; ++i) {
      if (ptr_table[i]) {
        memcpy(iPtr, ptr_table[i], size_table[i]);
        ptr_table[i] += size_table[i];
      }
      iPtr += size_table[i];
    }
  }

  m_layout = VertexBufferLayout::Interleaved;
  return GLGenBuffer(m_interleavedID, bufferType(), pymol::span{interleavedData});
}

bool VertexBufferGL::evaluate()
{
  switch (m_layout) {
  case VertexBufferLayout::Separate:
    return sepBufferData();
    break;
  case VertexBufferLayout::Sequential:
    return seqBufferData();
    break;
  case VertexBufferLayout::Interleaved:
    return interleaveBufferData();
  default:
    assert("Invalid VertexBufferLayout" && false);
    break;
  }
  return false;
}

bool VertexBufferGL::bufferData(BufferDataDesc&& desc)
{
  m_desc = std::move(desc);
  desc_glIDs = std::vector<GLuint>(m_desc.descs.size());
  return evaluate();
}

bool VertexBufferGL::bufferData(
    BufferDataDesc&& desc, const void* data, size_t len)
{
  m_desc = std::move(desc);
  desc_glIDs = std::vector<GLuint>(m_desc.descs.size());
  m_layout = VertexBufferLayout::Interleaved;
  m_stride = desc.stride.value_or(0);
  auto span = RawDataToByteSpan(data, len);
  return GLGenBuffer(m_interleavedID, bufferType(), span);
}

void VertexBufferGL::bufferSubData(size_t offset, size_t size, void* data, size_t index)
{
  assert("Invalid Desc index" && index < m_desc.descs.size());
  assert("Invalid GLDesc index" && index < desc_glIDs.size());
  auto glID = isInterleaved() ? m_interleavedID : desc_glIDs[index];
  glBindBuffer(bufferType(), glID);
  glBufferSubData(bufferType(), offset, size, data);
}

std::uint64_t IndexBufferGL::getBufferID() const
{
  return static_cast<std::uint64_t>(m_bufferID);
}

void IndexBufferGL::bind() const
{
  glBindBuffer(bufferType(), m_bufferID);
}

void IndexBufferGL::unbind()
{
  glBindBuffer(bufferType(), 0);
}

std::uint32_t IndexBufferGL::bufferType() const
{
  return GL_ELEMENT_ARRAY_BUFFER;
}

void IndexBufferGL::bufferSubData(
    std::size_t offset, pymol::span<const std::byte> data)
{
  glBindBuffer(bufferType(), m_bufferID);
  glBufferSubData(bufferType(), offset, data.size(), data.data());
}

void IndexBufferGL::copyFrom(pymol::span<const std::uint32_t> data)
{
  auto bytesSpan = pymol::as_bytes(data);
  if (!m_bufferID) {
    GLGenBuffer(m_bufferID, bufferType(), bytesSpan);
  } else {
    bufferSubData(0, bytesSpan);
  }
}
