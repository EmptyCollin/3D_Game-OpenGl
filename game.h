#ifndef GAME_H_
#define GAME_H_

#include <exception>
#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "scene_graph.h"
#include "resource_manager.h"
#include "camera.h"
#include "asteroid.h"
#include "CameraNode.h"

namespace game {

    // Exception type for the game
    class GameException: public std::exception
    {
        private:
            std::string message_;
        public:
            GameException(std::string message) : message_(message) {};
            virtual const char* what() const throw() { return message_.c_str(); };
            virtual ~GameException() throw() {};
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
            // Run the game: keep the application active
            void MainLoop(void); 

        private:
            // GLFW window
            GLFWwindow* window_;

            // Scene graph containing all nodes to render
            SceneGraph scene_;

            // Resources available to the game
            ResourceManager resman_;

            // Camera abstraction
            Camera camera_;

			// Skybox
			SceneNode *skybox_;

            // Flag to turn animation on/off
            bool animating_;

            // Methods to initialize the game
            void InitWindow(void);
            void InitView(void);
            void InitEventHandlers(void);
 
            // Methods to handle events
            static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Asteroid field
            // Create instance of one asteroid
            Asteroid *CreateAsteroidInstance(std::string entity_name, std::string object_name, std::string material_name);
			// Create entire random asteroid field
			void CreateAsteroidField(int num_asteroids = 1500);

			void CreateCameraInstance();
			void CreateCanonInstance();
			void CreateShipInstance();
			SceneNode *CreateMissileInstance();

			void TurretRotation();

			SceneNode *CreateInstance(std::string entity_name, std::string object_name, std::string material_name, std::string texture_name = std::string(""), std::string envmap_name = std::string(""));

			//handle missile
			void fire();
			void updateMissile();

			//variable to control Turret
			glm::quat orientation;
			float angle = 1.0472;
			float increasement = 0.02;
			float lowAn = 1.0472;
			float highAn = 2.0944;

			float Barrel_Length = 0.5;
			float LNincrease = 0.001;
			float lowLn = 0.11;
			float highLn = 0.66;

			//variable to control Ship
			bool appear = true;

    }; // class Game

} // namespace game

#endif // GAME_H_
