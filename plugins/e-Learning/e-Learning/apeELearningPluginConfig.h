//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     quicktype::Welcome data = nlohmann::json::parse(jsonString);

#pragma once

#include <nlohmann/json.hpp>

#include <optional>
#include <stdexcept>
#include <regex>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
	template <typename T>
	struct adl_serializer<std::shared_ptr<T>> {
		static void to_json(json & j, const std::shared_ptr<T> & opt) {
			if (!opt) j = nullptr; else j = *opt;
		}

		static std::shared_ptr<T> from_json(const json & j) {
			if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
		}
	};
}
#endif

namespace quicktype {
	using nlohmann::json;

	inline json get_untyped(const json & j, const char * property) {
		if (j.find(property) != j.end()) {
			return j.at(property).get<json>();
		}
		return json();
	}

	inline json get_untyped(const json & j, std::string property) {
		return get_untyped(j, property.data());
	}

	template <typename T>
	inline std::shared_ptr<T> get_optional(const json & j, const char * property) {
		if (j.find(property) != j.end()) {
			return j.at(property).get<std::shared_ptr<T>>();
		}
		return std::shared_ptr<T>();
	}

	template <typename T>
	inline std::shared_ptr<T> get_optional(const json & j, std::string property) {
		return get_optional<T>(j, property.data());
	}

	class Graph {
	public:
		Graph() = default;
		virtual ~Graph() = default;

	private:
		std::string src;
		std::string dst;
		int64_t angle;

	public:
		const std::string & get_src() const { return src; }
		std::string & get_mutable_src() { return src; }
		void set_src(const std::string & value) { this->src = value; }

		const std::string & get_dst() const { return dst; }
		std::string & get_mutable_dst() { return dst; }
		void set_dst(const std::string & value) { this->dst = value; }

		const int64_t & get_angle() const { return angle; }
		int64_t & get_mutable_angle() { return angle; }
		void set_angle(const int64_t & value) { this->angle = value; }
	};

	class Hotspot {
	public:
		Hotspot() = default;
		virtual ~Hotspot() = default;

	private:
		std::string id;
		double h;
		double v;
		double z;
		double width;
		int64_t src_width;
		int64_t src_height;
		bool transparent;
		std::vector<std::string> textures;
		int64_t texture_index;
		bool disabled;
		std::string onclick;
		std::string gameurl;
		int64_t iframe_height;
		int64_t iframe_width;

	public:
		const std::string & get_id() const { return id; }
		std::string & get_mutable_id() { return id; }
		void set_id(const std::string & value) { this->id = value; }

		const double & get_h() const { return h; }
		double & get_mutable_h() { return h; }
		void set_h(const double & value) { this->h = value; }

		const double & get_v() const { return v; }
		double & get_mutable_v() { return v; }
		void set_v(const double & value) { this->v = value; }

		const double & get_z() const { return z; }
		double & get_mutable_z() { return z; }
		void set_z(const double & value) { this->z = value; }

		const double & get_width() const { return width; }
		double & get_mutable_width() { return width; }
		void set_width(const double & value) { this->width = value; }

		const int64_t & get_src_width() const { return src_width; }
		int64_t & get_mutable_src_width() { return src_width; }
		void set_src_width(const int64_t & value) { this->src_width = value; }

		const int64_t & get_src_height() const { return src_height; }
		int64_t & get_mutable_src_height() { return src_height; }
		void set_src_height(const int64_t & value) { this->src_height = value; }

		const bool & get_transparent() const { return transparent; }
		bool & get_mutable_transparent() { return transparent; }
		void set_transparent(const bool & value) { this->transparent = value; }

		const std::vector<std::string> & get_textures() const { return textures; }
		std::vector<std::string> & get_mutable_textures() { return textures; }
		void set_textures(const std::vector<std::string> & value) { this->textures = value; }

		const int64_t & get_texture_index() const { return texture_index; }
		int64_t & get_mutable_texture_index() { return texture_index; }
		void set_texture_index(const int64_t & value) { this->texture_index = value; }

		const bool & get_disabled() const { return disabled; }
		bool & get_mutable_disabled() { return disabled; }
		void set_disabled(const bool & value) { this->disabled = value; }

		const std::string & get_onclick() const { return onclick; }
		std::string & get_mutable_onclick() { return onclick; }
		void set_onclick(const std::string & value) { this->onclick = value; }

		const std::string & get_gameurl() const { return gameurl; }
		std::string & get_mutable_gameurl() { return gameurl; }
		void set_gameurl(const std::string & value) { this->gameurl = value; }

		const int64_t & get_iframe_height() const { return iframe_height; }
		int64_t & get_mutable_iframe_height() { return iframe_height; }
		void set_iframe_height(const int64_t & value) { this->iframe_height = value; }

		const int64_t & get_iframe_width() const { return iframe_width; }
		int64_t & get_mutable_iframe_width() { return iframe_width; }
		void set_iframe_width(const int64_t & value) { this->iframe_width = value; }
	};

	class Room {
	public:
		Room() = default;
		virtual ~Room() = default;

	private:
		std::string id;
		std::string texture;
		int64_t rotation;
		std::shared_ptr<std::vector<quicktype::Hotspot>> hotspots;

	public:
		const std::string & get_id() const { return id; }
		std::string & get_mutable_id() { return id; }
		void set_id(const std::string & value) { this->id = value; }

		const std::string & get_texture() const { return texture; }
		std::string & get_mutable_texture() { return texture; }
		void set_texture(const std::string & value) { this->texture = value; }

		const int64_t & get_rotation() const { return rotation; }
		int64_t & get_mutable_rotation() { return rotation; }
		void set_rotation(const int64_t & value) { this->rotation = value; }

		std::shared_ptr<std::vector<quicktype::Hotspot>> get_hotspots() const { return hotspots; }
		void set_hotspots(std::shared_ptr<std::vector<quicktype::Hotspot>> value) { this->hotspots = value; }
	};

	class Welcome {
	public:
		Welcome() = default;
		virtual ~Welcome() = default;

	private:
		std::vector<quicktype::Graph> graph;
		std::string start_room;
		int64_t start_angle;
		int64_t start_vertical_angle;
		double minimap_scale;
		int64_t minimap_start_angle;
		std::string minimap_image;
		std::vector<quicktype::Room> rooms;

	public:
		const std::vector<quicktype::Graph> & get_graph() const { return graph; }
		std::vector<quicktype::Graph> & get_mutable_graph() { return graph; }
		void set_graph(const std::vector<quicktype::Graph> & value) { this->graph = value; }

		const std::string & get_start_room() const { return start_room; }
		std::string & get_mutable_start_room() { return start_room; }
		void set_start_room(const std::string & value) { this->start_room = value; }

		const int64_t & get_start_angle() const { return start_angle; }
		int64_t & get_mutable_start_angle() { return start_angle; }
		void set_start_angle(const int64_t & value) { this->start_angle = value; }

		const int64_t & get_start_vertical_angle() const { return start_vertical_angle; }
		int64_t & get_mutable_start_vertical_angle() { return start_vertical_angle; }
		void set_start_vertical_angle(const int64_t & value) { this->start_vertical_angle = value; }

		const double & get_minimap_scale() const { return minimap_scale; }
		double & get_mutable_minimap_scale() { return minimap_scale; }
		void set_minimap_scale(const double & value) { this->minimap_scale = value; }

		const int64_t & get_minimap_start_angle() const { return minimap_start_angle; }
		int64_t & get_mutable_minimap_start_angle() { return minimap_start_angle; }
		void set_minimap_start_angle(const int64_t & value) { this->minimap_start_angle = value; }

		const std::string & get_minimap_image() const { return minimap_image; }
		std::string & get_mutable_minimap_image() { return minimap_image; }
		void set_minimap_image(const std::string & value) { this->minimap_image = value; }

		const std::vector<quicktype::Room> & get_rooms() const { return rooms; }
		std::vector<quicktype::Room> & get_mutable_rooms() { return rooms; }
		void set_rooms(const std::vector<quicktype::Room> & value) { this->rooms = value; }
	};
}

namespace nlohmann {
	namespace detail {
		void from_json(const json & j, quicktype::Graph & x);
		void to_json(json & j, const quicktype::Graph & x);

		void from_json(const json & j, quicktype::Hotspot & x);
		void to_json(json & j, const quicktype::Hotspot & x);

		void from_json(const json & j, quicktype::Room & x);
		void to_json(json & j, const quicktype::Room & x);

		void from_json(const json & j, quicktype::Welcome & x);
		void to_json(json & j, const quicktype::Welcome & x);

		inline void from_json(const json & j, quicktype::Graph& x) {
			x.set_src(j.at("src").get<std::string>());
			x.set_dst(j.at("dst").get<std::string>());
			x.set_angle(j.at("angle").get<int64_t>());
		}

		inline void to_json(json & j, const quicktype::Graph & x) {
			j = json::object();
			j["src"] = x.get_src();
			j["dst"] = x.get_dst();
			j["angle"] = x.get_angle();
		}

		inline void from_json(const json & j, quicktype::Hotspot& x) {
			x.set_id(j.at("id").get<std::string>());
			x.set_h(j.at("h").get<double>());
			x.set_v(j.at("v").get<double>());
			x.set_z(j.at("z").get<double>());
			x.set_width(j.at("width").get<double>());
			x.set_src_width(j.at("srcWidth").get<int64_t>());
			x.set_src_height(j.at("srcHeight").get<int64_t>());
			x.set_transparent(j.at("transparent").get<bool>());
			x.set_textures(j.at("textures").get<std::vector<std::string>>());
			x.set_texture_index(j.at("textureIndex").get<int64_t>());
			x.set_disabled(j.at("disabled").get<bool>());
			x.set_onclick(j.at("onclick").get<std::string>());
			x.set_gameurl(j.at("gameurl").get<std::string>());
			x.set_iframe_height(j.at("iframeHeight").get<int64_t>());
			x.set_iframe_width(j.at("iframeWidth").get<int64_t>());
		}

		inline void to_json(json & j, const quicktype::Hotspot & x) {
			j = json::object();
			j["id"] = x.get_id();
			j["h"] = x.get_h();
			j["v"] = x.get_v();
			j["z"] = x.get_z();
			j["width"] = x.get_width();
			j["srcWidth"] = x.get_src_width();
			j["srcHeight"] = x.get_src_height();
			j["transparent"] = x.get_transparent();
			j["textures"] = x.get_textures();
			j["textureIndex"] = x.get_texture_index();
			j["disabled"] = x.get_disabled();
			j["onclick"] = x.get_onclick();
			j["gameurl"] = x.get_gameurl();
			j["iframeHeight"] = x.get_iframe_height();
			j["iframeWidth"] = x.get_iframe_width();
		}

		inline void from_json(const json & j, quicktype::Room& x) {
			x.set_id(j.at("id").get<std::string>());
			x.set_texture(j.at("texture").get<std::string>());
			x.set_rotation(j.at("rotation").get<int64_t>());
			x.set_hotspots(quicktype::get_optional<std::vector<quicktype::Hotspot>>(j, "hotspots"));
		}

		inline void to_json(json & j, const quicktype::Room & x) {
			j = json::object();
			j["id"] = x.get_id();
			j["texture"] = x.get_texture();
			j["rotation"] = x.get_rotation();
			j["hotspots"] = x.get_hotspots();
		}

		inline void from_json(const json & j, quicktype::Welcome& x) {
			x.set_graph(j.at("graph").get<std::vector<quicktype::Graph>>());
			x.set_start_room(j.at("startRoom").get<std::string>());
			x.set_start_angle(j.at("startAngle").get<int64_t>());
			x.set_start_vertical_angle(j.at("startVerticalAngle").get<int64_t>());
			x.set_minimap_scale(j.at("minimapScale").get<double>());
			x.set_minimap_start_angle(j.at("minimapStartAngle").get<int64_t>());
			x.set_minimap_image(j.at("minimapImage").get<std::string>());
			x.set_rooms(j.at("rooms").get<std::vector<quicktype::Room>>());
		}

		inline void to_json(json & j, const quicktype::Welcome & x) {
			j = json::object();
			j["graph"] = x.get_graph();
			j["startRoom"] = x.get_start_room();
			j["startAngle"] = x.get_start_angle();
			j["startVerticalAngle"] = x.get_start_vertical_angle();
			j["minimapScale"] = x.get_minimap_scale();
			j["minimapStartAngle"] = x.get_minimap_start_angle();
			j["minimapImage"] = x.get_minimap_image();
			j["rooms"] = x.get_rooms();
		}
	}
}
