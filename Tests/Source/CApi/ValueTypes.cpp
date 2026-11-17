#include "Harness.h"
#include <cstdint>
#include <doctest.h>
#include <limits>
#include <map>

using namespace CApiTest;

TEST_CASE("capi.values.variant_scalar_round_trips")
{
	RmlVariant* variant = rml_variant_create();
	REQUIRE(variant);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_NONE);

	bool b = false;
	rml_variant_set_bool(variant, true);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_BOOL);
	CHECK(rml_variant_get_bool(variant, &b));
	CHECK(b);

	int i = 0;
	rml_variant_set_int(variant, -12);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_INT);
	CHECK(rml_variant_get_int(variant, &i));
	CHECK(i == -12);

	int64_t i64 = 0;
	rml_variant_set_int64(variant, std::numeric_limits<int64_t>::min());
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_INT64);
	CHECK(rml_variant_get_int64(variant, &i64));
	CHECK(i64 == std::numeric_limits<int64_t>::min());

	unsigned int u = 0;
	rml_variant_set_uint(variant, 4000000000u);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_UINT);
	CHECK(rml_variant_get_uint(variant, &u));
	CHECK(u == 4000000000u);

	uint64_t u64 = 0;
	rml_variant_set_uint64(variant, std::numeric_limits<uint64_t>::max());
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_UINT64);
	CHECK(rml_variant_get_uint64(variant, &u64));
	CHECK(u64 == std::numeric_limits<uint64_t>::max());

	float f = 0.f;
	rml_variant_set_float(variant, 1.5f);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_FLOAT);
	CHECK(rml_variant_get_float(variant, &f));
	CHECK(f == 1.5f);

	double d = 0.0;
	rml_variant_set_double(variant, 2.25);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_DOUBLE);
	CHECK(rml_variant_get_double(variant, &d));
	CHECK(d == 2.25);

	rml_variant_set_string(variant, rml_sv("text"));
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_STRING);
	CHECK(ToString(rml_variant_get_string(variant)) == "text");

	// Embedded NUL bytes survive because views carry an explicit size.
	const char with_nul[] = {'a', '\0', 'b'};
	rml_variant_set_string(variant, RmlStringView{with_nul, 3});
	const RmlStringView view = rml_variant_get_string(variant);
	CHECK(view.size == 3);
	CHECK(std::string(view.data, view.size) == std::string(with_nul, 3));

	rml_variant_clear(variant);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_NONE);
	CHECK_FALSE(rml_variant_get_int(variant, &i));
	CHECK(rml_variant_get_string(variant).data == nullptr);
	CHECK(rml_variant_get_string(nullptr).data == nullptr);

	rml_variant_destroy(variant);
}

TEST_CASE("capi.values.variant_vector_and_colour_round_trips")
{
	RmlVariant* variant = rml_variant_create();

	RmlVector2f v2 = {};
	rml_variant_set_vector2f(variant, RmlVector2f{1.f, 2.f});
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_VECTOR2);
	CHECK(rml_variant_get_vector2f(variant, &v2));
	CHECK(v2.x == 1.f);
	CHECK(v2.y == 2.f);

	// Integer vectors are stored as float vectors.
	RmlVector2i v2i = {};
	rml_variant_set_vector2i(variant, RmlVector2i{-3, 9});
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_VECTOR2);
	CHECK(rml_variant_get_vector2i(variant, &v2i));
	CHECK(v2i.x == -3);
	CHECK(v2i.y == 9);

	RmlVector3f v3 = {};
	rml_variant_set_vector3f(variant, RmlVector3f{1.f, 2.f, 3.f});
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_VECTOR3);
	CHECK(rml_variant_get_vector3f(variant, &v3));
	CHECK(v3.z == 3.f);

	RmlVector4f v4 = {};
	rml_variant_set_vector4f(variant, RmlVector4f{1.f, 2.f, 3.f, 4.f});
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_VECTOR4);
	CHECK(rml_variant_get_vector4f(variant, &v4));
	CHECK(v4.w == 4.f);

	RmlColourb cb = {};
	rml_variant_set_colourb(variant, RmlColourb{10, 20, 30, 40});
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_COLOURB);
	CHECK(rml_variant_get_colourb(variant, &cb));
	CHECK(cb.red == 10);
	CHECK(cb.green == 20);
	CHECK(cb.blue == 30);
	CHECK(cb.alpha == 40);

	RmlColourf cf = {};
	rml_variant_set_colourf(variant, RmlColourf{0.1f, 0.2f, 0.3f, 0.4f});
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_COLOURF);
	CHECK(rml_variant_get_colourf(variant, &cf));
	CHECK(cf.alpha == doctest::Approx(0.4f));

	// Getters on an empty variant fail and leave the output untouched.
	rml_variant_clear(variant);
	RmlVector3f untouched = {7.f, 7.f, 7.f};
	CHECK_FALSE(rml_variant_get_vector3f(variant, &untouched));
	CHECK(untouched.x == 7.f);

	rml_variant_destroy(variant);
}

TEST_CASE("capi.values.variant_conversions_and_copy")
{
	RmlVariant* variant = rml_variant_create();

	rml_variant_set_string(variant, rml_sv("42"));
	int i = 0;
	CHECK(rml_variant_get_int(variant, &i));
	CHECK(i == 42);
	double d = 0.0;
	CHECK(rml_variant_get_double(variant, &d));
	CHECK(d == 42.0);

	rml_variant_set_int(variant, 1);
	bool b = false;
	CHECK(rml_variant_get_bool(variant, &b));
	CHECK(b);
	float f = 0.f;
	CHECK(rml_variant_get_float(variant, &f));
	CHECK(f == 1.f);

	// The borrowed string getter only returns string variants; it cannot convert without storage.
	CHECK(rml_variant_get_string(variant).data == nullptr);

	rml_variant_set_string(variant, rml_sv("original"));
	RmlVariant* copy = rml_variant_copy(variant);
	REQUIRE(copy);
	rml_variant_set_string(variant, rml_sv("changed"));
	CHECK(ToString(rml_variant_get_string(copy)) == "original");

	rml_variant_destroy(copy);
	rml_variant_destroy(variant);
}

TEST_CASE("capi.values.dictionary_operations")
{
	RmlDictionary* dictionary = rml_dictionary_create();
	REQUIRE(dictionary);
	CHECK(rml_dictionary_size(dictionary) == 0);
	CHECK(rml_dictionary_get(dictionary, rml_sv("missing")) == nullptr);

	RmlVariant* value = rml_variant_create();
	rml_variant_set_int(value, 1);
	rml_dictionary_set(dictionary, rml_sv("one"), value);
	rml_variant_set_string(value, rml_sv("two"));
	rml_dictionary_set(dictionary, rml_sv("two"), value);
	CHECK(rml_dictionary_size(dictionary) == 2);

	// Overwriting keeps the size and the dictionary holds its own copy.
	rml_variant_set_int(value, 11);
	rml_dictionary_set(dictionary, rml_sv("one"), value);
	rml_variant_set_int(value, 99);
	CHECK(rml_dictionary_size(dictionary) == 2);
	int one = 0;
	CHECK(rml_variant_get_int(rml_dictionary_get(dictionary, rml_sv("one")), &one));
	CHECK(one == 11);

	rml_dictionary_set(dictionary, rml_sv("ignored"), nullptr);
	rml_dictionary_set(nullptr, rml_sv("ignored"), value);
	CHECK(rml_dictionary_size(dictionary) == 2);

	std::map<std::string, RmlVariantType> seen;
	rml_dictionary_for_each(
		dictionary,
		[](void* user_data, RmlStringView key, const RmlVariant* entry) {
			(*static_cast<std::map<std::string, RmlVariantType>*>(user_data))[ToString(key)] = rml_variant_get_type(entry);
		},
		&seen);
	CHECK(seen.size() == 2);
	CHECK(seen["one"] == RML_VARIANT_TYPE_INT);
	CHECK(seen["two"] == RML_VARIANT_TYPE_STRING);
	rml_dictionary_for_each(dictionary, nullptr, nullptr);

	rml_variant_destroy(value);
	rml_dictionary_destroy(dictionary);
	rml_dictionary_destroy(nullptr);
}

TEST_CASE("capi.values.string_and_element_lists")
{
	RmlStringList* strings = rml_string_list_create();
	CHECK(rml_string_list_size(strings) == 0);
	rml_string_list_push(strings, rml_sv("a"));
	rml_string_list_push(strings, rml_sv("bc"));
	CHECK(rml_string_list_size(strings) == 2);
	CHECK(ToString(rml_string_list_get(strings, 1)) == "bc");
	CHECK(rml_string_list_get(strings, 2).data == nullptr);
	rml_string_list_destroy(strings);

	RmlElementList* elements = rml_element_list_create();
	CHECK(rml_element_list_size(elements) == 0);
	CHECK(rml_element_list_get(elements, 0) == nullptr);
	rml_element_list_clear(elements);
	rml_element_list_destroy(elements);
}

TEST_CASE("capi.values.owned_strings")
{
	RmlOwnedString owned = {};
	rml_owned_string_set(&owned, rml_sv("hello"));
	REQUIRE(owned.data);
	CHECK(owned.size == 5);
	CHECK(owned.data[5] == '\0');

	// Setting again frees the previous allocation.
	rml_owned_string_set(&owned, rml_sv("hi"));
	CHECK(std::string(owned.data, owned.size) == "hi");

	// An empty view still allocates, so empty and missing strings can be told apart.
	rml_owned_string_set(&owned, RmlStringView{"", 0});
	REQUIRE(owned.data);
	CHECK(owned.size == 0);

	rml_owned_string_set(&owned, RmlStringView{nullptr, 0});
	CHECK(owned.data == nullptr);
	CHECK(owned.size == 0);
	rml_owned_string_free(owned);
	rml_owned_string_set(nullptr, rml_sv("ignored"));

	CHECK(ToString(rml_sv(nullptr)).empty());
	CHECK(rml_sv(nullptr).size == 0);
}

TEST_CASE("capi.values.empty_and_missing_strings_from_elements")
{
	Session session("values_strings");
	session.Load("<rml><body><div id='d' data-empty=''/></body></rml>");
	RmlElement* div = session.ById("d");
	REQUIRE(div);

	RmlOwnedString empty = rml_element_get_attribute_string(div, rml_sv("data-empty"));
	CHECK(empty.data != nullptr);
	CHECK(empty.size == 0);
	rml_owned_string_free(empty);

	RmlOwnedString missing = rml_element_get_attribute_string(div, rml_sv("data-missing"));
	CHECK(missing.data == nullptr);
}

TEST_CASE("capi.values.version_matrix_layout_and_logging")
{
	const std::string version = Take(rml_get_version());
	REQUIRE_FALSE(version.empty());
	CHECK(version[0] >= '0');
	CHECK(version[0] <= '9');

#ifdef RMLUI_MATRIX_ROW_MAJOR
	CHECK(rml_matrix_is_row_major());
#else
	CHECK_FALSE(rml_matrix_is_row_major());
#endif

	Session session("values_log");
	const size_t before = session.system_log.messages.size();
	const RmlLogType types[] = {RML_LOG_TYPE_ALWAYS, RML_LOG_TYPE_WARNING, RML_LOG_TYPE_INFO};
	for (RmlLogType type : types)
		rml_log_message(type, rml_sv("capi log probe"));
	REQUIRE(session.system_log.messages.size() == before + 3);
	for (size_t i = 0; i < 3; i++)
	{
		CHECK(session.system_log.messages[before + i].first == types[i]);
		CHECK(session.system_log.messages[before + i].second == "capi log probe");
	}
}

TEST_CASE("capi.values.byte_char_and_pointer_round_trips")
{
	RmlVariant* variant = rml_variant_create();

	unsigned char byte = 0;
	rml_variant_set_byte(variant, 200);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_BYTE);
	CHECK(rml_variant_get_byte(variant, &byte));
	CHECK(byte == 200);
	int i = 0;
	CHECK(rml_variant_get_int(variant, &i));
	CHECK(i == 200);

	char c = 0;
	rml_variant_set_char(variant, 'q');
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_CHAR);
	CHECK(rml_variant_get_char(variant, &c));
	CHECK(c == 'q');
	CHECK_FALSE(rml_variant_get_byte(variant, &byte));

	int target = 0;
	void* pointer = nullptr;
	rml_variant_set_voidptr(variant, &target);
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_VOIDPTR);
	CHECK(rml_variant_get_voidptr(variant, &pointer));
	CHECK(pointer == &target);
	CHECK_FALSE(rml_variant_get_char(variant, &c));

	rml_variant_set_int(variant, 5);
	CHECK(rml_variant_get_byte(variant, &byte));
	CHECK(byte == 5);
	CHECK_FALSE(rml_variant_get_char(variant, &c));
	CHECK_FALSE(rml_variant_get_voidptr(variant, &pointer));
	CHECK(pointer == &target);

	rml_variant_destroy(variant);
}

TEST_CASE("capi.values.variant_getters_reject_unconvertible_values_without_logging")
{
	Session session("values_getters");
	const size_t before = session.system_log.messages.size();

	struct Source {
		void (*set)(RmlVariant*);
		bool numeric;
	};
	const Source sources[] = {
		{[](RmlVariant*) {}, false},
		{[](RmlVariant* v) { rml_variant_set_bool(v, true); }, true},
		{[](RmlVariant* v) { rml_variant_set_byte(v, 7); }, true},
		{[](RmlVariant* v) { rml_variant_set_char(v, 'x'); }, false},
		{[](RmlVariant* v) { rml_variant_set_int(v, -3); }, true},
		{[](RmlVariant* v) { rml_variant_set_int64(v, 1 << 20); }, true},
		{[](RmlVariant* v) { rml_variant_set_uint(v, 3); }, true},
		{[](RmlVariant* v) { rml_variant_set_uint64(v, 9); }, true},
		{[](RmlVariant* v) { rml_variant_set_float(v, 1.5f); }, true},
		{[](RmlVariant* v) { rml_variant_set_double(v, 2.5); }, true},
		{[](RmlVariant* v) { rml_variant_set_string(v, rml_sv("not a number")); }, false},
		{[](RmlVariant* v) { rml_variant_set_voidptr(v, v); }, false},
		{[](RmlVariant* v) { rml_variant_set_vector2f(v, RmlVector2f{1, 2}); }, false},
		{[](RmlVariant* v) { rml_variant_set_vector3f(v, RmlVector3f{1, 2, 3}); }, false},
		{[](RmlVariant* v) { rml_variant_set_vector4f(v, RmlVector4f{1, 2, 3, 4}); }, false},
		{[](RmlVariant* v) { rml_variant_set_colourb(v, RmlColourb{1, 2, 3, 4}); }, false},
		{[](RmlVariant* v) { rml_variant_set_colourf(v, RmlColourf{0.1f, 0.2f, 0.3f, 0.4f}); }, false},
	};

	for (const Source& source : sources)
	{
		RmlVariant* variant = rml_variant_create();
		source.set(variant);
		bool b;
		int i;
		int64_t i64;
		unsigned int u;
		uint64_t u64;
		float f;
		double d;
		unsigned char byte;
		char c;
		void* p;
		RmlVector2f v2;
		RmlVector2i v2i;
		RmlVector3f v3;
		RmlVector4f v4;
		RmlColourb cb;
		RmlColourf cf;
		const int numeric_successes = rml_variant_get_bool(variant, &b) + rml_variant_get_int(variant, &i) + rml_variant_get_int64(variant, &i64) +
			rml_variant_get_uint(variant, &u) + rml_variant_get_uint64(variant, &u64) + rml_variant_get_float(variant, &f) +
			rml_variant_get_double(variant, &d) + rml_variant_get_byte(variant, &byte);
		if (source.numeric)
			CHECK(numeric_successes == 8);
		else if (rml_variant_get_type(variant) != RML_VARIANT_TYPE_STRING)
			CHECK(numeric_successes == 0);
		rml_variant_get_char(variant, &c);
		rml_variant_get_voidptr(variant, &p);
		rml_variant_get_vector2f(variant, &v2);
		rml_variant_get_vector2i(variant, &v2i);
		rml_variant_get_vector3f(variant, &v3);
		rml_variant_get_vector4f(variant, &v4);
		rml_variant_get_colourb(variant, &cb);
		rml_variant_get_colourf(variant, &cf);
		RmlOwnedString text = {};
		rml_variant_to_string(variant, &text);
		rml_owned_string_free(text);
		rml_variant_destroy(variant);
	}

	for (size_t m = before; m < session.system_log.messages.size(); m++)
	{
		INFO(session.system_log.messages[m].second);
		CHECK(session.system_log.messages[m].first != RML_LOG_TYPE_ERROR);
		CHECK(session.system_log.messages[m].first != RML_LOG_TYPE_ASSERT);
	}
}

TEST_CASE("capi.values.variant_to_string_and_compound_parsing")
{
	RmlVariant* variant = rml_variant_create();
	RmlOwnedString text = {};

	CHECK_FALSE(rml_variant_to_string(variant, &text));
	rml_variant_set_float(variant, 1.5f);
	CHECK(rml_variant_to_string(variant, &text));
	CHECK(ToString(RmlStringView{text.data, text.size}) == "1.5");
	rml_variant_set_bool(variant, true);
	CHECK(rml_variant_to_string(variant, &text));
	CHECK(ToString(RmlStringView{text.data, text.size}) == "1");
	rml_variant_set_vector2f(variant, RmlVector2f{10, 20});
	CHECK(rml_variant_to_string(variant, &text));
	CHECK(ToString(RmlStringView{text.data, text.size}) == "10, 20");
	rml_variant_set_char(variant, 'c');
	CHECK_FALSE(rml_variant_to_string(variant, &text));
	CHECK(Take(text) == "10, 20");

	rml_variant_set_string(variant, rml_sv("3, 4"));
	RmlVector2f v2 = {};
	CHECK(rml_variant_get_vector2f(variant, &v2));
	CHECK(v2.x == 3.f);
	CHECK(v2.y == 4.f);

	rml_variant_set_vector2i(variant, RmlVector2i{-7, 9});
	CHECK(rml_variant_get_type(variant) == RML_VARIANT_TYPE_VECTOR2);
	rml_variant_set_vector2f(variant, RmlVector2f{-1.75f, 2.75f});
	RmlVector2i v2i = {};
	CHECK(rml_variant_get_vector2i(variant, &v2i));
	CHECK(v2i.x == -1);
	CHECK(v2i.y == 2);

	rml_variant_destroy(variant);
}

TEST_CASE("capi.values.dictionary_remove_clear_and_string_list_clear")
{
	RmlDictionary* dictionary = rml_dictionary_create();
	RmlVariant* value = rml_variant_create();
	rml_variant_set_int(value, 1);
	rml_dictionary_set(dictionary, rml_sv("a"), value);
	rml_dictionary_set(dictionary, rml_sv("b"), value);
	rml_dictionary_set(dictionary, rml_sv("c"), value);

	CHECK(rml_dictionary_remove(dictionary, rml_sv("b")));
	CHECK_FALSE(rml_dictionary_remove(dictionary, rml_sv("b")));
	CHECK(rml_dictionary_size(dictionary) == 2);
	CHECK(rml_dictionary_get(dictionary, rml_sv("b")) == nullptr);
	CHECK(rml_dictionary_get(dictionary, rml_sv("c")) != nullptr);
	rml_dictionary_clear(dictionary);
	CHECK(rml_dictionary_size(dictionary) == 0);
	rml_variant_destroy(value);
	rml_dictionary_destroy(dictionary);

	RmlStringList* strings = rml_string_list_create();
	rml_string_list_push(strings, rml_sv("x"));
	rml_string_list_push(strings, rml_sv("y"));
	rml_string_list_clear(strings);
	CHECK(rml_string_list_size(strings) == 0);
	rml_string_list_push(strings, rml_sv("z"));
	CHECK(ToString(rml_string_list_get(strings, 0)) == "z");
	rml_string_list_destroy(strings);
}
