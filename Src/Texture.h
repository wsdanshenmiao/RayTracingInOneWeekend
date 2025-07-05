#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "Color.h"
#include "Perlin.h"
#include <vector>

namespace DSM {

    struct TextureDesc 
    {
        std::uint64_t m_Width = 1;
        std::uint32_t m_Height = 1;
        std::uint16_t m_Depth = 1;
    };

    class Texture 
    {
    public:
        Texture() = default;
        Texture(const TextureDesc& desc) : m_Desc(desc) {}
        
        const TextureDesc& GetDesc() const { return m_Desc; }
        
        virtual Color Sample(Vector2f uv) const = 0;
        virtual Color Sample(Vector3f uvw) const = 0;

        std::uint64_t GetWidth() const { return m_Desc.m_Width; }
        std::uint32_t GetHeight() const { return m_Desc.m_Height; }
        std::uint16_t GetDepth() const { return m_Desc.m_Depth; }
    
    protected:
        TextureDesc m_Desc{};
    };

    class SolidColorTexture : public Texture
    {
    public:
        SolidColorTexture(const Color& col) : m_Color(col) {}
        SolidColorTexture(TextureDesc desc, float r, float g, float b) : m_Color(r, g, b) {}

        Color Sample(Vector2f uv) const override { return m_Color; }
        Color Sample(Vector3f uvw) const override { return m_Color; }

    private:
        Color m_Color;
    };

    class CheckerTexture : public Texture 
    {
    public:
        CheckerTexture(TextureDesc desc, float scale, std::shared_ptr<Texture> odd, std::shared_ptr<Texture> even)
            : Texture(desc), m_InvScale(1.f / scale), m_OddTexture(odd), m_EvenTexture(even) {}
        CheckerTexture(TextureDesc desc, float scale, const Color& odd, const Color& even)
            : Texture(desc), 
            m_InvScale(1.f / scale),
            m_OddTexture(std::make_shared<SolidColorTexture>(odd)),
            m_EvenTexture(std::make_shared<SolidColorTexture>(even)) {}

        Color Sample(Vector2f uv) const override;
        Color Sample(Vector3f uvw) const override;

    private:
        float m_InvScale = 1.0f;
        std::shared_ptr<Texture> m_OddTexture;
        std::shared_ptr<Texture> m_EvenTexture;
    };

    class ImageTexture : public Texture
    {
    public:
        ImageTexture(const std::string& filename);
        
        Color Sample(Vector2f uv) const override;
        Color Sample(Vector3f uvw) const override;

    private:
        std::vector<std::uint8_t> m_Data;
    };
    
    class NoiceTexture : public Texture
    {
    public:
        NoiceTexture(float scale = 1) : m_Scale(scale) {}

        Color Sample(Vector2f uv) const override;
        Color Sample(Vector3f uvw) const override;

    private:
        Perlin m_Noice{};
        float m_Scale{};
    };


} // namespace DSM



#endif