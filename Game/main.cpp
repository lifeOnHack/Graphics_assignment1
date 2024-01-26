#include "InputManager.h"
// #include "../DisplayGLFW/display.h"
#include "game.h"
#include "../res/includes/glm/glm.hpp"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc,char *argv[])
{
	const int DISPLAY_WIDTH = 512;
	const int DISPLAY_HEIGHT = 512;
	const float CAMERA_ANGLE = 0.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;
	int width, height, numComponents;
	Game *scn = new Game(CAMERA_ANGLE,(float)DISPLAY_WIDTH/DISPLAY_HEIGHT,NEAR,FAR);
	
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	unsigned char* img = stbi_load("../res/textures/lena256.jpg", &width, &height, &numComponents, 0);
	if (img == NULL)
	{
		std::cout << "can't open image" << std::endl;
		return 1;
	}
	Init(display);
	scn->Init();
	display.SetScene(scn);
	//need to use custom draw
	scn->AddTexture("../res/textures/lena256.jpg", false);
	scn->SetShapeTex(0, 0);
	scn->customDraw(1, 0, scn->BACK, true, false, 0);

	byte* edgeImg = edgeDetection(img, width, height);
	int s = stbi_write_png("../res/textures/edge.png", width, height, numComponents, edgeImg, width);
	scn->AddTexture("../res/textures/edge.png", false);
	scn->SetShapeTex(0, 1);
	scn->customDraw(1, 0, scn->BACK, false, false, 1);

	byte* halfonedImg = halftoning_algo(img, width, height);
	s = stbi_write_png("../res/textures/halfone.png", width*2, height*2, numComponents, halfonedImg, width*2);
	scn->AddTexture("../res/textures/halfone.png", false);
	scn->SetShapeTex(0, 2);
	scn->customDraw(1, 0, scn->BACK, false, false, 2);
	
	byte* fsImg = floyd_steinberg_algo(img, width, height);
	s = stbi_write_png("../res/textures/fs.png", width, height, numComponents, fsImg, width);
	scn->AddTexture("../res/textures/fs.png", false);
	scn->SetShapeTex(0, 3);
	scn->customDraw(1, 0, scn->BACK, false, false, 3);

	scn->Motion();
	display.SwapBuffers();
	while(!display.CloseWindow())
	{
		/*scn->Draw(1,0,scn->BACK,true,false);
		scn->Motion();
		display.SwapBuffers();*/
		display.PollEvents();	
			
	}
	//save to txt
	write_img_to_file("img4.txt", edgeImg, width * height, width, true);
	write_img_to_file("img5.txt", halfonedImg, width * height* 4, width*2, true);
	write_img_to_file("img6.txt", fsImg, width * height, width, false);
	stbi_image_free(img);
	stbi_image_free(edgeImg);
	stbi_image_free(halfonedImg);
	stbi_image_free(fsImg);
	delete scn;
	return 0;
}
