#pragma once
#include "common\\common.h"
#include "gui\\gui.h"
#include <ppl.h>
#include <concurrent_queue.h>

namespace scenario {
	class scenario_manager;
}

class world_state;

namespace messages {
	namespace message_type {
		constexpr int32_t GAINCB = 0;
		constexpr int32_t GAINCB_TARGET = 1;
		constexpr int32_t LOSECB = 2;
		constexpr int32_t LOSECB_TARGET = 3;
		constexpr int32_t REVOLTALLIANCE_CAUSE = 4;
		constexpr int32_t REVOLTALLIANCE_TARGET = 5;
		constexpr int32_t WAR_CAUSE = 6;
		constexpr int32_t WAR_TARGET = 7;
		constexpr int32_t WARGOAL_CAUSE = 8;
		constexpr int32_t WARGOAL_TARGET = 9;
		constexpr int32_t SOCREFORM = 10;
		constexpr int32_t SOCREFORM_BAD = 11;
		constexpr int32_t POLREFORM = 12;
		constexpr int32_t POLREFORM_BAD = 13;
		constexpr int32_t ECONOMICREFORM = 14;
		constexpr int32_t ECONOMICREFORM_BAD = 15;
		constexpr int32_t MILITARYREFORM = 16;
		constexpr int32_t MILITARYREFORM_BAD = 17;
		constexpr int32_t SETPARTY = 18;
		constexpr int32_t UPPERHOUSE = 19;
		constexpr int32_t ELECTIONSTART = 20;
		constexpr int32_t ELECTIONDONE = 21;
		constexpr int32_t BREAKCOUNTRY = 22;
		constexpr int32_t REBELS = 23;
		constexpr int32_t ANNEX_CAUSE = 24;
		constexpr int32_t ANNEX_TARGET = 25;
		constexpr int32_t PEACE_ACCEPT_CAUSE = 26;
		constexpr int32_t PEACE_ACCEPT_TARGET = 27;
		constexpr int32_t PEACE_DECLINE_CAUSE = 28;
		constexpr int32_t PEACE_DECLINE_TARGET = 29;
		constexpr int32_t MOBILIZE = 30;
		constexpr int32_t DEMOBILIZE = 31;
		constexpr int32_t BUILDING_DONE = 32;
		constexpr int32_t EVENTHAPPENOTHER = 33;
		constexpr int32_t EVENTHAPPENOTHEROPTION = 34;
		constexpr int32_t MAJOREVENTHAPPENOTHER = 35;
		constexpr int32_t MAJOREVENTHAPPENOTHEROPTION = 36;
		constexpr int32_t INVENTION = 37;
		constexpr int32_t TECH = 38;
		constexpr int32_t LEADERDIED = 39;
		constexpr int32_t LANDBATTLEOVER = 40;
		constexpr int32_t NAVALBATTLEOVER = 41;
		constexpr int32_t DECISIONOTHER = 42;
		constexpr int32_t NO_LONGER_GREAT_POWER = 43;
		constexpr int32_t BECAME_GREAT_POWER = 44;
		constexpr int32_t ALLIANCE_ACCEPT_CAUSE = 45;
		constexpr int32_t ALLIANCE_ACCEPT_TARGET = 46;
		constexpr int32_t ALLIANCE_DECLINE_CAUSE = 47;
		constexpr int32_t ALLIANCE_DECLINE_TARGET = 48;
		constexpr int32_t CANCELALLIANCE_CAUSE = 49;
		constexpr int32_t CANCELALLIANCE_TARGET = 50;
		constexpr int32_t INCREASEOPINION_CAUSE = 51;
		constexpr int32_t INCREASEOPINION_TARGET = 52;
		constexpr int32_t ADDTOSPHERE_CAUSE = 53;
		constexpr int32_t ADDTOSPHERE_TARGET = 54;
		constexpr int32_t REMOVEFROMSPHERE_CAUSE = 55;
		constexpr int32_t REMOVEFROMSPHERE_TARGET = 56;
		constexpr int32_t REMOVEFROMSPHERE_OTHER_TARGET = 57;
		constexpr int32_t INCREASERELATION_CAUSE = 58;
		constexpr int32_t INCREASERELATION_TARGET = 59;
		constexpr int32_t DECREASERELATION_CAUSE = 60;
		constexpr int32_t DECREASERELATION_TARGET = 61;
		constexpr int32_t INTERVENTION_SAME_SIDE = 62;
		constexpr int32_t INTERVENTION_OTHER_SIDE = 63;
		constexpr int32_t INTERVENTION = 64;
		constexpr int32_t BANKRUPTCY = 65;
		constexpr int32_t NEW_PARTY_AVAIL = 66;
		constexpr int32_t PARTY_UNAVAIL = 67;
		constexpr int32_t CB_DETECTED_CAUSE = 68;
		constexpr int32_t CB_DETECTED_TARGET = 69;
		constexpr int32_t CB_JUSTIFY_NO_LONGER_VALID = 70;
		constexpr int32_t JOIN_CRISIS_OFFER_CAUSE = 71;
		constexpr int32_t JOIN_CRISIS_OFFER_TARGET = 72;
		constexpr int32_t JOIN_CRISIS_OFFER_REJECT_CAUSE = 73;
		constexpr int32_t JOIN_CRISIS_OFFER_REJECT_TARGET = 74;
		constexpr int32_t JOIN_CRISIS_CAUSE = 75;
		constexpr int32_t JOIN_CRISIS_TARGET = 76;
		constexpr int32_t CRISIS_OFFER_CAUSE = 77;
		constexpr int32_t CRISIS_OFFER_TARGET = 78;
		constexpr int32_t CRISIS_OFFER_DECLINED_CAUSE = 79;
		constexpr int32_t CRISIS_OFFER_DECLINED_TARGET = 80;
		constexpr int32_t CRISIS_RESOLVED_CAUSE = 81;
		constexpr int32_t CRISIS_RESOLVED_TARGET = 82;
		constexpr int32_t CRISIS_STARTED = 83;
		constexpr int32_t CRISIS_BECAME_WAR_TARGET = 84;
	};

	constexpr int32_t message_count = 85;

	namespace message_category {
		constexpr int32_t combat = 0x0001;
		constexpr int32_t diplomacy = 0x0002;
		constexpr int32_t units = 0x0004;
		constexpr int32_t provinces = 0x0008;
		constexpr int32_t events = 0x0010;
		constexpr int32_t other = 0x0020;
	}

	enum class message_setting : int8_t {
		discard = 0,
		log = 1,
		popup = 2,
		popup_and_pause = 3
	};

	enum class group_setting : int8_t {
		self = 0,
		neighbors = 1,
		sphere_members = 2,
		vassals = 3,
		allies = 4,
		great_powers = 5,
		sphere_leader = 6,
		overlord = 7
	};

	constexpr int32_t group_setting_count = 8;

	struct message_display {
		int32_t max_importance = 0;
		message_setting max_setting = message_setting::discard;

		void operator+=(message_display const& o) {
			if(o.max_importance == max_importance)
				max_setting = std::max(o.max_setting, max_setting);
			else if(o.max_importance > max_importance)
				max_setting = o.max_setting;
			max_importance = std::max(max_importance, o.max_importance);
		}
		message_display operator+(message_display const& o) const {
			message_display t = *this;
			t += o;
			return t;
		}
	};

	int32_t nation_importance(world_state const& ws, nations::country_tag n);
	message_display determine_message_display(world_state const& ws, int32_t message_id, nations::country_tag n);

	constexpr auto default_popup_text = [](world_state const&, ui::xy_pair, ui::text_format&, ui::tagged_gui_object, ui::line_manager&) {};

	template<typename G, typename F = decltype(default_popup_text)>
	void handle_generic_message(world_state& ws, nations::country_tag goto_tag, message_display display, int32_t message_id, G const& replacements_maker, F const& popup_text = default_popup_text);

	using display_function = std::function<void(world_state&, ui::tagged_gui_object, ui::line_manager&, ui::text_format&)>;
	using log_display_function = std::function<void(world_state&, ui::tagged_gui_object, ui::text_box_line_manager&, ui::text_format&)>;

	struct message_instance {
		display_function func;
		std::variant<std::monostate, nations::country_tag, provinces::province_tag> goto_tag;
	};

	struct log_message_instance {
		log_display_function func;
		int32_t category = 0;
	};

	constexpr int32_t maximum_displayed_messages = 128;
	constexpr int32_t maximum_log_items = 128;

	using message_queue = concurrency::concurrent_queue<message_instance, concurrent_allocator<message_instance>>;
	using log_message_queue = concurrency::concurrent_queue<log_message_instance, concurrent_allocator<log_message_instance>>;


	class message_window_t;

	struct messages_manager {
		text_data::text_tag log_text[message_count];
		text_data::text_tag description_text[message_count];

		int8_t group_importance[group_setting_count] = {
			3i8, 1i8, 1i8, 2i8, 1i8, 1i8, 1i8, 2i8
		};
		message_setting settings[message_count * 4] = {
			message_setting::discard, message_setting::discard, message_setting::log, message_setting::popup, //GAINCB
			message_setting::discard, message_setting::discard, message_setting::log, message_setting::popup, //GAINCB_TARGET
			message_setting::discard, message_setting::discard, message_setting::log, message_setting::popup, //LOSECB
			message_setting::discard, message_setting::discard, message_setting::log, message_setting::popup, //LOSECB_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //REVOLTALLIANCE_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //REVOLTALLIANCE_TARGET
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //WAR_CAUSE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::popup_and_pause, //WAR_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //WARGOAL_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //WARGOAL_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //SOCREFORM
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //SOCREFORM_BAD
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //POLREFORM
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //POLREFORM_BAD
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //ECONOMICREFORM
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //ECONOMICREFORM_BAD
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //MILITARYREFORM
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //MILITARYREFORM_BAD
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //SETPARTY
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //UPPERHOUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //ELECTIONSTART
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //ELECTIONDONE
			message_setting::discard, message_setting::discard, message_setting::popup, message_setting::popup_and_pause, //BREAKCOUNTRY
			message_setting::discard, message_setting::discard, message_setting::popup, message_setting::popup, //REBELS
			message_setting::discard, message_setting::log, message_setting::popup, message_setting::discard, //ANNEX_CAUSE
			message_setting::discard, message_setting::log, message_setting::popup, message_setting::discard, //ANNEX_TARGET
			message_setting::discard, message_setting::log, message_setting::log, message_setting::popup, //PEACE_ACCEPT_CAUSE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //PEACE_ACCEPT_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //PEACE_DECLINE_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //PEACE_DECLINE_TARGET
			message_setting::discard, message_setting::log, message_setting::popup, message_setting::discard, //MOBILIZE
			message_setting::discard, message_setting::log, message_setting::popup, message_setting::discard, //DEMOBILIZE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::log, //BUILDING_DONE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //EVENTHAPPENOTHER
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //EVENTHAPPENOTHEROPTION
			message_setting::popup, message_setting::popup, message_setting::popup, message_setting::discard, //MAJOREVENTHAPPENOTHER
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //MAJOREVENTHAPPENOTHEROPTION
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //INVENTION
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //TECH
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::log, //LEADERDIED
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //LANDBATTLEOVER
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //NAVALBATTLEOVER
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //DECISIONOTHER
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //NO_LONGER_GREAT_POWER
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //BECAME_GREAT_POWER
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //ALLIANCE_ACCEPT_CAUSE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //ALLIANCE_ACCEPT_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //ALLIANCE_DECLINE_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //ALLIANCE_DECLINE_TARGET
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //CANCELALLIANCE_CAUSE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::popup, //CANCELALLIANCE_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //INCREASEOPINION_CAUSE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::popup, //INCREASEOPINION_TARGET
			message_setting::discard, message_setting::discard, message_setting::log, message_setting::discard, //ADDTOSPHERE_CAUSE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::popup, //ADDTOSPHERE_TARGET
			message_setting::discard, message_setting::discard, message_setting::log, message_setting::discard, //REMOVEFROMSPHERE_CAUSE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::popup, //REMOVEFROMSPHERE_TARGET
			message_setting::discard, message_setting::discard, message_setting::log, message_setting::popup, //REMOVEFROMSPHERE_OTHER_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //INCREASERELATION_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //INCREASERELATION_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //DECREASERELATION_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //DECREASERELATION_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //INTERVENTION_SAME_SIDE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //INTERVENTION_OTHER_SIDE
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //INTERVENTION
			message_setting::discard, message_setting::log, message_setting::log, message_setting::popup, //BANKRUPTCY
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //NEW_PARTY_AVAIL
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::log, //PARTY_UNAVAIL
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //CB_DETECTED_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //CB_DETECTED_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //CB_JUSTIFY_NO_LONGER_VALID
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //JOIN_CRISIS_OFFER_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //JOIN_CRISIS_OFFER_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //JOIN_CRISIS_OFFER_REJECT_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //JOIN_CRISIS_OFFER_REJECT_TARGET
			message_setting::discard, message_setting::log, message_setting::log, message_setting::discard, //JOIN_CRISIS_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //JOIN_CRISIS_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //CRISIS_OFFER_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //CRISIS_OFFER_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //CRISIS_OFFER_DECLINED_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //CRISIS_OFFER_DECLINED_TARGET
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::discard, //CRISIS_RESOLVED_CAUSE
			message_setting::discard, message_setting::discard, message_setting::discard, message_setting::popup, //CRISIS_RESOLVED_TARGET
			message_setting::popup, message_setting::popup, message_setting::popup, message_setting::popup, //CRISIS_STARTED
			message_setting::log, message_setting::log, message_setting::log, message_setting::popup, //CRISIS_BECAME_WAR_TARGET
		};
	};

	class message_window {
	public:
		std::unique_ptr<message_window_t> win;
		message_queue pending_messages;
		
		message_instance displayed_messages[maximum_displayed_messages] = {};
		int32_t current_message_count = 0;
		int32_t last_replaced_index = 0;
		int32_t currently_displayed_index = 0;

		log_message_queue pending_log_items;
		log_message_instance current_log[maximum_log_items] = { log_message_instance() };
		int32_t first_log_item = 0;

		int32_t message_categories = message_category::combat | message_category::diplomacy | message_category::events
			| message_category::other | message_category::provinces | message_category::units;

		message_window();
		~message_window();

		void hide_message_window(ui::gui_manager& gui_m);
		void update_message_window(ui::gui_manager& gui_m);
		void show_message_window(ui::gui_manager& gui_m);
		void init_message_window(world_state& ws);
	};

	class message_settings_window_t;

	class message_settings_window {
	public:
		std::unique_ptr<message_settings_window_t> win;
		bool showing_messages = true;
		bool setting_changed = false;
		uint32_t other_lb_position = 0;

		message_settings_window();
		~message_settings_window();

		void hide_message_settings_window(ui::gui_manager& gui_m);
		void update_message_settings_window(ui::gui_manager& gui_m);
		void show_message_settings_window(ui::gui_manager& gui_m);
		void init_message_settings_window(world_state& ws);
	};

	void init_message_text(scenario::scenario_manager& s);
	void submit_message(world_state& ws, std::variant<std::monostate, nations::country_tag, provinces::province_tag> goto_tag, display_function&& f);

	void cb_detected(world_state& ws, nations::country_tag by, nations::country_tag target, military::cb_type_tag type, float infamy_gained);
	void cb_construction_invalid(world_state& ws, nations::country_tag by, nations::country_tag target, military::cb_type_tag type);
	void acquired_cb(world_state& ws, nations::country_tag by, nations::country_tag target, military::cb_type_tag type);
	void lost_cb(world_state& ws, nations::country_tag by, nations::country_tag target, military::cb_type_tag type);
	void new_technology(world_state& ws, nations::country_tag by, technologies::tech_tag type);
	void new_invention(world_state& ws, nations::country_tag by, technologies::tech_tag type);
	void increase_opinion(world_state& ws, nations::country_tag by, nations::country_tag target, int32_t new_level);
	void add_to_sphere(world_state& ws, nations::country_tag sphere_leader, nations::country_tag target);
	void remove_from_sphere(world_state& ws, nations::country_tag actor, nations::country_tag old_sphere_leader, nations::country_tag target);
	void new_upper_house(world_state& ws, nations::country_tag n);
	void election_result(world_state& ws, nations::country_tag n, governments::party_tag winner, float vote);
}
