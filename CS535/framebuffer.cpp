#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include "scene.h"
#include "v3.h"
#include <iostream>

#include "tiffio.h"
#include <algorithm>

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

void FrameBuffer::SetGuarded(int u, int v, int color)
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
	u1 = std::min(u1, w);
	v1 = std::min(v1, h);
	return 1;
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