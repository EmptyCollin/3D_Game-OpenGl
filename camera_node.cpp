#include "camera_node.h"

game::CameraNode::CameraNode(Camera * c, std::string cameraName) :SceneNode(cameraName, NULL, NULL) {
	camera = c; 
	camera->SetCameraName(cameraName); 
	orientation_ = camera->GetOrientation();
	position_ = camera->GetPosition();
}

glm::vec3 game::CameraNode::GetPosition(void) const
{
	return camera->GetPosition();
}

glm::quat game::CameraNode::GetOrientation(void) const
{
	return camera->GetOrientation();
}

void game::CameraNode::Pitch(float angle)
{
	camera->Pitch(angle);
	orientation_ = camera->GetOrientation();
}

void game::CameraNode::Yaw(float angle)
{
	camera->Yaw(angle);
	orientation_ = camera->GetOrientation();
}

void game::CameraNode::Roll(float angle)
{
	camera->Roll(angle);
	orientation_ = camera->GetOrientation();
}

void game::CameraNode::SetPosition(glm::vec3 position)
{
	position_ = position;
	camera->SetPosition(position_);

}

void game::CameraNode::SetOrientation(glm::quat orientation)
{
	orientation_ = orientation;
	camera->SetOrientation(orientation_);
}


void game::CameraNode::Update(void)
{

	if (view == -1) {
		camera->SetLookAt(position_);
	}
	else { camera->SetLookAt(position_+(float)800*camera->GetForward()); }

	SceneNode::UpdateNodeInfo();
	camera->SetPosition(position_);
	camera->SetOrientation(orientation_);
}

void game::CameraNode::ChangeView(int v)
{
	view = v;
	camera->ChangeView(view);
}
