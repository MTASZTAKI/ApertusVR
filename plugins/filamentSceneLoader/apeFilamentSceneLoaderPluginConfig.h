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

    class Context {
    public:
        Context() = default;
        virtual ~Context() = default;

    private:
        int64_t unit_of_measure_scale;
        bool zup;
        std::string repo_path;

    public:
        const int64_t& get_unit_of_measure_scale() const { return unit_of_measure_scale; }
        int64_t& get_mutable_unit_of_measure_scale() { return unit_of_measure_scale; }
        void set_unit_of_measure_scale(const int64_t& value) { this->unit_of_measure_scale = value; }

        const bool& get_zup() const { return zup; }
        bool& get_mutable_zup() { return zup; }
        void set_zup(const bool& value) { this->zup = value; }

        const std::string& get_repo_path() const { return repo_path; }
        std::string& get_mutable_repo_path() { return repo_path; }
        void set_repo_path(const std::string& value) { this->repo_path = value; }
    };

    class SceneDescription {
    public:
        SceneDescription() = default;
        virtual ~SceneDescription() = default;

    private:
        quicktype::Context context;
        std::vector<std::string> scene;
        std::vector<int64_t> clones;
        std::vector<std::vector<double>> positions;

    public:
        const quicktype::Context& get_context() const { return context; }
        quicktype::Context& get_mutable_context() { return context; }
        void set_context(const quicktype::Context& value) { this->context = value; }

        const std::vector<std::string>& get_scene() const { return scene; }
        std::vector<std::string>& get_mutable_scene() { return scene; }
        void set_scene(const std::vector<std::string>& value) { this->scene = value; }

        const std::vector<int64_t>& get_clones() const { return clones; }
        std::vector<int64_t>& get_mutable_clones() { return clones; }
        void set_clones(const std::vector<int64_t>& value) { this->clones = value; }

        const std::vector<std::vector<double>>& get_positions() const { return positions; }
        std::vector<std::vector<double>>& get_mutable_positions() { return positions; }
        void set_positions(const std::vector<std::vector<double>>& value) { this->positions = value; }
    };
}

namespace nlohmann {
    namespace detail {
        void from_json(const json& j, quicktype::Context& x);
        void to_json(json& j, const quicktype::Context& x);

        void from_json(const json& j, quicktype::SceneDescription& x);
        void to_json(json& j, const quicktype::SceneDescription& x);

        inline void from_json(const json& j, quicktype::Context& x) {
            x.set_unit_of_measure_scale(j.at("UnitOfMeasureScale").get<int64_t>());
            x.set_zup(j.at("Zup").get<bool>());
            x.set_repo_path(j.at("RepoPath").get<std::string>());
        }

        inline void to_json(json& j, const quicktype::Context& x) {
            j = json::object();
            j["UnitOfMeasureScale"] = x.get_unit_of_measure_scale();
            j["Zup"] = x.get_zup();
            j["RepoPath"] = x.get_repo_path();
        }

        inline void from_json(const json& j, quicktype::SceneDescription& x) {
            x.set_context(j.at("context").get<quicktype::Context>());
            x.set_scene(j.at("scene").get<std::vector<std::string>>());
            x.set_clones(j.at("clones").get<std::vector<int64_t>>());
            x.set_positions(j.at("positions").get<std::vector<std::vector<double>>>());
        }

        inline void to_json(json& j, const quicktype::SceneDescription& x) {
            j = json::object();
            j["context"] = x.get_context();
            j["scene"] = x.get_scene();
            j["clones"] = x.get_clones();
            j["positions"] = x.get_positions();
        }
    }
}
