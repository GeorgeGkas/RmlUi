#include "Adapters.h"
#include "Conversions.h"
#include <RmlUi/CApi/Event.h>
#include <RmlUi/CApi/Log.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Event.h>

RmlEventPhase rml_event_get_phase(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return static_cast<RmlEventPhase>(static_cast<int>(Rml::CApi::FromC(event)->GetPhase()));
}

RmlElement* rml_event_get_current_element(RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(event)->GetCurrentElement());
}

RmlElement* rml_event_get_target_element(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(event)->GetTargetElement());
}

RmlStringView rml_event_get_type(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::ToStringView(Rml::CApi::FromC(event)->GetType());
}

uint16_t rml_event_get_id(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return static_cast<uint16_t>(Rml::CApi::FromC(event)->GetId());
}

void rml_event_stop_propagation(RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE_VOID(event);
	Rml::CApi::FromC(event)->StopPropagation();
}

void rml_event_stop_immediate_propagation(RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE_VOID(event);
	Rml::CApi::FromC(event)->StopImmediatePropagation();
}

bool rml_event_is_interruptible(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::FromC(event)->IsInterruptible();
}

bool rml_event_is_propagating(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::FromC(event)->IsPropagating();
}

bool rml_event_is_immediate_propagating(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::FromC(event)->IsImmediatePropagating();
}

const RmlDictionary* rml_event_get_parameters(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::ToC(&Rml::CApi::FromC(event)->GetParameters());
}

RmlVector2f rml_event_get_unprojected_mouse_screen_pos(const RmlEvent* event)
{
	RMLUI_CAPI_REQUIRE(event, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(event)->GetUnprojectedMouseScreenPos());
}

RmlEvent* rml_event_create(RmlElement* target, uint16_t id, RmlStringView type, const RmlDictionary* parameters, bool interruptible)
{
	const Rml::Dictionary empty;
	const Rml::Dictionary& used = parameters ? *Rml::CApi::FromC(parameters) : empty;
	auto* event = new Rml::Event(Rml::CApi::FromC(target), static_cast<Rml::EventId>(id), Rml::CApi::ToString(type), used, interruptible);
	return Rml::CApi::ToC(event);
}

void rml_event_destroy(RmlEvent* event)
{
	delete Rml::CApi::FromC(event);
}

RmlEventListener* rml_event_listener_create(const RmlEventListenerCallbacks* callbacks, void* user_data)
{
	if (!callbacks || !callbacks->process_event)
	{
		rml_log_message(RML_LOG_TYPE_ERROR, rml_sv("rml_event_listener_create: process_event callback must not be NULL"));
		return nullptr;
	}

	auto* adapter = new Rml::CApi::EventListenerAdapter(*callbacks, user_data);
	return Rml::CApi::ToC(static_cast<Rml::EventListener*>(adapter));
}

void rml_event_listener_destroy(RmlEventListener* listener)
{
	RMLUI_CAPI_REQUIRE_VOID(listener);
	delete Rml::CApi::FromC(listener);
}