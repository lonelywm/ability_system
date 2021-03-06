#include "../include/ability_system.h"
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iomanip>

struct User {
    std::string id;
    std::string name;
    std::string sex;
    int age;
};

namespace asr {
    struct Title;
    struct Display;
    struct Select;
    struct Data;
    struct Keys;
}

struct ATitle;
struct ADisplay;
struct ASelect;
struct AData;
struct AKeys;

struct ATitle: public Ability<asr::Title> {
    std::vector<std::string> asInvoke() {
        return std::vector<std::string> {"Id", "Name", "Sex", "Age"};
    }
};

struct ASelect: public Ability<asr::Select, asr::Data> {
    std::map<std::string, bool> selected;
    AData* pData;
    void asInit(AData& Data) { pData = &Data; }
    bool asInvoke(std::string id) {
        if (!selected.count(id)) {
            selected[id] = true;
            return true;
        }
        selected.erase(id);
        return false;
    }
};

struct AData: public Ability<asr::Data> {
    std::map<std::string, User> userMap;
    AData() {
        userMap["01"] = User{"01", "Tom", "m", 18};
        userMap["02"] = User{"02", "Lucy", "f", 17};
    }
};

struct ADisplay: public Ability<asr::Display, asr::Data> {
    AData* pData;
    void asInit(AData& Data) { pData = &Data; }
    std::vector<std::string> asInvoke(std::string id) {
        User& user = pData->userMap[id];
        std::vector<std::string> list;
        list.push_back(user.id);
        list.push_back(user.name);
        list.push_back(user.sex);
        list.push_back(std::to_string(user.age));
        return list;
    }
};

struct AKeys: public Ability<asr::Keys, asr::Data> {
    AData* pData;
    void asInit(AData& Data) { pData = &Data; }
    std::vector<std::string> asInvoke() {
        std::vector<std::string> keys;
        for (auto item: pData->userMap) {
            keys.push_back(item.first);
        }
        return keys;
    }
};

class Table: public AbilityContainer<ASelect, ADisplay, AKeys, AData, ATitle> {};

int main() {
    Table table;
    for (auto data: table.invoke<asr::Title>()) {
        std::cout << std::setw(8) << data << " ";
    }
    std::cout << std::endl;

    for (auto key: table.invoke<asr::Keys>()) {
        for (auto data: table.invoke<asr::Display>(key)) {
            std::cout << std::setw(8) << data << " ";
        }
        std::cout << std::endl;
    }
}