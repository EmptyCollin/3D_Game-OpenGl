#include <iostream>
#include <time.h>
#include <sstream>

#include "game.h"
#include "bin/path_config.h"

namespace game {

	// Some configuration constants
	// They are written here as global variables, but ideally they should be loaded from a configuration file

	// Main window settings
	const std::string window_title_g = "Matrix Hell";
	const unsigned int window_width_g = 800;
	const unsigned int window_height_g = 600;
	const bool window_full_screen_g = false;

	// Viewport and camera settings
	float camera_near_clip_distance_g = 0.01;
	float camera_far_clip_distance_g = 1000.0;
	float camera_fov_g = 20.0; // Field-of-view of camera
	const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
	glm::vec3 camera_position_g(0.0, 0.0, 2.0);
	glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
	glm::vec3 camera_up_g(0.0, 1.0, 0.0);

	// Materials 
	const std::string material_directory_g = MATERIAL_DIRECTORY;


	Game::Game(void) {

		// Don't do work in the constructor, leave it for the Init() function
	}


	void Game::Init(void) {

		// Run all initialization steps
		InitWindow();
		InitView();
		InitEventHandlers();

		// Set variables
		animating_ = true;
	}


	void Game::InitWindow(void) {

		// Initialize the window management library (GLFW)
		if (!glfwInit()) {
			throw(GameException(std::string("Could not initialize the GLFW library")));
		}

		// Create a window and its OpenGL context
		if (window_full_screen_g) {
			window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
		}
		else {
			window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
		}
		if (!window_) {
			glfwTerminate();
			throw(GameException(std::string("Could not create window")));
		}

		// Make the window's context the current one
		glfwMakeContextCurrent(window_);

		// Initialize the GLEW library to access OpenGL extensions
		// Need to do it after initializing an OpenGL context
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) {
			throw(GameException(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
		}
	}


	void Game::InitView(void) {

		// Set up z-buffer
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);


	}


	void Game::InitEventHandlers(void) {

		// Set event callbacks
		glfwSetKeyCallback(window_, KeyCallback);
		glfwSetFramebufferSizeCallback(window_, ResizeCallback);

		// Set pointer to game object, so that callbacks can access it
		glfwSetWindowUserPointer(window_, (void *)this);
	}


	void Game::SetupResources(void) {


		// Create a torus
		resman_.CreateTorus("TorusMesh");

		resman_.Create2Dsquare("2DSquare");

		// Load material to be applied to torus
		std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/three-term_shiny_blue");
		resman_.LoadResource(Material, "ShinyBlueMaterial", filename.c_str());

		// Load texture to be applied to the cube
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/checker.png");
		resman_.LoadResource(Texture, "Checker", filename.c_str());

		// Load texture to be applied to the turret
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/camouflage.png");
		resman_.LoadResource(Texture, "Camouflage", filename.c_str());
		// Load texture to be applied to the turret
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/white.png");
		resman_.LoadResource(Texture, "White", filename.c_str());


		// Load material to be applied to the cube
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/textured_material");
		resman_.LoadResource(Material, "TexturedMaterial", filename.c_str());


		// Use sphere to better analyze the environment map
		//resman_.CreateSphere("TorusMesh");



		// Can also check reflections on a cube
		std::string cube_filename = std::string(MATERIAL_DIRECTORY) + std::string("/dense_cube.aobj");
		//resman_.LoadResource(Mesh, "TorusMesh", cube_filename.c_str());

		// Create a cube for the skybox
		resman_.CreateCube("CubeMesh");

		// Load material to be applied to asteroids
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
		resman_.LoadResource(Material, "ObjectMaterial", filename.c_str());

		// Create mirror plane
		resman_.CreateMirror("MirrorMesh");


		// Load material to be applied to torus
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/envmap");
		resman_.LoadResource(Material, "EnvMapMaterial", filename.c_str());

		// Load cube map to be applied to skybox
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/skybox/siege.tga");
		resman_.LoadResource(CubeMap, "SkyboxCubeMap", filename.c_str());

		// Load material to be applied to skybox
		filename = std::string(MATERIAL_DIRECTORY) + std::string("/skybox");
		resman_.LoadResource(Material, "SkyboxMaterial", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/ground.png");
		resman_.LoadResource(Texture, "Ground", filename.c_str());

		filename = std::string(MATERIAL_DIRECTORY) + std::string("/common_texture_material");
		resman_.LoadResource(Material, "c_t_material", filename.c_str());


		resman_.CreateCylinder ("Cylinder");
	}


	void Game::SetupScene(void) {

		// Set background color for the scene
		scene_.SetBackgroundColor(viewport_background_color_g);

		// Set up camera
		SetUpCamera();

		// Set background color for the scene
		scene_.SetBackgroundColor(viewport_background_color_g);


		
		// Create an instance of the torus mesh
		Common *torus = (Common *)CreateCommonInstance("TorusInstance1", "TorusMesh", "EnvMapMaterial", "", "SkyboxCubeMap");
		// Scale the instance
		torus->Scale(glm::vec3(2));
		torus->Translate(glm::vec3(0.0, 0.0, 0.0));
		torus->SetAngleSpeed(0.02);
		scene_.AddNode(torus);
		

		
		// Create skybox
		skybox_ = CreateSkyBoxInstance("Skyox1", "CubeMesh", "SkyboxMaterial", "SkyboxCubeMap");
		skybox_->Scale(glm::vec3(50));
		//skybox_->Translate(glm::vec3(0.0, 100.0, 0.0));
		scene_.AddNode(skybox_);
		
		
		// Create a lake
		Common *lake = CreateCommonInstance("Lake1", "MirrorMesh", "EnvMapMaterial", "", "SkyboxCubeMap");
		// Scale the instance
		lake->Scale(glm::vec3(20));
		lake->Translate(glm::vec3(0.0, -24.8, 0.0));
		lake->Rotate(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0)));
		scene_.AddNode(lake);

		
		
		// Create a ground
		Common * ground = CreateCommonInstance("Ground1", "2DSquare", "c_t_material","Ground");
		ground->Scale(glm::vec3(500));
		ground->Translate(glm::vec3(0.0, -25, 0.0));
		ground->Rotate(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0)));
		scene_.AddNode(ground);
		

		CreateBird();
		
	}

	void Game::CreateBird() {
		resman_.CreatePyramid("Bird_head", 0.44, 0.2, 0.40);
		resman_.CreateCylinder("Bird_body", 0.20, 0.87);
		//							float thick, float bot, float top, float height
		resman_.CreateTrape("Bird_wings", 0.1, 0.46, 0.3, 0.86);
		resman_.CreateTriangle("Bird_wings_tip", 0.1, 0.3, 0.1, 0.66, false);
		//							float thick, float bot, float top, float height, bool tip
		resman_.CreateTriangle("Bird_beak", 0.2, 0.2, 0.03, 0.39, true);
		resman_.CreateTail("Bird_tail", 0.2, 0.2);

		Common *Body = CreateCommonInstance("Body", "Bird_body", "TexturedMaterial", "White");
		Body->Scale(glm::vec3(1.0, 1.0, 1.0));
		Body->Translate(glm::vec3(0.0, 0.0, 0.3));
		Body->Rotate(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(1, 0, 0)));
		Body->Rotate(glm::angleAxis(glm::pi<float>() , glm::vec3(0, 0, 1)));
		Body->SetParent(camera_);
			   
		Common *Head = CreateCommonInstance("Head", "Bird_head", "TexturedMaterial", "White");
		Head->Scale(glm::vec3(1.0, 1.0, 1.0));
		Head->Translate(glm::vec3(0.30, 0.9, 0.0));

		Common *LWing = CreateCommonInstance("LWing", "Bird_wings", "TexturedMaterial", "White");
		LWing->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_LWing = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		LWing->Rotate(rotation_LWing);
		glm::quat rotation_LWing2 = glm::angleAxis(180 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
		LWing->Rotate(rotation_LWing2);
		LWing->Translate(glm::vec3(0.5, 0.3, 0.0));

		Common *LWing_tip = CreateCommonInstance("LWing_tip", "Bird_wings_tip", "TexturedMaterial", "White");
		LWing_tip->Scale(glm::vec3(1.0, 1.0, 1.0));
		LWing_tip->Translate(glm::vec3(0.16, 0.8, 0.0));

		Common *RWing = CreateCommonInstance("RWing", "Bird_wings", "TexturedMaterial", "White");
		RWing->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_RWing = glm::angleAxis(-90 * -glm::pi<float>() / 180.0f, glm::vec3(0.0, 0.0, 1.0));
		RWing->Rotate(rotation_RWing);
		RWing->Translate(glm::vec3(-0.5, 0.3, 0.0));

		Common *RWing_tip = CreateCommonInstance("LWing_tip", "Bird_wings_tip", "TexturedMaterial", "White");
		RWing_tip->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_RWing_Tip = glm::angleAxis(180 * -glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		RWing_tip->Rotate(rotation_RWing_Tip);
		RWing_tip->Translate(glm::vec3(0.16, -1.82, 0.0));

		Common *Beak = CreateCommonInstance("Beak", "Bird_beak", "TexturedMaterial", "White");
		Beak->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Beak = glm::angleAxis(-90 * glm::pi<float>() / 180.0f, glm::vec3(0.0, 1.0, 0.0));
		Beak->Rotate(rotation_Beak);
		Beak->Translate(glm::vec3(0.0, 1.16, 0.37));

		Common *Tail = CreateCommonInstance("Tail", "Bird_tail", "TexturedMaterial", "White");
		Tail->Scale(glm::vec3(1.0, 1.0, 1.0));
		glm::quat rotation_Tail = glm::angleAxis(12 * glm::pi<float>() / 180.0f, glm::vec3(1.0, 0.0, 0.0));
		Tail->Rotate(rotation_Tail);
		Tail->Translate(glm::vec3(0.0, -0.7, -0.1));

		Head->SetParent(Body);
		LWing->SetParent(Body);
		RWing->SetParent(Body);
		Beak->SetParent(Body);
		LWing_tip->SetParent(LWing);
		RWing_tip->SetParent(LWing);
		Tail->SetParent(Body);


		// Create third Camera
		int width, height;
		glfwGetFramebufferSize(window_, &width, &height);
		glViewport(0, 0, width, height);

		Camera* camera = new Camera();
		// Set current view
		camera->SetView(camera_position_g+glm::vec3(-2,6,10), camera_->GetPosition(), glm::normalize(glm::vec3(0,1,0)));
		// Set projection
		camera->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

		CameraNode* thirdCamera = new CameraNode(camera, "ThirdCamera");
		thirdCamera->SetParent(Body);

	}

	void Game::SetUpCamera()
	{
		// Set viewport
		int width, height;
		glfwGetFramebufferSize(window_, &width, &height);
		glViewport(0, 0, width, height);

		Camera* camera = new Camera();
		// Set current view
		camera->SetView(camera_position_g, camera_look_at_g, camera_up_g);
		// Set projection
		camera->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

		CameraNode *CNode = new CameraNode(camera, "Camera");
		scene_.AddNode(CNode);
		camera_ = CNode;


	}

	void Game::MainLoop(void) {

		// Loop while the user did not close the window
		while (!glfwWindowShouldClose(window_)) {

			// remove distoried object
			std::vector<SceneNode*>::iterator it;
			std::vector<SceneNode*> list = scene_.GetNodeList();
			for (it = list.begin(); it != list.end();) {
				if ((*it)->GetShouldBeDestoried()) { std::cout << (*it)->GetName() << " is removed\n"; it = list.erase(it); }
				else ++it;
			}
			scene_.SetNodeList(list);

			// Animate the scene
			if (animating_) {
				static double last_time = 0;
				double current_time = glfwGetTime();
				if ((current_time - last_time) > 0.01) {
					scene_.Update();
					last_time = current_time;
					firecooldown = firecooldown - 0.01 <= 0 ? 0 : firecooldown - 0.01;

					// Draw the scene
					CameraNode * fcNode = (CameraNode *)scene_.GetNode("Camera");
					CameraNode * tcNode = (CameraNode *)scene_.GetNode("ThirdCamera");
					if (thirdview) {
						scene_.Draw(tcNode->GetCamera());
					}
					else {
						scene_.Draw(fcNode->GetCamera());
					}
				}
			}



			// Push buffer drawn in the background onto the display
			glfwSwapBuffers(window_);

			// Update other events like input handling
			glfwPollEvents();
		}
	}


	void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		// Get user data with a pointer to the game class
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;

		// Quit game if 'q' is pressed
		if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}

		// Stop animation if space bar is pressed
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			game->animating_ = (game->animating_ == true) ? false : true;
		}

		if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
			game->thirdview = game->thirdview == true ? false : true;
		}

		// Fire control
		if (key == GLFW_KEY_1 && game->firecooldown == 0) {
			game->firecooldown = game->firegap;
			game->CreateMissile();
		}


		// View control
		float rot_factor(glm::pi<float>() / 80);
		float trans_factor = 1;
		CameraNode* cNode = (CameraNode *)game->scene_.GetNode("Camera");
		if (key == GLFW_KEY_UP) {
			cNode->Pitch(rot_factor);
		}
		if (key == GLFW_KEY_DOWN) {
			cNode->Pitch(-rot_factor);
		}
		if (key == GLFW_KEY_LEFT) {
			cNode->Yaw(rot_factor);
		}
		if (key == GLFW_KEY_RIGHT) {
			cNode->Yaw(-rot_factor);
		}
		if (key == GLFW_KEY_S) {
			cNode->Roll(-rot_factor);
		}
		if (key == GLFW_KEY_X) {
			cNode->Roll(rot_factor);
		}
		if (key == GLFW_KEY_A) {
			cNode->Translate(cNode->GetCamera()->GetForward()*trans_factor);
			game->skybox_->Translate(cNode->GetCamera()->GetForward()*trans_factor);
		}
		if (key == GLFW_KEY_Z) {
			cNode->Translate(-cNode->GetCamera()->GetForward()*trans_factor);
			game->skybox_->Translate(-cNode->GetCamera()->GetForward()*trans_factor);
		}
		if (key == GLFW_KEY_J) {
			cNode->Translate(-cNode->GetCamera()->GetSide()*trans_factor);
			game->skybox_->Translate(-cNode->GetCamera()->GetSide()*trans_factor);
		}
		if (key == GLFW_KEY_L) {
			cNode->Translate(cNode->GetCamera()->GetSide()*trans_factor);
			game->skybox_->Translate(cNode->GetCamera()->GetSide()*trans_factor);
		}
		if (key == GLFW_KEY_I) {
			cNode->Translate(cNode->GetCamera()->GetUp()*trans_factor);
			game->skybox_->Translate(cNode->GetCamera()->GetUp()*trans_factor);
		}
		if (key == GLFW_KEY_K) {
			cNode->Translate(-cNode->GetCamera()->GetUp()*trans_factor);
			game->skybox_->Translate(-cNode->GetCamera()->GetUp()*trans_factor);
		}
	}


	void Game::ResizeCallback(GLFWwindow* window, int width, int height) {

		// Set up viewport and camera projection based on new window size
		glViewport(0, 0, width, height);
		void* ptr = glfwGetWindowUserPointer(window);
		Game *game = (Game *)ptr;

		CameraNode* cNode = (CameraNode *)game->scene_.GetNode("Camera");
		cNode->GetCamera()->SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);

	}


	Game::~Game() {

		glfwTerminate();
	}



	
	void Game::CreateMissile()
	{
		
		Missile* missile = CreateMissileInstance("Missile", "Cylinder", "ObjectMaterial");
		missile->SetRenderState(false);
		missile->SetLifeTime(1.0);
		missile->SetScale(glm::vec3(0.05, 1, 0.05));
		CameraNode* cNode = (CameraNode*)scene_.GetNode("Camera");
		missile->SetPosition(cNode->GetCamera()->GetPosition() + 0.2f*cNode->GetCamera()->GetForward());
		missile->SetOrientation(cNode->GetCamera()->GetOrientation());
		missile->Rotate(glm::normalize(glm::angleAxis(-(float)glm::pi<float>() / 2, glm::vec3(1, 0, 0))));
		missile->SetForward(glm::vec3(0, 1, 0));
		missile->SetMaxSpeed(1);
		missile->SetSpeed(cNode->GetSpeed() + 0.2);
		missile->SetFictionFactor(0);
		//missile->selectTarget(scene_.GetNodeList());

		scene_.AddNode(missile);
		
	}

	ResourceSet Game::CollectSource(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet;
		Resource *geom = resman_.GetResource(object_name);
		if (!geom) {
			throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
		}
		theSet.g = geom;

		Resource *mat = resman_.GetResource(material_name);
		if (!mat) {
			throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
		}
		theSet.m = mat;
		
		Resource *tex = NULL;
		if (texture_name != "") {
			tex = resman_.GetResource(texture_name);
			if (!tex) {
				throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
			}
		}
		theSet.t = tex;

		Resource *envmap = NULL;
		if (envmap_name != "") {
			envmap = resman_.GetResource(envmap_name);
			if (!envmap) {
				throw(GameException(std::string("Could not find resource \"") + envmap_name + std::string("\"")));
			}
		}
		theSet.e = envmap;
		return theSet;
	}

	SkyBox* Game::CreateSkyBoxInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet = CollectSource(entity_name, object_name, material_name, texture_name, envmap_name);

		SkyBox *scn = new SkyBox(entity_name, theSet.g, theSet.m, theSet.t, theSet.e);
		return scn;
	}

	Common* Game::CreateCommonInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet = CollectSource(entity_name, object_name, material_name, texture_name, envmap_name);
		Common *scn = new Common(entity_name, theSet.g, theSet.m, theSet.t, theSet.e);
		return scn;
	}

	Missile* Game::CreateMissileInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {
		ResourceSet theSet = CollectSource(entity_name, object_name, material_name, texture_name, envmap_name);
		Missile *scn = new Missile(entity_name, theSet.g, theSet.m, theSet.t, theSet.e);
		return scn;
	}

} // namespace game
