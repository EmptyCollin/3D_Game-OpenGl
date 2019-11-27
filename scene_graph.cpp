#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "scene_graph.h"

namespace game {

SceneGraph::SceneGraph(void){

    background_color_ = glm::vec3(0.0, 0.0, 0.0);
}


SceneGraph::~SceneGraph(){
}


void SceneGraph::SetBackgroundColor(glm::vec3 color){

    background_color_ = color;
}


glm::vec3 SceneGraph::GetBackgroundColor(void) const {

    return background_color_;
}
 

SceneNode *SceneGraph::CreateNode(std::string node_name, Resource *geometry, Resource *material){

    // Create scene node with the specified resources
    SceneNode *scn = new SceneNode(node_name, geometry, material);

    // Add node to the scene
    root->GetChildren().push_back(scn);

    return scn;
}


void SceneGraph::AddNode(SceneNode *node){
	node->SetParent(root);
	root->AddChild(node);
}


SceneNode *SceneGraph::GetNode(std::string node_name) const {

    // Find node with the specified name
    for (int i = 0; i < root->GetChildren().size(); i++){
        if (root->GetChildren()[i]->GetName() == node_name){
            return root->GetChildren()[i];
        }
		if (root->GetChildren()[i]->GetChildren().size() > 0) {
			SceneNode* a = root->GetChildren()[i];
			SceneNode* b = a->findIt(node_name);
			if (b != NULL) { return b; };
		}
    }
    return NULL;

}



std::vector<SceneNode *>::const_iterator SceneGraph::begin() const {

    return root->GetChildren().begin();
}


std::vector<SceneNode *>::const_iterator SceneGraph::end() const {

    return root->GetChildren().end();
}


void SceneGraph::Draw(Camera *camera){

    // Clear background
    glClearColor(background_color_[0], 
                 background_color_[1],
                 background_color_[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//std::cout << root->GetChildren().size();
    // Draw all scene nodes
    for (int i = 0; i < root->GetChildren().size(); i++){	
		root->GetChildren()[i]->Draw(camera);		
    }
}


void SceneGraph::Update(void){

    for (int i = 0; i < root->GetChildren().size(); i++){
		root->GetChildren()[i]->Update();
    }
}

} // namespace game
