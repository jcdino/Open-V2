#include "gtest\\gtest.h"
#include "nations\\nations.h"
#include "nations\\nations_functions.h"
#include "scenario\\scenario.h"
#include "scenario\\scenario_io.h"
#include "world_state\\world_state.h"
#include <ppl.h>
#include "fake_fs\\fake_fs.h"
#include "population\\population_io.h"
#include "provinces\\province_functions.h"
#include "nations\\nations_io.h"
#include "population\\population_function.h"

#undef min
#undef max

#define RANGE(x) (x), (x) + (sizeof((x))/sizeof((x)[0])) - 1

using namespace nations;

TEST(nations_tests, nation_creation) {
	world_state ws;

	concurrency::task_group tg;
	serialization::deserialize_from_file(u"D:\\VS2007Projects\\open_v2_test_data\\test_scenario.bin", ws.s, tg);
	tg.wait();

	ready_world_state(ws);

	const char gtag[] = "GER";
	auto ger_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(gtag)));
	auto ger_nation = make_nation_for_tag(ws, ger_tag);
	
	EXPECT_NE(country_tag(), ger_nation);
	EXPECT_EQ(ws.w.nation_s.nations.get<nation::tag>(ger_nation), ger_tag);
	EXPECT_NE(country_tag(), ger_nation);

	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].default_name.adjective, ws.w.nation_s.nations.get<nation::adjective>(ger_nation));
	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].default_name.name, ws.w.nation_s.nations.get<nation::name>(ger_nation));
	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].base_flag, ws.w.nation_s.nations.get<nation::flag>(ger_nation));
	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].color.g, ws.w.nation_s.nations.get<nation::current_color>(ger_nation).g);


	const char usat[] = "USA";
	auto usa_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(usat)));
	auto usa_nation = make_nation_for_tag(ws, usa_tag);

	EXPECT_NE(nations::country_tag(), usa_nation);
	EXPECT_EQ(ws.w.nation_s.nations.get<nation::tag>(usa_nation), usa_tag);
	EXPECT_NE(country_tag(), usa_nation);
	EXPECT_NE(ger_nation, usa_nation);
}

TEST(nations_tests, province_ownership) {
	world_state ws;
	concurrency::task_group tg;
	serialization::deserialize_from_file(u"D:\\VS2007Projects\\open_v2_test_data\\test_scenario.bin", ws.s, tg);
	tg.wait();
	ready_world_state(ws);

	const char gert[] = "GER";
	auto ger_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(gert)));
	auto ger_nation = make_nation_for_tag(ws, ger_tag);

	auto prov_tag = provinces::province_tag(104ui16);
	auto province_region = ws.s.province_m.province_container.get<province::state_id>(prov_tag);

	EXPECT_NE(provinces::state_tag(), province_region);

	provinces::silent_set_province_owner(ws, ger_nation, prov_tag);

	EXPECT_EQ(1ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(1ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(province_region, get(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation), 0ui32).region_id);
	EXPECT_NE(state_tag(), get(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation), 0ui32).state);
	EXPECT_EQ(ger_nation, provinces::province_owner(ws, prov_tag));
	EXPECT_EQ(get(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation), 0ui32).state, provinces::province_state(ws, prov_tag));

	auto old_state_instance = provinces::province_state(ws, prov_tag);

	provinces::silent_remove_province_owner(ws, prov_tag);

	EXPECT_EQ(0ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(0ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(nations::country_tag(), provinces::province_owner(ws, prov_tag));
	EXPECT_EQ(state_tag(), provinces::province_state(ws, prov_tag));
	EXPECT_NE(old_state_instance, ws.w.nation_s.states.get<index_type_marker>(old_state_instance));
}

TEST(nations_tests, adding_states) {
	world_state ws;
	concurrency::task_group tg;
	serialization::deserialize_from_file(u"D:\\VS2007Projects\\open_v2_test_data\\test_scenario.bin", ws.s, tg);
	tg.wait();
	ready_world_state(ws);

	const char gert[] = "GER";
	auto ger_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(gert)));
	auto ger_nation = make_nation_for_tag(ws, ger_tag);

	auto prov_tag_a = provinces::province_tag(104ui16);
	auto prov_tag_b = provinces::province_tag(103ui16);
	auto prov_tag_c = provinces::province_tag(102ui16);

	provinces::silent_set_province_owner(ws, ger_nation, prov_tag_a);
	provinces::silent_set_province_owner(ws, ger_nation, prov_tag_b);

	EXPECT_EQ(2ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(1ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_NE(state_tag(), provinces::province_state(ws, prov_tag_b));
	EXPECT_EQ(provinces::province_state(ws, prov_tag_a), provinces::province_state(ws, prov_tag_b));
	EXPECT_EQ(ger_nation, provinces::province_owner(ws, prov_tag_a));
	EXPECT_EQ(ger_nation, provinces::province_owner(ws, prov_tag_b));

	EXPECT_EQ(true, contains_item(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation), prov_tag_a));
	EXPECT_EQ(false, contains_item(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation), prov_tag_c));

	provinces::silent_set_province_owner(ws, ger_nation, prov_tag_c);

	EXPECT_EQ(3ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(2ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(ger_nation, provinces::province_owner(ws, prov_tag_c));

	EXPECT_NE(state_tag(), provinces::province_state(ws, prov_tag_c));
	EXPECT_NE(provinces::province_state(ws, prov_tag_a), provinces::province_state(ws, prov_tag_c));

	provinces::silent_remove_province_owner(ws, prov_tag_a);
	EXPECT_EQ(2ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(2ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(nations::country_tag(), provinces::province_owner(ws, prov_tag_a));

	EXPECT_EQ(false, contains_item(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation), prov_tag_a));
	EXPECT_EQ(true, contains_item(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation), prov_tag_c));

	EXPECT_NE(provinces::province_state(ws, prov_tag_a), provinces::province_state(ws, prov_tag_b));

	provinces::silent_remove_province_owner(ws, prov_tag_b);
	EXPECT_EQ(1ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(1ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(nations::country_tag(), provinces::province_owner(ws, prov_tag_b));

	EXPECT_EQ(state_tag(), provinces::province_state(ws, prov_tag_b));
	EXPECT_EQ(provinces::province_state(ws, prov_tag_a), provinces::province_state(ws, prov_tag_b));

	provinces::silent_remove_province_owner(ws, prov_tag_c);
	EXPECT_EQ(0ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(0ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(nations::country_tag(), provinces::province_owner(ws, prov_tag_c));

	EXPECT_EQ(provinces::province_state(ws, prov_tag_a), provinces::province_state(ws, prov_tag_c));
}

TEST(nations_tests, province_control) {
	world_state ws;
	concurrency::task_group tg;
	serialization::deserialize_from_file(u"D:\\VS2007Projects\\open_v2_test_data\\test_scenario.bin", ws.s, tg);
	tg.wait();
	ready_world_state(ws);

	const char gert[] = "GER";
	auto ger_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(gert)));
	auto ger_nation = make_nation_for_tag(ws, ger_tag);

	auto prov_tag = provinces::province_tag(104ui16);
	auto province_region = ws.s.province_m.province_container.get<province::state_id>(prov_tag);

	EXPECT_NE(provinces::state_tag(), province_region);

	provinces::silent_set_province_controller(ws, ger_nation, prov_tag);

	EXPECT_EQ(0ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(1ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::controlled_provinces>(ger_nation)));
	EXPECT_EQ(0ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(ger_nation, provinces::province_controller(ws, prov_tag));
	EXPECT_EQ(true, contains_item(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::controlled_provinces>(ger_nation), prov_tag));

	provinces::silent_remove_province_controller(ws, prov_tag);

	EXPECT_EQ(0ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::owned_provinces>(ger_nation)));
	EXPECT_EQ(0ui32, get_size(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::controlled_provinces>(ger_nation)));
	EXPECT_EQ(0ui32, get_size(ws.w.nation_s.state_arrays, ws.w.nation_s.nations.get<nation::member_states>(ger_nation)));
	EXPECT_EQ(country_tag(), provinces::province_controller(ws, prov_tag));
	EXPECT_EQ(false, contains_item(ws.w.province_s.province_arrays, ws.w.nation_s.nations.get<nation::controlled_provinces>(ger_nation), prov_tag));
}

class preparse_test_files {
public:
	directory_representation f_root = directory_representation(u"F:");
	directory_representation test1 = directory_representation(u"test1", f_root);
	directory_representation history = directory_representation(u"history", test1);
	directory_representation pops = directory_representation(u"pops", history);
	directory_representation pop_date = directory_representation(u"1800.1.1", pops);
	file_representation hf2 = file_representation(u"fileb.txt", pop_date,
		"853 = {\r\n"
		"	clergymen = {\r\n"
		"		culture = catalan\r\n"
		"		religion = orthodox\r\n"
		"		size = 50\r\n"
		"	}\r\n"
		"	farmers = {\r\n"
		"		culture = albanian\r\n"
		"		religion = orthodox\r\n"
		"		size = 6750\r\n"
		"	}\r\n"
		"}\r\n");
	directory_representation countries = directory_representation(u"countries", history);
	directory_representation units = directory_representation(u"units", history);
	directory_representation dir_1836 = directory_representation(u"1836", units);
	file_representation oob_a = file_representation(u"USA_oob.txt", units, "JAP = { value = 100 }");
	file_representation oob_b = file_representation(u"USA_oob.txt", dir_1836, "MEX = { value = 200 }");
	file_representation nfile = file_representation(u"GER - Germany.txt", countries,
		"capital = 487\r\n"
		"primary_culture = spanish\r\n"
		"culture = catalan\r\n"
		"culture = basque\r\n"
		"religion = catholic\r\n"
		"government = hms_government\r\n"
		"plurality = 25.0\r\n"
		"nationalvalue = nv_equality\r\n"
		"literacy = 0.75\r\n"
		"non_state_culture_literacy = 0.15\r\n"
		"civilized = yes\r\n"
		"prestige = 40\r\n"
		"slavery = yes_slavery\r\n"
		"press_rights = censored_press\r\n"
		"ruling_party = SPA_conservative\r\n"
		"last_election = 1834.1.1\r\n"
		"upper_house = {\r\n"
		"	fascist = 0\r\n"
		"	liberal = 35\r\n"
		"	conservative = 60\r\n"
		"	reactionary = 5\r\n"
		"	anarcho_liberal = 0\r\n"
		"	socialist = 0\r\n"
		"	communist = 0\r\n"
		"}\r\n"
		"foreign_investment = {\r\n"
		"	MEX = 10000\r\n"
		"	JAP = 2000\r\n"
		"}\r\n"
		"govt_flag = {\r\n"
		"	government = hms_government\r\n"
		"	flag = democracy\r\n"
		"}\r\n"
		"post_napoleonic_thought = 1\r\n"
		"authoritarianism = yes\r\n"
		"consciousness = 2\r\n"
		"nonstate_consciousness = 1\r\n"
		"schools = commerce_tech_school\r\n"
		"oob = \"USA_oob.txt\"\r\n"
		"set_country_flag = serfdom_not_abolished\r\n"
		"1838.1.1 = {\r\n"
		"	decision = trail_of_tears\r\n"
		"	set_global_flag = american_civil_war_has_happened\r\n"
		"	press_rights = free_press\r\n"
		"	flintlock_rifles = 1\r\n"
		"	upper_house = {\r\n"
		"		fascist = 0\r\n"
		"		liberal = 0\r\n"
		"		conservative = 95\r\n"
		"		reactionary = 5\r\n"
		"		anarcho_liberal = 0\r\n"
		"		socialist = 0\r\n"
		"		communist = 0\r\n"
		"	}\r\n"
		"	oob = \"/1836/USA_oob.txt\"\r\n"
		"	clr_country_flag = serfdom_not_abolished\r\n"
		"}\r\n");

	preparse_test_files() {
		set_default_root(f_root);
	}
};

TEST(nations_tests, read_nations_files_simple) {
	world_state ws;
	concurrency::task_group tg;
	serialization::deserialize_from_file(u"D:\\VS2007Projects\\open_v2_test_data\\test_scenario.bin", ws.s, tg);
	tg.wait();
	ready_world_state(ws);

	preparse_test_files real_fs;
	file_system f;
	f.set_root(u"F:\\test1");

	population::read_all_pops(f.get_root(), ws, date_to_tag(boost::gregorian::date(1801, boost::gregorian::Jan, 1)));
	
	const char gert[] = "GER";
	auto ger_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(gert)));
	auto ger_nation = make_nation_for_tag(ws, ger_tag);

	provinces::silent_set_province_owner(ws, ger_nation, provinces::province_tag(853ui16));

	std::vector<std::pair<country_tag, events::decision_tag>> decisions;
	read_nations_files(ws, date_to_tag(boost::gregorian::date(1801, boost::gregorian::Jan, 1)), f.get_root(), decisions);

	const char japt[] = "JAP";
	auto jap_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(japt)));
	auto jap_nation = make_nation_for_tag(ws, jap_tag);

	const char mext[] = "MEX";
	auto mex_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(mext)));
	auto mex_nation = make_nation_for_tag(ws, mex_tag);

	EXPECT_EQ(100, get_relationship(ws, ger_nation, jap_nation));
	EXPECT_EQ(0, get_relationship(ws, ger_nation, mex_nation));
	EXPECT_EQ(provinces::province_tag(487ui16), ws.w.nation_s.nations.get<nation::current_capital>(ger_nation));

	EXPECT_EQ(tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "spanish")), ws.w.nation_s.nations.get<nation::primary_culture>(ger_nation));
	EXPECT_EQ(2ui32, get_size(ws.w.culture_s.culture_arrays, ws.w.nation_s.nations.get<nation::accepted_cultures>(ger_nation)));
	EXPECT_EQ(true, contains_item(ws.w.culture_s.culture_arrays, ws.w.nation_s.nations.get<nation::accepted_cultures>(ger_nation), tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "catalan"))));
	EXPECT_EQ(true, contains_item(ws.w.culture_s.culture_arrays, ws.w.nation_s.nations.get<nation::accepted_cultures>(ger_nation), tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "basque"))));
	EXPECT_EQ(tag_from_text(ws.s.culture_m.named_religion_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "catholic")), ws.w.nation_s.nations.get<nation::national_religion>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.governments_m.named_government_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "hms_government")), ws.w.nation_s.nations.get<nation::current_government>(ger_nation));
	EXPECT_EQ(0.25f, ws.w.nation_s.nations.get<nation::plurality>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.modifiers_m.named_national_modifiers_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "nv_equality")), ws.w.nation_s.nations.get<nation::national_value>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.modifiers_m.named_national_modifiers_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "commerce_tech_school")), ws.w.nation_s.nations.get<nation::tech_school>(ger_nation));
	EXPECT_EQ(true, ws.w.nation_s.nations.get<nation::is_civilized>(ger_nation));
	EXPECT_EQ(40.0f, ws.w.nation_s.nations.get<nation::base_prestige>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.governments_m.named_party_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "SPA_conservative")), ws.w.nation_s.nations.get<nation::ruling_party>(ger_nation));
	EXPECT_EQ(date_to_tag(boost::gregorian::date(1834, boost::gregorian::Jan, 1)), ws.w.nation_s.nations.get<nation::last_election>(ger_nation));
	EXPECT_EQ(ws.w.nation_s.upper_house.get(ger_nation, tag_from_text(ws.s.ideologies_m.named_ideology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "liberal"))), 35ui8);
	EXPECT_EQ(ws.w.nation_s.upper_house.get(ger_nation, tag_from_text(ws.s.ideologies_m.named_ideology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "conservative"))), 60ui8);
	EXPECT_EQ(10000.0f, get_foreign_investment(ws, ger_nation, mex_nation));
	EXPECT_EQ(2000.0f, get_foreign_investment(ws, ger_nation, jap_nation));
	EXPECT_EQ(tag_from_text(ws.s.issues_m.named_option_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "yes_slavery")), ws.w.nation_s.active_issue_options.get(ger_nation, tag_from_text(ws.s.issues_m.named_issue_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "slavery"))));
	EXPECT_EQ(tag_from_text(ws.s.issues_m.named_option_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "censored_press")), ws.w.nation_s.active_issue_options.get(ger_nation, tag_from_text(ws.s.issues_m.named_issue_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "press_rights"))));
	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].monarchy_flag, ws.w.culture_s.country_flags_by_government.get(ger_tag, tag_from_text(ws.s.governments_m.named_government_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "absolute_monarchy"))));
	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].base_flag, ws.w.culture_s.country_flags_by_government.get(ger_tag, tag_from_text(ws.s.governments_m.named_government_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "hms_government"))));

	EXPECT_EQ(true, bit_vector_test(ws.w.nation_s.active_technologies.get_row(ger_nation), tag_from_text(ws.s.technology_m.named_technology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "post_napoleonic_thought"))));
	EXPECT_EQ(true, bit_vector_test(ws.w.nation_s.active_technologies.get_row(ger_nation), tag_from_text(ws.s.technology_m.named_technology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "authoritarianism"))));
	EXPECT_EQ(false, bit_vector_test(ws.w.nation_s.active_technologies.get_row(ger_nation), tag_from_text(ws.s.technology_m.named_technology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "flintlock_rifles"))));

	EXPECT_EQ(0.0f, ws.w.variable_s.global_variables[tag_from_text(ws.s.variables_m.named_global_variables, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "american_civil_war_has_happened"))]);

	EXPECT_EQ(1ui32, get_size(ws.w.variable_s.national_flags_arrays, ws.w.nation_s.nations.get<nation::national_flags>(ger_nation)));
	EXPECT_EQ(true, contains_item(ws.w.variable_s.national_flags_arrays, ws.w.nation_s.nations.get<nation::national_flags>(ger_nation), tag_from_text(ws.s.variables_m.named_national_flags, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "serfdom_not_abolished"))));

	auto pop_range = get_range(ws.w.population_s.pop_arrays, ws.w.province_s.province_state_container.get<province_state::pops>(provinces::province_tag(853ui16)));
	EXPECT_EQ(2i64, std::end(pop_range) - std::begin(pop_range));

	for(auto p : pop_range) {
		if(ws.w.population_s.pops.get<pop::culture>(p) == tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "catalan"))) {
			EXPECT_EQ(population::get_literacy_direct(ws, p), 0.75f);
			EXPECT_EQ(population::get_consciousness_direct(ws, p), 2.0f);
		} else {
			EXPECT_EQ(population::get_literacy_direct(ws, p), 0.15f);
			EXPECT_EQ(population::get_consciousness_direct(ws, p), 1.0f);
		}
	}

	EXPECT_EQ(0ui64, decisions.size());
}

TEST(nations_tests, read_nations_files_layered) {
	world_state ws;
	concurrency::task_group tg;
	serialization::deserialize_from_file(u"D:\\VS2007Projects\\open_v2_test_data\\test_scenario.bin", ws.s, tg);
	tg.wait();
	ready_world_state(ws);

	preparse_test_files real_fs;
	file_system f;
	f.set_root(u"F:\\test1");

	population::read_all_pops(f.get_root(), ws, date_to_tag(boost::gregorian::date(1840, boost::gregorian::Jan, 1)));

	const char gert[] = "GER";
	auto ger_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(gert)));
	auto ger_nation = make_nation_for_tag(ws, ger_tag);

	provinces::silent_set_province_owner(ws, ger_nation, provinces::province_tag(853ui16));

	std::vector<std::pair<country_tag, events::decision_tag>> decisions;
	read_nations_files(ws, date_to_tag(boost::gregorian::date(1840, boost::gregorian::Jan, 1)), f.get_root(), decisions);

	const char japt[] = "JAP";
	auto jap_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(japt)));
	auto jap_nation = make_nation_for_tag(ws, jap_tag);

	const char mext[] = "MEX";
	auto mex_tag = tag_from_text(ws.s.culture_m.national_tags_index, cultures::tag_to_encoding(RANGE(mext)));
	auto mex_nation = make_nation_for_tag(ws, mex_tag);

	EXPECT_EQ(0, get_relationship(ws, ger_nation, jap_nation));
	EXPECT_EQ(200, get_relationship(ws, ger_nation, mex_nation));
	EXPECT_EQ(provinces::province_tag(487ui16), ws.w.nation_s.nations.get<nation::current_capital>(ger_nation));

	EXPECT_EQ(tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "spanish")), ws.w.nation_s.nations.get<nation::primary_culture>(ger_nation));
	EXPECT_EQ(2ui32, get_size(ws.w.culture_s.culture_arrays, ws.w.nation_s.nations.get<nation::accepted_cultures>(ger_nation)));
	EXPECT_EQ(true, contains_item(ws.w.culture_s.culture_arrays, ws.w.nation_s.nations.get<nation::accepted_cultures>(ger_nation), tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "catalan"))));
	EXPECT_EQ(true, contains_item(ws.w.culture_s.culture_arrays, ws.w.nation_s.nations.get<nation::accepted_cultures>(ger_nation), tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "basque"))));
	EXPECT_EQ(tag_from_text(ws.s.culture_m.named_religion_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "catholic")), ws.w.nation_s.nations.get<nation::national_religion>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.governments_m.named_government_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "hms_government")), ws.w.nation_s.nations.get<nation::current_government>(ger_nation));
	EXPECT_EQ(0.25f, ws.w.nation_s.nations.get<nation::plurality>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.modifiers_m.named_national_modifiers_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "nv_equality")), ws.w.nation_s.nations.get<nation::national_value>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.modifiers_m.named_national_modifiers_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "commerce_tech_school")), ws.w.nation_s.nations.get<nation::tech_school>(ger_nation));
	EXPECT_EQ(true, ws.w.nation_s.nations.get<nation::is_civilized>(ger_nation));
	EXPECT_EQ(40.0f, ws.w.nation_s.nations.get<nation::base_prestige>(ger_nation));
	EXPECT_EQ(tag_from_text(ws.s.governments_m.named_party_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "SPA_conservative")), ws.w.nation_s.nations.get<nation::ruling_party>(ger_nation));
	EXPECT_EQ(date_to_tag(boost::gregorian::date(1834, boost::gregorian::Jan, 1)), ws.w.nation_s.nations.get<nation::last_election>(ger_nation));
	EXPECT_EQ(ws.w.nation_s.upper_house.get(ger_nation, tag_from_text(ws.s.ideologies_m.named_ideology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "liberal"))), 0ui8);
	EXPECT_EQ(ws.w.nation_s.upper_house.get(ger_nation, tag_from_text(ws.s.ideologies_m.named_ideology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "conservative"))), 95ui8);
	EXPECT_EQ(10000.0f, get_foreign_investment(ws, ger_nation, mex_nation));
	EXPECT_EQ(2000.0f, get_foreign_investment(ws, ger_nation, jap_nation));
	EXPECT_EQ(tag_from_text(ws.s.issues_m.named_option_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "yes_slavery")), ws.w.nation_s.active_issue_options.get(ger_nation, tag_from_text(ws.s.issues_m.named_issue_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "slavery"))));
	EXPECT_EQ(tag_from_text(ws.s.issues_m.named_option_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "free_press")), ws.w.nation_s.active_issue_options.get(ger_nation, tag_from_text(ws.s.issues_m.named_issue_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "press_rights"))));
	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].monarchy_flag, ws.w.culture_s.country_flags_by_government.get(ger_tag, tag_from_text(ws.s.governments_m.named_government_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "absolute_monarchy"))));
	EXPECT_EQ(ws.s.culture_m.national_tags[ger_tag].base_flag, ws.w.culture_s.country_flags_by_government.get(ger_tag, tag_from_text(ws.s.governments_m.named_government_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "hms_government"))));

	EXPECT_EQ(true, bit_vector_test(ws.w.nation_s.active_technologies.get_row(ger_nation), tag_from_text(ws.s.technology_m.named_technology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "post_napoleonic_thought"))));
	EXPECT_EQ(true, bit_vector_test(ws.w.nation_s.active_technologies.get_row(ger_nation), tag_from_text(ws.s.technology_m.named_technology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "authoritarianism"))));
	EXPECT_EQ(true, bit_vector_test(ws.w.nation_s.active_technologies.get_row(ger_nation), tag_from_text(ws.s.technology_m.named_technology_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "flintlock_rifles"))));

	EXPECT_EQ(1.0f, ws.w.variable_s.global_variables[tag_from_text(ws.s.variables_m.named_global_variables, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "american_civil_war_has_happened"))]);

	EXPECT_EQ(0ui32, get_size(ws.w.variable_s.national_flags_arrays, ws.w.nation_s.nations.get<nation::national_flags>(ger_nation)));
	EXPECT_EQ(false, contains_item(ws.w.variable_s.national_flags_arrays, ws.w.nation_s.nations.get<nation::national_flags>(ger_nation), tag_from_text(ws.s.variables_m.named_national_flags, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "serfdom_not_abolished"))));

	auto pop_range = get_range(ws.w.population_s.pop_arrays, ws.w.province_s.province_state_container.get<province_state::pops>(provinces::province_tag(853ui16)));
	EXPECT_EQ(2i64, std::end(pop_range) - std::begin(pop_range));

	for(auto p : pop_range) {
		if(ws.w.population_s.pops.get<pop::culture>(p) == tag_from_text(ws.s.culture_m.named_culture_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "catalan"))) {
			EXPECT_EQ(population::get_literacy_direct(ws, p), 0.75f);
			EXPECT_EQ(population::get_consciousness_direct(ws, p), 2.0f);
		} else {
			EXPECT_EQ(population::get_literacy_direct(ws, p), 0.15f);
			EXPECT_EQ(population::get_consciousness_direct(ws, p), 1.0f);
		}
	}

	EXPECT_EQ(1ui64, decisions.size());
	EXPECT_EQ(ger_nation, decisions[0].first);
	EXPECT_EQ(tag_from_text(ws.s.event_m.decisions_by_title_index, text_data::get_existing_text_handle(ws.s.gui_m.text_data_sequences, "trail_of_tears_TITLE")), decisions[0].second);
}

