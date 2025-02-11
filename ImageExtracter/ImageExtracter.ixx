export module ImageExtracter;
export import ImageLoader;
import <map>;
import <mutex>;
export import <array>;
export import <ranges>;
export import <algorithm>;
export import <utility>;
export import <cmath>;
namespace
{
	template <typename Key, typename Elem>
	Elem GetVal(const std::map<Key, Elem>& maps, Key key)
	{
		auto it = maps.lower_bound(key);
		auto pr = std::prev(it);

		if (pr != maps.end() && it != maps.end())
		{
			if (std::abs(static_cast<double>(pr->first - key)) < std::abs(static_cast<double>(it->first - key)))
				return pr->second;
			else if (it != maps.end())
				return it->second;
		}

		return maps.begin()->second;
	}

	constexpr long long split(unsigned int a)
	{
		a = (a | a << 12) & 00014000377;
		a = (a | a << 8) & 00014170017;
		a = (a | a << 4) & 00303030303;
		a = (a | a << 2) & 01111111111;
		return a;
	}

	constexpr long long zValue(unsigned int r, unsigned g, unsigned b)
	{
		return split(r) + (split(g) << 1) + (split(b) << 2);
	}
}

export
{
	class ImageExtracter
	{
	public:
		ImageExtracter(unsigned int size) : size(size) {};
		void AddImage(const Image& image);
		Image CreatePixelatedPicture(const Image& original);
		size_t GetHashedImages() const { return m_imagesByPixels.size(); }
		static Image ChangeResolution(const Image& original, unsigned int newWidth, unsigned int newHeight);
	private:
		std::map<double, Image> m_imagesByPixels;
		std::pair<double, Image> m_biggest;
		std::mutex mtx;
		unsigned int size;
		static double GetImageHash(const Image& image);
		Image FindByPixels(unsigned char R, unsigned char G, unsigned char B);
	};



	double ImageExtracter::GetImageHash(const Image& image)
	{
		unsigned long long R{ 0llu }, G{ 0llu }, B{ 0llu };
		size_t pixels = image.m_width * image.m_height;
		for (auto row{ 0u }; row < image.m_height; ++row)
		{
			for (auto column{ 0u }; column < image.m_width; ++column)
			{
				auto pixel = image.GetPixel(row, column);
				R += pixel.R;
				G += pixel.G;
				B += pixel.B;
			}
		}

		if (pixels)
		{
			R /= pixels;
			G /= pixels;
			B /= pixels;
		}

		return zValue(R, G, B);
	}

	void ImageExtracter::AddImage(const Image& image)
	{
		std::lock_guard<std::mutex> lock(mtx);
		const auto hash = GetImageHash(image);
		if (hash >= m_biggest.first)
		{
			m_biggest.first = hash;
			m_biggest.second = image;
		}
		m_imagesByPixels[hash] = image;
	}

	Image ImageExtracter::CreatePixelatedPicture(const Image& original)
	{
		Image newImage;
		newImage.m_width = original.m_width * size;
		newImage.m_height = original.m_height * size;
		newImage.m_channels = 4;
		newImage.m_size = newImage.m_width * newImage.m_height * newImage.m_channels;
		newImage.m_imageData.resize(newImage.m_size);

		for (auto row{ 0u }; row < original.m_height; ++row)
		{
			for (auto column{ 0u }; column < original.m_width; ++column)
			{
				const auto pixel = original.GetPixel(row, column);
				const auto imageCopy = FindByPixels(pixel.R, pixel.G, pixel.B);
				for (size_t rowI = 0; rowI < size; ++rowI)
				{
					for (size_t columnI = 0; columnI < size; ++columnI)
					{
						const auto i = row * size + rowI;
						const auto j = column * size + columnI;
						newImage.SetPixel(i, j, imageCopy.GetPixel(rowI, columnI));
					}
				}
			}
		}

		return newImage;
	}

	Image ImageExtracter::FindByPixels(unsigned char R, unsigned char G, unsigned char B)
	{
		double hash = zValue(R, G, B);
		return GetVal(m_imagesByPixels, hash);
	}

	Image ImageExtracter::ChangeResolution(const Image& original, unsigned int newWidth, unsigned int newHeight)
	{
		Image image{};
		image.m_channels = original.m_channels;
		image.m_width = newWidth;
		image.m_height = newHeight;
		image.m_size = image.m_channels * newWidth * newHeight;
		image.path = original.path;
		image.m_imageData.resize(image.m_size);

		const auto pixelsWidthRatio = static_cast<float>(original.m_width) / newWidth;
		const auto pixelsHeightRatio = static_cast<float>(original.m_height) / newHeight;

		for (size_t row{ 0u }; row < newHeight; ++row)
		{
			const auto startingRow = static_cast<size_t>(row * pixelsHeightRatio);
			for (size_t column{ 0u }; column < newWidth; ++column)
			{
				const auto startingColumn = static_cast<size_t>(column * pixelsWidthRatio);
				std::array<unsigned long, 4> pixelValues{ 0u, 0u, 0u, 0u };
				size_t pixelObserved = 0;
				auto lastRow = std::clamp(static_cast<unsigned int>(startingRow + pixelsHeightRatio), 0u, original.m_height);
				for (size_t rowI = startingRow; rowI < lastRow; ++rowI)
				{
					auto lastColumn = std::clamp(static_cast<unsigned int>(startingColumn + pixelsWidthRatio), 0u, original.m_width);
					for (size_t columnI = startingColumn; columnI < lastColumn; ++columnI)
					{
						const auto pixel = original.GetPixel(rowI, columnI);
						pixelValues[0] += pixel.R;
						pixelValues[1] += pixel.G;
						pixelValues[2] += pixel.B;
						pixelValues[3] += pixel.A;

						pixelObserved++;
					}
				}

				if (pixelObserved > 0)
				{
					std::for_each(pixelValues.begin(), pixelValues.end(), [pixelObserved](auto& v) {v /= pixelObserved; });
				}

				auto newPixel = RGBA{ static_cast<unsigned char>(pixelValues[0]), static_cast<unsigned char>(pixelValues[1]), static_cast<unsigned char>(pixelValues[2]), static_cast<unsigned char>(pixelValues[3]) };
				image.SetPixel(row, column, newPixel);
			}
		}

		return image;
	}
}