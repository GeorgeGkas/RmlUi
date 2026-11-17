#include "Conversions.h"
#include <RmlUi/CApi/Variant.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Dictionary.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Variant.h>

namespace {

// Rml::Variant::GetInto logs an error for conversions without a type converter, and some numeric pairs (such as
// byte to int) have none. Numbers are therefore converted here, and only strings go through the type converters.
template <typename T>
bool GetNumber(const RmlVariant* variant, T* out)
{
	const Rml::Variant& value = *Rml::CApi::FromC(variant);
	switch (value.GetType())
	{
	case Rml::Variant::BOOL: *out = static_cast<T>(value.GetReference<bool>()); return true;
	case Rml::Variant::BYTE: *out = static_cast<T>(value.GetReference<Rml::byte>()); return true;
	case Rml::Variant::FLOAT: *out = static_cast<T>(value.GetReference<float>()); return true;
	case Rml::Variant::DOUBLE: *out = static_cast<T>(value.GetReference<double>()); return true;
	case Rml::Variant::INT: *out = static_cast<T>(value.GetReference<int>()); return true;
	case Rml::Variant::INT64: *out = static_cast<T>(value.GetReference<int64_t>()); return true;
	case Rml::Variant::UINT: *out = static_cast<T>(value.GetReference<unsigned int>()); return true;
	case Rml::Variant::UINT64: *out = static_cast<T>(value.GetReference<uint64_t>()); return true;
	case Rml::Variant::STRING: return value.GetInto(*out);
	default: return false;
	}
}

// Reads a vector or colour stored as 'type', or parsed from a string.
template <typename T, typename C>
bool GetCompound(const RmlVariant* variant, Rml::Variant::Type type, C* out)
{
	const Rml::Variant& value = *Rml::CApi::FromC(variant);
	if (value.GetType() != type && value.GetType() != Rml::Variant::STRING)
		return false;
	T result;
	if (!value.GetInto(result))
		return false;
	*out = Rml::CApi::ToC(result);
	return true;
}

bool HasStringConverter(Rml::Variant::Type type)
{
	switch (type)
	{
	case Rml::Variant::NONE:
	case Rml::Variant::CHAR:
	case Rml::Variant::SCRIPTINTERFACE:
	case Rml::Variant::VOIDPTR: return false;
	default: return true;
	}
}

} // namespace

RmlVariant* rml_variant_create(void)
{
	return Rml::CApi::ToC(new Rml::Variant());
}

void rml_variant_destroy(RmlVariant* variant)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	delete Rml::CApi::FromC(variant);
}

RmlVariant* rml_variant_copy(const RmlVariant* variant)
{
	RMLUI_CAPI_REQUIRE(variant, {});
	return Rml::CApi::ToC(new Rml::Variant(*Rml::CApi::FromC(variant)));
}

void rml_variant_clear(RmlVariant* variant)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	Rml::CApi::FromC(variant)->Clear();
}

RmlVariantType rml_variant_get_type(const RmlVariant* variant)
{
	RMLUI_CAPI_REQUIRE(variant, {});
	return static_cast<RmlVariantType>(Rml::CApi::FromC(variant)->GetType());
}

void rml_variant_set_bool(RmlVariant* variant, bool value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_bool(const RmlVariant* variant, bool* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_int(RmlVariant* variant, int value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_int(const RmlVariant* variant, int* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_int64(RmlVariant* variant, int64_t value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_int64(const RmlVariant* variant, int64_t* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_uint(RmlVariant* variant, unsigned int value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_uint(const RmlVariant* variant, unsigned int* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_uint64(RmlVariant* variant, uint64_t value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_uint64(const RmlVariant* variant, uint64_t* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_float(RmlVariant* variant, float value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_float(const RmlVariant* variant, float* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_double(RmlVariant* variant, double value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_double(const RmlVariant* variant, double* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_string(RmlVariant* variant, RmlStringView value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::ToString(value);
}

RmlStringView rml_variant_get_string(const RmlVariant* variant)
{
	if (!variant)
		return {};

	const Rml::Variant* value = Rml::CApi::FromC(variant);
	if (value->GetType() != Rml::Variant::STRING)
		return {};

	const Rml::String& string = value->GetReference<Rml::String>();
	return Rml::CApi::ToStringView(string);
}

void rml_variant_set_vector2f(RmlVariant* variant, RmlVector2f value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_vector2f(const RmlVariant* variant, RmlVector2f* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetCompound<Rml::Vector2f>(variant, Rml::Variant::VECTOR2, out);
}

void rml_variant_set_vector2i(RmlVariant* variant, RmlVector2i value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	const Rml::Vector2i vec = Rml::CApi::FromC(value);
	*Rml::CApi::FromC(variant) = Rml::Vector2f(static_cast<float>(vec.x), static_cast<float>(vec.y));
}

bool rml_variant_get_vector2i(const RmlVariant* variant, RmlVector2i* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	RmlVector2f result;
	if (!GetCompound<Rml::Vector2f>(variant, Rml::Variant::VECTOR2, &result))
		return false;
	out->x = static_cast<int>(result.x);
	out->y = static_cast<int>(result.y);
	return true;
}

void rml_variant_set_vector3f(RmlVariant* variant, RmlVector3f value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_vector3f(const RmlVariant* variant, RmlVector3f* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetCompound<Rml::Vector3f>(variant, Rml::Variant::VECTOR3, out);
}

void rml_variant_set_vector4f(RmlVariant* variant, RmlVector4f value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_vector4f(const RmlVariant* variant, RmlVector4f* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetCompound<Rml::Vector4f>(variant, Rml::Variant::VECTOR4, out);
}

void rml_variant_set_colourb(RmlVariant* variant, RmlColourb value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_colourb(const RmlVariant* variant, RmlColourb* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetCompound<Rml::Colourb>(variant, Rml::Variant::COLOURB, out);
}

void rml_variant_set_colourf(RmlVariant* variant, RmlColourf value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_colourf(const RmlVariant* variant, RmlColourf* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetCompound<Rml::Colourf>(variant, Rml::Variant::COLOURF, out);
}

void rml_variant_set_byte(RmlVariant* variant, unsigned char value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = static_cast<Rml::byte>(value);
}

bool rml_variant_get_byte(const RmlVariant* variant, unsigned char* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	return GetNumber(variant, out);
}

void rml_variant_set_char(RmlVariant* variant, char value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_char(const RmlVariant* variant, char* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	const Rml::Variant& value = *Rml::CApi::FromC(variant);
	if (value.GetType() != Rml::Variant::CHAR)
		return false;
	*out = value.GetReference<char>();
	return true;
}

void rml_variant_set_voidptr(RmlVariant* variant, void* value)
{
	RMLUI_CAPI_REQUIRE_VOID(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_voidptr(const RmlVariant* variant, void** out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	const Rml::Variant& value = *Rml::CApi::FromC(variant);
	if (value.GetType() != Rml::Variant::VOIDPTR)
		return false;
	*out = value.GetReference<void*>();
	return true;
}

bool rml_variant_to_string(const RmlVariant* variant, RmlOwnedString* out)
{
	RMLUI_CAPI_REQUIRE(variant && out, {});
	const Rml::Variant& value = *Rml::CApi::FromC(variant);
	Rml::String result;
	if (!HasStringConverter(value.GetType()) || !value.GetInto(result))
		return false;
	rml_owned_string_set(out, Rml::CApi::ToStringView(result));
	return true;
}

RmlDictionary* rml_dictionary_create(void)
{
	return Rml::CApi::ToC(new Rml::Dictionary());
}

void rml_dictionary_destroy(RmlDictionary* dictionary)
{
	delete Rml::CApi::FromC(dictionary);
}

void rml_dictionary_set(RmlDictionary* dictionary, RmlStringView key, const RmlVariant* value)
{
	if (!dictionary || !value)
		return;

	Rml::Dictionary& target = *Rml::CApi::FromC(dictionary);
	Rml::String key_string = Rml::CApi::ToString(key);
	auto it = target.find(key_string);
	if (it != target.end())
		it->second = *Rml::CApi::FromC(value);
	else
		target.emplace(std::move(key_string), *Rml::CApi::FromC(value));
}

const RmlVariant* rml_dictionary_get(const RmlDictionary* dictionary, RmlStringView key)
{
	RMLUI_CAPI_REQUIRE(dictionary, {});
	const Rml::Variant* value = Rml::GetIf(*Rml::CApi::FromC(dictionary), Rml::CApi::ToString(key));
	return Rml::CApi::ToC(value);
}

size_t rml_dictionary_size(const RmlDictionary* dictionary)
{
	RMLUI_CAPI_REQUIRE(dictionary, {});
	return Rml::CApi::FromC(dictionary)->size();
}

bool rml_dictionary_remove(RmlDictionary* dictionary, RmlStringView key)
{
	RMLUI_CAPI_REQUIRE(dictionary, {});
	return Rml::CApi::FromC(dictionary)->erase(Rml::CApi::ToString(key)) > 0;
}

void rml_dictionary_clear(RmlDictionary* dictionary)
{
	RMLUI_CAPI_REQUIRE_VOID(dictionary);
	Rml::CApi::FromC(dictionary)->clear();
}

void rml_dictionary_for_each(const RmlDictionary* dictionary, RmlDictionaryForEachFunc callback, void* user_data)
{
	RMLUI_CAPI_REQUIRE_VOID(dictionary);
	if (!callback)
		return;

	for (const auto& entry : *Rml::CApi::FromC(dictionary))
		callback(user_data, Rml::CApi::ToStringView(entry.first), Rml::CApi::ToC(&entry.second));
}

size_t rml_variant_list_size(const RmlVariantList* list)
{
	RMLUI_CAPI_REQUIRE(list, {});
	return Rml::CApi::FromC(list)->size();
}

const RmlVariant* rml_variant_list_get(const RmlVariantList* list, size_t index)
{
	RMLUI_CAPI_REQUIRE(list, {});
	const Rml::VariantList& variant_list = *Rml::CApi::FromC(list);
	if (index >= variant_list.size())
		return nullptr;
	return Rml::CApi::ToC(&variant_list[index]);
}

RmlElementList* rml_element_list_create(void)
{
	return Rml::CApi::ToC(new Rml::ElementList());
}

void rml_element_list_destroy(RmlElementList* list)
{
	RMLUI_CAPI_REQUIRE_VOID(list);
	delete Rml::CApi::FromC(list);
}

void rml_element_list_clear(RmlElementList* list)
{
	RMLUI_CAPI_REQUIRE_VOID(list);
	Rml::CApi::FromC(list)->clear();
}

size_t rml_element_list_size(const RmlElementList* list)
{
	RMLUI_CAPI_REQUIRE(list, {});
	return Rml::CApi::FromC(list)->size();
}

RmlElement* rml_element_list_get(RmlElementList* list, size_t index)
{
	RMLUI_CAPI_REQUIRE(list, {});
	Rml::ElementList& elements = *Rml::CApi::FromC(list);
	if (index >= elements.size())
		return nullptr;
	return Rml::CApi::ToC(elements[index]);
}

RmlStringList* rml_string_list_create(void)
{
	return Rml::CApi::ToC(new Rml::StringList());
}

void rml_string_list_destroy(RmlStringList* list)
{
	RMLUI_CAPI_REQUIRE_VOID(list);
	delete Rml::CApi::FromC(list);
}

size_t rml_string_list_size(const RmlStringList* list)
{
	RMLUI_CAPI_REQUIRE(list, {});
	return Rml::CApi::FromC(list)->size();
}

RmlStringView rml_string_list_get(const RmlStringList* list, size_t index)
{
	RMLUI_CAPI_REQUIRE(list, {});
	const Rml::StringList& strings = *Rml::CApi::FromC(list);
	if (index >= strings.size())
		return {};
	return Rml::CApi::ToStringView(strings[index]);
}

void rml_string_list_push(RmlStringList* list, RmlStringView value)
{
	RMLUI_CAPI_REQUIRE_VOID(list);
	Rml::CApi::FromC(list)->push_back(Rml::CApi::ToString(value));
}

void rml_string_list_clear(RmlStringList* list)
{
	RMLUI_CAPI_REQUIRE_VOID(list);
	Rml::CApi::FromC(list)->clear();
}