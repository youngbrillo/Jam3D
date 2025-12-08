#include "uuid.hpp"
#include "../utils/random.hpp"
#include <sstream>

namespace jam
{
    UUID::UUID()
        : m_UUID(jam::GetRandomUint64())
    {

    }

    UUID::UUID(uint64_t uuid)
        :m_UUID(uuid)
    {

    }

    std::string UUID::toString() const
    {
        std::stringstream stream;
        stream.imbue(std::locale("C"));
        stream << std::uppercase << std::hex << this->m_UUID;
        return stream.str();
    }

}