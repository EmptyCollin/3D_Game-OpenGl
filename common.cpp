#include "common.h"

namespace game {

Common::Common(const std::string name, const Resource *geometry, const Resource *material, const Resource *texture, const Resource *envmap) 
		: SceneNode(name, geometry, material,texture,envmap) {
}


Common::~Common(){
}




void Common::Update(void){
	
	if ((angleSpeed !=0) && (rotRange != 0) &&(abs(angle + angleSpeed) >= abs(rotRange))) {
		angleSpeed *= -1;
	}
	angle += angleSpeed;
	Rotate(glm::normalize(glm::angleAxis(angleSpeed*glm::pi<float>(), rotAxis)));

	if ((transSpeed != 0) && (transRange != 0) && (abs(offset + transSpeed) >= abs(transRange))) {
		transSpeed *= -1;
	}
	offset += transSpeed;
	Translate(offset* transRange *transAxis);

	SceneNode::UpdateNodeInfo();
}
            
} // namespace game
