#include "Conversions.h"
#include <cstdlib>
#include <cstring>

namespace Rml {
namespace CApi {

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

	rml_owned_string_free(*out);
	out->data = nullptr;
	out->size = 0;

	if (!view.data)
		return;

	char* data = static_cast<char*>(std::malloc(view.size + 1));
	if (!data)
		return;

	std::memcpy(data, view.data, view.size);
	data[view.size] = '\0';
	out->data = data;
	out->size = view.size;
}

bool rml_matrix_is_row_major(void)
{
#ifdef RMLUI_MATRIX_ROW_MAJOR
	return true;
#else
	return false;
#endif
}