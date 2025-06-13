#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Math/Vector.h"
#include "Color.h"

namespace DSM {

    struct TextureDesc 
    {
        std::uint64_t m_Width = 0;
        std::uint32_t m_Height = 0;
        std::uint16_t m_Depth = 0;
    };

    class Texture 
    {
    public:
        Texture(const TextureDesc& desc) : m_Desc(desc) {}

        virtual Color Sample(Vector2f uv) const = 0;
        const TextureDesc& GetDesc() const { return m_Desc; }
    
    protected:
        TextureDesc m_Desc;
    };

    class SolidColorTexture : public Texture {
    public:
        SolidColorTexture(TextureDesc desc, const Color& col) : Texture(desc), m_Color(col) {}
        SolidColorTexture(TextureDesc desc, float r, float g, float b) : Texture(desc), m_Color(r, g, b) {}

        Color Sample(Vector2f uv) const override { return m_Color; }

    private:
        Color m_Color;
    };

    class CheckerTexture : public Texture {
    public:
        CheckerTexture(TextureDesc desc, float scale, std::shared_ptr<Texture> odd, std::shared_ptr<Texture> even)
            : Texture(desc), m_InvScale(1.f / scale), m_OddTexture(odd), m_EvenTexture(even) {}
        CheckerTexture(TextureDesc desc, float scale, const Color& odd, const Color& even)
            : Texture(desc), 
            m_InvScale(1.f / scale),
            m_OddTexture(std::make_shared<SolidColorTexture>(desc, odd)),
            m_EvenTexture(std::make_shared<SolidColorTexture>(desc, even)) {}

        Color Sample(Vector2f uv) const override;

    private:
        float m_InvScale = 1.0f;
        std::shared_ptr<Texture> m_OddTexture;
        std::shared_ptr<Texture> m_EvenTexture;
    };

    

} // namespace DSM



#endif