// Termm--Fall 2020

#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <vector>
#include <math.h>
using namespace glm;

#define PI 3.14159265

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.5, 0.7, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();

    initDefault();
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//---------------------------------------------------------------------------------------- Spring 2020
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

void A2::initViewing() {
    vec3 look_from = vec3(0.0f,0.0f,10.0f);
    vec3 look_at = vec3(0.0f, 0.0f, -1.0f);
    vec3 up = vec3(look_from.x, look_from.y + 1.0f, look_from.z);

    float x0 = look_from.x, y0 = look_from.y, z0 = look_from.z;

    mat4 T = mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            -x0, -y0, -z0, 1
            );

    vec3 vz = (look_at - look_from) / distance(look_at, look_from);
    vec3 vx = cross(up, vz) / distance( cross(up , vz), vec3(0,0,0));
    vec3 vy = cross(vz, vx);

    mat4 R = mat4(
            vx.x, vx.y, vx.z, 0,
            vy.x, vy.y, vy.z, 0,
            vz.x, vz.y, vz.z, 0,
            0,0,0,1
            );

    view = R * T;

   // cout << "view is " << view << endl;

}

void A2::initDefault() {

    // viewport
    viewport_x1 = 0.05f;
    viewport_y1 = 0.05f;
    viewport_height = 0.95;
    viewport_length = 0.95;
    viewport_x2 = viewport_x1 + viewport_length;
    viewport_y2 = viewport_y1 + viewport_height;

    // Interaction
    interactionMode = rotateModel;
    mouse_prev_x_pos = 0;
    mouse_prev_y_pos = 0;


    fov = 30.0f;

    far = 30.0f;
    near = 5.0f;

    model_matrix = mat4(1.0f);
    model_scale_matrix = mat4(1.0f);
    initViewing();

    initCubeModel();
    initModelFrame();
    initWorldFrame();

    update_projection();
}

void A2::update_projection() {
    float aspect = viewport_height / viewport_length;

    float theta = fov / 2;
    float cot = cos (theta * PI / 180) / sin (theta * PI / 180);

    projection_matrix = mat4(
            cot/aspect, 0, 0, 0,
            0, cot, 0, 0,
            0, 0, (far + near) / (far - near), 1,
            0, 0, (-2 * far * near) / (far - near), 0
    );

    //cout << "projection matrix is " << projection_matrix << endl;
}

void A2::startModelScale(double dis_x, double dis_y, double dis_z) {
    model_scale_matrix *= mat4(
            1 + dis_x, 0, 0, 0,
            0, 1 + dis_y, 0, 0,
            0, 0, 1 + dis_z, 0,
            0, 0, 0, 1
            );
}

void A2::startViewTranslation(double dis_x, double dis_y, double dis_z) {
    view *= inverse( mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            dis_x, dis_y, dis_z, 1
    ));
}

void A2::startModelTranslation(double dis_x, double dis_y, double dis_z) {
    model_matrix *= mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            dis_x, dis_y, dis_z, 1
            );
}

void A2::startViewRotation(char axis, double distance) {
    switch (axis) {
        case 'x':
            view *= inverse(mat4(
                    1,0,0,0,
                    0, cos(distance), sin(distance),0,
                    0, -sin(distance), cos(distance),0,
                    0, 0, 0, 1
            ));
            break;
        case 'y':
            view *= inverse( mat4(
                    cos(distance), 0, -sin(distance), 0,
                    0, 1, 0, 0,
                    sin(distance), 0, cos(distance), 0,
                    0, 0, 0, 1
            ));
            break;
        case 'z':
            view *= inverse( mat4(
                    cos(distance), sin(distance), 0, 0,
                    -sin(distance), cos(distance), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
            ));
            break;
        default:
            break;
    }
}

void A2::startModelRotation(char axis, double distance) {
    switch (axis) {
        case 'x':
            model_matrix *= mat4(
                    1,0,0,0,
                    0, cos(distance), sin(distance),0,
                    0, -sin(distance), cos(distance),0,
                    0, 0, 0, 1
                    );
            break;
        case 'y':
            model_matrix *= mat4(
                    cos(distance), 0, -sin(distance), 0,
                    0, 1, 0, 0,
                    sin(distance), 0, cos(distance), 0,
                    0, 0, 0, 1
                    );
            break;
        case 'z':
            model_matrix *= mat4(
                    cos(distance), sin(distance), 0, 0,
                    -sin(distance), cos(distance), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1
                    );
            break;
        default:
            break;
    }
}

void A2::startPerspective(char identifier, double distance) {
    if (identifier == 'l') {
        if (fov > 5 && fov < 160)
            fov += distance;
    }

    if (identifier == 'm') {
        near += distance;
    }

    if (identifier == 'r') {
        far += distance;
    }
}

void A2::initModelFrame() {
    model_frame_vecs.clear();
    model_frame_vecs.push_back(make_pair(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)) );
    model_frame_vecs.push_back(make_pair(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)) );
    model_frame_vecs.push_back(make_pair(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)) );
}

void A2::initWorldFrame() {
    world_frame_vecs.clear();
    world_frame_vecs.push_back(make_pair(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)) );
    world_frame_vecs.push_back(make_pair(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)) );
    world_frame_vecs.push_back(make_pair(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f)) );
}

void A2::initCubeModel() {

    cube_model_vecs.clear();

    vec3 back_left_bottom = vec3(-1.0f, -1.0f, -1.0f);
    vec3 back_right_bottom = vec3(1.0f, -1.0f, -1.0f);

    vec3 back_left_top = vec3(-1.0f, 1.0f, -1.0f);
    vec3 back_right_top = vec3(1.0f, 1.0f, -1.0f);

    vec3 front_left_bottom = vec3(-1.0f, -1.0f, 1.0f);
    vec3 front_right_bottom = vec3(1.0f, -1.0f, 1.0f);
    vec3 front_left_top = vec3(-1.0f, 1.0f, 1.0f);
    vec3 front_right_top = vec3(1.0f, 1.0f, 1.0f);

    // back bottom
    cube_model_vecs.push_back(make_pair(back_left_bottom, back_right_bottom));

    // back top
    cube_model_vecs.push_back(make_pair(back_left_top, back_right_top));

    // back left
    cube_model_vecs.push_back(make_pair(back_left_bottom, back_left_top));

    // back right
    cube_model_vecs.push_back(make_pair(back_right_bottom, back_right_top));

    // left bottom
    cube_model_vecs.push_back(make_pair(back_left_bottom, front_left_bottom));

    // left top
    cube_model_vecs.push_back(make_pair(back_left_top, front_left_top));

    // front left
    cube_model_vecs.push_back(make_pair(front_left_bottom, front_left_top));

    // front bottom
    cube_model_vecs.push_back(make_pair(front_left_bottom, front_right_bottom));

    // front top
    cube_model_vecs.push_back(make_pair(front_left_top, front_right_top));

    // front right
    cube_model_vecs.push_back(make_pair(front_right_bottom, front_right_top));

    // right bottom
    cube_model_vecs.push_back(make_pair(front_right_bottom, back_right_bottom));

    // right top
    cube_model_vecs.push_back(make_pair(front_right_top, back_right_top));
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & V0,   // Line Start (NDC coordinate)
		const glm::vec2 & V1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = V0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = V1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

    // draw viewport
    setLineColour(vec3(1, (float) 192/255 , (float) 203/255 ));
    // bottom
    drawLine( vec2(0 - viewport_length, 0 - viewport_height) , vec2(0+viewport_length, 0 - viewport_height) );
    // top
    drawLine( vec2(0 - viewport_length, 0 + viewport_height) , vec2(0+viewport_length, 0 + viewport_height) );
    // left
    drawLine( vec2(0 - viewport_length, 0 - viewport_height) , vec2(0-viewport_length, 0 + viewport_height) );
    // right
    drawLine( vec2(0 + viewport_length, 0 - viewport_height) , vec2(0+viewport_length, 0 + viewport_height) );


    // draw cube
    for (int i = 0; i < cube_model_vecs.size() ; ++i) {
        setLineColour(vec3(1,1,1));
        pair<vec3, vec3> line = cube_model_vecs[i];
        vec3 p1 = line.first;
        vec3 p2 = line.second;

        vec4 start_point = projection_matrix * view * model_matrix * model_scale_matrix * vec4(p1, 1.0f);
        vec4 end_point = projection_matrix * view * model_matrix * model_scale_matrix * vec4(p2, 1.0f);

        drawLine(vec2(start_point.x / start_point.w, start_point.y / start_point.w),
                 vec2(end_point.x / end_point.w, end_point.y / end_point.w));
    }


    // draw the model frame
    for (int i = 0; i < model_frame_vecs.size(); ++i) {
        if (i == 0) setLineColour(vec3(1,0,0));
        else if (i == 1) setLineColour(vec3(0,1,0));
        else setLineColour(vec3(0,0,1));

        pair<vec3, vec3> line = model_frame_vecs[i];
        vec3 p1 = line.first;
        vec3 p2 = line.second;
        vec4 start_point = projection_matrix * view * model_matrix * vec4(p1, 1.0f);
        vec4 end_point = projection_matrix * view * model_matrix * vec4(p2, 1.0f);

      //  cout << "model frame is " << end_point << endl;

        drawLine(vec2(start_point.x / start_point.w, start_point.y / start_point.w ),
                 vec2(end_point.x / end_point.w, end_point.y / end_point.w));
    }

    // draw the world frame
    for (int i = 0; i < world_frame_vecs.size(); ++i) {
        if (i == 0) setLineColour(vec3( 0 , 1, 1));
        else if (i == 1) setLineColour(vec3(0.5f,0,0.5f));
        else setLineColour(vec3(1,1,0));

        pair<vec3, vec3> line = world_frame_vecs[i];
        vec3 p1 = line.first;
        vec3 p2 = line.second;
        vec4 start_point = projection_matrix * view  * vec4(p1, 1.0f);
        vec4 end_point = projection_matrix * view  * vec4(p2, 1.0f);

        drawLine(vec2(start_point.x / start_point.w, start_point.y / start_point.w ),
                 vec2(end_point.x / end_point.w, end_point.y / end_point.w));
    }

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


        ImGui::RadioButton("Rotate View      (o)", (int*) &interactionMode, rotateView);
        ImGui::RadioButton("Translate View   (e)", (int*) &interactionMode, translateView);
        ImGui::RadioButton("Perspective      (p)", (int*) &interactionMode, perspective);
        ImGui::RadioButton("Rotate Model     (r)", (int*) &interactionMode, rotateModel);
        ImGui::RadioButton("Translate Model  (t)", (int*) &interactionMode, translateModel);
        ImGui::RadioButton("Scale Model      (s)", (int*) &interactionMode, scaleModel);
        ImGui::RadioButton("Viewport         (v)", (int*) &interactionMode, viewPort);


        if( ImGui::Button( "Reset" ) ) {
            initDefault();
        }


		// Add more gui elements here here ...


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
        ImGui::Text( "Near: %f", near );
        ImGui::Text( "Far: %f", far);


    ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);
    double distance_x = xPos - mouse_prev_x_pos;
    distance_x *= 0.001;
    double distance_y = yPos - mouse_prev_y_pos;

//    cout << "current mode is " << interactionMode << endl;

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        switch (interactionMode) {
            case rotateView:
                if (ImGui::IsMouseDown(0)) {
                    startViewRotation('x', distance_x);
                    //cout << "mouse click left" << endl;
                } else if (ImGui::IsMouseDown(1)) {
                    startViewRotation('z', distance_x);
                    //cout << "right click " << endl;
                } else if (ImGui::IsMouseDown(2)) {
                    startViewRotation('y', distance_x);
                    //cout << "middle click" << endl;
                }
                break;
            case translateView:
                if (ImGui::IsMouseDown(0)) {
                    startViewTranslation(distance_x, 0, 0);
                    //cout << "mouse click left" << endl;
                } else if (ImGui::IsMouseDown(1)) {
                    startViewTranslation(0, 0, distance_x);
                    //cout << "right click " << endl;
                } else if (ImGui::IsMouseDown(2)) {
                    startViewTranslation(0,distance_x,0);
                    //cout << "middle click" << endl;
                }

                break;
            case perspective:
                if (ImGui::IsMouseDown(0)) {
                    startPerspective('l', distance_x);
                    //cout << "mouse click left" << endl;
                } else if (ImGui::IsMouseDown(1)) {
                    startPerspective('m', distance_x);
                    //cout << "right click " << endl;
                } else if (ImGui::IsMouseDown(2)) {
                    startPerspective('r', distance_x);
                    //cout << "middle click" << endl;
                }
                break;
            case rotateModel:
                if (ImGui::IsMouseDown(0)) {
                    startModelRotation('x', distance_x);
                    //cout << "mouse click left" << endl;
                } else if (ImGui::IsMouseDown(1)) {
                    startModelRotation('z', distance_x);
                    //cout << "right click " << endl;
                } else if (ImGui::IsMouseDown(2)) {
                    startModelRotation('y', distance_x);
                    //cout << "middle click" << endl;
                }
                break;
            case translateModel:
                if (ImGui::IsMouseDown(0)) {
                    startModelTranslation(distance_x, 0, 0);
                    //cout << "mouse click left" << endl;
                } else if (ImGui::IsMouseDown(1)) {
                    startModelTranslation(0, 0, distance_x);
                    //cout << "right click " << endl;
                } else if (ImGui::IsMouseDown(2)) {
                    startModelTranslation(0,distance_x,0);
                    //cout << "middle click" << endl;
                }

                break;

            case scaleModel:
                if (ImGui::IsMouseDown(0)) {
                    startModelScale(distance_x, 0, 0);
                    //cout << "mouse click left" << endl;
                } else if (ImGui::IsMouseDown(1)) {
                    startModelScale(0, 0, distance_x);
                    //cout << "right click " << endl;
                } else if (ImGui::IsMouseDown(2)) {
                    startModelScale(0,distance_x,0);
                    //cout << "middle click" << endl;
                }


                break;
            case viewPort:

                break;
            default:
                break;
        }
    }
    mouse_prev_x_pos = xPos;
    mouse_prev_y_pos = yPos;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if (action == GLFW_PRESS) {
        eventHandled = true;
        if(key == GLFW_KEY_O) {
            interactionMode = rotateView;
        } else if (key == GLFW_KEY_E) {
            interactionMode = translateView;
        } else if (key == GLFW_KEY_P) {
            interactionMode = perspective;
        } else if (key == GLFW_KEY_R) {
            interactionMode = rotateModel;
        } else if (key == GLFW_KEY_T) {
            interactionMode = translateModel;
        } else if (key == GLFW_KEY_S) {
            interactionMode = scaleModel;
        } else if (key == GLFW_KEY_V) {
            interactionMode = viewPort;
        } else if (key == GLFW_KEY_A) {
            initDefault();
        } else if (key == GLFW_KEY_Q) {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        } else {
            eventHandled = false;
        }
    }

	return eventHandled;
}
