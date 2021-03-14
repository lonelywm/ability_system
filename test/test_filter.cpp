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
    struct Data {
        virtual User& GetUser(std::string) = 0;
        virtual std::vector<std::string> Keys() = 0;
    };
    struct Keys;
}

struct ATitle;
struct ADisplay;
struct ASelect;
struct AData;
struct AKeys;
struct AFilter;

struct ATitle: public Ability<asr::Title> {
    std::vector<std::string> asInvoke() {
        return std::vector<std::string> {"Id", "Name", "Sex", "Age"};
    }
};

struct AData: public Ability<asr::Data>, public asr::Data {
    std::map<std::string, User> userMap;
    AData() {
        userMap["01"] = User{"01", "Tom", "m", 18};
        userMap["02"] = User{"02", "Lucy", "f", 17};
    }
    virtual User& GetUser(std::string id) override {
        return userMap[id];
    }
    virtual std::vector<std::string> Keys() override {
        std::vector<std::string> keys;
        for (auto item: userMap) {
            keys.push_back(item.first);
        }
        return keys;
    }
};

struct ASelect: public Ability<asr::Select, asr::Data> {
    std::map<std::string, bool> selected;
    asr::Data* pData;
    void asInit(asr::Data& Data) { pData = &Data; }
    bool asInvoke(std::string id) {
        if (!selected.count(id)) {
            selected[id] = true;
            return true;
        }
        selected.erase(id);
        return false;
    }
};

struct ADisplay: public Ability<asr::Display, asr::Data> {
    asr::Data* pData;
    void asInit(asr::Data& Data) { pData = &Data; }
    std::vector<std::string> asInvoke(std::string id) {
        User& user = pData->GetUser(id);
        std::vector<std::string> list;
        list.push_back(user.id);
        list.push_back(user.name);
        list.push_back(user.sex);
        list.push_back(std::to_string(user.age));
        return list;
    }
};

struct AKeys: public Ability<asr::Keys, asr::Data> {
    asr::Data* pData;
    void asInit(asr::Data& Data) { pData = &Data; }
    std::vector<std::string> asInvoke() {
        std::vector<std::string> keys = pData->Keys();
        return keys;
    }
};

struct AFilter: public Ability<asr::Data, asr::Data>, public asr::Data {
    std::map<std::string, User> userMapExtra;
    void asInit(AData& pData) {
        userMapExtra = pData.userMap;
        userMapExtra["03"] = User{"03", "Hanmei", "f", 16};
    }
    virtual User& GetUser(std::string id) override {
        return userMapExtra[id];
    }
    virtual std::vector<std::string> Keys() override {
        std::vector<std::string> keys;
        for (auto item: userMapExtra) {
            keys.push_back(item.first);
        }
        return keys;
    }
};

class Table: public AbilityContainer<ASelect, ADisplay, AKeys, AFilter, AData, ATitle> {};

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