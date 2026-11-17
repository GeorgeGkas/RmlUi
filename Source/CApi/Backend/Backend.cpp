#include <RmlUi/CApi/Backend.h>
#include <RmlUi_Backend.h>
#include <string>

namespace {

RmlBackendKeyDownFunc key_down_callback = nullptr;
void* key_down_user_data = nullptr;

bool ForwardKeyDown(Rml::Context* context, Rml::Input::KeyIdentifier key, int key_modifier, float native_dp_ratio, bool priority)
{
	if (!key_down_callback)
		return true;
	return key_down_callback(key_down_user_data, reinterpret_cast<RmlContext*>(context), static_cast<RmlKeyIdentifier>(key), key_modifier,
		native_dp_ratio, priority);
}

} // namespace

bool rml_backend_initialize(RmlStringView window_name, int width, int height, bool allow_resize)
{
	if (!window_name.data || window_name.size == 0)
		return Backend::Initialize("", width, height, allow_resize != false);

	const std::string terminated(window_name.data, window_name.size);
	return Backend::Initialize(terminated.c_str(), width, height, allow_resize != false);
}

void rml_backend_shutdown(void)
{
	Backend::Shutdown();
}

RmlSystemInterface* rml_backend_get_system_interface(void)
{
	return reinterpret_cast<RmlSystemInterface*>(Backend::GetSystemInterface());
}

RmlRenderInterface* rml_backend_get_render_interface(void)
{
	return reinterpret_cast<RmlRenderInterface*>(Backend::GetRenderInterface());
}

bool rml_backend_process_events(RmlContext* ctx, RmlBackendKeyDownFunc callback, void* user_data, bool power_save)
{
	key_down_callback = callback;
	key_down_user_data = user_data;
	const bool keep_running = Backend::ProcessEvents(reinterpret_cast<Rml::Context*>(ctx), callback ? &ForwardKeyDown : nullptr, power_save != false);
	key_down_callback = nullptr;
	key_down_user_data = nullptr;
	return keep_running;
}

void rml_backend_request_exit(void)
{
	Backend::RequestExit();
}

void rml_backend_begin_frame(void)
{
	Backend::BeginFrame();
}

void rml_backend_present_frame(void)
{
	Backend::PresentFrame();
}

RmlStringView rml_backend_get_name(void)
{
	static const char name[] = RMLUI_CAPI_BACKEND_NAME;
	RmlStringView view;
	view.data = name;
	view.size = sizeof(name) - 1;
	return view;
}
