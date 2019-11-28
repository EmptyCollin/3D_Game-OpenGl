#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "camera_node.h"
#include "SkyBox.h"
#include "common.h"
#include "missile.h"


namespace game {

	// Exception type for the game
	class GameException : public std::exception
	{
	private:
		std::string message_;
	public:
		GameException(std::string message) : message_(message) {};
		virtual const char* what() const throw() { return message_.c_str(); };
		virtual ~GameException() throw() {};
	};

	struct ResourceSet {
		Resource* g = NULL;
		Resource* m = NULL;
		Resource* t = NULL;
		Resource* e = NULL;
	};

	// Game application
	class Game {

	public:
		// Constructor and destructor
		Game(void);
		~Game();
		// Call Init() before calling any other method
		void Init(void);
		// Set up resources for the game
		void SetupResources(void);
		// Set up initial scene
		void SetupScene(void);
		void CreateBird();
		// Run the game: keep the application active
		void MainLoop(void);

	private:
		// GLFW window
		GLFWwindow* window_;

		// Scene graph containing all nodes to render
		SceneGraph scene_;

		// Resources available to the game
		ResourceManager resman_;

		// Flag to turn animation on/off
		bool animating_;

		// Methods to initialize the game
		void InitWindow(void);
		void InitView(void);
		void InitEventHandlers(void);

		// Methods to handle events
		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void ResizeCallback(GLFWwindow* window, int width, int height);

		// Camera abstraction
		CameraNode * camera_;
		SkyBox * skybox_;
		void CreateMissile();

		ResourceSet CollectSource(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name, std::string envmap_name);

		SkyBox * CreateSkyBoxInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string envmap_name = std::string(""));

		Common * CreateCommonInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string envmap_name = std::string(""));
		
		void SetUpCamera();
		
		int view = 1; // first view 1, third view -1
		bool thirdview = false; // if the view is needed to be changed
		float firecooldown = 0.0; // the current cool down time of firing 
		float firegap = 0.5;	// the less gap time of firing

	}; // class Game

} // namespace game

#endif // GAME_H_
