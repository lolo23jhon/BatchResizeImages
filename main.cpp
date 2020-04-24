#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <memory>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

const std::unordered_set<std::string> ALLOWED_EXTENSIONS{
	"jpg", 
	"jpeg", 
	"png",
	"bmp",
	"gif",
	"tiff"
};

bool validateInteger(const std::string& t_str) {
	static const std::string numbers{ "1234567890" };

	if (t_str[0] == '0') {
		return false;
	}

	for (const auto& c : t_str) {
		if (numbers.find(c) == std::string::npos) {
			return false;
		}
	}
	return true;
}

bool validateExtension(const std::string& t_path) {
	std::size_t extPos{ t_path.rfind('.') };
	if (extPos == std::string::npos) {
		return false;
	}

	std::string extension{ t_path.substr(extPos + 1) };
	if (extension.empty()) {
		return false;
	}

	if (ALLOWED_EXTENSIONS.find(extension) == ALLOWED_EXTENSIONS.cend()) {
		return false;
	}

	return true;
}

void pathNotFound(const std::string& t_path, const std::string& t_itemName = "Path") {
	std::cout << t_itemName << " \"" << t_path << "\" does not exist." << std::endl;
}

void pathIsNotDir(const std::string& t_path, const std::string& t_itemName = "Path") {
	std::cout << t_itemName << " \"" << t_path << " is not a directory." << std::endl;
}

int main(int argc, char* argv[]) {

	if (argc != 5) {
		std::cout << "args: <new width> <new height> <input path> <output path>\n";
		return 0;
	}

	std::string widthStr{ argv[2] };
	std::string heightStr{ argv[1] };


	if (!validateInteger(widthStr)) {
		std::cout << "Invalid width value." << std::endl;
		return 0;
	}

	if (!validateInteger(heightStr)) {
		std::cout << "Invalid height value." << std::endl;
		return 0;
	}

	std::string inputPathStr{ argv[3] };
	std::string outputPathStr{ argv[4] };

	std::filesystem::path inputPath{ inputPathStr };
	std::filesystem::path outputPath{ outputPathStr };

	bool is_error{ false };

	if (!std::filesystem::exists(inputPath)) {
		pathNotFound(inputPathStr);
		is_error = true;
	}

	// Path existence validation
	if (!std::filesystem::exists(outputPath)) {
		pathNotFound(outputPathStr);
		is_error = true;
	}

	if (is_error) {
		return 0;
	}

	// Check if paths are directories
	if (!std::filesystem::is_directory(inputPath)) {
		pathIsNotDir(inputPathStr);
		is_error = true;
	}

	if (!std::filesystem::is_directory(outputPath)) {
		pathIsNotDir(outputPathStr);
		is_error = true;
	}

	if (is_error) {
		return 0;
	}

	int width{ std::stoi(widthStr) };
	int height{ std::stoi(heightStr) };
	
	// Set the input path string to the format of the filesysyem (/, \\, \ annoyances ).
	inputPathStr = inputPath.string();
	outputPathStr = outputPath.string();

	std::size_t inputPathLength{ inputPathStr.length()};
	
	std::cout << "Resizing images." << std::endl;
	int numImages{ 0 };
	for (const auto& entry : std::filesystem::directory_iterator(inputPath)) {
		std::string path{entry.path().string() };
		if (entry.is_directory() || !validateExtension(path)) {
			continue;
		}

		cv::Mat img{ cv::imread(path)};
		std::string newPath{outputPathStr + '\\' +path.substr(inputPathLength+1)};

		cv::resize(img, img, {width, height});
		cv::imwrite(newPath, img);

		numImages++;
	}


	std::cout << "Done resizing " << numImages << " images to size " << width << 'x'<< height << '.'<< std::endl;
	return 0;
}