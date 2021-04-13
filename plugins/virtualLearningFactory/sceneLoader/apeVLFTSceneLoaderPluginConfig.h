//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Welcome data = nlohmann::json::parse(jsonString);
// https://app.quicktype.io/

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

    class Representation {
        public:
        Representation() = default;
        virtual ~Representation() = default;

        private:
        std::string file;
        std::shared_ptr<double> unit;

        public:
        const std::string & get_file() const { return file; }
        std::string & get_mutable_file() { return file; }
        void set_file(const std::string & value) { this->file = value; }

        std::shared_ptr<double> get_unit() const { return unit; }
        void set_unit(std::shared_ptr<double> value) { this->unit = value; }
    };

    class Asset {
        public:
        Asset() = default;
        virtual ~Asset() = default;

        private:
        std::string id;
        std::string type;
        std::shared_ptr<std::string> descr;
        std::shared_ptr<std::vector<Representation>> representations;
        std::shared_ptr<std::vector<double>> position;
        std::shared_ptr<std::string> placement_rel_to;
        std::shared_ptr<std::string> parent_object;
        std::shared_ptr<std::string> model;
        std::shared_ptr<std::vector<double>> rotation;
        std::shared_ptr<std::vector<std::string>> connected_to;

        public:
        const std::string & get_id() const { return id; }
        std::string & get_mutable_id() { return id; }
        void set_id(const std::string & value) { this->id = value; }

        const std::string & get_type() const { return type; }
        std::string & get_mutable_type() { return type; }
        void set_type(const std::string & value) { this->type = value; }

        std::shared_ptr<std::string> get_descr() const { return descr; }
        void set_descr(std::shared_ptr<std::string> value) { this->descr = value; }

        std::shared_ptr<std::vector<Representation>> get_representations() const { return representations; }
        void set_representations(std::shared_ptr<std::vector<Representation>> value) { this->representations = value; }

        std::shared_ptr<std::vector<double>> get_position() const { return position; }
        void set_position(std::shared_ptr<std::vector<double>> value) { this->position = value; }

        std::shared_ptr<std::string> get_placement_rel_to() const { return placement_rel_to; }
        void set_placement_rel_to(std::shared_ptr<std::string> value) { this->placement_rel_to = value; }

        std::shared_ptr<std::string> get_parent_object() const { return parent_object; }
        void set_parent_object(std::shared_ptr<std::string> value) { this->parent_object = value; }

        std::shared_ptr<std::string> get_model() const { return model; }
        void set_model(std::shared_ptr<std::string> value) { this->model = value; }

        std::shared_ptr<std::vector<double>> get_rotation() const { return rotation; }
        void set_rotation(std::shared_ptr<std::vector<double>> value) { this->rotation = value; }

        std::shared_ptr<std::vector<std::string>> get_connected_to() const { return connected_to; }
        void set_connected_to(std::shared_ptr<std::vector<std::string>> value) { this->connected_to = value; }
    };

    class Context {
        public:
        Context() = default;
        virtual ~Context() = default;

        private:
        double unit_of_measure_scale;
        bool zup;
        std::string repo_path;

        public:
        const double & get_unit_of_measure_scale() const { return unit_of_measure_scale; }
        double & get_mutable_unit_of_measure_scale() { return unit_of_measure_scale; }
        void set_unit_of_measure_scale(const double & value) { this->unit_of_measure_scale = value; }

        const bool & get_zup() const { return zup; }
        bool & get_mutable_zup() { return zup; }
        void set_zup(const bool & value) { this->zup = value; }

        const std::string & get_repo_path() const { return repo_path; }
        std::string & get_mutable_repo_path() { return repo_path; }
        void set_repo_path(const std::string & value) { this->repo_path = value; }
    };

    class Scene {
        public:
        Scene() = default;
        virtual ~Scene() = default;

        private:
        Context context;
        std::vector<std::string> scene;
        std::vector<Asset> assets;

        public:
        const Context & get_context() const { return context; }
        Context & get_mutable_context() { return context; }
        void set_context(const Context & value) { this->context = value; }

        const std::vector<std::string> & get_scene() const { return scene; }
        std::vector<std::string> & get_mutable_scene() { return scene; }
        void set_scene(const std::vector<std::string> & value) { this->scene = value; }

        const std::vector<Asset> & get_assets() const { return assets; }
        std::vector<Asset> & get_mutable_assets() { return assets; }
        void set_assets(const std::vector<Asset> & value) { this->assets = value; }
    };
}

namespace nlohmann {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
namespace detail {
#endif
<<<<<<< HEAD
        void from_json(const json& j, quicktype::Representation& x);
        void to_json(json& j, const quicktype::Representation& x);
=======
    void from_json(const json & j, quicktype::Representation & x);
    void to_json(json & j, const quicktype::Representation & x);
>>>>>>> 46e0671cb2bc7d856fc1550eb86b888043141f72

        void from_json(const json& j, quicktype::Asset& x);
        void to_json(json& j, const quicktype::Asset& x);

        void from_json(const json& j, quicktype::Context& x);
        void to_json(json& j, const quicktype::Context& x);

        void from_json(const json& j, quicktype::Scene& x);
        void to_json(json& j, const quicktype::Scene& x);

        inline void from_json(const json& j, quicktype::Representation& x) {
            x.set_file(j.at("file").get<std::string>());
            x.set_unit(quicktype::get_optional<double>(j, "unit"));
        }

        inline void to_json(json& j, const quicktype::Representation& x) {
            j = json::object();
            j["file"] = x.get_file();
            j["unit"] = x.get_unit();
        }

        inline void from_json(const json& j, quicktype::Asset& x) {
            x.set_id(j.at("id").get<std::string>());
            x.set_type(j.at("type").get<std::string>());
            x.set_descr(quicktype::get_optional<std::string>(j, "descr"));
            x.set_representations(quicktype::get_optional<std::vector<quicktype::Representation>>(j, "representations"));
            x.set_position(quicktype::get_optional<std::vector<double>>(j, "position"));
            x.set_placement_rel_to(quicktype::get_optional<std::string>(j, "placementRelTo"));
            x.set_parent_object(quicktype::get_optional<std::string>(j, "parentObject"));
            x.set_model(quicktype::get_optional<std::string>(j, "model"));
            x.set_rotation(quicktype::get_optional<std::vector<double>>(j, "rotation"));
            x.set_connected_to(quicktype::get_optional<std::vector<std::string>>(j, "connectedTo"));
        }

        inline void to_json(json& j, const quicktype::Asset& x) {
            j = json::object();
            j["id"] = x.get_id();
            j["type"] = x.get_type();
            j["descr"] = x.get_descr();
            j["representations"] = x.get_representations();
            j["position"] = x.get_position();
            j["placementRelTo"] = x.get_placement_rel_to();
            j["parentObject"] = x.get_parent_object();
            j["model"] = x.get_model();
            j["rotation"] = x.get_rotation();
            j["connectedTo"] = x.get_connected_to();
        }

        inline void from_json(const json& j, quicktype::Context& x) {
            x.set_unit_of_measure_scale(j.at("UnitOfMeasureScale").get<double>());
            x.set_zup(j.at("Zup").get<bool>());
            x.set_repo_path(j.at("RepoPath").get<std::string>());
        }

        inline void to_json(json& j, const quicktype::Context& x) {
            j = json::object();
            j["UnitOfMeasureScale"] = x.get_unit_of_measure_scale();
            j["Zup"] = x.get_zup();
            j["RepoPath"] = x.get_repo_path();
        }

        inline void from_json(const json& j, quicktype::Scene& x) {
            x.set_context(j.at("context").get<quicktype::Context>());
            x.set_scene(j.at("scene").get<std::vector<std::string>>());
            x.set_assets(j.at("assets").get<std::vector<quicktype::Asset>>());
        }

        inline void to_json(json& j, const quicktype::Scene& x) {
            j = json::object();
            j["context"] = x.get_context();
            j["scene"] = x.get_scene();
            j["assets"] = x.get_assets();
        }
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    }
<<<<<<< HEAD
=======
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
}
>>>>>>> 46e0671cb2bc7d856fc1550eb86b888043141f72
#endif
}

