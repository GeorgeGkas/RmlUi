#include "Conversions.h"
#include <RmlUi/CApi/Variant.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Dictionary.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Variant.h>

RmlVariant* rml_variant_create(void)
{
	return Rml::CApi::ToC(new Rml::Variant());
}

void rml_variant_destroy(RmlVariant* variant)
{
	RMLUI_ASSERT(variant);
	delete Rml::CApi::FromC(variant);
}

RmlVariant* rml_variant_copy(const RmlVariant* variant)
{
	RMLUI_ASSERT(variant);
	return Rml::CApi::ToC(new Rml::Variant(*Rml::CApi::FromC(variant)));
}

void rml_variant_clear(RmlVariant* variant)
{
	RMLUI_ASSERT(variant);
	Rml::CApi::FromC(variant)->Clear();
}

RmlVariantType rml_variant_get_type(const RmlVariant* variant)
{
	RMLUI_ASSERT(variant);
	return static_cast<RmlVariantType>(Rml::CApi::FromC(variant)->GetType());
}

void rml_variant_set_bool(RmlVariant* variant, bool value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_bool(const RmlVariant* variant, bool* out)
{
	RMLUI_ASSERT(variant && out);
	return Rml::CApi::FromC(variant)->GetInto(*out);
}

void rml_variant_set_int(RmlVariant* variant, int value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_int(const RmlVariant* variant, int* out)
{
	RMLUI_ASSERT(variant && out);
	return Rml::CApi::FromC(variant)->GetInto(*out);
}

void rml_variant_set_int64(RmlVariant* variant, int64_t value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_int64(const RmlVariant* variant, int64_t* out)
{
	RMLUI_ASSERT(variant && out);
	return Rml::CApi::FromC(variant)->GetInto(*out);
}

void rml_variant_set_uint(RmlVariant* variant, unsigned int value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_uint(const RmlVariant* variant, unsigned int* out)
{
	RMLUI_ASSERT(variant && out);
	return Rml::CApi::FromC(variant)->GetInto(*out);
}

void rml_variant_set_uint64(RmlVariant* variant, uint64_t value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_uint64(const RmlVariant* variant, uint64_t* out)
{
	RMLUI_ASSERT(variant && out);
	return Rml::CApi::FromC(variant)->GetInto(*out);
}

void rml_variant_set_float(RmlVariant* variant, float value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_float(const RmlVariant* variant, float* out)
{
	RMLUI_ASSERT(variant && out);
	return Rml::CApi::FromC(variant)->GetInto(*out);
}

void rml_variant_set_double(RmlVariant* variant, double value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = value;
}

bool rml_variant_get_double(const RmlVariant* variant, double* out)
{
	RMLUI_ASSERT(variant && out);
	return Rml::CApi::FromC(variant)->GetInto(*out);
}

void rml_variant_set_string(RmlVariant* variant, RmlStringView value)
{
	RMLUI_ASSERT(variant);
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
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_vector2f(const RmlVariant* variant, RmlVector2f* out)
{
	RMLUI_ASSERT(variant && out);
	Rml::Vector2f result;
	if (!Rml::CApi::FromC(variant)->GetInto(result))
		return false;
	*out = Rml::CApi::ToC(result);
	return true;
}

void rml_variant_set_vector2i(RmlVariant* variant, RmlVector2i value)
{
	RMLUI_ASSERT(variant);
	const Rml::Vector2i vec = Rml::CApi::FromC(value);
	*Rml::CApi::FromC(variant) = Rml::Vector2f(static_cast<float>(vec.x), static_cast<float>(vec.y));
}

bool rml_variant_get_vector2i(const RmlVariant* variant, RmlVector2i* out)
{
	RMLUI_ASSERT(variant && out);
	Rml::Vector2f result;
	if (!Rml::CApi::FromC(variant)->GetInto(result))
		return false;
	Rml::Vector2i as_int(static_cast<int>(result.x), static_cast<int>(result.y));
	*out = Rml::CApi::ToC(as_int);
	return true;
}

void rml_variant_set_vector3f(RmlVariant* variant, RmlVector3f value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_vector3f(const RmlVariant* variant, RmlVector3f* out)
{
	RMLUI_ASSERT(variant && out);
	Rml::Vector3f result;
	if (!Rml::CApi::FromC(variant)->GetInto(result))
		return false;
	*out = Rml::CApi::ToC(result);
	return true;
}

void rml_variant_set_vector4f(RmlVariant* variant, RmlVector4f value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_vector4f(const RmlVariant* variant, RmlVector4f* out)
{
	RMLUI_ASSERT(variant && out);
	Rml::Vector4f result;
	if (!Rml::CApi::FromC(variant)->GetInto(result))
		return false;
	*out = Rml::CApi::ToC(result);
	return true;
}

void rml_variant_set_colourb(RmlVariant* variant, RmlColourb value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_colourb(const RmlVariant* variant, RmlColourb* out)
{
	RMLUI_ASSERT(variant && out);
	Rml::Colourb result;
	if (!Rml::CApi::FromC(variant)->GetInto(result))
		return false;
	*out = Rml::CApi::ToC(result);
	return true;
}

void rml_variant_set_colourf(RmlVariant* variant, RmlColourf value)
{
	RMLUI_ASSERT(variant);
	*Rml::CApi::FromC(variant) = Rml::CApi::FromC(value);
}

bool rml_variant_get_colourf(const RmlVariant* variant, RmlColourf* out)
{
	RMLUI_ASSERT(variant && out);
	Rml::Colourf result;
	if (!Rml::CApi::FromC(variant)->GetInto(result))
		return false;
	*out = Rml::CApi::ToC(result);
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
	RMLUI_ASSERT(dictionary);
	const Rml::Variant* value = Rml::GetIf(*Rml::CApi::FromC(dictionary), Rml::CApi::ToString(key));
	return Rml::CApi::ToC(value);
}

size_t rml_dictionary_size(const RmlDictionary* dictionary)
{
	RMLUI_ASSERT(dictionary);
	return Rml::CApi::FromC(dictionary)->size();
}

void rml_dictionary_for_each(const RmlDictionary* dictionary, RmlDictionaryForEachFunc callback, void* user_data)
{
	RMLUI_ASSERT(dictionary);
	if (!callback)
		return;

	for (const auto& entry : *Rml::CApi::FromC(dictionary))
		callback(user_data, Rml::CApi::ToStringView(entry.first), Rml::CApi::ToC(&entry.second));
}

size_t rml_variant_list_size(const RmlVariantList* list)
{
	RMLUI_ASSERT(list);
	return Rml::CApi::FromC(list)->size();
}

const RmlVariant* rml_variant_list_get(const RmlVariantList* list, size_t index)
{
	RMLUI_ASSERT(list);
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
	RMLUI_ASSERT(list);
	delete Rml::CApi::FromC(list);
}

void rml_element_list_clear(RmlElementList* list)
{
	RMLUI_ASSERT(list);
	Rml::CApi::FromC(list)->clear();
}

size_t rml_element_list_size(const RmlElementList* list)
{
	RMLUI_ASSERT(list);
	return Rml::CApi::FromC(list)->size();
}

RmlElement* rml_element_list_get(RmlElementList* list, size_t index)
{
	RMLUI_ASSERT(list);
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
	RMLUI_ASSERT(list);
	delete Rml::CApi::FromC(list);
}

size_t rml_string_list_size(const RmlStringList* list)
{
	RMLUI_ASSERT(list);
	return Rml::CApi::FromC(list)->size();
}

RmlStringView rml_string_list_get(const RmlStringList* list, size_t index)
{
	RMLUI_ASSERT(list);
	const Rml::StringList& strings = *Rml::CApi::FromC(list);
	if (index >= strings.size())
		return {};
	return Rml::CApi::ToStringView(strings[index]);
}

void rml_string_list_push(RmlStringList* list, RmlStringView value)
{
	RMLUI_ASSERT(list);
	Rml::CApi::FromC(list)->push_back(Rml::CApi::ToString(value));
}