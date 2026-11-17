#include "Harness.h"
#include <doctest.h>
#include <map>

using namespace CApiTest;

namespace {

const char* form_document = R"(
<rml>
<head>
<style>
	body { width: 600px; height: 400px; font-family: LatoLatin; font-size: 14px; }
	form, input, textarea, select, progress, tabset, p { display: block; }
	input, select { width: 200px; height: 20px; }
	textarea { width: 200px; height: 60px; }
	selectbox { display: block; }
	selectbox option { display: block; height: 20px; }
	progress { width: 100px; height: 10px; }
	tabs, panels, tab, panel { display: block; }
</style>
</head>
<body>
	<form id="form">
		<input id="name" type="text" name="name" value="Ada"/>
		<input id="agree" type="checkbox" name="agree" value="yes" checked/>
		<input id="mute" type="checkbox" name="mute" value="on"/>
		<input id="disabled" type="text" name="ignored" value="x" disabled/>
		<textarea id="notes" name="notes" cols="20" rows="3">Hello</textarea>
		<select id="choice" name="choice">
			<option value="a">A</option>
			<option value="b" selected>B</option>
		</select>
		<input id="send" type="submit" name="action" value="send">Send</input>
	</form>
	<progress id="bar" value="0.25" max="1"/>
	<tabset id="tabs">
		<tab>One</tab><panel>First</panel>
		<tab>Two</tab><panel>Second</panel>
	</tabset>
	<p id="plain">Plain</p>
</body>
</rml>
)";

RmlElementFormControl* Control(Session& session, const char* id)
{
	RmlElement* element = session.ById(id);
	REQUIRE(element);
	RmlElementFormControl* control = rml_element_as_form_control(element);
	REQUIRE(control);
	return control;
}

struct SubmitLog {
	int calls = 0;
	std::map<std::string, std::string> values;
};

void CollectSubmitValue(void* user_data, RmlStringView key, const RmlVariant* value)
{
	static_cast<SubmitLog*>(user_data)->values[ToString(key)] = ToString(rml_variant_get_string(value));
}

void OnSubmit(void* user_data, RmlEvent* event)
{
	SubmitLog* log = static_cast<SubmitLog*>(user_data);
	log->calls += 1;
	log->values.clear();
	rml_dictionary_for_each(rml_event_get_parameters(event), &CollectSubmitValue, log);
}

} // namespace

TEST_CASE("capi.forms.casts_round_trip_and_reject_wrong_types")
{
	Session session("forms_casts");
	session.Load(form_document);
	RmlElement* form = session.ById("form");
	RmlElement* name = session.ById("name");
	RmlElement* notes = session.ById("notes");
	RmlElement* choice = session.ById("choice");
	RmlElement* bar = session.ById("bar");
	RmlElement* tabs = session.ById("tabs");
	RmlElement* plain = session.ById("plain");

	REQUIRE(rml_element_as_form(form));
	CHECK(rml_form_as_element(rml_element_as_form(form)) == form);

	REQUIRE(rml_element_as_form_control_input(name));
	CHECK(rml_form_control_input_as_element(rml_element_as_form_control_input(name)) == name);
	CHECK(rml_form_control_as_element(rml_element_as_form_control(name)) == name);

	REQUIRE(rml_element_as_form_control_text_area(notes));
	CHECK(rml_form_control_text_area_as_element(rml_element_as_form_control_text_area(notes)) == notes);
	CHECK(rml_element_as_form_control_input(notes) == nullptr);

	REQUIRE(rml_element_as_form_control_select(choice));
	CHECK(rml_form_control_select_as_element(rml_element_as_form_control_select(choice)) == choice);

	REQUIRE(rml_element_as_progress(bar));
	CHECK(rml_progress_as_element(rml_element_as_progress(bar)) == bar);
	CHECK(rml_element_as_form_control(bar) == nullptr);

	REQUIRE(rml_element_as_tab_set(tabs));
	CHECK(rml_tab_set_as_element(rml_element_as_tab_set(tabs)) == tabs);

	CHECK(rml_element_as_form(plain) == nullptr);
	CHECK(rml_element_as_form_control(plain) == nullptr);
	CHECK(rml_element_as_form_control_select(plain) == nullptr);
	CHECK(rml_element_as_form_control_text_area(plain) == nullptr);
	CHECK(rml_element_as_progress(plain) == nullptr);
	CHECK(rml_element_as_tab_set(plain) == nullptr);
}

TEST_CASE("capi.forms.form_control_common_accessors")
{
	Session session("forms_common");
	session.Load(form_document);
	RmlElementFormControl* name = Control(session, "name");
	RmlElementFormControl* agree = Control(session, "agree");
	RmlElementFormControl* mute = Control(session, "mute");
	RmlElementFormControl* disabled = Control(session, "disabled");

	CHECK(Take(rml_form_control_get_name(name)) == "name");
	rml_form_control_set_name(name, rml_sv("full_name"));
	CHECK(Take(rml_form_control_get_name(name)) == "full_name");

	CHECK(Take(rml_form_control_get_value(name)) == "Ada");
	rml_form_control_set_value(name, rml_sv("Grace"));
	CHECK(Take(rml_form_control_get_value(name)) == "Grace");

	CHECK(rml_form_control_is_submitted(agree));
	CHECK_FALSE(rml_form_control_is_submitted(mute));
	CHECK(rml_form_control_is_submitted(name));

	CHECK(rml_form_control_is_disabled(disabled));
	CHECK_FALSE(rml_form_control_is_disabled(name));
	rml_form_control_set_disabled(name, true);
	CHECK(rml_form_control_is_disabled(name));
	rml_form_control_set_disabled(name, false);
	CHECK_FALSE(rml_form_control_is_disabled(name));
}

TEST_CASE("capi.forms.text_input_selection")
{
	Session session("forms_input");
	session.Load(form_document);
	RmlElement* name = session.ById("name");
	RmlElementFormControlInput* input = rml_element_as_form_control_input(name);
	REQUIRE(input);
	rml_form_control_set_value(rml_element_as_form_control(name), rml_sv("Hello World"));
	rml_element_focus(name, false);
	session.Update();

	rml_form_control_input_set_selection_range(input, 6, 11);
	int start = -1;
	int end = -1;
	RmlOwnedString selected = {};
	rml_form_control_input_get_selection(input, &start, &end, &selected);
	CHECK(start == 6);
	CHECK(end == 11);
	CHECK(Take(selected) == "World");

	rml_form_control_input_select(input);
	rml_form_control_input_get_selection(input, &start, &end, nullptr);
	CHECK(start == 0);
	CHECK(end == 11);

	// Null outputs are allowed.
	rml_form_control_input_get_selection(input, nullptr, nullptr, nullptr);

	rml_form_control_input_set_composition_range(input, 0, 5);
	CHECK(Take(rml_form_control_get_value(rml_element_as_form_control(name))) == "Hello World");
}

TEST_CASE("capi.forms.text_area_attributes_and_selection")
{
	Session session("forms_textarea");
	session.Load(form_document);
	RmlElement* notes = session.ById("notes");
	RmlElementFormControlTextArea* text_area = rml_element_as_form_control_text_area(notes);
	REQUIRE(text_area);

	CHECK(rml_form_control_text_area_get_num_columns(text_area) == 20);
	CHECK(rml_form_control_text_area_get_num_rows(text_area) == 3);
	rml_form_control_text_area_set_num_columns(text_area, 40);
	rml_form_control_text_area_set_num_rows(text_area, 6);
	CHECK(rml_form_control_text_area_get_num_columns(text_area) == 40);
	CHECK(rml_form_control_text_area_get_num_rows(text_area) == 6);

	rml_form_control_text_area_set_max_length(text_area, 12);
	CHECK(rml_form_control_text_area_get_max_length(text_area) == 12);

	CHECK(rml_form_control_text_area_get_word_wrap(text_area));
	rml_form_control_text_area_set_word_wrap(text_area, false);
	CHECK_FALSE(rml_form_control_text_area_get_word_wrap(text_area));

	CHECK(Take(rml_form_control_get_value(rml_element_as_form_control(notes))) == "Hello");
	rml_element_focus(notes, false);
	session.Update();
	rml_form_control_text_area_set_selection_range(text_area, 1, 4);
	int start = -1;
	int end = -1;
	RmlOwnedString selected = {};
	rml_form_control_text_area_get_selection(text_area, &start, &end, &selected);
	CHECK(start == 1);
	CHECK(end == 4);
	CHECK(Take(selected) == "ell");

	rml_form_control_text_area_select(text_area);
	rml_form_control_text_area_get_selection(text_area, &start, &end, nullptr);
	CHECK(start == 0);
	CHECK(end == 5);
	rml_form_control_text_area_set_composition_range(text_area, 0, 2);
}

TEST_CASE("capi.forms.select_options_and_box")
{
	Session session("forms_select");
	RmlElementDocument* document = session.Load(form_document);
	RmlElement* choice = session.ById("choice");
	RmlElementFormControlSelect* select = rml_element_as_form_control_select(choice);
	REQUIRE(select);
	RmlElementFormControl* control = rml_element_as_form_control(choice);

	CHECK(rml_form_control_select_get_num_options(select) == 2);
	CHECK(rml_form_control_select_get_selection(select) == 1);
	CHECK(Take(rml_form_control_get_value(control)) == "b");

	rml_form_control_select_set_selection(select, 0);
	CHECK(rml_form_control_select_get_selection(select) == 0);
	CHECK(Take(rml_form_control_get_value(control)) == "a");

	RmlElement* option0 = rml_form_control_select_get_option(select, 0);
	REQUIRE(option0);
	CHECK(Take(rml_element_get_attribute_string(option0, rml_sv("value"))) == "a");
	CHECK(rml_form_control_select_get_option(select, 10) == nullptr);

	CHECK(rml_form_control_select_add_rml(select, rml_sv("C"), rml_sv("c"), -1, true) == 2);
	CHECK(rml_form_control_select_get_num_options(select) == 3);

	RmlElement* option = rml_document_create_element(document, rml_sv("option"));
	rml_element_set_attribute_string(option, rml_sv("value"), rml_sv("first"));
	rml_element_set_inner_rml(option, rml_sv("First"));
	CHECK(rml_form_control_select_add_element(select, option, 0) == 0);
	CHECK(rml_form_control_select_get_num_options(select) == 4);
	CHECK(rml_form_control_select_get_option(select, 0) == option);

	// An option that already belongs to the select cannot be added again.
	CHECK(rml_form_control_select_add_element(select, option, -1) == -1);
	CHECK(rml_form_control_select_get_num_options(select) == 4);

	rml_form_control_select_remove(select, 0);
	CHECK(rml_form_control_select_get_num_options(select) == 3);

	CHECK_FALSE(rml_form_control_select_is_select_box_visible(select));
	rml_form_control_select_show_select_box(select);
	CHECK(rml_form_control_select_is_select_box_visible(select));
	rml_form_control_select_hide_select_box(select);
	CHECK_FALSE(rml_form_control_select_is_select_box_visible(select));
	rml_form_control_select_show_select_box(select);
	rml_form_control_select_cancel_select_box(select);
	CHECK_FALSE(rml_form_control_select_is_select_box_visible(select));

	rml_form_control_select_remove_all(select);
	CHECK(rml_form_control_select_get_num_options(select) == 0);
	session.Update();
}

TEST_CASE("capi.forms.progress_value_and_max")
{
	Session session("forms_progress");
	session.Load(form_document);
	RmlElementProgress* progress = rml_element_as_progress(session.ById("bar"));
	REQUIRE(progress);

	CHECK(rml_progress_get_value(progress) == doctest::Approx(0.25f));
	CHECK(rml_progress_get_max(progress) == doctest::Approx(1.f));
	rml_progress_set_max(progress, 4.f);
	rml_progress_set_value(progress, 3.f);
	CHECK(rml_progress_get_max(progress) == doctest::Approx(4.f));
	CHECK(rml_progress_get_value(progress) == doctest::Approx(3.f));
	session.Update();
}

TEST_CASE("capi.forms.tab_set_tabs_and_panels")
{
	Session session("forms_tabs");
	RmlElementDocument* document = session.Load(form_document);
	RmlElement* tabs_element = session.ById("tabs");
	RmlElementTabSet* tabs = rml_element_as_tab_set(tabs_element);
	REQUIRE(tabs);

	CHECK(rml_tab_set_get_num_tabs(tabs) == 2);
	CHECK(rml_tab_set_get_active_tab(tabs) == 0);
	rml_tab_set_set_active_tab(tabs, 1);
	CHECK(rml_tab_set_get_active_tab(tabs) == 1);

	rml_tab_set_set_tab_rml(tabs, 0, rml_sv("Uno"));
	rml_tab_set_set_panel_rml(tabs, 0, rml_sv("<p id='primero'>Primero</p>"));
	CHECK(session.ById("primero") != nullptr);

	RmlElement* tab = rml_document_create_element(document, rml_sv("tab"));
	rml_element_set_id(tab, rml_sv("third_tab"));
	RmlElement* panel = rml_document_create_element(document, rml_sv("panel"));
	rml_element_set_id(panel, rml_sv("third_panel"));
	rml_tab_set_set_tab_element(tabs, 2, tab);
	rml_tab_set_set_panel_element(tabs, 2, panel);
	CHECK(rml_tab_set_get_num_tabs(tabs) == 3);
	CHECK(session.ById("third_tab") == tab);
	CHECK(session.ById("third_panel") == panel);

	// Already-parented elements are refused with a warning.
	rml_tab_set_set_tab_element(tabs, 3, tab);
	rml_tab_set_set_panel_element(tabs, 3, panel);
	CHECK(rml_tab_set_get_num_tabs(tabs) == 3);
	CHECK(session.system_log.HasMessage("rml_tab_set_set_tab_element ignored"));
	CHECK(session.system_log.HasMessage("rml_tab_set_set_panel_element ignored"));

	rml_tab_set_remove_tab(tabs, 2);
	CHECK(rml_tab_set_get_num_tabs(tabs) == 2);
	session.Update();
}

TEST_CASE("capi.forms.submit_collects_enabled_named_values")
{
	Session session("forms_submit");
	session.Load(form_document);
	RmlElement* form_element = session.ById("form");
	RmlElementForm* form = rml_element_as_form(form_element);
	REQUIRE(form);

	SubmitLog log;
	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = &OnSubmit;
	RmlEventListener* listener = rml_event_listener_create(&callbacks, &log);
	rml_element_add_event_listener(form_element, rml_sv("submit"), listener, false);

	rml_form_submit(form, rml_sv("action"), rml_sv("manual"));
	CHECK(log.calls == 1);
	CHECK(log.values["action"] == "manual");
	CHECK(log.values["name"] == "Ada");
	CHECK(log.values["agree"] == "yes");
	CHECK(log.values["choice"] == "b");
	CHECK(log.values["notes"] == "Hello");
	CHECK(log.values.count("mute") == 0);
	CHECK(log.values.count("ignored") == 0);

	rml_form_submit(form, rml_sv(""), rml_sv("fallback"));
	CHECK(log.values["submit"] == "fallback");

	rml_element_click(session.ById("send"));
	CHECK(log.calls == 3);
	CHECK(log.values["action"] == "send");

	rml_element_remove_event_listener(form_element, rml_sv("submit"), listener, false);
	rml_event_listener_destroy(listener);
}
