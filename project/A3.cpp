// Termm-Fall 2020

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "include/stb/stb_image.h"
using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_uvAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vbo_vertexUvs(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0),
      transition_factor(0.01),
      model_translation(1.0f),
      mouseLeftClick(false),
      mouseMiddleClick(false),
      mouseRightClick(false),
      interactionMode(position),
      do_picking(false),
      circle(false),
      backface(false),
      frontface(false),
      z_buffer(true)
{
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

void A3::uploadTexture()
{

	unsigned int texture;
	glGenTextures(1, &texture);	
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char *data = stbi_load("Assets/Textures/container.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		std::cout << "Receieve data" << std::endl;
    		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
    		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.0, 1.0, 0.0, 1.0);

	createShaderProgram();
	uploadTexture();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});

	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();

	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		// Enable the vextex shader attribute location for "uv" when rendering 
		// m_uvAttribLocation = m_shader.getAttribLocation("uv");
		// glEnableVertexAttribArray(m_uvAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex texture uv data
	{
		glGenBuffers(1, &m_vbo_vertexUvs);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUvs);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUvBytes(),
				meshConsolidator.getVertexUvDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexUvs" into the
	// "uv" vertex attribute location for any bound vertex shader program.
	// glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUvs);
	// glVertexAttribPointer(m_uvAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(10.0f, 10.0f, 10.0f);
	m_light.rgbIntensity = vec3(1.0f); // light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;


		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.25f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

void A3::reset_all() {
    reset_orientation();
    reset_joints();
    reset_orientation();
}

void A3::reset_position() {
    model_translation = glm::mat4();
}

void A3::reset_orientation() {}

void A3::reset_joints() {}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

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

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Application")) {
                if (ImGui::MenuItem("Reset Position (I)")) {

                }
                if (ImGui::MenuItem("Reset Orientation (O)")) {

                }
                if (ImGui::MenuItem("Reset Joints (S)")) {

                }
                if (ImGui::MenuItem("Quit (Q)")) {
                    glfwSetWindowShouldClose(m_window, GL_TRUE);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Options")) {
                if (ImGui::MenuItem("Circle (C)")) {
                    circle = !circle;
                }
                if (ImGui::MenuItem("Z-buffer (Z)")) {
                    z_buffer = !z_buffer;
                }
                if (ImGui::MenuItem("Backface culling (B)")) {
                    backface = !backface;
                }
                if (ImGui::MenuItem("Frontface culling (F)")) {
                    frontface = !frontface;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

		// Add more gui elements here here ...
        ImGui::RadioButton("Position/Orientation     (P)", (int*) &interactionMode, position);
        ImGui::RadioButton("Joint   (J)", (int*) &interactionMode, joint);


		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
    if (z_buffer) {
        glEnable( GL_DEPTH_TEST );
    }
    if (frontface && backface) {
        glEnable( GL_CULL_FACE );
        glCullFace(GL_FRONT_AND_BACK);
    } else if (frontface) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    } else if (backface) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }


	renderSceneGraph(*m_rootNode);

    if (z_buffer) {
        glDisable( GL_DEPTH_TEST );
    }
    if (frontface || backface) {
        glDisable(GL_CULL_FACE);
    }
    if (circle) {
        renderArcCircle();
    }
}

//----------------------------------------------------------------------------------------
void A3::renderSceneGraph(const SceneNode & root) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

    glm::mat4 prev_trans = m_rootNode->trans;

    m_rootNode->trans = model_translation * m_rootNode->trans;

    std::vector<glm::mat4> stack;

    m_rootNode->render(m_shader, m_batchInfoMap, m_view, stack);

    m_rootNode->set_transform(prev_trans);

//
//	for ( SceneNode * node : root.children) {
//
//		if (node->m_nodeType != NodeType::GeometryNode)
//			continue;
//
//        //cout << "model translation is " << model_translation << endl;
//
//        //node->trans = model_translation ;
//
//        //cout << "model translation is " << node->trans << endl;
//
//
//        const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);
//
//		updateShaderUniforms(m_shader, *geometryNode, m_view * model_translation );
//
//
//		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
//		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
//
//		//-- Now render the mesh:
//		m_shader.enable();
//		glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
//		m_shader.disable();
//	}

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
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
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {


	bool eventHandled(false);

    double xChange = xPos - prev_posX;
    double yChange = yPos - prev_posY;

    //cout << "xChange " << xChange;

    if (interactionMode == position) {
        if (mouseLeftClick) {
            model_translation = translate(model_translation, vec3(xChange * transition_factor, -yChange * transition_factor, 0));
        }
        if (mouseMiddleClick) {
            model_translation = translate(model_translation, vec3(0,0,yChange * transition_factor));
        }
    }




	// Fill in with event handling code...

    prev_posX = xPos;
    prev_posY = yPos;

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);


    if (!ImGui::IsMouseHoveringAnyWindow()) {
        if (actions == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                mouseLeftClick = true;
                if (interactionMode == joint) {
                    start_picking();
                }
            }
            if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                mouseMiddleClick = true;
            }
        }

        if (actions == GLFW_RELEASE) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                mouseLeftClick = false;
            }
            if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
                mouseMiddleClick = false;
            }
        }
    }

	// Fill in with event handling code...

	return eventHandled;
}

void A3::find_node(unsigned int id, SceneNode* node) {
    if (node == nullptr) return;
    if (node->m_nodeId == id) {

        if (node->isSelected) {
            node->isSelected = false;
            if (node->parent->m_nodeType == NodeType::JointNode) {
                selected_joints.erase(node->parent);
            }
        } else {
            node->isSelected = true;
            if (node->parent->m_nodeType == NodeType::JointNode) {
                selected_joints.insert( node->parent);
            }
        }
        return;
    }

    for (auto child : node->children) {
        find_node(id, child);
    }

}

void A3::start_picking() {
    double xpos, ypos;
    glfwGetCursorPos( m_window, &xpos, &ypos );

    do_picking = true;
    m_rootNode->do_picking = true;
    uploadCommonSceneUniforms();
    glClearColor(1.0, 1.0, 1.0, 1.0 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glClearColor(0.35, 0.35, 0.35, 1.0);

    draw();

    // I don't know if these are really necessary anymore.
    // glFlush();
    // glFinish();

    CHECK_GL_ERRORS;

    // Ugly -- FB coordinates might be different than Window coordinates
    // (e.g., on a retina display).  Must compensate.
    xpos *= double(m_framebufferWidth) / double(m_windowWidth);
    // WTF, don't know why I have to measure y relative to the bottom of
    // the window in this case.
    ypos = m_windowHeight - ypos;
    ypos *= double(m_framebufferHeight) / double(m_windowHeight);

    GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
    // A bit ugly -- don't want to swap the just-drawn false colours
    // to the screen, so read from the back buffer.
    glReadBuffer( GL_BACK );
    // Actually read the pixel at the mouse location.
    glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
    CHECK_GL_ERRORS;

    // Reassemble the object ID.
    unsigned int what = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
    find_node(what, m_rootNode.get());


    do_picking = false;
    m_rootNode->do_picking = false;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
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
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}
