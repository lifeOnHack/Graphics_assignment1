#pragma once
#include "scene.h"
#include "../res/includes/glfw/include/GLFW/glfw3.h"
#include "stb_image.h"
#include <iostream>
typedef  unsigned char byte;
using std::string;
#define MAXWID 255

class Game : public Scene
{
public:
	
	Game();
	Game(float angle,float relationWH,float near, float far);
	void Init();
	void Update(const glm::mat4 &MVP,const glm::mat4 &Model,const int  shaderIndx);
	void ControlPointUpdate();
	void WhenRotate();
	void WhenTranslate();
	void Motion();
	~Game(void);
};
byte gradOnPix(unsigned char* img, double* ang, int pixX, int pixY, int w);
byte* edgeDetection(unsigned char* img, int width, int height);
void half_toning_fill(unsigned char* new_img, int new_x, int new_y, unsigned char mask[], int new_width);
unsigned char* halftoning_algo(unsigned char* img_data, int width, int height);
unsigned char* floyd_steinberg_algo(unsigned char* img_data, int width, int height);
void write_img_to_file(const char* filename, unsigned char* img_data, int length_bytes,int, bool is_binary);
