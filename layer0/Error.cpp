#include "Error.h"

#include <sstream>

namespace pymol
{
const std::string& Error::what() const noexcept
{
  if (m_isCached) {
    return m_cachedErrMsg;
  }

  if (m_formatter) {
    std::ostringstream stream;
    m_formatter(stream); // Execute the lambda
    m_cachedErrMsg = stream.str();
  }

  m_isCached = true;
  return m_cachedErrMsg;
}

} // namespace pymol
