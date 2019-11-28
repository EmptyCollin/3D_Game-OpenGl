#include "SkyBox.h"

game::SkyBox::SkyBox(const std::string name, const Resource * geometry, const Resource * material, const Resource * texture, const Resource * envmap)
	: SceneNode(name,geometry,material,texture,envmap)
{
}

void game::SkyBox::Update(void)
{
	SceneNode::UpdateNodeInfo();
}
