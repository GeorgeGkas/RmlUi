#include "Conversions.h"
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/ElementInstancer.h>
#include <RmlUi/Core/Factory.h>
#include <RmlUi/Core/Log.h>
#include <cstdlib>
#include <cstring>

namespace Rml {
namespace CApi {

namespace {

class CallerElementInstancer final : public ElementInstancer {
public:
	ElementPtr InstanceElement(Element* parent, const String& tag, const XMLAttributes& attributes) override
	{
		return Factory::InstanceElement(parent, tag, tag, attributes);
	}
	void ReleaseElement(Element* element) override { delete element; }
};

CallerElementInstancer caller_element_instancer;

} // namespace

void ReportInvalidArgument(const char* function, const char* condition)
{
	Log::Message(Log::LT_ERROR, "%s: invalid argument, expected '%s'.", function, condition);
}

bool IsCallerOwned(const Element* element)
{
	if (!element || element->GetParentNode() || element->GetContext())
		return false;
	for (int i = 0; i < GetNumContexts(); i++)
	{
		if (Context* context = GetContext(i); context && context->GetRootElement() == element)
			return false;
	}
	return true;
}

void AdoptCallerElement(Element* element)
{
	// No effect for elements created by an instancer, since the first instancer set on an element is kept.
	element->SetInstancer(&caller_element_instancer);
}

Rml::String ToString(RmlStringView view)
{
	if (!view.data || view.size == 0)
		return {};
	return Rml::String(view.data, view.size);
}

RmlStringView ToStringView(const Rml::String& string)
{
	RmlStringView view;
	view.data = string.c_str();
	view.size = string.size();
	return view;
}

RmlOwnedString ToOwnedString(const Rml::String& string)
{
	RmlOwnedString owned = {};
	rml_owned_string_set(&owned, ToStringView(string));
	return owned;
}

} // namespace CApi
} // namespace Rml

void rml_owned_string_free(RmlOwnedString str)
{
	if (str.data)
	{
		std::free(str.data);
	}
}

void rml_owned_string_set(RmlOwnedString* out, RmlStringView view)
{
	if (!out)
		return;

	// Copy before freeing the previous value, since the view may point into it.
	char* data = nullptr;
	if (view.data)
	{
		data = static_cast<char*>(std::malloc(view.size + 1));
		if (data)
		{
			std::memcpy(data, view.data, view.size);
			data[view.size] = '\0';
		}
	}

	rml_owned_string_free(*out);
	out->data = data;
	out->size = data ? view.size : 0;
}

bool rml_matrix_is_row_major(void)
{
#ifdef RMLUI_MATRIX_ROW_MAJOR
	return true;
#else
	return false;
#endif
}