#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"
#include "v3.h"
#include <iostream>

#include "tiffio.h"
#include <algorithm>
#include "AABB.h"
#include "MathTool.h"

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h)
	: Fl_Gl_Window(u0, v0, _w, _h, nullptr)
{
	w = _w;
	h = _h;
	pix = new unsigned int[w * h];
}


void FrameBuffer::draw()
{
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);
}

int FrameBuffer::handle(int event)
{
	switch (event)
	{
	case FL_KEYBOARD:
		{
			KeyboardHandle();
			return 0;
		}
	default:
		break;
	}
	return 0;
}

void FrameBuffer::KeyboardHandle()
{
	int key = Fl::event_key();
	switch (key)
	{
	case ',':
		{
			cerr << "INFO: pressed ," << endl;
			break;
		}
	default:
		cerr << "INFO: do not understand keypress" << endl;
	}
}


void FrameBuffer::SetBGR(unsigned int bgr)
{
	for (int uv = 0; uv < w * h; uv++)
		pix[uv] = bgr;
}

void FrameBuffer::Set(int u, int v, int color)
{
	//todo
	// pix[u*w + v] = color;
	pix[(h - 1 - v) * w + u] = color;
}

void FrameBuffer::SetGuarded(int u, int v, unsigned int color)
{
	// clip to window 
	if (u < 0 || v < 0 || u > h - 1 || v > w - 1)
		return;

	Set(u, v, color);
}

// load a tiff image to pixel buffer
void FrameBuffer::LoadTiff(char* fname)
{
	TIFF* in = TIFFOpen(fname, "r");
	if (in == nullptr)
	{
		cerr << fname << " could not be opened" << endl;
		return;
	}

	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height)
	{
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w * h];
		size(w, h);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0)
	{
		cerr << "failed to load " << fname << endl;
	}

	TIFFClose(in);
}

// save as tiff image
void FrameBuffer::SaveAsTiff(char* fname)
{
	TIFF* out = TIFFOpen(fname, "w");

	if (out == nullptr)
	{
		cerr << fname << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < (unsigned int)h; row++)
	{
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}

	TIFFClose(out);
}

int FrameBuffer::ClipToScreen(int& u0, int& v0, int& u1, int& v1)
{
	// Out of screen 
	if (u0 > w || v0 > h || u1 < 0 || v1 < 0)
		return 0;

	u0 = std::max(0, u0);
	v0 = std::max(0, v0);
	u1 = std::min(u1, w-1);
	v1 = std::min(v1, h-1);
	return 1;
}

int FrameBuffer::ClipToScreen(float& u0, float& v0, float& u1, float& v1)
{
	// Out of screen 
	if (u0 > w || v0 > h || u1 < 0 || v1 < 0)
		return 0;

	u0 = std::max(0.0f, u0);
	v0 = std::max(0.0f, v0);
	u1 = std::min(u1, static_cast<float>(w - 1));
	v1 = std::min(v1, static_cast<float>(h - 1));
	return 1;
}

void FrameBuffer::DrawSegment(V3 p1, V3 p2, V3 c1, V3 c2)
{
	V3 v2v1 = p2 - p1;
	int steps;
	if(v2v1[0]>v2v1[1])
	{
		steps = static_cast<int>(v2v1[0]);
	}
	else
	{
		steps = static_cast<int>(v2v1[1]);
	}

	int u = static_cast<int>(p1[0]);
	int v = static_cast<int>(p1[1]);
	for(int stepi = 0 ; stepi < steps; ++stepi)
	{
		float ratio = static_cast<float>(stepi) / static_cast<float>(steps - 1);
		V3 color = c1 * (1.0f - ratio) + c2 * ratio;

		SetGuarded(u + stepi * static_cast<int>(v2v1[0] / (steps - 1)),
			v + stepi * static_cast<int>(v2v1[1] / (steps - 1)),
			color.GetColor());
	}
}

void FrameBuffer::DrawRectangle(int u0, int v0, int u1, int v1, unsigned color)
{
	if (!ClipToScreen(u0, v0, u1, v1))
		return;

	for (int i = u0; i < u1; ++i)
	{
		for (int j = v0; j < v1; ++j)
		{
			SetGuarded(i, j, color);
		}
	}
}

void FrameBuffer::DrawCircle(int u0, int v0, int r, unsigned int color)
{
	// bounding box, then iterate the bounding box area
	int u1 = u0 - r, v1 = v0 - r, u2 = u0 + r, v2 = v0 + r;
	if (!ClipToScreen(u1, v1, u2, v2))
		return;

	for (int u = u1; u <= u2; ++u)
	{
		for (int v = v1; v <= v2; ++v)
		{
			if((u-u0)*(u - u0) + (v-v0)*(v-v0)<=r*r)
				SetGuarded(u, v, color);
		}
	}
}

void FrameBuffer::DrawEllipse(int u0, int v0, float r0, float r1, unsigned color)
{
	int u1 = u0 - r0, v1 = v0 - r1, u2 = u0 + r0, v2 = u0 + r1;
	if (!ClipToScreen(u1, v1, u2, v2))
		return;

	for (int u = u1; u <= u2; ++u)
	{
		for (int v = v1; v <= v2; ++v)
		{
			if ((u - u0)*(u - u0)/(r0*r0) + (v - v0)*(v - v0)/(r1*r1) <= 1.0f)
			{
				SetGuarded(u, v, color);
			}
		}
	}
}

void FrameBuffer::DrawPoint(int u, int v, unsigned color)
{
	SetGuarded(u, v, color);
}

void FrameBuffer::Draw3DPoint(PPC* camera, V3 p, unsigned color, int pointSize)
{
	V3 pp;
	if (!camera->Project(p, pp) || pp[0] < 0 || pp[1] < 0 || pp[0] >= w || pp[1] >= h)
		return;

	int u = static_cast<int>(pp[0]);
	int v = static_cast<int>(pp[1]);
	int halfPointSize = pointSize / 2;
	DrawRectangle(u - halfPointSize, v - halfPointSize, u + halfPointSize, v + halfPointSize, color);
}

void FrameBuffer::DrawTriangle(PPC* camera,V3 p1, V3 p2, V3 p3, unsigned color)
{
	V3 pp1, pp2, pp3;  // image space point coordinate
	if (!camera->Project(p1, pp1) || !camera->Project(p2, pp2) || !camera->Project(p3, pp3))
		return;

	auto DetectInside = [&](V3 p, V3 t1, V3 t2, V3 t3)
	{
		float x[3], y[3];
		x[1] = t1[0];
		x[2] = t2[0];
		y[1] = t1[1];
		y[2] = t2[1];

		float xCo = y[2] - y[1];
		float yCo = x[2] - x[1];
		float x1y2 = x[1]*y[2];
		float y1x2 = y[1]*x[2];
		float res1 = p[0] * xCo - p[1] * yCo - x1y2 + y1x2;
		float res2 = t3[0] * xCo - t3[1] * yCo - x1y2 + y1x2;

		return res1 * res2 >= 0.0f ? true : false;
	};

	AABB bbTri(pp1);
	bbTri.AddPoint(pp2);
	bbTri.AddPoint(pp3);
	ClipToScreen(bbTri.corners[0][0], bbTri.corners[0][1], bbTri.corners[1][0], bbTri.corners[1][1]);
	
	// Rasterize bbox 
	int left = static_cast<int>(bbTri.corners[0][0] + 0.5f), right = static_cast<int>(bbTri.corners[1][0] - 0.5f);
	int top = static_cast<int>(bbTri.corners[0][1] + 0.5f), bottom = static_cast<int>(bbTri.corners[1][1] - 0.5f);

	for(int i = top; i <= bottom; ++i)
	{
		for(int j = left; j <= right; ++j)
		{
			V3 p(j,i, 0.0f);
			bool s1 = DetectInside(p, pp1, pp2,pp3);
			bool s2 = DetectInside(p, pp2, pp3,pp1);
			bool s3 = DetectInside(p, pp3, pp1,pp2);
			if (s1 == true && s2 == true && s3 == true)
			{
				DrawPoint(j, i, color);
			}
		}
	}
}
