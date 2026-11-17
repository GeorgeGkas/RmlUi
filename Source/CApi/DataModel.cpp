#include "Conversions.h"
#include <RmlUi/CApi/DataModel.h>
#include <RmlUi/CApi/Variant.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/DataTypes.h>
#include <RmlUi/Core/DataVariable.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/Variant.h>

namespace {

class DataTypeDefinitionAdapter final : public Rml::VariableDefinition {
public:
	DataTypeDefinitionAdapter(Rml::DataVariableType type, const RmlDataTypeDefinitionCallbacks& callbacks, void* user_data) :
		Rml::VariableDefinition(type), callbacks(callbacks), user_data(user_data)
	{}

	bool Get(void* ptr, Rml::Variant& variant) override
	{
		if (!callbacks.get)
			return false;
		return callbacks.get(user_data, ptr, Rml::CApi::ToC(&variant));
	}

	bool Set(void* ptr, const Rml::Variant& variant) override
	{
		if (!callbacks.set)
			return false;
		return callbacks.set(user_data, ptr, Rml::CApi::ToC(&variant));
	}

	int Size(void* ptr) override
	{
		if (!callbacks.size)
			return Rml::VariableDefinition::Size(ptr);
		return callbacks.size(user_data, ptr);
	}

	Rml::DataVariable Child(void* ptr, const Rml::DataAddressEntry& address) override
	{
		RmlDataVariable child = {};
		if (address.index >= 0)
		{
			if (!callbacks.child_by_index)
				return {};
			child = callbacks.child_by_index(user_data, ptr, address.index);
		}
		else
		{
			if (!callbacks.child_by_name)
				return {};
			child = callbacks.child_by_name(user_data, ptr, Rml::CApi::ToStringView(address.name));
		}

		auto* definition = reinterpret_cast<Rml::VariableDefinition*>(child.definition);
		if (!definition)
			return {};
		return Rml::DataVariable(definition, child.ptr);
	}

	Rml::StringList ReflectMemberNames() override
	{
		if (!callbacks.member_names)
			return {};

		RmlStringList* names = rml_string_list_create();
		callbacks.member_names(user_data, names);
		Rml::StringList result = *Rml::CApi::FromC(names);
		rml_string_list_destroy(names);
		return result;
	}

private:
	RmlDataTypeDefinitionCallbacks callbacks;
	void* user_data;
};

template <typename T>
bool BindScalar(RmlDataModelConstructor constructor, RmlStringView name, T* ptr)
{
	Rml::DataModelConstructor ctor = Rml::CApi::FromC(constructor);
	if (!ctor || !ptr)
		return false;
	return ctor.Bind(Rml::CApi::ToString(name), ptr);
}

} // namespace

bool rml_data_model_bind_bool(RmlDataModelConstructor constructor, RmlStringView name, bool* ptr)
{
	return BindScalar(constructor, name, ptr);
}

bool rml_data_model_bind_int(RmlDataModelConstructor constructor, RmlStringView name, int* ptr)
{
	return BindScalar(constructor, name, ptr);
}

bool rml_data_model_bind_int64(RmlDataModelConstructor constructor, RmlStringView name, int64_t* ptr)
{
	return BindScalar(constructor, name, ptr);
}

bool rml_data_model_bind_uint64(RmlDataModelConstructor constructor, RmlStringView name, uint64_t* ptr)
{
	return BindScalar(constructor, name, ptr);
}

bool rml_data_model_bind_float(RmlDataModelConstructor constructor, RmlStringView name, float* ptr)
{
	return BindScalar(constructor, name, ptr);
}

bool rml_data_model_bind_double(RmlDataModelConstructor constructor, RmlStringView name, double* ptr)
{
	return BindScalar(constructor, name, ptr);
}

bool rml_data_model_bind_func(RmlDataModelConstructor constructor, RmlStringView name, RmlDataModelGetFunc get, RmlDataModelSetFunc set,
	void* user_data)
{
	Rml::DataModelConstructor ctor = Rml::CApi::FromC(constructor);
	if (!ctor || !get)
		return false;

	Rml::DataGetFunc get_func = [get, user_data](Rml::Variant& variant) { get(user_data, Rml::CApi::ToC(&variant)); };
	Rml::DataSetFunc set_func;
	if (set)
		set_func = [set, user_data](const Rml::Variant& variant) { set(user_data, Rml::CApi::ToC(&variant)); };

	return ctor.BindFunc(Rml::CApi::ToString(name), std::move(get_func), std::move(set_func));
}

RmlDataTypeDefinition* rml_data_type_definition_create(RmlDataVariableType type, const RmlDataTypeDefinitionCallbacks* callbacks, void* user_data)
{
	if (!callbacks)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "Data type definition requires callbacks.");
		return nullptr;
	}

	auto* adapter = new DataTypeDefinitionAdapter(static_cast<Rml::DataVariableType>(type), *callbacks, user_data);
	return reinterpret_cast<RmlDataTypeDefinition*>(adapter);
}

void rml_data_type_definition_destroy(RmlDataTypeDefinition* definition)
{
	delete reinterpret_cast<DataTypeDefinitionAdapter*>(definition);
}

bool rml_data_model_bind_custom(RmlDataModelConstructor constructor, RmlStringView name, RmlDataTypeDefinition* definition, void* ptr)
{
	Rml::DataModelConstructor ctor = Rml::CApi::FromC(constructor);
	auto* variable_definition = reinterpret_cast<Rml::VariableDefinition*>(definition);
	if (!ctor || !variable_definition || !ptr)
		return false;

	return ctor.BindCustomDataVariable(Rml::CApi::ToString(name), Rml::DataVariable(variable_definition, ptr));
}

bool rml_data_model_bind_event_callback(RmlDataModelConstructor constructor, RmlStringView name, RmlDataModelEventFunc callback, void* user_data)
{
	Rml::DataModelConstructor ctor = Rml::CApi::FromC(constructor);
	if (!ctor || !callback)
		return false;

	return ctor.BindEventCallback(Rml::CApi::ToString(name),
		[callback, user_data](Rml::DataModelHandle handle, Rml::Event& event, const Rml::VariantList& arguments) {
			callback(user_data, Rml::CApi::ToC(handle), Rml::CApi::ToC(&event), Rml::CApi::ToC(&arguments));
		});
}

void rml_data_model_register_transform_func(RmlDataModelConstructor constructor, RmlStringView name, RmlDataModelTransformFunc callback,
	void* user_data)
{
	Rml::DataModelConstructor ctor = Rml::CApi::FromC(constructor);
	if (!ctor || !callback)
		return;

	ctor.RegisterTransformFunc(Rml::CApi::ToString(name), [callback, user_data](const Rml::VariantList& arguments) {
		Rml::Variant result;
		callback(user_data, Rml::CApi::ToC(&arguments), Rml::CApi::ToC(&result));
		return result;
	});
}
