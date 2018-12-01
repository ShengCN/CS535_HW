#pragma once
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "ppc.h"
#include "v3.h"

class CubeMap;
using std::vector;
using std::unordered_map;
struct PointProperty;
struct TextureInfo
{
	vector<unsigned int> texture;
	int w, h;
	TextureInfo() :w(0), h(0){};
	TextureInfo(int _w, int _h) :w(_w), h(_h) { texture.resize(w*h); };
};

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int *pix;	// pixel 
	float *zb;			// z buffer
	int w, h;

	// GPU rendering parameters
	bool isgpu;

	static unordered_map<std::string, vector<shared_ptr<TextureInfo>>> textures; 
	static unordered_map<std::string, GLuint> gpuTexID;

	FrameBuffer(int u0, int v0, int _w, int _h);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void SetBGR(unsigned int bgr);
	
	void SetGuarded(int u, int v, unsigned int color);
	void LoadTiff(const char* fname);
	void SaveAsTiff(const char* fname);
	void SaveTextureAsTiff(string fname, const string textureName, int loD);
	int ClipToScreen(int& u0, int& v0, int& u1, int& v1);
	int ClipToScreen(float& u0, float& v0, float& u1, float& v1);
	bool IsInScreen(int u, int v);
	void ClearBGRZ(unsigned int bgr, float z0);
	void ClearZ(float z0);
	bool DepthTest(int u, int v, float z);
	float GetZ(int u, int v);
	unsigned int Get(int u, int v);
	bool LoadTexture(const std::string texFile);
	bool LoadTextureGPU(const std::string texFile);
	bool LoadCubemapGPU(vector<string> cubemaps);

	// Draw something
	void DrawSegment(V3 p0, V3 c0, V3 p1, V3 c1);
	void Draw3DSegment(shared_ptr<PPC> ppc, V3 p1, V3 c1, V3 p2, V3 c2);
	void DrawRectangle(int u0, int v0, int u1, int v1, unsigned int color);
	void DrawCircle(int u0, int v0, int r, unsigned int color);
	void DrawEllipse(int u0, int v0, float r0, float r1, unsigned int color);
	void DrawPoint(int u, int v, unsigned int color);
	void Draw3DPoint(shared_ptr<PPC> ppc, V3 p, unsigned int color, int pointSize);
	void Draw3DTriangle(PPC* ppc, V3 p1, V3 p2, V3 p3, V3 color);
	void Draw3DTriangle(PPC* ppc, V3 p0, V3 c0, V3 p1, V3 c1, V3 p2, V3 c2);
	void DrawTexture(const std::string texFile, int LoD = 0);
	void DrawCubeMap(shared_ptr<PPC> ppc, shared_ptr<CubeMap> cubemap);

	// DEBUG PPC
	void DrawPPC(shared_ptr<PPC> wPPC, shared_ptr<PPC> tPPC, float vf);	// visualize target PPC using wPPC
	void VisualizeCurrView(shared_ptr<PPC> ppc0, float currf, shared_ptr<PPC> ppc1, shared_ptr<FrameBuffer> fb1);
	void VisualizeCurrView3D(shared_ptr<PPC> ppc0, shared_ptr<PPC> ppc1, shared_ptr<FrameBuffer> fb1); 
	V3 LookupColor(std::string texFile, float s, float t, float &alpha, int pixelSz = -1);
	V3 BilinearLookupColor(float s, float t, float &alpha);
	V3 BilinearLookupColor(shared_ptr<TextureInfo> tex, float s, float t);
	V3 BilinearLookupColor(shared_ptr<TextureInfo> tex, float s, float t, float &alpha);
	V3 Light(PointProperty pp, V3 L, PPC *ppc);	// point property, ppc

	// Texture downsampling
	void PrepareTextureLoD(string texFile);

	// GPU render 
	void SetupGPU();
	void SaveGPU2CPU() { glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix); };
	void SaveGPUAsTiff(const string saveFile);
	GLuint SaveCPU2GPUtexture();

	void Resample(shared_ptr<PPC> ppc0, shared_ptr<FrameBuffer> fb1, shared_ptr<PPC> ppc1);
	float ColorDifference(shared_ptr<PPC> ppc0, shared_ptr<FrameBuffer> fb1, shared_ptr<PPC> ppc1);

private:
	void Set(int u, int v, int color);
	tuple<float, float> GetPixelCenter(int u, int v) { return tuple<float, float>(0.5f + static_cast<float>(u), 0.5f + static_cast<float>(v)); }
};

