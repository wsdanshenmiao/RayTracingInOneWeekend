#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"
#include "stb_image.h"

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

    Color CheckerTexture::Sample(Vector3f uvw) const
    {
        return Sample(Vector2f{uvw[0], uvw[1]});
    }

    ImageTexture::ImageTexture(const std::string &filename)
    {
        stbi_uc* imgData = nullptr;
        
        int width, height, components;
        imgData = stbi_load(filename.c_str(), &width, &height, &components, 3);
        if (imgData == nullptr) return;
        
        m_Desc.m_Width = static_cast<std::uint64_t>(width);
        m_Desc.m_Height = static_cast<std::uint32_t>(height);
        m_Desc.m_Depth = 1;

        std::size_t texSize = width * height * 3;
        m_Data.resize(texSize);
        std::memcpy(m_Data.data(), imgData, texSize);

        stbi_image_free(imgData);
    }

    Color ImageTexture::Sample(Vector2f uv) const
    {
        std::size_t w = uv[0] * m_Desc.m_Width;
        std::size_t h = uv[1] * m_Desc.m_Height;
        auto it = m_Data.begin();
        std::advance(it, (h * m_Desc.m_Width + w) * 3);
        Color col{};
        for(int i = 0; i < 3 && it != m_Data.end(); ++i, ++it) {
            col[i] = (*it) / 255.0f;
        }
        return col;
    }

    Color ImageTexture::Sample(Vector3f uvw) const
    {
        return Sample(Vector2f{uvw[0], uvw[1]});
    }

    Color NoiceTexture::Sample(Vector3f uvw) const
    {
        return Color(.5, .5, .5) * (1 + std::sin(m_Scale * uvw[2] + 10 * m_Noice.Turb(uvw, 7)));
    }

    Color NoiceTexture::Sample(Vector2f uv) const
    {
        return Sample(Vector3f{uv[0], uv[1], 0});
    }

}