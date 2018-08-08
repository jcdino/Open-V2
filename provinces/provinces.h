#pragma once
#include <stdint.h>
#include "common\\common.h"
#include "common\\shared_tags.h"
#include "concurrency_tools\\concurrency_tools.hpp"

namespace modifiers {
	class modifiers_manager;
}

namespace nations {
	struct nation;
	struct state_instance;
}

namespace provinces {
	struct timed_provincial_modifier {
		date_tag expiration;
		modifiers::provincial_modifier_tag mod;

		bool operator<(timed_provincial_modifier const& other)  const noexcept { return mod < other.mod; }
		bool operator==(timed_provincial_modifier const& other) const noexcept { return mod == other.mod; }
	};

	struct province_state {
		nations::nation* owner = nullptr; // 8
		nations::nation* controller = nullptr; // 16
		population::rebel_faction* rebel_controller = nullptr; // 24
		nations::state_instance* state_instance = nullptr; // 32

		atomic_tag<date_tag> last_update; // 36
		float nationalism = 0.0f; // 40
		float siege_progress = 0.0f;
		date_tag last_controller_change;
		date_tag last_immigration;

		int32_t employed_workers = 0; // in RGO
		float last_produced = 0.0f; // in RGO

		set_tag<cultures::national_tag> cores;
		array_tag<population::pop_tag> pops;
		set_tag<modifiers::provincial_modifier_tag> static_modifiers;
		set_tag<timed_provincial_modifier> timed_modifiers;
		//array of active plans involving this province

		text_data::text_tag name;
		modifiers::provincial_modifier_tag crime;
		modifiers::provincial_modifier_tag terrain;
		int16_t life_rating = 0i16;

		province_tag id;

		cultures::culture_tag dominant_culture;

		economy::goods_tag rgo_production;
		economy::goods_tag artisan_production;

		issues::option_tag dominant_issue;
		ideologies::ideology_tag dominant_ideology;
		cultures::religion_tag dominant_religion;

		uint8_t fort_level = 0ui8;
		uint8_t railroad_level = 0ui8;
		uint8_t naval_base_level = 0ui8;
		uint8_t rgo_size = 1ui8;

		uint8_t flags = 0ui8;

		constexpr static uint8_t is_blockaded = 0x01;
		constexpr static uint8_t is_overseas  = 0x02;
	};

	struct province {
		constexpr static uint16_t sea = 0x0001;
		constexpr static uint16_t coastal = 0x0002;
		constexpr static uint16_t lake = 0x0004;

		modifiers::provincial_modifier_tag continent;
		modifiers::provincial_modifier_tag climate;

		state_tag state_id;
		province_tag id;

		uint16_t flags = 0;
	};

	

	class provinces_state {
	public:
		tagged_vector<province_state, province_tag> province_state_container;
		tagged_fixed_blocked_2dvector<modifiers::value_type, province_tag, uint32_t, aligned_allocator_32<modifiers::value_type>> provincial_modifiers;
		tagged_fixed_2dvector<float, province_tag, ideologies::ideology_tag> party_loyalty;
		tagged_fixed_blocked_2dvector<int32_t, province_tag, population::demo_tag, aligned_allocator_32<int32_t>> province_demographics;

		std::vector<uint8_t> is_canal_enabled;

		stable_variable_vector_storage_mk_2<cultures::national_tag, 4, 8192> core_arrays;
		stable_variable_vector_storage_mk_2<modifiers::provincial_modifier_tag, 4, 8192> static_modifier_arrays;
		stable_variable_vector_storage_mk_2<timed_provincial_modifier, 4, 8192> timed_modifier_arrays;

		stable_variable_vector_storage_mk_2<province_tag, 4, 8192> province_arrays;

		// backs:
		//  nation -> owned provinces
		//  nation -> controlled provinces
		//  tag -> core provinces
	};

	class province_manager {
	public:
		tagged_vector<province, province_tag> province_container;
		tagged_vector<text_data::text_tag, state_tag> state_names;

		v_vector<province_tag, state_tag> states_to_province_index;
		boost::container::flat_map<text_data::text_tag, state_tag> named_states_index;

		v_vector<province_tag, province_tag> same_type_adjacency;
		v_vector<province_tag, province_tag> coastal_adjacency;
		std::vector<std::pair<province_tag, province_tag>> canals;

		boost::container::flat_map<modifiers::provincial_modifier_tag, graphics::obj_definition_tag> terrain_graphics;

		std::vector<uint16_t> province_map_data;
		int32_t province_map_width = 0;
		int32_t province_map_height = 0; 
	};

	struct color_to_terrain_map {
		modifiers::provincial_modifier_tag data[256] = { modifiers::provincial_modifier_tag() };
	};

	constexpr uint32_t rgb_to_prov_index(uint8_t r, uint8_t g, uint8_t b) {
		return static_cast<uint32_t>(r) | (static_cast<uint32_t>(g) << 8) | (static_cast<uint32_t>(b) << 16);
	}
}
