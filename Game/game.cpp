#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#define THR_HOLD 75

void applyGaussianBlur(byte* inputImage, byte* outputImage, int width, int height, int kernelSize, double sigma) {
	// Generate the Gaussian kernel
	std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize, 0.0));
	double sum = 0.0;

	for (int i = -kernelSize / 2; i <= kernelSize / 2; ++i) {
		for (int j = -kernelSize / 2; j <= kernelSize / 2; ++j) {
			double value = std::exp(-(i * i + j * j) / (2 * sigma * sigma));
			kernel[i + kernelSize / 2][j + kernelSize / 2] = value;
			sum += value;
		}
	}

	// Normalize the kernel
	for (int i = 0; i < kernelSize; ++i) {
		for (int j = 0; j < kernelSize; ++j) {
			kernel[i][j] /= sum;
		}
	}

	// Apply the Gaussian blur
	for (int y = kernelSize / 2; y < height - kernelSize / 2; ++y) {
		for (int x = kernelSize / 2; x < width - kernelSize / 2; ++x) {
			double accumulator = 0.0;
			for (int i = -kernelSize / 2; i <= kernelSize / 2; ++i) {
				for (int j = -kernelSize / 2; j <= kernelSize / 2; ++j) {
					int pixelValue = inputImage[(x + j) + width * (y + i)];
					accumulator += pixelValue * kernel[i + kernelSize / 2][j + kernelSize / 2];
				}
			}
			outputImage[x+width*y]= static_cast<int>(accumulator);
		}
	}
}

byte gradOnPix(unsigned char* img,double* ang, int pixX, int pixY, int w) {
	if (pixX == 0 || pixY == 0 || pixX == MAXWID || pixY == MAXWID)
	{
		return 0;
	}
	int gx = /*img->pixels[pixY * img->width + pixX]*/ -img[pixY * w + pixX - 1]
		+ img[pixY * w + pixX + 1];
	int gy = /*img->pixels[pixY * img->width + pixX]*/ -img[(pixY - 1) * w + pixX] +
		img[(pixY + 1) * w + pixX];
	ang[pixX+w*pixY] = std::atan2(gy, gx);
	return static_cast<byte>(std::sqrt(gx * gx + gy * gy));
	/*[0 -1 0]
	  [-1 0 1]
	  [0 1 0]*/
}

byte* edgeDetection(unsigned char* img, int width, int height)
{
	byte* newImg = new unsigned char[width * height];
	byte* gradMag = new byte[width * height];
	double* gradAngle = new double[width * height];
	applyGaussianBlur(img, newImg, width, height, 3, 1);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			gradMag[y * width + x] = gradOnPix(img, gradAngle, x, y, width);
			//newImg[y * width + x]  = gradMag[y * width + x] > THR_HOLD ? 255 : 0;
			//if(y==70) std::cout << x << "\n";
		}//std::cout << y<<", ";
	}
	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			double angle = gradAngle[x + y * width];

			// Convert the angle to one of four discrete directions: 0, 45, 90, 135 degrees
			angle = fmod(angle, 180.0);
			if (angle < 0) {
				angle += 180.0;
			}
			//angle 0
			if ((0 <= angle && angle < 22.5) || (157.5 <= angle && angle <= 180))
				angle = 0;
			//angle 45
			else if (22.5 <= angle && angle < 67.5)
				angle = 45;
			//angle 90
			else if (67.5 <= angle && angle < 112.5) 
				angle = 90;
			//angle 135
			else if (112.5 <= angle && angle < 157.5) 
				angle = 135;

			int pixelValue = gradMag[y * width + x];

			switch (static_cast<int>(angle)) {
			case 0:
			case 180:
				newImg[x + width * y] = (pixelValue >= gradMag[x - 1 + width * y] &&
					pixelValue >= gradMag[x + 1 + width * y] ? pixelValue : 0);
				break;

			case 45:
				newImg[x + width * y] = (pixelValue >= gradMag[x + 1 + width * (y-1)] &&
					pixelValue >= gradMag[x - 1 + width * (y+1)] ? pixelValue : 0);
				break;

			case 90:
				newImg[x + width * y] = (pixelValue >= gradMag[x + width * (y - 1)] &&
					pixelValue >= gradMag[x + width * (y + 1)] ? pixelValue : 0);
				break;

			case 135:
				newImg[x + width * y] = (pixelValue >= gradMag[x - 1 + width * (y - 1)] &&
					pixelValue >= gradMag[x + 1 + width * (y + 1)] ? pixelValue : 0);
				break;
			default:
				std::cout << "dont have correct angle\n";
				break;
			}
			

		}
	}
	//TODO: normal to 0 and 255 the new_img
	for (int i = 0; i < width*height; i++)
	{
		newImg[i] = newImg[i] > THR_HOLD ? 255 : 0;
	}
	delete[] gradAngle;
	delete[] gradMag;
	return newImg;
}

void half_toning_fill(unsigned char* new_img, int new_x, int new_y, unsigned char mask[], int new_width)
{
	new_img[(new_y * new_width) + new_x] = mask[0]; //mapped pixel
	new_img[(new_y * new_width) + new_x + 1] = mask[1]; //1 right
	new_img[(new_y + 1) * new_width + new_x + 1] = mask[2]; // 1 bottom, 1 right
	new_img[(new_y + 1) * new_width + new_x] = mask[3];// 1 below
}


unsigned char* halftoning_algo(unsigned char* img_data, int width, int height) {
	int num_of_intensities = 256;
	double max_intensity = num_of_intensities - 1;
	int new_height = 2 * height;
	int new_width = 2 * width;
	int slot; // index of mask filter to be chosen
	unsigned char* new_img_data = new unsigned char[new_height * new_width]; // dynamic memory to assign an initial 0 value
	unsigned char masks[5][4] = { {0,0,0,0}, //5 different masks - 2*2 + 1, indexed 0 to 4, of increasing intensity (0 mask all black .. 4 all white) mask will be picked based on source pixel calculation
					   {0,0,0,255},
						{255,0,255,0},
						{255,255,0,255},
						{255,255,255,255}
	};

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			slot = (int)(((double)img_data[y * width + x] / max_intensity) * 5);
			if (slot == 5) { slot = 4; };
			half_toning_fill(new_img_data, 2 * x, 2 * y, masks[slot], new_width);
		}
	}
	return new_img_data;
}
unsigned char* floyd_steinberg_algo(unsigned char* img_data, int width, int height)
{

	unsigned char* new_img_data = new unsigned char[height * width];

	// clear new image
	for (int i = 0; i < height * width; i++)
	{
		new_img_data[i] = 0;
	}

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int current = img_data[y * width + x];
			int accumulatedError = new_img_data[y * width + x];

			int newPixel = int(float(current + accumulatedError) / 256 * 16);
			if (newPixel > 15) newPixel = 15;

			new_img_data[y * width + x] = newPixel * 16;

			int error = abs(current - newPixel * 16);

			if (x + 1 < width)
			{
				new_img_data[y * width + x + 1] += int(error * 7 / 16);
			}
			if (x - 1 >= 0 && y + 1 < height)
			{
				new_img_data[(y + 1) * width + x - 1] += int(error * 3 / 16);
			}
			if (y + 1 < height)
			{
				new_img_data[(y + 1) * width + x] += int(error * 5 / 16);
			}
			if (x + 1 < width && y + 1 < height)
			{
				new_img_data[(y + 1) * width + x + 1] += int(error / 16);
			}
		}
	}
	return new_img_data;
}


static void printMat(const glm::mat4 mat)
{
	std::cout<<" matrix:"<<std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout<< mat[j][i]<<" ";
		std::cout<<std::endl;
	}
}

Game::Game() : Scene()
{
}

Game::Game(float angle ,float relationWH, float near1, float far1) : Scene(angle,relationWH,near1,far1)
{ 	
}

void Game::Init()
{		

	AddShader("../res/shaders/pickingShader");	
	AddShader("../res/shaders/basicShader");
	
	//AddTexture("../res/textures/grass.bmp",false);

	AddShape(Plane,-1,TRIANGLES);
	
	pickedShape = 0;
	
	//SetShapeTex(0,0);
	MoveCamera(0,zTranslate,10);
	pickedShape = -1;
	
	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx)
{
	Shader *s = shaders[shaderIndx];
	int r = ((pickedShape+1) & 0x000000FF) >>  0;
	int g = ((pickedShape+1) & 0x0000FF00) >>  8;
	int b = ((pickedShape+1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal",Model);
	s->SetUniform4f("lightDirection", 0.0f , 0.0f, -1.0f, 0.0f);
	if(shaderIndx == 0)
		s->SetUniform4f("lightColor",r/255.0f, g/255.0f, b/255.0f,1.0f);
	else 
		s->SetUniform4f("lightColor",0.7f,0.8f,0.1f,1.0f);
	s->Unbind();
}

void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::Motion()
{
	if(isActive)
	{
	}
}

Game::~Game(void)
{
}

void write_img_to_file(const char* filename, unsigned char* img_data, int length_bytes, int width,bool is_binary) {
	std::fstream img5_ostream;
	img5_ostream.open(filename, std::fstream::out | std::fstream::trunc); //stream open for writing, override any data on the file if already exists
	if (img5_ostream.is_open()) {
		for (int i = 0; i < length_bytes; i++) {
			int intensity = img_data[i];
			if (intensity && is_binary) { intensity = 1; }
			else { intensity = intensity >> 4; }
			img5_ostream << intensity << ",";
			if (i%width == 0 )
			{
				img5_ostream << "\n";
			}
		}
	}
	else {
		std::cout << "Couldn't open the text file with name: " << filename << std::endl;
	}

	img5_ostream.close();

}