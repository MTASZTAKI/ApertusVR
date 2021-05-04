//  To parse this JSON data, first install
//
//      Boost     http://www.boost.org
//      json.hpp  https://github.com/nlohmann/json
//
//  Then include this file, and then do
//
//     Welcome data = nlohmann::json::parse(jsonString);

#pragma once

#include "json.hpp"

#include <optional>
#include <stdexcept>
#include <regex>

namespace CoreConfig {
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

    class Lobby {
        public:
        Lobby() = default;
        virtual ~Lobby() = default;

        private:
        std::string ip;
        std::string port;
        std::string room;

        public:
        const std::string & get_ip() const { return ip; }
        std::string & get_mutable_ip() { return ip; }
        void set_ip(const std::string & value) { this->ip = value; }

        const std::string & get_port() const { return port; }
        std::string & get_mutable_port() { return port; }
        void set_port(const std::string & value) { this->port = value; }

        const std::string & get_room() const { return room; }
        std::string & get_mutable_room() { return room; }
        void set_room(const std::string & value) { this->room = value; }
    };

    class Lan {
        public:
        Lan() = default;
        virtual ~Lan() = default;

        private:
        std::string ip;
        std::string port;

        public:
        const std::string & get_ip() const { return ip; }
        std::string & get_mutable_ip() { return ip; }
        void set_ip(const std::string & value) { this->ip = value; }

        const std::string & get_port() const { return port; }
        std::string & get_mutable_port() { return port; }
        void set_port(const std::string & value) { this->port = value; }
    };

    class Internet {
        public:
        Internet() = default;
        virtual ~Internet() = default;

        private:
        Lan nat_punch_through;
        Lobby lobby;

        public:
        const Lan & get_nat_punch_through() const { return nat_punch_through; }
        Lan & get_mutable_nat_punch_through() { return nat_punch_through; }
        void set_nat_punch_through(const Lan & value) { this->nat_punch_through = value; }

        const Lobby & get_lobby() const { return lobby; }
        Lobby & get_mutable_lobby() { return lobby; }
        void set_lobby(const Lobby & value) { this->lobby = value; }
    };

    class Network {
        public:
        Network() = default;
        virtual ~Network() = default;

        private:
        std::string user;
        std::string participant;
        Internet internet;
        Lan lan;
        std::string selected;
        std::string resource_zip_url;
        std::string resource_md5_url;
        std::string resource_download_location;
        std::vector<std::string> resource_locations;

        public:
        const std::string & get_user() const { return user; }
        std::string & get_mutable_user() { return user; }
        void set_user(const std::string & value) { this->user = value; }

        const std::string & get_participant() const { return participant; }
        std::string & get_mutable_participant() { return participant; }
        void set_participant(const std::string & value) { this->participant = value; }

        const Internet & get_internet() const { return internet; }
        Internet & get_mutable_internet() { return internet; }
        void set_internet(const Internet & value) { this->internet = value; }

        const Lan & get_lan() const { return lan; }
        Lan & get_mutable_lan() { return lan; }
        void set_lan(const Lan & value) { this->lan = value; }

        const std::string & get_selected() const { return selected; }
        std::string & get_mutable_selected() { return selected; }
        void set_selected(const std::string & value) { this->selected = value; }

        const std::string & get_resource_zip_url() const { return resource_zip_url; }
        std::string & get_mutable_resource_zip_url() { return resource_zip_url; }
        void set_resource_zip_url(const std::string & value) { this->resource_zip_url = value; }

        const std::string & get_resource_md5_url() const { return resource_md5_url; }
        std::string & get_mutable_resource_md5_url() { return resource_md5_url; }
        void set_resource_md5_url(const std::string & value) { this->resource_md5_url = value; }

        const std::string & get_resource_download_location() const { return resource_download_location; }
        std::string & get_mutable_resource_download_location() { return resource_download_location; }
        void set_resource_download_location(const std::string & value) { this->resource_download_location = value; }

        const std::vector<std::string> & get_resource_locations() const { return resource_locations; }
        std::vector<std::string> & get_mutable_resource_locations() { return resource_locations; }
        void set_resource_locations(const std::vector<std::string> & value) { this->resource_locations = value; }
    };

    class Welcome {
        public:
        Welcome() = default;
        virtual ~Welcome() = default;

        private:
        std::vector<std::string> plugins;
        Network network;

        public:
        const std::vector<std::string> & get_plugins() const { return plugins; }
        std::vector<std::string> & get_mutable_plugins() { return plugins; }
        void set_plugins(const std::vector<std::string> & value) { this->plugins = value; }

        const Network & get_network() const { return network; }
        Network & get_mutable_network() { return network; }
        void set_network(const Network & value) { this->network = value; }
    };
}

namespace nlohmann {
    void from_json(const json & j, CoreConfig::Lobby & x);
    void to_json(json & j, const CoreConfig::Lobby & x);

    void from_json(const json & j, CoreConfig::Lan & x);
    void to_json(json & j, const CoreConfig::Lan & x);

    void from_json(const json & j, CoreConfig::Internet & x);
    void to_json(json & j, const CoreConfig::Internet & x);

    void from_json(const json & j, CoreConfig::Network & x);
    void to_json(json & j, const CoreConfig::Network & x);

    void from_json(const json & j, CoreConfig::Welcome & x);
    void to_json(json & j, const CoreConfig::Welcome & x);

    inline void from_json(const json & j, CoreConfig::Lobby& x) {
        x.set_ip(j.at("ip").get<std::string>());
        x.set_port(j.at("port").get<std::string>());
        x.set_room(j.at("room").get<std::string>());
    }

    inline void to_json(json & j, const CoreConfig::Lobby & x) {
        j = json::object();
        j["ip"] = x.get_ip();
        j["port"] = x.get_port();
        j["room"] = x.get_room();
    }

    inline void from_json(const json & j, CoreConfig::Lan& x) {
        x.set_ip(j.at("ip").get<std::string>());
        x.set_port(j.at("port").get<std::string>());
    }

    inline void to_json(json & j, const CoreConfig::Lan & x) {
        j = json::object();
        j["ip"] = x.get_ip();
        j["port"] = x.get_port();
    }

    inline void from_json(const json & j, CoreConfig::Internet& x) {
        x.set_nat_punch_through(j.at("natPunchThrough").get<CoreConfig::Lan>());
        x.set_lobby(j.at("lobby").get<CoreConfig::Lobby>());
    }

    inline void to_json(json & j, const CoreConfig::Internet & x) {
        j = json::object();
        j["natPunchThrough"] = x.get_nat_punch_through();
        j["lobby"] = x.get_lobby();
    }

    inline void from_json(const json & j, CoreConfig::Network& x) {
        x.set_user(j.at("user").get<std::string>());
        x.set_participant(j.at("participant").get<std::string>());
        x.set_internet(j.at("internet").get<CoreConfig::Internet>());
        x.set_lan(j.at("lan").get<CoreConfig::Lan>());
        x.set_selected(j.at("selected").get<std::string>());
        x.set_resource_zip_url(j.at("resourceZipUrl").get<std::string>());
        x.set_resource_md5_url(j.at("resourceMd5Url").get<std::string>());
        x.set_resource_download_location(j.at("resourceDownloadLocation").get<std::string>());
        x.set_resource_locations(j.at("resourceLocations").get<std::vector<std::string>>());
    }

    inline void to_json(json & j, const CoreConfig::Network & x) {
        j = json::object();
        j["user"] = x.get_user();
        j["participant"] = x.get_participant();
        j["internet"] = x.get_internet();
        j["lan"] = x.get_lan();
        j["selected"] = x.get_selected();
        j["resourceZipUrl"] = x.get_resource_zip_url();
        j["resourceMd5Url"] = x.get_resource_md5_url();
        j["resourceDownloadLocation"] = x.get_resource_download_location();
        j["resourceLocations"] = x.get_resource_locations();
    }

    inline void from_json(const json & j, CoreConfig::Welcome& x) {
        x.set_plugins(j.at("plugins").get<std::vector<std::string>>());
        x.set_network(j.at("network").get<CoreConfig::Network>());
    }

    inline void to_json(json & j, const CoreConfig::Welcome & x) {
        j = json::object();
        j["plugins"] = x.get_plugins();
        j["network"] = x.get_network();
    }
}
