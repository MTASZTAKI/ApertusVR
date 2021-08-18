//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Scene data = nlohmann::json::parse(jsonString);

#pragma once

#include "nlohmann/json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>

namespace quicktype {
    using nlohmann::json;

    inline json get_untyped(const json& j, const char* property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<json>();
        }
        return json();
    }

    inline json get_untyped(const json& j, std::string property) {
        return get_untyped(j, property.data());
    }

    class Scene {
    public:
        Scene() = default;
        virtual ~Scene() = default;

    private:
        std::string render_system;

    public:
        const std::string& get_render_system() const { return render_system; }
        std::string& get_mutable_render_system() { return render_system; }
        void set_render_system(const std::string& value) { this->render_system = value; }
    };
}

namespace nlohmann {
    void from_json(const json& j, quicktype::Scene& x);
    void to_json(json& j, const quicktype::Scene& x);

    inline void from_json(const json& j, quicktype::Scene& x) {
        x.set_render_system(j.at("renderSystem").get<std::string>());
    }

    inline void to_json(json& j, const quicktype::Scene& x) {
        j = json::object();
        j["renderSystem"] = x.get_render_system();
    }
}
