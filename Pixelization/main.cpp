#include "main.h"

int main(int argc, char **argv) {
	//setup input variables
	char defaultLoc;
	std::string file;
	cv::Mat image;

	//use std::in to take in desired image file
	std::cout << "Using root \"../Images/\" folder? (y/n) ";
	std::cin >> defaultLoc;
	while (TRUE) {
		//get file location
		if (defaultLoc == 'n')
			std::cout << "Enter full path to desired image: ";
		else if (defaultLoc == 'y')
			std::cout << "Enter image filename: " << IMG_PATH;
		else
			std::cout << "Invalid input. Using default location: " << IMG_PATH;
		std::cin >> file;

		//try to read the image
		if (defaultLoc != 'n')
			file = IMG_PATH + file;
		image = cv::imread(IMG_PATH + file);

		//if image exists, continue, else, reprompt
		if (!image.empty())
			break;
		else
			std::cout << "Could not open or find the image" << std::endl;
	}

	//get the target spSize
	int spSize = -1;
	std::cout << "Enter desired size of superpixels for ("
		<< image.cols << ", " << image.rows << "): ";
	std::cin >> spSize;
	while (spSize > std::min(image.rows, image.cols) || spSize <= 0) {
		std::cout << "Invalid number.  Please enter a number between 0 and the size of the chosen image: ";
		std::cin >> spSize;
	}

	//get the target scale size
	int displayScale = 10;
	std::cout << "Enter desired scale size for displaying the output image: ";
	std::cin >> displayScale;
	while (displayScale < 0) {
		std::cout << "Invalid number.  Please enter a positive number: ";
		std::cin >> displayScale;
	}

	//hardcode initial values for now
	//TODO: these should be taken in as inputs to program (start from console, then by gui)
	//int spSize = 10; //squirrel size = 9, scale = 10, mult = 2
	//int displayScale = 30;
	//std::string filename = IMG_PATH "squirrel.jpg"; //spSize 36, scale 30
	//std::string filename = IMG_PATH "chi.jpg"; //spSize 10, scale 30
	//std::string filename = IMG_PATH "flowers.jpg"; //spSize 36, scale 10
	//std::string filename = IMG_PATH "test.png"; //spSize 2
	//std::string filename = IMG_PATH "shield.png"; //spSize 10

	runAlgo(file, spSize, displayScale);
	return 0;
}

void runAlgo(std::string filename, int spSize, int displayScale) {
	//check if user constraint should be activated
	bool constrained = false;
	char c = 'l';
	/*while (c != 'y' && c != 'n') {
		std::cout << "Would you like to activate user constraints? (y/n)? ";
		std::cin >> c;
		if (c == 'n')
			constrained = false;
	}*/

	//load image
	cv::Mat image = cv::imread(filename);

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
	Palette palette = Palette(&pimage, &spImage, PALETTE_SIZE, startTemp, startColor1, startColor2);
	Slic slic = Slic(&pimage, &spImage, &palette);
	double curTemp = 1.1 * startTemp;
	int count = 0;
	//std::cout << palette.importMat(RESULTS_PATH "squirrel mask.png"); //TODO: importance map is a dud
	   
	bool reiterate = false;
	while (curTemp > FINAL_TEMP || reiterate) {
		std::cout << "Starting iteration at temp " << curTemp << " : ";
		count++;
		//refine the superpixels
		slic.refineSP();
		//pimage.printAssignments();

		//refine the palette
		palette.associatePalette();
		palette.refinePalette();
		std::string file = RESULTS_PATH "squirrel curTemp ";
		file.append(std::to_string(int(curTemp)));
		file.append(" pic ");
		file.append(std::to_string(count));
		file.append(".png");
		palette.displayPixelImage(displayScale, file);
		std::cout << "Change value = " << palette.getChange() << std::endl;

		if (palette.getChange() < TEMP_CHANGE_THRESH) {
			//check if user wants to contrain pixels
			bool constrainPixel = true;
			c = 'l';
			while (c != 'y' && c != 'n' && constrained) {
				std::cout << "Would you like to activate user constraints? (y/n)? ";
				std::cin >> c;
				if (c == 'n')
					break;
			}

			//check if user wants to constrain the palette
			bool constrainPal = true;
			c = 'l';
			while (c != 'y' && c != 'n' && constrained) {
				std::cout << "Would you like to activate user constraints? (y/n)? ";
				std::cin >> c;
				if (c == 'n')
					break;
			}

			if (palette.getCurSize() == PALETTE_SIZE * 2) {
				std::cout << "Convergence has occured with a full palette.  Outputting...\n";
				break;
			}
			std::cout << "Convergence has occured.  Attempting to expand palette...\n";
			palette.expandPalette();
			curTemp = palette.getCurTemp();
		}
	}

	//post process and output image
	palette.displayPixelImage(displayScale, RESULTS_PATH "output.png", TRUE);
	palette.saturatePalette();
	palette.displayPixelImage(displayScale, RESULTS_PATH "saturated output.png", TRUE);
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
	cv::Mat data = image.reshape(1, 3); //switching to (3,1) changes image to 1d color image

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
	return 2 * pca.eigenvalues.at<float>(0);

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
			spData[num][SP_DATA_BVAL] / spData[num][SP_DATA_COUNT] });
		spImage->getPixel(num)->setAvgColor(color);
	}
}