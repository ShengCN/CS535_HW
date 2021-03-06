#pragma once
#include "ppc.h"
#include "TM.h"
#include "scene.h"

// Singleton object to capsulate all global variables
class GlobalVariables
{
public:
	static GlobalVariables* Instance();

	// Global variables
	bool isHighResolution;
	int highResoW, highResoH;
	int resoW, resoH;

	Scene *curScene;
	string projectedTextureName;

	bool isRenderProjectedTexture;
	bool depthTest, lodTexture;
	bool isRecording;
	bool isDebugZbuffer;
	bool isSaveLodTextures;
	bool isLight;
	bool isRayTracing;
	bool isUseSBB;
	bool isOpenMP;
	bool isPrintFPS;
	bool isWireFrame;

	float dbgParameter;

	vector<string> cubemapFiles;

	// For HW5
	bool isRefraction;
	float refractRatio;
	string checkerBoxTexName;
	string recordName;
	bool isShadow;
	int tmAnimationID;
	bool isDBGRaytracing;
	bool isCubemapMipmap;

private:
	GlobalVariables();
	static GlobalVariables* _instance;
};

