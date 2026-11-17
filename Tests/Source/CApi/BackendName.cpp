#include <RmlUi/CApi/Backend.h>
#include <doctest.h>
#include <string>

TEST_CASE("capi.backend.name")
{
	const RmlStringView name = rml_backend_get_name();
	REQUIRE(name.data);
	CHECK(std::string(name.data, name.size) == RMLUI_CAPI_BACKEND_EXPECTED);
}
