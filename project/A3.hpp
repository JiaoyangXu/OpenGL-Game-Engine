// Termm-Fall 2020

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "JointNode.hpp"
#include "Shadow.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <set>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

    // position
    double prev_posX;
    double prev_posY;
    double transition_factor;
    glm::mat4 model_translation;

    // mouse click
    bool mouseLeftClick;
    bool mouseMiddleClick;
    bool mouseRightClick;

    //options
    bool circle;
    bool z_buffer;
    bool backface;
    bool frontface;

    enum InteractionMode {position, joint};
    InteractionMode interactionMode;

    std::set<SceneNode*> selected_joints;

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderSceneGraph(const SceneNode &node, const ShaderProgram& shader, bool is_shadow);
	void renderArcCircle();

	void uploadTexture();
	void initShadowMap();
	void ConfigureShaderAndMatrices();

	void constructShadowMap();

	void initFloor();

    // picking
    void start_picking();
    void find_node(unsigned int id, SceneNode* node);

    // reset
    void reset_position();
    void reset_orientation();
    void reset_joints();
    void reset_all();

	glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLuint m_vbo_vertexUvs;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	GLint m_uvAttribLocation;
	GLint m_shadow_positionAttribLocation;
	ShaderProgram m_shader;

	ShaderProgram shadow_shader;

	ShaderProgram debugDepthQuad_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;

	Shadow m_shadow;

	unsigned int depthMapFBO;

	unsigned int planeVAO;

	unsigned int texture_id;
	unsigned int shadowMap_id;

	glm::mat4 lightSpaceMatrix;


    // picking
    bool do_picking;

};
