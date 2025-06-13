#include "Texture.h"

namespace DSM {
    Color CheckerTexture::Sample(Vector2f uv) const
    {
        int x = std::floor(uv[0] * m_Desc.m_Width * m_InvScale);
        int y = std::floor(uv[1] * m_Desc.m_Height * m_InvScale);
        if ((x + y) % 2 == 0) {
            return m_EvenTexture->Sample(uv);
        }
        return m_OddTexture->Sample(uv);
    }

}