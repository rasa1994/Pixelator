import ImageExtracter;
import ProcessParallelization;
import <string>;
import <thread>;
import <vector>;
import <iostream>;
import <filesystem>;

auto main(int args, char* arguments[]) -> int
{
	std::string rootPath{};
	std::string imagePath{};

	std::cout << "Type root path: \n";
	std::getline(std::cin, rootPath);

	unsigned int imageSize{};
	std::cout << "Type in image width and height" << std::endl;
	std::cin >> imageSize;

	auto start = std::chrono::steady_clock::now();

	ImageExtracter extracter(imageSize);

	std::vector<std::string> pathsPng;

	const auto toLower = [](const std::string& pathString)
		{
			std::string copy = pathString;
			std::ranges::for_each(copy, [](char& a) { a = std::tolower(a); });
			return copy;
		};
	for (auto paths : std::filesystem::recursive_directory_iterator(rootPath))
	{
		const auto pathString = toLower(paths.path().string());
		if (pathString.find(".png") != std::string::npos 
			|| pathString.find(".jpg") != std::string::npos 
			|| pathString.find(".jpeg") != std::string::npos)
		{
			pathsPng.push_back(pathString);
		}
	}

	auto func = [imageSize, &extracter](const std::string& pathString) 
		{
			try
			{
				ErrorType error{};
				auto image = LoadImageFile(pathString, error);

				if (image.m_width >= imageSize && image.m_height >= imageSize/* && image.m_channels == 4*/)
				{
					auto extracted = ImageExtracter::ChangeResolution(image, imageSize, imageSize);
					extracter.AddImage(extracted);
				}
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}

		};

	Parallel_For(pathsPng, func);

	auto end = std::chrono::steady_clock::now();

	std::cout << "Elements: " << extracter.GetHashedImages() << std::endl;
	do
	{
		std::cout << "Type image path \n";
		std::cin >> imagePath;
		ErrorType error{};
		auto newImage = extracter.CreatePixelatedPicture(LoadImageFile(imagePath, error));
		WriteImage(newImage, imagePath, error);
	} while (imagePath != "end");
}
