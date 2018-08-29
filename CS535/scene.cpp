#include "scene.h"
#include "v3.h"
#include "m33.h"
#include <stdlib.h>
#include <algorithm>
#include "MathTool.h"

Scene *scene;

#include <fstream>

#include <iostream>

Scene::Scene() {

	gui = new GUI();
	gui->show();

	int u0 = 20;
	int v0 = 20;
	int w = 640;
	int h = 480;

	fb = new FrameBuffer(u0, v0, w, h);
	fb->label("SW Framebuffer");
	fb->show();


	gui->uiw->position(u0, v0 + fb->h + 60);

	Render();

}

void Scene::Render() 
{
	fb->SetBGR(0xFFFFFFFF);
	fb->redraw();
}

bool Scene::DBGFramebuffer()
{
	int u0 = 50, v0 = 50, u1 = 300, v1 = 300;
	int r = 30;
	int stepN = 100;

	// for (int stepi = 0; stepi < stepN; ++stepi)
	// {
	// 	fb->SetBGR(0xFFFFFFFF);
	// 	fb->DrawRectangle(u0 + stepi, v0, u1+stepi, v1, 0xFF00FFFF);
	// 	fb->DrawCircle(u0+stepi, v0, r, 0xFFFF0000);
	// 	fb->redraw();
	// 	Fl::check();
	// }

	fb->DrawSegment(V3(50.0f, 50.0f, 0.0f), V3(300.0f, 300.0f, 0.0f),0xFF0000FF);

	std::cerr << "DBGFramebuffer passed!\n";
	return true;
}

bool Scene::DBGV3()
{
	V3 v1(1.0f), v2(0.5f),v3(3.0f,0.0f,0.0f);
	v1[0] = 0.0f;

	cerr << v1 << v1 * v2 << endl << v1 - v2 << v1.cross(v2)<<v1*3.0f<<(v1*3.0f)/3.0f;
	cerr << v1.Length() << endl << v3.Normalize();
	if (!FloatEqual(v1[0], 0.0f)
		|| !FloatEqual(v1[2], 1.0)
		|| !FloatEqual(v1*v2, 1.0f)
		|| !FloatEqual((v1-v2)[1],0.5f)
		|| v1.cross(v2)!=V3(0.0f,0.5f,-0.5f)
		|| v1*3.0f != V3(0.0f,3.0f,3.0f)
		|| (v1*3.0f) / 3.0f != v1
		|| v3.Normalize() != V3(1.0f,0.0f,0.0f)
		|| v3.Normalize().Length() != 1.0f)
	{
		cerr << "DBGV3 error\n";
		return false;
	}

	cerr << "DBGV3 passed\n";
	return true;
}

bool Scene::DBGM3()
{
	M33 m0, m1, m2, m3;
	m0[0] = V3(1.0f, 0.0f, 0.0f);
	m0[1] = V3(0.0f, 1.0f, 0.0f);
	m0[2] = V3(0.0f, 0.0f, 1.0f);
	V3 v(0.5f);
	m1[0] = V3(0.5f, 0.0f, 0.0f);
	m1[1] = V3(0.0f, 0.5f, 0.0f);
	m1[2] = V3(0.0f, 0.0f, 0.5f);

	m2[0] = V3(0.0f, 0.5f, 0.0f);
	m2[1] = V3(0.0f, 0.0f, 0.1f);
	m2[2] = V3(0.0f, 0.0f, 0.0f);

	m3[0] = V3(0.0f, 0.0f, 0.0f);
	m3[1] = V3(0.5f, 0.0f, 0.0f);
	m3[2] = V3(0.0f, 0.1f, 0.0f);

	cerr << m0 << m0 * v << m0 * m1 << m0.Det() << endl;
	cerr << m0 / 2.0f << m1.Inverse() << m2 << m2.Transpose();

	if(m0[0] != V3(1.0f, 0.0f, 0.0f) 
		|| m0*v !=v
		|| m0*m1 != m1
		|| !FloatEqual(m0.Det(), 1.0f)
		|| m0 / 2.0f != m1
		|| m0.Inverse() != m0
		|| m1.Inverse() != m1*4.0f
		|| m2.Transpose() != m3
		|| m0.Inverse() != m0)
	{
		cerr << "DBGM3 error\n";
		return false;
	}

	cerr << "DBGM3 passed \n";
	return true;
}

void Scene::DBG() {
	
	// cerr << "INFO: pressed DBG" << endl;
	cerr << "Begin DBG\n";
	if (DBGV3() && DBGM3() && DBGFramebuffer())
		cerr << "All pased! \n";
	else
		cerr << "Not pass!\n";
	
	fb->redraw();
}
