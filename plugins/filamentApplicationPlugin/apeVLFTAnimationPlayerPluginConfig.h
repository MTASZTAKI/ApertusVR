//  To parse this JSON data, first install
//
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     animationQuicktype::Animations data = nlohmann::json::parse(jsonString);
// generated by https://app.Quicktype.io/ with all options are OFF and Others/Use Permissive is ON

#pragma once

#include "nlohmann/json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>

#ifndef NLOHMANN_OPT_HELPER
#define NLOHMANN_OPT_HELPER
namespace nlohmann {
    template <typename T>
    struct adl_serializer<std::shared_ptr<T>> {
        static void to_json(json& j, const std::shared_ptr<T>& opt) {
            if (!opt) j = nullptr; else j = *opt;
        }

        static std::shared_ptr<T> from_json(const json& j) {
            if (j.is_null()) return std::unique_ptr<T>(); else return std::unique_ptr<T>(new T(j.get<T>()));
        }
    };
}
#endif

namespace animationQuicktype {
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

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json& j, const char* property) {
        if (j.find(property) != j.end()) {
            return j.at(property).get<std::shared_ptr<T>>();
        }
        return std::shared_ptr<T>();
    }

    template <typename T>
    inline std::shared_ptr<T> get_optional(const json& j, std::string property) {
        return get_optional<T>(j, property.data());
    }

    class Bookmark {
    public:
        Bookmark() = default;
        virtual ~Bookmark() = default;

    private:
        std::string name;
        std::string time;

    public:
        const std::string& get_name() const { return name; }
        std::string& get_mutable_name() { return name; }
        void set_name(const std::string& value) { this->name = value; }

        const std::string& get_time() const { return time; }
        std::string& get_mutable_time() { return time; }
        void set_time(const std::string& value) { this->time = value; }
    };

    class Context {
    public:
        Context() = default;
        virtual ~Context() = default;

    private:
        bool asset_trail;

    public:
        const bool& get_asset_trail() const { return asset_trail; }
        bool& get_mutable_asset_trail() { return asset_trail; }
        void set_asset_trail(const bool& value) { this->asset_trail = value; }
    };

    enum class EventType : int { ANIMATION, ANIMATION_ADDITIVE, ATTACH, HIDE, LINK, SHOW, STATE, TRAIL };

    class Event {
    public:
        Event() = default;
        virtual ~Event() = default;

    private:
        animationQuicktype::EventType type;
        std::shared_ptr<std::string> descr;
        std::shared_ptr<std::vector<double>> position;
        std::shared_ptr<std::vector<double>> rotation;
        std::shared_ptr<std::string> placement_rel_to;
        std::shared_ptr<std::string> data;
        std::shared_ptr<bool> value;
        std::shared_ptr<std::string> url;

    public:
        const animationQuicktype::EventType& get_type() const { return type; }
        animationQuicktype::EventType& get_mutable_type() { return type; }
        void set_type(const animationQuicktype::EventType& value) { this->type = value; }

        std::shared_ptr<std::string> get_descr() const { return descr; }
        void set_descr(std::shared_ptr<std::string> value) { this->descr = value; }

        std::shared_ptr<std::vector<double>> get_position() const { return position; }
        void set_position(std::shared_ptr<std::vector<double>> value) { this->position = value; }

        std::shared_ptr<std::vector<double>> get_rotation() const { return rotation; }
        void set_rotation(std::shared_ptr<std::vector<double>> value) { this->rotation = value; }

        std::shared_ptr<std::string> get_placement_rel_to() const { return placement_rel_to; }
        void set_placement_rel_to(std::shared_ptr<std::string> value) { this->placement_rel_to = value; }

        std::shared_ptr<std::string> get_data() const { return data; }
        void set_data(std::shared_ptr<std::string> value) { this->data = value; }

        std::shared_ptr<bool> get_value() const { return value; }
        void set_value(std::shared_ptr<bool> value) { this->value = value; }

        std::shared_ptr<std::string> get_url() const { return url; }
        void set_url(std::shared_ptr<std::string> value) { this->url = value; }
    };

    enum class TriggerType : int { TIMESTAMP };

    class Trigger {
    public:
        Trigger() = default;
        virtual ~Trigger() = default;

    private:
        animationQuicktype::TriggerType type;
        std::string data;

    public:
        const animationQuicktype::TriggerType& get_type() const { return type; }
        animationQuicktype::TriggerType& get_mutable_type() { return type; }
        void set_type(const animationQuicktype::TriggerType& value) { this->type = value; }

        const std::string& get_data() const { return data; }
        std::string& get_mutable_data() { return data; }
        void set_data(const std::string& value) { this->data = value; }
    };

    class Action {
    public:
        Action() = default;
        virtual ~Action() = default;

    private:
        animationQuicktype::Trigger trigger;
        animationQuicktype::Event event;

    public:
        const animationQuicktype::Trigger& get_trigger() const { return trigger; }
        animationQuicktype::Trigger& get_mutable_trigger() { return trigger; }
        void set_trigger(const animationQuicktype::Trigger& value) { this->trigger = value; }

        const animationQuicktype::Event& get_event() const { return event; }
        animationQuicktype::Event& get_mutable_event() { return event; }
        void set_event(const animationQuicktype::Event& value) { this->event = value; }
    };

    class Node {
    public:
        Node() = default;
        virtual ~Node() = default;

    private:
        std::string name;
        std::vector<animationQuicktype::Action> actions;

    public:
        const std::string& get_name() const { return name; }
        std::string& get_mutable_name() { return name; }
        void set_name(const std::string& value) { this->name = value; }

        const std::vector<animationQuicktype::Action>& get_actions() const { return actions; }
        std::vector<animationQuicktype::Action>& get_mutable_actions() { return actions; }
        void set_actions(const std::vector<animationQuicktype::Action>& value) { this->actions = value; }
    };

    class Animations {
    public:
        Animations() = default;
        virtual ~Animations() = default;

    private:
        animationQuicktype::Context context;
        std::vector<animationQuicktype::Node> nodes;
        std::vector<animationQuicktype::Bookmark> bookmarks;

    public:
        const animationQuicktype::Context& get_context() const { return context; }
        animationQuicktype::Context& get_mutable_context() { return context; }
        void set_context(const animationQuicktype::Context& value) { this->context = value; }

        const std::vector<animationQuicktype::Node>& get_nodes() const { return nodes; }
        std::vector<animationQuicktype::Node>& get_mutable_nodes() { return nodes; }
        void set_nodes(const std::vector<animationQuicktype::Node>& value) { this->nodes = value; }

        const std::vector<animationQuicktype::Bookmark>& get_bookmarks() const { return bookmarks; }
        std::vector<animationQuicktype::Bookmark>& get_mutable_bookmarks() { return bookmarks; }
        void set_bookmarks(const std::vector<animationQuicktype::Bookmark>& value) { this->bookmarks = value; }
    };
}

namespace nlohmann {
    namespace detail {
        void from_json(const json& j, animationQuicktype::Bookmark& x);
        void to_json(json& j, const animationQuicktype::Bookmark& x);

        void from_json(const json& j, animationQuicktype::Context& x);
        void to_json(json& j, const animationQuicktype::Context& x);

        void from_json(const json& j, animationQuicktype::Event& x);
        void to_json(json& j, const animationQuicktype::Event& x);

        void from_json(const json& j, animationQuicktype::Trigger& x);
        void to_json(json& j, const animationQuicktype::Trigger& x);

        void from_json(const json& j, animationQuicktype::Action& x);
        void to_json(json& j, const animationQuicktype::Action& x);

        void from_json(const json& j, animationQuicktype::Node& x);
        void to_json(json& j, const animationQuicktype::Node& x);

        void from_json(const json& j, animationQuicktype::Animations& x);
        void to_json(json& j, const animationQuicktype::Animations& x);

        void from_json(const json& j, animationQuicktype::EventType& x);
        void to_json(json& j, const animationQuicktype::EventType& x);

        void from_json(const json& j, animationQuicktype::TriggerType& x);
        void to_json(json& j, const animationQuicktype::TriggerType& x);

        inline void from_json(const json& j, animationQuicktype::Bookmark& x) {
            x.set_name(j.at("name").get<std::string>());
            x.set_time(j.at("time").get<std::string>());
        }

        inline void to_json(json& j, const animationQuicktype::Bookmark& x) {
            j = json::object();
            j["name"] = x.get_name();
            j["time"] = x.get_time();
        }

        inline void from_json(const json& j, animationQuicktype::Context& x) {
            x.set_asset_trail(j.at("assetTrail").get<bool>());
        }

        inline void to_json(json& j, const animationQuicktype::Context& x) {
            j = json::object();
            j["assetTrail"] = x.get_asset_trail();
        }

        inline void from_json(const json& j, animationQuicktype::Event& x) {
            x.set_type(j.at("type").get<animationQuicktype::EventType>());
            x.set_descr(animationQuicktype::get_optional<std::string>(j, "descr"));
            x.set_position(animationQuicktype::get_optional<std::vector<double>>(j, "position"));
            x.set_rotation(animationQuicktype::get_optional<std::vector<double>>(j, "rotation"));
            x.set_placement_rel_to(animationQuicktype::get_optional<std::string>(j, "placementRelTo"));
            x.set_data(animationQuicktype::get_optional<std::string>(j, "data"));
            x.set_value(animationQuicktype::get_optional<bool>(j, "value"));
            x.set_url(animationQuicktype::get_optional<std::string>(j, "URL"));
        }

        inline void to_json(json& j, const animationQuicktype::Event& x) {
            j = json::object();
            j["type"] = x.get_type();
            j["descr"] = x.get_descr();
            j["position"] = x.get_position();
            j["rotation"] = x.get_rotation();
            j["placementRelTo"] = x.get_placement_rel_to();
            j["data"] = x.get_data();
            j["value"] = x.get_value();
            j["URL"] = x.get_url();
        }

        inline void from_json(const json& j, animationQuicktype::Trigger& x) {
            x.set_type(j.at("type").get<animationQuicktype::TriggerType>());
            x.set_data(j.at("data").get<std::string>());
        }

        inline void to_json(json& j, const animationQuicktype::Trigger& x) {
            j = json::object();
            j["type"] = x.get_type();
            j["data"] = x.get_data();
        }

        inline void from_json(const json& j, animationQuicktype::Action& x) {
            x.set_trigger(j.at("trigger").get<animationQuicktype::Trigger>());
            x.set_event(j.at("event").get<animationQuicktype::Event>());
        }

        inline void to_json(json& j, const animationQuicktype::Action& x) {
            j = json::object();
            j["trigger"] = x.get_trigger();
            j["event"] = x.get_event();
        }

        inline void from_json(const json& j, animationQuicktype::Node& x) {
            x.set_name(j.at("name").get<std::string>());
            x.set_actions(j.at("actions").get<std::vector<animationQuicktype::Action>>());
        }

        inline void to_json(json& j, const animationQuicktype::Node& x) {
            j = json::object();
            j["name"] = x.get_name();
            j["actions"] = x.get_actions();
        }

        inline void from_json(const json& j, animationQuicktype::Animations& x) {
            x.set_context(j.at("context").get<animationQuicktype::Context>());
            x.set_nodes(j.at("nodes").get<std::vector<animationQuicktype::Node>>());
            x.set_bookmarks(j.at("bookmarks").get<std::vector<animationQuicktype::Bookmark>>());
        }

        inline void to_json(json& j, const animationQuicktype::Animations& x) {
            j = json::object();
            j["context"] = x.get_context();
            j["nodes"] = x.get_nodes();
            j["bookmarks"] = x.get_bookmarks();
        }

        inline void from_json(const json& j, animationQuicktype::EventType& x) {
            if (j == "animation") x = animationQuicktype::EventType::ANIMATION;
            else if (j == "animationAdditive") x = animationQuicktype::EventType::ANIMATION_ADDITIVE;
            else if (j == "hide") x = animationQuicktype::EventType::HIDE;
            else if (j == "link") x = animationQuicktype::EventType::LINK;
            else if (j == "show") x = animationQuicktype::EventType::SHOW;
            else if (j == "state") x = animationQuicktype::EventType::STATE;
            else if (j == "trail") x = animationQuicktype::EventType::TRAIL;
            else if (j == "attach") x = animationQuicktype::EventType::ATTACH;
            else throw "Input JSON does not conform to schema";
        }

        inline void to_json(json& j, const animationQuicktype::EventType& x) {
            switch (x) {
            case animationQuicktype::EventType::ANIMATION: j = "animation"; break;
            case animationQuicktype::EventType::ANIMATION_ADDITIVE: j = "animationAdditive"; break;
            case animationQuicktype::EventType::HIDE: j = "hide"; break;
            case animationQuicktype::EventType::LINK: j = "link"; break;
            case animationQuicktype::EventType::SHOW: j = "show"; break;
            case animationQuicktype::EventType::STATE: j = "state"; break;
            case animationQuicktype::EventType::TRAIL: j = "trail"; break;
            case animationQuicktype::EventType::ATTACH: j = "attach"; break;
            default: throw "This should not happen";
            }
        }

        inline void from_json(const json& j, animationQuicktype::TriggerType& x) {
            if (j == "timestamp") x = animationQuicktype::TriggerType::TIMESTAMP;
            else throw "Input JSON does not conform to schema";
        }

        inline void to_json(json& j, const animationQuicktype::TriggerType& x) {
            switch (x) {
            case animationQuicktype::TriggerType::TIMESTAMP: j = "timestamp"; break;
            default: throw "This should not happen";
            }
        }
    }
}

