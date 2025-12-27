#include "Application.h"

#include "AppLayer.h"

int main()
{
	GUI::ApplicationSpecification spec;
	spec.Name = "Real time ray tracer";

	GUI::Application app(spec);
	app.PushLayer<AppLayer>();
	app.Run();
}
