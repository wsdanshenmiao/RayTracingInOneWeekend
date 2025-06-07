#ifndef __IMAGE_H__
#define __IMAGE_H__


#include "Color.h"
#include <vector>


namespace DSM{
    class Image
    {
    public:
        Image(std::uint32_t width, std::uint32_t height) noexcept
            :m_Width(width), m_Height(height), m_Data(width * height) { }
        Image(std::uint32_t width, std::uint32_t height, std::vector<Color> data)
        {
            SetData(width, height, std::move(data));
        }

		std::uint32_t GetWidth() const noexcept { return m_Width; }
		std::uint32_t GetHeight() const noexcept { return m_Height; }

        Color GetPixel(std::uint32_t x, std::uint32_t y) const;
		const std::vector<Color>& GetData() const noexcept { return m_Data; }

        void SetPixel(std::uint32_t x, std::uint32_t y, Color col);
        void SetData(std::uint32_t width, std::uint32_t height, const std::vector<Color>& data);
        void SetData(std::uint32_t width, std::uint32_t height, std::vector<Color>&& data);

        void SaveToFilePPM(const std::string& filename) const noexcept;

    private:
        std::uint32_t m_Width;
        std::uint32_t m_Height;

        std::vector<Color> m_Data;
    };
}


#endif // __IMAGE_H__