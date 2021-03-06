#pragma once
#include "v3.h"
#include "framebuffer.h"
#include "AABB.h"
#include <vector>
#include "cubemap.h"
#include "CGInterface.h"

using std::vector;
class BillBoard;

struct PointProperty
{
	V3 p, c, n;		// position, color, normal
	float s, t;		// texture s, texture t
	PointProperty(V3 _p, V3 _c, V3 _n, float _s, float _t) :p(_p), c(_c), n(_n), s(_s), t(_t) {};
};

// Triangle Mesh
class TM
{
public:
	vector<V3> verts, colors, normals, staticVerts, staticNorms;		// vertices and colors
	vector<float> vertST;
	vector<unsigned int>  tris;		// indices
	int vertsN, trisN;
	int id;
	vector<shared_ptr<BillBoard>> reflectorBB;	// all the possible reflections
	int hasST;
	int isCubemap;
	int isBox;
	int isGround;

	// Shadder files
	shared_ptr<CGInterface> cgi;
	shared_ptr<ShaderOneInterface> soi;
	string shaderOneFile;

	std::string tex;
	int pixelSz;		// approximate projected aabb size
	bool isEnvMapping;
	bool isShowObjColor;
	bool isRefraction;

	TM() :vertsN(0), trisN(0), pixelSz(0), isEnvMapping(false), 
		isShowObjColor(true),id(tmIDCounter++), isRefraction(false),
		hasST(0), isCubemap(0), isGround(0) {};
	void SetRectangle(V3 O, float rw, float rh);
	void SetTriangle(PointProperty p0, PointProperty p1, PointProperty p2);
	void SetQuad(PointProperty p0, PointProperty p1, PointProperty p2, PointProperty p3);
	void SetQuad(V3 O, V3 n, V3 up, float sz, float s = 1.0f, float t = 1.0f);
	void SetBillboard(V3 O, V3 n, V3 up, float sz, float s = 1.0f, float t = 1.0f);
	void SetUnitBox();
	void SetText(std::string tf);
	void Allocate();
	void SetAllPointsColor(V3 color);	// set color to verts
	void SetShaderOne(const string shaderFile) { shaderOneFile = shaderFile; };
	void SetColor(V3 color);
	tuple<V3, V3, V3, V3> GetCornerAndAxis();

	// Rasterization
	void RenderPoints(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb);
	void RenderWireFrame(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb);
	void RenderFill(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb);
	void RenderFillZ(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb); // only draw z buffer
	void RenderAABB(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb);
	void RenderBB(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb, shared_ptr<FrameBuffer> bbTexture);
	void RenderHW(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> curfb);

	// Transformation
	void RotateAboutArbitraryAxis(V3 O, V3 a, float angled);
	void Translate(V3 tv);
	void Scale(float scf);		// normalize size to some scf
	void LoadModelBin(char *fname);
	AABB ComputeAABB();
	float ComputeSBBR(V3 c);	// given c, compute r
	void PositionAndSize(V3 tmC, float tmSize);
	V3 GetCenter();

	// Ray tracing
	void RayTracing(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb);
	
	// Shading
	tuple<V3, float> Shading(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb, int u, int v, float w, PointProperty& pp, V3 dn = V3(0.0f));

	// Lighting 
	void Light(V3 mc, V3 L, shared_ptr<PPC> ppc);	  // Per vertex light
	V3 Light(shared_ptr<PPC> ppc, PointProperty& pp, int u, int v, float w); // Per pixel  light 
	bool ComputeShadowEffect(shared_ptr<PPC> ppc, int u, int v, float z, float &sdEffect);
	bool IsPixelInProjection(int u, int v, float z, V3 &color, float &alpha);
	V3 HomographMapping(V3 uvw, shared_ptr<PPC> ppc1, shared_ptr<PPC> ppc2);
	V3 ClampColor(V3 color);

	// Env mapping
	tuple<V3, float> EnvMapping(shared_ptr<PPC> ppc, shared_ptr<FrameBuffer> fb, shared_ptr<CubeMap> cubemap, V3 p, V3 n, V3 dn = V3(0.0f));
	int EnvBBIntersection(vector<shared_ptr<BillBoard>> bbs, V3 p, V3 viewDir, float &distance, V3 &color, float &alpha);

	// Morphing
	void SphereMorph(V3 c,float r, float fract);
	void WaterAnimation(float t);

	// Ray geometry intersection
	tuple<float, float, float, float> RayTriangleIntersect(V3 C, V3 ray, V3 p0, V3 p1, V3 p2);
	tuple<PointProperty, float> RayMeshIntersect(V3 C, V3 ray);									// Return pp and closest w

	~TM();
	static int tmIDCounter;
};
 
