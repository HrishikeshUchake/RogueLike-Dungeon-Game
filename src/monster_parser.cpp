#include "monster_template.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <regex>
#include <unordered_set>

Dice Dice::parse(const std::string& s) {
    std::regex dice_regex(R"((-?\d+)\+(\d+)d(\d+))");
    std::smatch match;
    if (std::regex_match(s, match, dice_regex)) {
        return Dice{ std::stoi(match[1]), std::stoi(match[2]), std::stoi(match[3]) };
    } else {
        throw std::runtime_error("Invalid dice format: " + s);
    }
}

int Dice::roll() const {
    static std::random_device rd;
    static std::mt19937 rng(rd());
    int total = base;
    for (int i = 0; i < dice; ++i) {
        std::uniform_int_distribution<int> dist(1, sides);
        total += dist(rng);
    }
    return total;
}

std::string Dice::to_string() const {
    return std::to_string(base) + "+" + std::to_string(dice) + "d" + std::to_string(sides);
}


std::vector<MonsterTemplate> parse_monsters(const std::string& filepath) {
    std::ifstream in(filepath);
    std::vector<MonsterTemplate> monsters;
    std::string line;

    if (!in || !std::getline(in, line) || line != "RLG327 MONSTER DESCRIPTION 1") {
        std::cerr << "Invalid monster file header.\n";
        return monsters;
    }

    while (std::getline(in, line)) {
        if (line != "BEGIN MONSTER") continue;

        MonsterTemplate m;
        std::unordered_set<std::string> fields;
        bool error = false;

        while (std::getline(in, line) && line != "END") {
            std::istringstream iss(line);
            std::string keyword;
            iss >> keyword;

            if (keyword == "NAME") {
                if (!std::getline(iss >> std::ws, m.name) || fields.count("NAME")) { error = true; break; }
                fields.insert("NAME");
            } else if (keyword == "DESC") {
                std::string desc, tmp;
                while (std::getline(in, tmp) && tmp != ".") {
                    desc += tmp + "\n";
                }
                if (fields.count("DESC")) { error = true; break; }
                m.description = desc;
                fields.insert("DESC");
            } else if (keyword == "SYMB") {
                char c;
                if (!(iss >> c) || fields.count("SYMB")) { error = true; break; }
                m.symbol = c;
                fields.insert("SYMB");
            } else if (keyword == "COLOR") {
                std::string color;
                while (iss >> color)
                    m.colors.push_back(color);
                if (fields.count("COLOR") || m.colors.empty()) { error = true; break; }
                fields.insert("COLOR");
            } else if (keyword == "SPEED") {
                std::string dice_str;
                iss >> dice_str;
                try { m.speed = Dice::parse(dice_str); }
                catch (...) { error = true; break; }
                fields.insert("SPEED");
            } else if (keyword == "ABIL") {
                std::string abil;
                while (iss >> abil)
                    m.abilities.push_back(abil);
                if (fields.count("ABIL") || m.abilities.empty()) { error = true; break; }
                fields.insert("ABIL");
            } else if (keyword == "HP") {
                std::string dice_str;
                iss >> dice_str;
                try { m.hp = Dice::parse(dice_str); }
                catch (...) { error = true; break; }
                fields.insert("HP");
            } else if (keyword == "DAM") {
                std::string dice_str;
                iss >> dice_str;
                try { m.damage = Dice::parse(dice_str); }
                catch (...) { error = true; break; }
                fields.insert("DAM");
            } else if (keyword == "RRTY") {
                int val;
                if (!(iss >> val) || fields.count("RRTY")) { error = true; break; }
                m.rarity = val;
                fields.insert("RRTY");
            } else {
                error = true;
                break;
            }
        }

        if (!error && fields.size() == 9)
            monsters.push_back(m);
    }

    return monsters;
}

void print_monsters(const std::vector<MonsterTemplate>& monsters) {
    for (const auto& m : monsters) {
        std::cout << m.name << "\n"
                  << m.description
                  << m.symbol << "\n";
        for (auto& c : m.colors) std::cout << c << " ";
        std::cout << "\n";
        std::cout << m.speed.to_string() << "\n";
        for (auto& ab : m.abilities) std::cout << ab << " ";
        std::cout << "\n";
        std::cout << m.hp.to_string() << "\n"
                  << m.damage.to_string() << "\n"
                  << m.rarity << "\n"
                  << "\n";
    }
}
