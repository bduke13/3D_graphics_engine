#include "AppWindow.h"
#include <Windows.h>
#include "Vector3D.h"
#include "Matrix4x4.h"
#include "InputSystem.h"
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <windows.h>

// Struct with 3 member variables: position of each vertex, first color, and secondary color (which can be used to create a color animation, NOT USED IN IMPLEMENTATION)
struct vertex
{
	Vector3D position;
	Vector3D color;
	Vector3D color1;
};


__declspec(align(16))
// Struct with member variables of type Matrix4x4 that define the viewing window
struct constant
{
	Matrix4x4 m_world;
	Matrix4x4 m_view;
	Matrix4x4 m_proj;
	unsigned int m_time;
};


AppWindow::AppWindow()
{
}

void AppWindow::updateQuadPosition()
{
	constant cc;
	cc.m_time = ::GetTickCount64();

	m_delta_pos += m_delta_time / 10.0f;
	if (m_delta_pos > 1.0f)
		m_delta_pos = 0;


	Matrix4x4 temp;

	// Set scale of window
	cc.m_world.setScale(Vector3D(m_scale_shape, m_scale_shape, m_scale_shape));

	temp.setIdentity();
	// Set initial oritentation of shape in radians in about the z-axis
	temp.setRotationZ(-1.58f);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rot_y);
	cc.m_world *= temp;

	temp.setIdentity();
	temp.setRotationX(m_rot_x);
	cc.m_world *= temp;


	cc.m_view.setIdentity();
	cc.m_proj.setOrthoLH
	(
		(this->getClientWindowRect().right - this->getClientWindowRect().left) / 300.0f,
		(this->getClientWindowRect().bottom - this->getClientWindowRect().top) / 300.0f,
		-4.0f,
		4.0f
	);


	m_cb->update(GraphicsEngine::get()->getImmediateDeviceContext(), &cc);
}


AppWindow::~AppWindow()
{
}


// Read CSV file 
class objectInfo {
	// Class that contains all relevant information on an object
public:
	// Method that splits string on commas
	std::vector<float> parseLineFloat(std::string line) {
		std::vector<float> result;
		std::stringstream s_stream(line); // create string stream from the string
		while (s_stream.good()) {
			std::string substr;
			getline(s_stream, substr, ','); // get first string delimited by comma
			result.push_back(std::stof(substr));
		}
		return result;
	}

	std::vector<float> parseLineInt(std::string line) {
		std::vector<float> result;
		std::stringstream s_stream(line); // Create string stream from the string
		while (s_stream.good()) {
			std::string substr;
			getline(s_stream, substr, ','); // Get first string delimited by comma
			result.push_back(std::stoi(substr));
		}
		return result;
	}
	// Constructor
	objectInfo() {

		std::string path = "C:\\Users\\bek.dukenbaev\\Downloads\\object.txt";
		
		// Prompt the filepath
		std::cout << "Enter full filepath of the object file" << std::endl;
		std::cin >> path;
		
		std::ifstream myinfile(path);
		std::string line;
		std::vector<float> splitLineFloat;
		std::vector<float> splitLineInt;
		//get first line
		getline(myinfile, line);
		splitLineInt = parseLineInt(line);
		//get number of verticies and faces
		int numVerticies = (int)splitLineInt[0];
		int numFaces = (int)splitLineInt[1];
		//get all verticies from file
		for (int i = 0; i < numVerticies; i++) {
			getline(myinfile, line);
			splitLineFloat = parseLineFloat(line);
			verticies.push_back(splitLineFloat);
		}
		//get all faces in file
		for (int i = 0; i < numFaces; i++) {
			getline(myinfile, line);
			splitLineInt = parseLineInt(line);
			for (int j = 0; j < splitLineInt.size(); j++) {
				faces.push_back(splitLineInt[j]);
			}
			
		}
		
		//set vector3D
		for (int i = 0; i < verticies.size(); i++) {
			Vector3D newVec = Vector3D(verticies[i][1], verticies[i][2], verticies[i][3]);
			verticies3d.push_back({ newVec, Vector3D(0, 0, 1.5f), Vector3D(0, 0, 1.5f) });
		}
		//fet length of verticies and faces lists
		sizeVertices = verticies3d.size();
		sizeFaces = faces.size();

		//define arrays to shape size
		vertex_list = new vertex[sizeVertices];
		index_list = new unsigned int[sizeFaces];

		//fill arrays from vectors
		for (int i = 0; i < sizeVertices; i++) {
			vertex_list[i] = verticies3d[i];
		}
		for (int i = 0; i < sizeFaces; i++) {
			index_list[i] = faces[i];
		}
	}

	//public variables
	vertex* vertex_list;
	unsigned int* index_list;
	int sizeVertices;
	int sizeFaces;

private:
	//private variables
	std::vector<std::vector<float>> verticies;
	std::vector<vertex> verticies3d;
	std::vector<int> faces;
};

// 
void AppWindow::onCreate()
{	
	Window::onCreate();
	InputSystem::get()->addListener(this);
	GraphicsEngine::get()->init();

	m_swap_chain = GraphicsEngine::get()->createSwapChain();

	RECT rc = this->getClientWindowRect();
	m_swap_chain->init(this->m_hwnd, rc.right - rc.left, rc.bottom - rc.top);

	//create my object from file
	objectInfo myObj;
	
	m_vb = GraphicsEngine::get()->createVertexBuffer();
	UINT size_list = myObj.sizeVertices;

	m_ib = GraphicsEngine::get()->createIndexBuffer();
	UINT size_index_list = myObj.sizeFaces;

	m_ib->load(myObj.index_list, size_index_list);


	// The rest of these functions initialize the vertex and pixel shaders
	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	GraphicsEngine::get()->compileVertexShader(L"VertexShader.hlsl", "vsmain", &shader_byte_code, &size_shader);

	m_vs = GraphicsEngine::get()->createVertexShader(shader_byte_code, size_shader);
	m_vb->load(myObj.vertex_list, sizeof(vertex), size_list, shader_byte_code, size_shader);

	GraphicsEngine::get()->releaseCompiledShader();

	GraphicsEngine::get()->compilePixelShader(L"PixelShader.hlsl", "psmain", &shader_byte_code, &size_shader);
	m_ps = GraphicsEngine::get()->createPixelShader(shader_byte_code, size_shader);
	GraphicsEngine::get()->releaseCompiledShader();

	constant cc;
	cc.m_time = 0;

	m_cb = GraphicsEngine::get()->createConstantBuffer();
	m_cb->load(&cc, sizeof(constant));

}

void AppWindow::onUpdate()
{
	Window::onUpdate();

	InputSystem::get()->update();

	// Set background color 
	GraphicsEngine::get()->getImmediateDeviceContext()->clearRenderTargetColor(this->m_swap_chain,
		0, 0.0f, 0.0f, 1);
	// Set viewport
	RECT rc = this->getClientWindowRect();
	GraphicsEngine::get()->getImmediateDeviceContext()->setViewportSize(rc.right - rc.left, rc.bottom - rc.top);

	updateQuadPosition();

	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_vs, m_cb);
	GraphicsEngine::get()->getImmediateDeviceContext()->setConstantBuffer(m_ps, m_cb);

	// Allow the default shader in the graphics pipeline to draw
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexShader(m_vs);
	GraphicsEngine::get()->getImmediateDeviceContext()->setPixelShader(m_ps);

	// Set the vertices of the shape to draw
	GraphicsEngine::get()->getImmediateDeviceContext()->setVertexBuffer(m_vb);
	// Set the indeces of the shape to draw
	GraphicsEngine::get()->getImmediateDeviceContext()->setIndexBuffer(m_ib);
	
	// Draw the shape
	GraphicsEngine::get()->getImmediateDeviceContext()->drawIndexedTriangleList(m_ib->getSizeIndexList(), 0, 0);
	m_swap_chain->present(true);
	m_old_delta = m_new_delta;
	m_new_delta = ::GetTickCount64();

	m_delta_time = (m_old_delta) ? ((m_new_delta - m_old_delta) / 1000.0f) : 0;
}

void AppWindow::onDestroy()
{
	Window::onDestroy();
	m_vb->release();
	m_ib->release();
	m_cb->release();
	m_swap_chain->release();
	m_vs->release();
	m_ps->release();
	GraphicsEngine::get()->release();
}

void AppWindow::onFocus()
{
	InputSystem::get()->addListener(this);
}

void AppWindow::onKillFocus()
{
	InputSystem::get()->removeListener(this);
}

// WASD key inputs
void AppWindow::onKeyDown(int key)
{
	if (key == 'W')
	{
		m_rot_x += 3.14f * m_delta_time;
	}
	else if (key == 'S')
	{
		m_rot_x -= 3.14f * m_delta_time;
	}
	else if (key == 'A')
	{
		m_rot_y += 3.14f * m_delta_time;
	}
	else if (key == 'D')
	{
		m_rot_y -= 3.14f * m_delta_time;
	}
}

// If blank, then not used in the final project
void AppWindow::onKeyUp(int key)
{

}


// Mouse input
bool click = false;
void AppWindow::onMouseMove(const Point& delta_mouse_pos)
{	
	// If left mouse button is held
	if (click == true) {
		m_rot_x -= delta_mouse_pos.m_y * m_delta_time;
		m_rot_y -= delta_mouse_pos.m_x * m_delta_time;
	}
}

void AppWindow::onLeftMouseDown(const Point& mouse_pos)
{
	click = true;
}

void AppWindow::onLeftMouseUp(const Point& mouse_pos)
{
	click = false;
}

void AppWindow::onRightMouseDown(const Point& mouse_pos)
{
}

void AppWindow::onRightMouseUp(const Point& mouse_pos)
{
}