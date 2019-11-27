#include "CameraNode.h"

namespace game {

	CameraNode::CameraNode(const std::string name, const Resource *geometry, const Resource *material) : SceneNode(name, geometry, material)
	{
	}


	CameraNode::~CameraNode()
	{
	}

}