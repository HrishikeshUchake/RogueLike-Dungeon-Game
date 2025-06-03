#include "object_template.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>
#include <regex>

ObjectType parse_type(const std::string& type_str) {
    if (type_str == "WEAPON") return ObjectType::WEAPON;
    if (type_str == "OFFHAND") return ObjectType::OFFHAND;
    if (type_str == "RANGED") return ObjectType::RANGED;
    if (type_str == "ARMOR") return ObjectType::ARMOR;
    if (type_str == "HELMET") return ObjectType::HELMET;
    if (type_str == "CLOAK") return ObjectType::CLOAK;
    if (type_str == "GLOVES") return ObjectType::GLOVES;
    if (type_str == "BOOTS") return ObjectType::BOOTS;
    if (type_str == "RING") return ObjectType::RING;
    if (type_str == "AMULET") return ObjectType::AMULET;
    if (type_str == "LIGHT") return ObjectType::LIGHT;
    if (type_str == "SCROLL") return ObjectType::SCROLL;
    if (type_str == "BOOK") return ObjectType::BOOK;
    if (type_str == "FLASK") return ObjectType::FLASK;
    if (type_str == "GOLD") return ObjectType::GOLD;
    if (type_str == "AMMUNITION") return ObjectType::AMMUNITION;
    if (type_str == "FOOD") return ObjectType::FOOD;
    if (type_str == "WAND") return ObjectType::WAND;
    if (type_str == "CONTAINER") return ObjectType::CONTAINER;
    if (type_str == "STACK") return ObjectType::STACK;
    return ObjectType::INVALID;
}

ObjectInstance ObjectTemplate::generate_instance() const {
    ObjectInstance obj;
    obj.name = name;
    obj.symbol = symbol;
    obj.color = colors;
    obj.hit = hit.roll();
    obj.dodge = dodge.roll();
    obj.defense = defense.roll();
    obj.weight = weight.roll();
    obj.speed = speed.roll();
    obj.attribute = attribute.roll();
    obj.value = value.roll();
    obj.damage = damage;
    obj.is_artifact = artifact;
    obj.description = description;
    obj.x = obj.y = -1; // Unplaced initially
    return obj;
}

std::vector<ObjectTemplate> parse_objects(const std::string& filepath) {
    std::ifstream in(filepath);
    std::vector<ObjectTemplate> objects;
    std::string line;

    if (!in || !std::getline(in, line) || line != "RLG327 OBJECT DESCRIPTION 1") {
        std::cerr << "Invalid object file header.\n";
        return objects;
    }

    while (std::getline(in, line)) {
        if (line != "BEGIN OBJECT") continue;

        ObjectTemplate obj;
        std::unordered_set<std::string> fields;
        bool error = false;

        while (std::getline(in, line) && line != "END") {
            std::istringstream iss(line);
            std::string keyword;
            iss >> keyword;

            if (keyword == "NAME") {
                if (!std::getline(iss >> std::ws, obj.name) || fields.count("NAME")) { error = true; break; }
                fields.insert("NAME");
            } else if (keyword == "DESC") {
                std::string desc, tmp;
                while (std::getline(in, tmp) && tmp != ".") desc += tmp + "\n";
                if (fields.count("DESC")) { error = true; break; }
                obj.description = desc;
                fields.insert("DESC");
            } else if (keyword == "TYPE") {
                std::string type_str;
                iss >> type_str;
                obj.type = parse_type(type_str);
                fields.insert("TYPE");
            } else if (keyword == "SYMB") {
                char c;
                if (!(iss >> c)) { error = true; break; }
                obj.symbol = c;
                fields.insert("SYMB");
            } else if (keyword == "COLOR") {
                std::string c;
                while (iss >> c) obj.colors.push_back(c);
                fields.insert("COLOR");
            } else if (keyword == "HIT") {
                std::string s; iss >> s; obj.hit = Dice::parse(s); fields.insert("HIT");
            } else if (keyword == "DODGE") {
                std::string s; iss >> s; obj.dodge = Dice::parse(s); fields.insert("DODGE");
            } else if (keyword == "DEF") {
                std::string s; iss >> s; obj.defense = Dice::parse(s); fields.insert("DEF");
            } else if (keyword == "WEIGHT") {
                std::string s; iss >> s; obj.weight = Dice::parse(s); fields.insert("WEIGHT");
            } else if (keyword == "SPEED") {
                std::string s; iss >> s; obj.speed = Dice::parse(s); fields.insert("SPEED");
            } else if (keyword == "ATTR") {
                std::string s; iss >> s; obj.attribute = Dice::parse(s); fields.insert("ATTR");
            } else if (keyword == "VAL") {
                std::string s; iss >> s; obj.value = Dice::parse(s); fields.insert("VAL");
            } else if (keyword == "DAM") {
                std::string s; iss >> s; obj.damage = Dice::parse(s); fields.insert("DAM");
            } else if (keyword == "ART") {
                std::string a; iss >> a;
                obj.artifact = (a == "TRUE");
                fields.insert("ART");
            } else if (keyword == "RRTY") {
                int r; iss >> r; obj.rarity = r; fields.insert("RRTY");
            } else {
                error = true;
                break;
            }
        }

        if (!error && fields.size() >= 13) objects.push_back(obj);
    }

    return objects;
}

void print_objects(const std::vector<ObjectTemplate>& objs) {
    for (const auto& o : objs) {
        std::cout << o.name << "\n" << o.description;
        std::cout << o.symbol << "\n";
        for (const auto& c : o.colors) std::cout << c << " ";
        std::cout << "\n";
        std::cout << o.hit.to_string() << " " << o.dodge.to_string() << " "
                  << o.defense.to_string() << " " << o.weight.to_string() << " "
                  << o.speed.to_string() << " " << o.attribute.to_string() << " "
                  << o.value.to_string() << " " << o.damage.to_string() << "\n";
        std::cout << (o.artifact ? "TRUE" : "FALSE") << "\n";
        std::cout << o.rarity << "\n\n";
    }
}
