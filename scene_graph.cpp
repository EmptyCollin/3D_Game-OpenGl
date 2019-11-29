#include <stdexcept>
#include <iostream>
#include <fstream>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
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
	AddNode(scn);

    return scn;
}

void SceneGraph::AddNode(SceneNode *node) {
	hieNodeList.push_back(node);
	return;
}


SceneNode* SceneGraph::GetNode(std::string node_name){

    // Find node with the specified name
    for (int i = 0; i < hieNodeList.size(); i++){
		if (hieNodeList[i]->GetName() == node_name) {
				return hieNodeList[i];
		}
		else if(hieNodeList[i]->GetChildren()->size()>0){
			SceneNode* r = hieNodeList[i]->FindIt(node_name);
			if (r != NULL) {
				return r;
			}
		}
    }
	std::cout << node_name << " not found" << std::endl;
    return NULL;
}


/*
std::vector<std::vector<SceneNode *>>::const_iterator SceneGraph::begin() const {

    return node_.begin();
}


std::vector<SceneNode *>::const_iterator SceneGraph::end() const {

    return node_.end();
}
*/

void SceneGraph::Draw(Camera *camera){

    // Clear background
    glClearColor(background_color_[0], 
                 background_color_[1],
                 background_color_[2], 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (int i = 0; i < hieNodeList.size(); i++) {

		if (hieNodeList[i]->GetName() == "Camera" && camera->GetCameraName() == "Camera") continue;
		else hieNodeList[i]->Draw(camera);
	}

}


void SceneGraph::Update(void){

	for (int i = 0; i < hieNodeList.size(); i++) {
		hieNodeList[i]->Update();
	}
}

} // namespace game
