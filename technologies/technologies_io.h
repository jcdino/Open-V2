#pragma once
#include "common\\common.h"
#include "technologies.h"
#include "simple_fs\\simple_fs.h"
#include "Parsers\\parsers.hpp"
#include "text_data\\text_data.h"
#include "simple_serialize\\simple_serialize.hpp"
#include <ppl.h>

class world_state;

template<>
class serialization::serializer<technologies::technology_category> : public serialization::memcpy_serializer<technologies::technology_category> {};
template<>
class serialization::serializer<technologies::technology_subcategory> : public serialization::memcpy_serializer<technologies::technology_subcategory> {};
template<>
class serialization::serializer<technologies::technology> : public serialization::memcpy_serializer<technologies::technology> {};

template<>
class serialization::serializer<technologies::technologies_state> {
public:
	static constexpr bool has_static_size = false;
	static constexpr bool has_simple_serialize = false;

	static void serialize_object(std::byte* &output, technologies::technologies_state const& obj, world_state const&) {
		//serialize(output, obj.discovery_count);
	}
	static void deserialize_object(std::byte const* &input, technologies::technologies_state& obj, world_state&) {
		//deserialize(input, obj.discovery_count);
	}
	static size_t size(technologies::technologies_state const& obj, world_state const&) {
		return 0;
	}
};

template<>
class serialization::serializer<technologies::technologies_manager> {
public:
	static constexpr bool has_static_size = false;
	static constexpr bool has_simple_serialize = false;

	static void rebuild_indexes(technologies::technologies_manager& obj) {
		for(auto const& i_cat : obj.technology_categories)
			obj.named_category_index.emplace(i_cat.name, i_cat.id);
		for(auto const& i_subcat : obj.technology_subcategories)
			obj.named_subcategory_index.emplace(i_subcat.name, i_subcat.id);
		for(auto const& i_tech : obj.technologies_container)
			obj.named_technology_index.emplace(i_tech.name, i_tech.id);
	}

	static void serialize_object(std::byte* &output, technologies::technologies_manager const& obj) {
		serialize(output, obj.technology_categories);
		serialize(output, obj.technology_subcategories);
		serialize(output, obj.technologies_container);
		serialize(output, obj.inventions);
		serialize(output, obj.production_adjustments);
		// serialize(output, obj.unit_type_adjustments);
		serialize(output, obj.rebel_org_gain);
		serialize(output, obj.tech_modifier_names);
	}
	static void deserialize_object(std::byte const* &input, technologies::technologies_manager& obj) {
		deserialize(input, obj.technology_categories);
		deserialize(input, obj.technology_subcategories);
		deserialize(input, obj.technologies_container);
		deserialize(input, obj.inventions);
		deserialize(input, obj.production_adjustments);
		// deserialize(input, obj.unit_type_adjustments);
		deserialize(input, obj.rebel_org_gain);
		deserialize(input, obj.tech_modifier_names);

		rebuild_indexes(obj);
	}
	static void deserialize_object(std::byte const* &input, technologies::technologies_manager& obj, concurrency::task_group& tg) {
		deserialize(input, obj.technology_categories);
		deserialize(input, obj.technology_subcategories);
		deserialize(input, obj.technologies_container);
		deserialize(input, obj.inventions);
		deserialize(input, obj.production_adjustments);
		// deserialize(input, obj.unit_type_adjustments);
		deserialize(input, obj.rebel_org_gain);
		deserialize(input, obj.tech_modifier_names);

		tg.run([&obj]() { rebuild_indexes(obj); });
	}
	static size_t size(technologies::technologies_manager const& obj) {
		return 
			serialize_size(obj.technology_categories) +
			serialize_size(obj.technology_subcategories) +
			serialize_size(obj.technologies_container) +
			serialize_size(obj.inventions) +
			serialize_size(obj.production_adjustments) +
			// serialize_size(obj.unit_type_adjustments) +
			serialize_size(obj.rebel_org_gain) + 
			serialize_size(obj.tech_modifier_names);
	}
	template<typename T>
	static size_t size(technologies::technologies_manager const& obj, T const&) {
		return size(obj);
	}
};

namespace graphics {
	class texture_manager;
}

namespace technologies {
	struct parsing_environment;

	class parsing_state {
	public:
		std::unique_ptr<parsing_environment> impl;

		parsing_state(text_data::text_sequences& tl, const directory& tech_directory, technologies_manager& m, modifiers::modifiers_manager& mm, graphics::texture_manager& tex);
		parsing_state(parsing_state&&) noexcept;
		~parsing_state();
	};

	void pre_parse_single_tech_file(parsing_environment& state, const token_group* start, const token_group* end);
	void pre_parse_technologies(
		parsing_state& state,
		const directory& source_directory);
	void pre_parse_inventions(
		parsing_state& state,
		const directory& source_directory);

	void prepare_technologies_read(scenario::scenario_manager& s);
	void read_inventions(parsing_state const& state, scenario::scenario_manager& s);
	void read_technologies(parsing_state const& state, scenario::scenario_manager& s);
	void name_tech_modifiers(technologies_manager& m, text_data::text_sequences& text);
}
