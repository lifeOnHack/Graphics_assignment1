//#include "../DisplayGLFW/display.h"
//#include "stb_image.h"
//#include <iostream>
//typedef  unsigned char byte;
////#include <string>
//using std::string;
//#define MAXWID 255
//
//static GLFWimage* loadImgToOneDArr(const string& path) {
//	GLFWimage* img = new GLFWimage;
//	int numComponents;
//	img->pixels = stbi_load(path.c_str(), &(img->width), &(img->height), &numComponents,1);
//	if (img->pixels == NULL)
//	{
//		std::cout << "can't open image" << std::endl;
//		stbi_image_free(img->pixels);
//		free(img);
//		return nullptr;
//	}
//	return img;
//}
//
//byte gradOnPix(GLFWimage* img, int pixX,int pixY) {
//	if (pixX==0 || pixY==0 || pixX==MAXWID || pixY==MAXWID)
//	{
//		return 0;
//	}
//	int gx = /*img->pixels[pixY * img->width + pixX]*/ - img->pixels[pixY * img->width + pixX - 1]
//		+ img->pixels[pixY * img->width + pixX+1];
//	int gy = /*img->pixels[pixY * img->width + pixX]*/ - img->pixels[(pixY - 1) * img->width + pixX] +
//		img->pixels[(pixY+ 1) * img->width + pixX];
//	return std::sqrt(gx*gx + gy*gy);
//	/*[0 -1 0]
//	  [-1 0 1]
//	  [0 1 0]*/
//}
//GLFWimage* edgeDetection(const string& path) {
//	GLFWimage* img = loadImgToOneDArr(path);
//	byte* newImg = new byte[img->width*img->height];
//	for (int y = 0; y < img->height; y++)
//	{
//		for (int x = 0; x < img->width; x++)
//		{
//			newImg[y * img->height + x] = gradOnPix(img, x, y);
//		}
//	}
//	stbi_image_free(img->pixels);
//	img->pixels = newImg;
//	return img;
//}
