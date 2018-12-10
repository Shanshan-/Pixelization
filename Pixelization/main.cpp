#include "main.h"

int main(int argc, char **argv) {
	//hardcode initial values for now
	//TODO: these should be taken in as inputs to program (start from console, then by gui)
	int paletteSize = 8;
	int spSize = 500; //squirrel size = 9, scale = 10, mult = 2

	//load image
	cv::Mat image = cv::imread(IMG_PATH "obama.png"); //spSize 
	//cv::Mat image = cv::imread(IMG_PATH "chi.jpg"); //spSize 
	//cv::Mat image = cv::imread(IMG_PATH "flowers.jpg"); //spSize 36
	//cv::Mat image = cv::imread(IMG_PATH "test.png"); //spSize 2
	//cv::Mat image = cv::imread(IMG_PATH "shield.png"); //spSize 10
	//TODO: program breaks if length and width are not exact multiples of spSize
	if (image.empty()) {
		char c;
		std::cout << "Could not open or find the image" << std::endl;
		std::cin >> c;
		exit(-1);
	}

	//determine the mean color of image and the starting temperature
	cv::cvtColor(image, image, cv::COLOR_BGR2Lab);
	cv::Scalar meanColor = getMeanColor(image);

	//perform PCA, and extract results
	cv::Scalar startColor1 = cv::Scalar({ meanColor[0], meanColor[1], meanColor[2] });
	cv::Scalar startColor2 = cv::Scalar({ meanColor[0], meanColor[1], meanColor[2] });
	double startTemp = initPCA(image, &startColor1, &startColor2);
	
	//generate image classes to work with
	PicImage pimage = PicImage(image);
	SPImage spImage = SPImage(image.rows, image.cols, spSize, &pimage);
	std::cout << "Image objects initialized\n";

	//initialize superpixels
	initializeSP(&pimage, &spImage, spSize);
	std::cout << "Superpixels initialized\n";

	//initialize other variables/objects needed for loop
	Palette palette = Palette(&pimage, &spImage, paletteSize, startTemp, startColor1, startColor2);
	Slic slic = Slic(&pimage, &spImage, &palette);
	double curTemp = 1.1 * startTemp;
	int count = 0;
	
	while (curTemp > FINAL_TEMP) {
		std::cout << "Starting iteration at temp " << curTemp << " : ";
		count++;
		//refine the superpixels
		slic.refineSP();
		//pimage.printAssignments();

		//refine the palette
		palette.associatePalette();
		palette.refinePalette();
		std::string file = RESULTS_PATH "obama curTemp ";
		file.append(std::to_string(int(curTemp)));
		file.append(" pic ");
		file.append(std::to_string(count));
		file.append(".png");
		palette.displayPixelImage(15, file);
		std::cout << "Change value = " << palette.getChange() << std::endl;

		if (palette.getChange() < TEMP_CHANGE_THRESH) {
			if (palette.getCurSize() == paletteSize * 2) {
				std::cout << "Convergence has occured with a full palette.  Outputting...\n";
				break;
			}
			std::cout << "Convergence has occured.  Attempting to expand palette...\n";
			palette.expandPalette();
			curTemp = palette.getCurTemp();
		}
	}

	//post process and output image
	palette.displayPixelImage(15, RESULTS_PATH "output.png", TRUE);
	palette.saturatePalette();
	palette.displayPixelImage(15, RESULTS_PATH "saturated output.png", TRUE);
	return 0;
}

// Get the average color of all pixels
cv::Scalar getMeanColor(cv::Mat image) {
	cv::Scalar result = cv::Scalar(0.0, 0.0, 0.0, 0.0);
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			result[0] += image.at<cv::Vec3b>(x, y)[0];
			result[1] += image.at<cv::Vec3b>(x, y)[1];
			result[2] += image.at<cv::Vec3b>(x, y)[2];
		}
	}
	result[0] = result[0] / (image.rows * image.cols);
	result[1] = result[1] / (image.rows * image.cols);
	result[2] = result[2] / (image.rows * image.cols);
	return result;
}

double initPCA(cv::Mat image, cv::Scalar* start1, cv::Scalar* start2) {
	//Assemble a data matrix
	cv::Mat data = image.reshape(1,3); //switching to (3,1) changes image to 1d color image

	//Using PCA (mean calculated by PCA function)
	cv::PCA pca = cv::PCA(data, cv::Mat(), CV_PCA_DATA_AS_COL);

	//check the data
	//std::cout << "Vectors:\n" << pca.eigenvectors << std::endl;
	//std::cout << pca.eigenvectors.type() << std::endl;
	/*std::cout << "Vectors:\n" << pca.eigenvectors.at<float>(0) << std::endl;
	std::cout << "Vectors:\n" << pca.eigenvectors.at<float>(0, 0) << std::endl;
	std::cout << "Vectors:\n" << pca.eigenvectors.at<float>(0, 1) << std::endl;
	std::cout << "Vectors:\n" << pca.eigenvectors.at<float>(0, 2) << std::endl;*/
	//std::cout << "Values:\n" << pca.eigenvalues << std::endl;
	//std::cout << "Mean:\n" << pca.mean << std::endl;
	/*std::cout << pca.eigenvalues.type() << std::endl;
	std::cout << pca.eigenvalues.at<float>(0) << std::endl;*/

	//set the start colors
	(*start1)[0] += PCA_FACTOR * pca.eigenvectors.at<float>(0, 0);
	(*start1)[1] += PCA_FACTOR * pca.eigenvectors.at<float>(0, 1);
	(*start1)[2] += PCA_FACTOR * pca.eigenvectors.at<float>(0, 2);
	(*start2)[0] -= PCA_FACTOR * pca.eigenvectors.at<float>(0, 0);
	(*start2)[1] -= PCA_FACTOR * pca.eigenvectors.at<float>(0, 1);
	(*start2)[2] -= PCA_FACTOR * pca.eigenvectors.at<float>(0, 2);

	//return the initial temperature 
	return 2*pca.eigenvalues.at<float>(0);

	//to find cv::Mat.type()
	//https://stackoverflow.com/questions/10167534/how-to-find-out-what-type-of-a-mat-object-is-with-mattype-in-opencv
}

void initializeSP(PicImage* pimage, SPImage* spImage, int size) {
	//set up storage for centroid data
	std::vector<std::vector<double>> spData;
	spData.assign(spImage->numPixels(), std::vector<double>(6, 0));
	std::vector<std::vector<double>> distData;
	distData.assign(pimage->rows(), std::vector<double>(pimage->cols(), INT_MAX));

	//determine centroid locations, store in spImage
	double offset = (double)size / 2.0;
	for (int num = 0; num < spImage->numPixels(); num++) {
		spData[num][SP_DATA_XCOOR] = size * std::floor((double)num / spImage->cols()) + offset;
		spData[num][SP_DATA_YCOOR] = size * (num % spImage->cols()) + offset;
		spImage->getPixel(num)->setCentroid(spData[num][SP_DATA_XCOOR], spData[num][SP_DATA_YCOOR]);
	}
	//spImage->printCentroids();

	//assign pixels based on distance to centroid, store in pimage
	for (int num = 0; num < spImage->numPixels(); num++) {
		int lxbound = std::max(0, int(std::floor(spData[num][SP_DATA_XCOOR])) - 2 * size);
		int lybound = std::max(0, int(std::floor(spData[num][SP_DATA_YCOOR])) - 2 * size);
		int uxbound = std::min(pimage->rows(), int(std::ceil(spData[num][SP_DATA_XCOOR])) + 2 * size);
		int uybound = std::min(pimage->cols(), int(std::ceil(spData[num][SP_DATA_YCOOR])) + 2 * size);
		for (int x = lxbound; x < uxbound; x++) {
			for (int y = lybound; y < uybound; y++) {
				double dist = std::hypot(spData[num][SP_DATA_XCOOR] - x, spData[num][SP_DATA_YCOOR] - y);
				if (dist <= distData[x][y]) {
					pimage->getPixel(x, y)->setSpNum(num);
					distData[x][y] = dist;
				}
			}
		}
	}
	//pimage->printAssignments();

	//aggregate all color data
	for (int num = 0; num < pimage->numPixels(); num++) {
		auto pix = pimage->getPixel(num);
		spData[(*pix).getSpNum()][SP_DATA_LVAL] += (*pix).getColor()[0];		//l value agg
		spData[(*pix).getSpNum()][SP_DATA_AVAL] += (*pix).getColor()[1];		//a value agg
		spData[(*pix).getSpNum()][SP_DATA_BVAL] += (*pix).getColor()[2];		//b value agg
		spData[(*pix).getSpNum()][SP_DATA_COUNT] += 1;							//counter, used for averaging
	}

	//calculate average color, store in spImage
	for (int num = 0; num < spImage->numPixels(); num++) {
		cv::Scalar color = cv::Scalar({
			spData[num][SP_DATA_LVAL] / spData[num][SP_DATA_COUNT],
			spData[num][SP_DATA_AVAL] / spData[num][SP_DATA_COUNT],
			spData[num][SP_DATA_BVAL] / spData[num][SP_DATA_COUNT]});
		spImage->getPixel(num)->setAvgColor(color);
	}
}