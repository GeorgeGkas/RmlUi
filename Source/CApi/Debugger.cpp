#include "Conversions.h"
#include <RmlUi/CApi/Debugger.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Debugger/Debugger.h>

bool rml_debugger_initialise(RmlContext* ctx)
{
	RMLUI_ASSERT(ctx);
	return Rml::Debugger::Initialise(Rml::CApi::FromC(ctx));
}

void rml_debugger_shutdown(void)
{
	Rml::Debugger::Shutdown();
}

bool rml_debugger_set_context(RmlContext* ctx)
{
	RMLUI_ASSERT(ctx);
	return Rml::Debugger::SetContext(Rml::CApi::FromC(ctx));
}

void rml_debugger_set_visible(bool visible)
{
	Rml::Debugger::SetVisible(visible);
}

bool rml_debugger_is_visible(void)
{
	return Rml::Debugger::IsVisible();
}