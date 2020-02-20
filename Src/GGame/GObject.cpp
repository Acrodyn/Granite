#include "GGame/GObject.h"

GObject::GObject(std::string model, std::string texture) : mesh(model)
{
	mesh.SetTexture(texture);
}
