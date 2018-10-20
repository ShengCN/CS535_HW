#pragma once
#include "ppc.h"
#include "TM.h"
#include "scene.h"

class GlobalVariables
{
public:
	static GlobalVariables* Instance();

	// Global variables
	Scene *curScene;
	bool isRenderProjectedTexture;
	string projectedTextureName;
private:
	GlobalVariables();
	static GlobalVariables* _instance;
};

