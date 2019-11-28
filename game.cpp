#include <iostream>
#include <time.h>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>

#include "game.h"
#include "bin/path_config.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Main window settings
const std::string window_title_g = "Demo";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

// Viewport and camera settings
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 1000.0;
float camera_fov_g = 50.0; // Field-of-view of camera
const glm::vec3 viewport_background_color_g(0.0, 0.0, 0.0);
glm::vec3 camera_position_g(0.0, 10.0, 20.0);
glm::vec3 camera_look_at_g(0.0, 0.0, 0.0);
glm::vec3 camera_up_g(0.0, 1.0, 0.0);


bool FIRST_VIEW = false;

// Materials 
const std::string material_directory_g = MATERIAL_DIRECTORY;


Game::Game(void){

    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void){

    // Run all initialization steps
    InitWindow();
    InitView();
    InitEventHandlers();

    // Set variables
    animating_ = true;
}

       
void Game::InitWindow(void){

    // Initialize the window management library (GLFW)
    if (!glfwInit()){
        throw(GameException(std::string("Could not initialize the GLFW library")));
    }

    // Create a window and its OpenGL context
    if (window_full_screen_g){
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), glfwGetPrimaryMonitor(), NULL);
    } else {
        window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g.c_str(), NULL, NULL);
    }
    if (!window_){
        glfwTerminate();
        throw(GameException(std::string("Could not create window")));
    }

    // Make the window's context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK){
        throw(GameException(std::string("Could not initialize the GLEW library: ")+std::string((const char *) glewGetErrorString(err))));
    }
}


void Game::InitView(void){

    // Set up z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Set viewport
    int width, height;
    glfwGetFramebufferSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // Set up camera
    // Set current view
    camera_.SetView(camera_position_g, camera_look_at_g, camera_up_g);
    // Set projection
    camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


void Game::InitEventHandlers(void){

    // Set event callbacks
    glfwSetKeyCallback(window_, KeyCallback);
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Set pointer to game object, so that callbacks can access it
    glfwSetWindowUserPointer(window_, (void *) this);
}


void Game::SetupResources(void){

    // Create a simple sphere to represent the asteroids
    resman_.CreateSphere("SimpleSphereMesh", 1.0, 10, 10);
	resman_.CreateTorus("SimpleTorusMesh", 0.4, 0.2, 90, 30);
	resman_.CreateCube("SimpleCubeMesh", 0.16);
	resman_.CreateSphere("SimpleSphereMesh_Ship", 1.2, 10, 10);
	resman_.CreateCylinder("SimpleCylinderMesh_ShipCanon", 0.2, 0.4);

	resman_.CreateCylinder("SimpleCylinderMesh_Center",0.45,0.5);
	resman_.CreateCylinder("SimpleCylinderMesh_Base", 0.24, 0.6);
	resman_.CreateCylinder("SimpleCylinderMesh_Canon", 0.09, 0.45);
	resman_.CreateCylinder("SimpleCylinderMesh_Barrel", 0.05, 0.88);


	// Create a torus
	resman_.CreateTorus("TorusMesh");

	// Use sphere to better analyze the environment map
	//resman_.CreateSphere("TorusMesh");


	
	// Can also check reflections on a cube
	std::string cube_filename = std::string(MATERIAL_DIRECTORY) + std::string("/dense_cube.aobj");
	//resman_.LoadResource(Mesh, "TorusMesh", cube_filename.c_str());

	// Create a cube for the skybox
	resman_.CreateCube("CubeMesh");

	// Load material to be applied to asteroids
    std::string filename = std::string(MATERIAL_DIRECTORY) + std::string("/material");
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

}


void Game::SetupScene(void){

    // Set background color for the scene
    scene_.SetBackgroundColor(viewport_background_color_g);

    // Create asteroid field
	CreateCameraInstance();
	CreateShipInstance();

	/*
	// Create an instance of the torus mesh
	game::SceneNode *torus = CreateInstance("TorusInstance1", "CubeMesh", "EnvMapMaterial", "", "SkyboxCubeMap");
	// Scale the instance
	torus->Scale(glm::vec3(5));
	torus->Translate(glm::vec3(0.0, 0.0, 0.0));
	scene_.AddNode(torus);
	*/
	// Create skybox
	skybox_ = CreateInstance("CubeInstance1", "CubeMesh", "SkyboxMaterial", "SkyboxCubeMap");
	skybox_->Scale(glm::vec3(200));
	skybox_->Translate(glm::vec3(0.0, 100.0, 0.0));
	scene_.AddNode(skybox_);

	// Create a lake
	game::SceneNode *lake = CreateInstance("Lake1", "MirrorMesh", "EnvMapMaterial", "", "SkyboxCubeMap");
	// Scale the instance
	lake->Scale(glm::vec3(20));
	lake->Translate(glm::vec3(0.0, 0.5, 0.0));
	lake->Rotate(glm::angleAxis(glm::pi<float>() / 2.0f, glm::vec3(1.0, 0.0, 0.0)));
	scene_.AddNode(lake);

}

//create missile, and determine whether hit or not by ray-sphere intersection
void Game::fire() {
	SceneNode *missile = CreateMissileInstance();
	glm::vec3 B = camera_.GetForward();
	glm::vec3 A = camera_.GetPosition();

	for (int i = 0; i < scene_.getRoot()->GetChildren().size(); i += 1) {
		SceneNode *NODE = scene_.getRoot()->GetChildren()[i];

		if ((NODE->GetName() != "Ship_Body" && NODE->GetName() != "Base" && NODE->GetName() != "Missile" && NODE->getLive() == true)) {
			glm::vec3 C = NODE->GetPosition();
			float a = glm::dot(B, B);
			float b = 2 * glm::dot(B, A-C);
			float c = glm::dot(A-C,A-C) - 1;
			if (b*b - 4 * a*c >= 0) {
				glm::vec3 Pos = NODE->GetPosition();
				float distance = sqrt((A.x - Pos.x)*(A.x - Pos.x) + (A.y - Pos.y)*(A.y - Pos.y) + (A.z - Pos.z)*(A.z - Pos.z));
				float Time = distance / 5;
				NODE->setLiveTime(Time);
				NODE->setLive(false);
				missile->setLiveTime(Time);
				missile->setLive(false);

			}
		}
	}
	scene_.AddNode(missile);
}


//handle missile firing and hit
void Game::updateMissile(){
	for (int i = 0; i < scene_.getRoot()->GetChildren().size(); i += 1) {
		SceneNode *NODE = scene_.getRoot()->GetChildren()[i];
		if (NODE->GetName() == "Missile") {
			glm::vec3 Pos = NODE->GetPosition();
			NODE->SetPosition(glm::vec3(Pos.x, Pos.y, Pos.z - 5.0));			
		}
	}
}

void Game::MainLoop(void){
    // Loop while the user did not close the window
    while (!glfwWindowShouldClose(window_)){
        // Animate the scene
        if (animating_){
            static double last_time = 0;
            double current_time = glfwGetTime();
            if ((current_time - last_time) > 0.05){
                scene_.Update();
                last_time = current_time;

				//Canon control
				glm::quat rotation = glm::angleAxis(glm::pi<float>() / 45.0f, glm::vec3(0.0, 1.0, 0.0));
				orientation *= rotation;

				// canon rotation
				if (angle <= lowAn && increasement < 0) {
					increasement *= -1;
				}
				else if (angle >= highAn && increasement > 0) {
					increasement *= -1;
				}

				angle += increasement;

				// canon in and out
				if (Barrel_Length <= lowLn && LNincrease < 0) {
					LNincrease *= -1;
				}
				else if (Barrel_Length >= highLn && LNincrease > 0) {
					LNincrease *= -1;
				}

				Barrel_Length += LNincrease;


				// Update time
				last_time = current_time;

				
            }
        }
		//handle velocity, acceleration
		camera_.updatePos();

		//handle transf
		camera_.updateTransf();
		scene_.getRoot()->SetTran(camera_.getTransf());

		//handle missile
		updateMissile();

		TurretRotation();

        // Draw the scene
        scene_.Draw(&camera_);

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);

        // Update other events like input handling
        glfwPollEvents();
    }
}


void Game::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){

    // Get user data with a pointer to the game class
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;

    // Quit game if 'q' is pressed
    if (key == GLFW_KEY_Q && action == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    // Stop animation if space bar is pressed
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS){
        game->animating_ = (game->animating_ == true) ? false : true;
    }

    // View control
    float rot_factor(glm::pi<float>() / 180);
    float trans_factor = 1.0;
	
	//for view change
	float trans_factors = 2.0;
	float view_factor = 15.0;
    if (key == GLFW_KEY_UP){
        game->camera_.Pitch(rot_factor);
    }
    if (key == GLFW_KEY_DOWN){
        game->camera_.Pitch(-rot_factor);
    }
    if (key == GLFW_KEY_LEFT){
        game->camera_.Yaw(rot_factor);
    }
    if (key == GLFW_KEY_RIGHT){
        game->camera_.Yaw(-rot_factor);
    }
    if (key == GLFW_KEY_S){
        game->camera_.Roll(-rot_factor);
    }
    if (key == GLFW_KEY_X){
        game->camera_.Roll(rot_factor);
    }
    if (key == GLFW_KEY_A){
        game->camera_.setAcceleration(-0.01);
    }
    if (key == GLFW_KEY_Z){
		game->camera_.setAcceleration(0.01);
    }
    if (key == GLFW_KEY_J){
        game->camera_.Translate(-game->camera_.GetSide()*trans_factor);
    }
    if (key == GLFW_KEY_L){
        game->camera_.Translate(game->camera_.GetSide()*trans_factor);
    }
    if (key == GLFW_KEY_I){
        game->camera_.Translate(game->camera_.GetUp()*trans_factor);
    }
    if (key == GLFW_KEY_K){
        game->camera_.Translate(-game->camera_.GetUp()*trans_factor);
    }


	//change view setting
	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		if (game->scene_.GetNode("Ship_Body")->getAppear() == true) {
			game->scene_.GetNode("Ship_Body")->setAppear(false);
			game->camera_.Translate(game->camera_.GetForward()*view_factor);
			game->camera_.Translate(-game->camera_.GetUp()*trans_factors);
			FIRST_VIEW = true;
		}
		else {
			game->scene_.GetNode("Ship_Body")->setAppear(true); 
			game->camera_.Translate(-game->camera_.GetForward()*view_factor);
			game->camera_.Translate(game->camera_.GetUp()*trans_factors);
			FIRST_VIEW = false;
		}
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		game->fire();
	}

}


void Game::ResizeCallback(GLFWwindow* window, int width, int height){

    // Set up viewport and camera projection based on new window size
    glViewport(0, 0, width, height);
    void* ptr = glfwGetWindowUserPointer(window);
    Game *game = (Game *) ptr;
    game->camera_.SetProjection(camera_fov_g, camera_near_clip_distance_g, camera_far_clip_distance_g, width, height);
}


Game::~Game(){
    
    glfwTerminate();
}


Asteroid *Game::CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name){

    // Get resources
    Resource *geom = resman_.GetResource(object_name);
    if (!geom){
        throw(GameException(std::string("Could not find resource \"")+object_name+std::string("\"")));
    }

    Resource *mat = resman_.GetResource(material_name);
    if (!mat){
        throw(GameException(std::string("Could not find resource \"")+material_name+std::string("\"")));
    }
    // Create asteroid instance
    Asteroid *ast = new Asteroid(entity_name, geom, mat);
    scene_.AddNode(ast);
    return ast;
}


void Game::CreateAsteroidField(int num_asteroids){
	
    // Create a number of asteroid instances
    for (int i = 0; i < num_asteroids; i++){
        // Create instance name
        std::stringstream ss;
        ss << i;
        std::string index = ss.str();
        std::string name = "AsteroidInstance" + index;

        // Create asteroid instance
        Asteroid *ast = CreateAsteroidInstance(name, "SimpleSphereMesh", "ObjectMaterial");

        // Set attributes of asteroid: random position, orientation, and
        // angular momentum
        ast->SetPosition(glm::vec3(-300.0 + 600.0*((float) rand() / RAND_MAX), -300.0 + 600.0*((float) rand() / RAND_MAX), 600.0*((float) rand() / RAND_MAX)));
        ast->SetOrientation(glm::normalize(glm::angleAxis(glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
        ast->SetAngM(glm::normalize(glm::angleAxis(0.05f*glm::pi<float>()*((float) rand() / RAND_MAX), glm::vec3(((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX), ((float) rand() / RAND_MAX)))));
    }
}

void Game::CreateCameraInstance() {

	// Get resources
	Resource *geom = resman_.GetResource("SimpleSphereMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleSphereMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ObjectMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}

	// Create asteroid instance
	CameraNode *cam = new CameraNode("Camera", geom, mat);
	cam->SetCamera(camera_);
	scene_.SetRoot(cam);
}

void Game::CreateCanonInstance() {
	// Get resources
	//center
	Resource *geom_C = resman_.GetResource("SimpleCylinderMesh_Center");
	if (!geom_C) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleCylinderMesh_Center" + std::string("\"")));
	}
	Resource *mat_C = resman_.GetResource("ObjectMaterial");
	if (!mat_C) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}
	//base
	Resource *geom_B = resman_.GetResource("SimpleCylinderMesh_Base");
	if (!geom_B) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleCylinderMesh_Base" + std::string("\"")));
	}
	Resource *mat_B = resman_.GetResource("ObjectMaterial");
	if (!mat_B) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}
	//canon
	Resource *geom_CA = resman_.GetResource("SimpleCylinderMesh_Canon");
	if (!geom_CA) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleCylinderMesh_Canon" + std::string("\"")));
	}
	Resource *mat_CA = resman_.GetResource("ObjectMaterial");
	if (!mat_CA) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}
	//outcanon
	Resource *geom_O = resman_.GetResource("SimpleCylinderMesh_Barrel");
	if (!geom_O) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleCylinderMesh_Barrel" + std::string("\"")));
	}
	Resource *mat_O = resman_.GetResource("ObjectMaterial");
	if (!mat_O) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}

	SceneNode *Center = new SceneNode("Center", geom_C, mat_C);
	SceneNode *Base = new SceneNode("Base", geom_B, mat_B);
	SceneNode *Canon = new SceneNode("Canon", geom_CA, mat_CA);
	SceneNode *Barrel = new SceneNode("Barrel", geom_O, mat_O);

	//set children
	Base->AddChild(Center);
	Center->AddChild(Canon);
	Canon->AddChild(Barrel);
	//set parent
	Center->SetParent(Base);
	Canon->SetParent(Center);
	Barrel->SetParent(Canon);

	//build up turret
	Base->SetPosition(glm::vec3(0,0,770));
	Base->SetOrientation(camera_.GetOrientation());
	Center->SetPosition(glm::vec3(0, 0.4, 0));
	Canon->SetPosition(glm::vec3(-0.4,0,0));
	//Barrel->SetPosition(glm::vec3(0, Barrel_Length,0));

	scene_.AddNode(Base);
}

void Game::CreateShipInstance() {
	Resource *geom = resman_.GetResource("SimpleSphereMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleSphereMesh" + std::string("\"")));
	}
	Resource *mat = resman_.GetResource("ObjectMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}
	Resource *geom_w = resman_.GetResource("SimpleTorusMesh");
	if (!geom_w) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleTorusMesh" + std::string("\"")));
	}
	Resource *mat_w = resman_.GetResource("ObjectMaterial");
	if (!mat_w) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}
	Resource *geom_c = resman_.GetResource("SimpleCylinderMesh_ShipCanon");
	if (!geom_c) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleCylinderMesh_ShipCanon" + std::string("\"")));
	}
	Resource *mat_c = resman_.GetResource("ObjectMaterial");
	if (!mat_c) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}

	SceneNode *Ship_Body = new SceneNode("Ship_Body",geom,mat);
	SceneNode *Ship_LWings = new SceneNode("Ship_Wings", geom_w, mat_w);
	SceneNode *Ship_RWings = new SceneNode("Ship_Wings", geom_w, mat_w);
	SceneNode *Ship_Canon = new SceneNode("Ship_Canon", geom_c, mat_c);

	Ship_Body->SetPosition(glm::vec3(0, -2, -15));
	Ship_Body->SetOrientation(camera_.GetOrientation());
	Ship_LWings->SetPosition(glm::vec3(-0.9, 0, 0));
	Ship_RWings->SetPosition(glm::vec3(0.9, 0, 0));
	Ship_Canon->SetPosition(glm::vec3(0.0, 0, -1));
	
	//Rotation of wings
	float angle = glm::pi<float>() / 2;
	glm::quat orientation_;
	glm::quat rotation = glm::angleAxis(angle, camera_.GetSide());
	orientation_ = rotation * camera_.GetOrientation();
	orientation_ = glm::normalize(orientation_);
	Ship_LWings->SetOrientation(orientation_);
	Ship_RWings->SetOrientation(orientation_);
	Ship_Canon->SetOrientation(orientation_);

	//set children
	Ship_Body->AddChild(Ship_LWings);
	Ship_Body->AddChild(Ship_RWings);
	Ship_Body->AddChild(Ship_Canon);
	//set parent
	Ship_LWings->SetParent(Ship_Body);
	Ship_RWings->SetParent(Ship_Body);
	Ship_Canon->SetParent(Ship_Body);

	scene_.AddNode(Ship_Body);
}

SceneNode *Game::CreateMissileInstance() {

	// Get resources
	Resource *geom = resman_.GetResource("SimpleCubeMesh");
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + "SimpleCubeMesh" + std::string("\"")));
	}

	Resource *mat = resman_.GetResource("ObjectMaterial");
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + "ObjectMaterial" + std::string("\"")));
	}

	// Create asteroid instance
	SceneNode *missile = new SceneNode("Missile", geom, mat);
	//missile->SetPosition(camera_.GetPosition());
	if (FIRST_VIEW){ missile->SetTran(camera_.getTransf()); }
	else{
		missile->SetTran(camera_.getTransf()); 
		missile->SetPosition(glm::vec3(0.0, -2, -15));
	}
	missile->SetOrientation(camera_.GetOrientation());
	
	return missile;
}

void Game::TurretRotation() {
	//Turret control
	
	scene_.GetNode("Canon")->SetRotation(angle);
	scene_.GetNode("Barrel")->SetPosition(glm::vec3(0, Barrel_Length, 0));
	scene_.GetNode("Base")->SetOrientation(orientation);
}


SceneNode *Game::CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name) {

	Resource *geom = resman_.GetResource(object_name);
	if (!geom) {
		throw(GameException(std::string("Could not find resource \"") + object_name + std::string("\"")));
	}

	Resource *mat = resman_.GetResource(material_name);
	if (!mat) {
		throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
	}

	Resource *tex = NULL;
	if (texture_name != "") {
		tex = resman_.GetResource(texture_name);
		if (!tex) {
			throw(GameException(std::string("Could not find resource \"") + material_name + std::string("\"")));
		}
	}

	Resource *envmap = NULL;
	if (envmap_name != "") {
		envmap = resman_.GetResource(envmap_name);
		if (!envmap) {
			throw(GameException(std::string("Could not find resource \"") + envmap_name + std::string("\"")));
		}
	}

	SceneNode *scn = scene_.CreateNode(entity_name, geom, mat, tex, envmap);
	return scn;
}

} // namespace game
