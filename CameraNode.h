#ifndef CAMERANODE_H_
#define CAMERANODE_H_

#include "scene_node.h"

namespace game {

	class CameraNode :public SceneNode
	{
	public:
		CameraNode(const std::string name, const Resource *geometry, const Resource *material);
		~CameraNode();

		void SetCamera(Camera ca) { camera = ca; }
	private:
		Camera camera;
	};

}

#endif