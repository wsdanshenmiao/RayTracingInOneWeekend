#include "Image.h"
#include <stdexcept>
#include <fstream>
#include <format>

namespace DSM {
	Color Image::GetPixel(std::uint32_t x, std::uint32_t y) const
	{
		if (x >= m_Width || y >= m_Height) {
			throw std::out_of_range("Pixel coordinates out of bounds.");
		}
		return m_Data[y * m_Width + x];
	}

	void Image::SetPixel(std::uint32_t x, std::uint32_t y, Color col)
	{
		if (x >= m_Width || y >= m_Height) {
			throw std::out_of_range("Pixel coordinates out of bounds.");
		}
		m_Data[y * m_Width + x] = col;
	}

	void Image::SetData(std::uint32_t width, std::uint32_t height, const std::vector<Color>& data)
	{
		if (data.size() != width * height) {
			throw std::length_error("Data size does not match width and height.");
		}
		m_Width = width;
		m_Height = height;
		m_Data = data;
	}

	void Image::SetData(std::uint32_t width, std::uint32_t height, std::vector<Color>&& data)
	{
		if (data.size() != width * height) {
			throw std::length_error("Data size does not match width and height.");
		}
		m_Width = width;
		m_Height = height;
		m_Data = std::move(data);
	}
	void Image::SaveToFilePPM(const std::string& filename) const noexcept
	{
		std::ofstream file(filename);
		if (!file.is_open()) {
			std::clog << std::format("Failed to open file: {}\n", filename);
			return;
		}

		file << std::format("P3\n {} {} \n255\n", m_Width, m_Height);
		for (std::uint32_t j = 0; j < m_Height; ++j) {
			for (std::uint32_t i = 0; i < m_Width; ++i) {
				Vector3i finalColor = GetPixel(i, j).GetColor();
				file << std::format("{} {} {}\n", finalColor[0], finalColor[1], finalColor[2]);
			}
		}
		
		file.close();
	}
}
