// Termm--Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <utility>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

    // initialization for values
    void initCubeModel();
    void initModelFrame();
    void initWorldFrame();
    void initDefault();
    void initViewing();

    // update
    void update_projection();


    // rotation
    void startModelRotation(char axis, double distance);
    void startViewRotation(char axis, double distance);

    // translation
    void startModelTranslation(double dis_x, double dis_y, double dis_z);
    void startViewTranslation(double dis_x, double dis_y, double dis_z);

    // perspective
    void startPerspective(char identifier, double distance);

    // scale
    void startModelScale(double dis_x, double dis_y, double dis_z);

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);

    enum RotationOption {x, y, z};

    enum InteractionMode {rotateView, translateView, perspective, rotateModel, translateModel, scaleModel, viewPort};

    // cube, and frames
    std::vector< std::pair<glm::vec3, glm::vec3> > cube_model_vecs;
    std::vector< std::pair<glm::vec3, glm::vec3> > model_frame_vecs;
    std::vector< std::pair<glm::vec3, glm::vec3> > world_frame_vecs;


    ShaderProgram m_shader;

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;

	glm::vec3 m_currentLineColour;

    // model operation

    // rotation
    float m_shape_rotation;
    RotationOption rotationOption;
    InteractionMode interactionMode;

    // mouse event
    double mouse_prev_x_pos;
    double mouse_prev_y_pos;

    // viewport
    float viewport_x1;
    float viewport_x2;
    float viewport_y1;
    float viewport_y2;

    float viewport_length;
    float viewport_height;


    float fov;

    float far;
    float near;


    // transformation matrix
    glm::mat4 view;
    glm::mat4 model_matrix;
    glm::mat4 model_scale_matrix;
    glm::mat4 projection_matrix;

};
