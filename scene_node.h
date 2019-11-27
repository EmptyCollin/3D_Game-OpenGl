#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "camera.h"

namespace game {

    // Class that manages one object in a scene 
    class SceneNode {

        public:
            // Create scene node from given resources
            SceneNode(const std::string name, const Resource *geometry, const Resource *material);

            // Destructor
            ~SceneNode();
            
            // Get name of node
            const std::string GetName(void) const;

            // Get node attributes
            glm::vec3 GetPosition(void) const;
            glm::quat GetOrientation(void) const;
            glm::vec3 GetScale(void) const;

            // Set node attributes
            void SetPosition(glm::vec3 position);
            void SetOrientation(glm::quat orientation);
            void SetScale(glm::vec3 scale);
            
            // Perform transformations on node
            void Translate(glm::vec3 trans);
            void Rotate(glm::quat rot);
            void Scale(glm::vec3 scale);

            // Draw the node according to scene parameters in 'camera'
            // variable
            virtual void Draw(Camera *camera);

            // Update the node
            virtual void Update(void);

            // OpenGL variables
            GLenum GetMode(void) const;
            GLuint GetArrayBuffer(void) const;
            GLuint GetElementArrayBuffer(void) const;
            GLsizei GetSize(void) const;
            GLuint GetMaterial(void) const;

			void SetParent(SceneNode *P) { parent = P; }
			void AddChild(SceneNode *C) { children.push_back(C); }
			SceneNode* GetParent() { return parent; }
			std::vector<SceneNode*> GetChildren() { return children; }

			glm::mat4 getTran() { return TransferMatrix; }
			void SetTran(glm::mat4 Tr) { TransferMatrix = Tr; }
			SceneNode *findIt(std::string node_name);

			void SetRotation(float r) { Rotation = r; }

			bool getAppear() { return appear; }
			void setAppear(bool a) { appear = a; }

			float getVel() { return vel; }

			void setLive(bool l) { live = l; }
			void setLiveTime(float lt) { liveTime = lt; }

			bool getLive() { return live; }

        private:
            std::string name_; // Name of the scene node
            GLuint array_buffer_; // References to geometry: vertex and array buffers
            GLuint element_array_buffer_;
            GLenum mode_; // Type of geometry
            GLsizei size_; // Number of primitives in geometry
            GLuint material_; // Reference to shader program
            glm::vec3 position_; // Position of node
            glm::quat orientation_; // Orientation of node
            glm::vec3 scale_; // Scale of node

			glm::mat4 TransferMatrix = glm::mat4(1.0);

            // Set matrices that transform the node in a shader program
            void SetupShader(GLuint program,Camera* camera);

			SceneNode *parent;
			std::vector<SceneNode*> children;

			//Special variable for canon
			float Rotation;

			//Special variable for ship
			bool appear = true;

			//Special variable for missile
			float vel = 5.0;

			float liveTime = 5000;
			bool live = true;

    }; // class SceneNode

} // namespace game

#endif // SCENE_NODE_H_
