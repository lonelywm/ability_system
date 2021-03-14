#include <iostream>
#include "../include/ability_system.h"


// ability system interface
namespace asi {
    struct Selection;
    struct Select;
    struct Data;
    struct Show;
}

struct AData;
struct AShow;
struct AFilter;
struct ACopy;
struct ASelect;
struct ASelection;


struct ASelection: public Ability<asi::Selection> {
    std::string name = "Selection";
    void asInit() {
        std::cout << "init Selection" << std::endl;
    }
    std::string asInvoke () {
        return name;
    }
};

struct ASelect: public Ability<asi::Select, asi::Selection> {
    std::string name = "Select";
    ASelection* Sel;
    void asInit(ASelection& Selection) {
        Sel = &Selection;
        std::cout << "init Select" << std::endl;
    }
    std::string asInvoke () {
        return name + " " + Sel->name;
    }
};

struct AData : public Ability<asi::Data> {
    std::string name = "Data";
    void asInit() {
        std::cout << "init Data" << std::endl;
    }
    std::string asInvoke (...) {
        return name;
    }
};

struct AShow : public Ability<asi::Show, asi::Data> {
    std::string name = "Show";
    AData* pData;
    void asInit(AData& Data) {
        pData = &Data;
        std::cout << "init Show" << std::endl;
    }
    std::string asInvoke () {
        return name + " " + pData->name;
    }
};

struct AFilter : public Ability<asi::Data, asi::Data> {
    std::string name = "Filter";
    AData* pData;
    void asInit(AData& Data) {
        pData = &Data;
        std::cout << "init filter" << std::endl;
    }
    std::string asInvoke() {
        return name + " " + pData->name;
    }
};

struct ACopy : public Ability<asi::Data, asi::Data> {
    std::string name = "Copy";
    AFilter* pFilter;
    void asInit(AFilter& Filter) {
        pFilter = &Filter;
        std::cout << "init copy" << std::endl;
    }
    std::string asInvoke () {
        return name + " " + pFilter->name;
    }
};

int main() {
    AbilityContainer<ACopy, ASelection, AFilter, AShow, AData, ASelect> ab;
    // 
    std::cout << "...invoke begin..." << std::endl;
    std::cout << ab.invoke<asi::Select>() << std::endl;
    std::cout << ab.invoke<asi::Data>() << std::endl;
    std::cout << ab.invoke<asi::Show>() << std::endl;
    std::cout << ab.invoke<asi::Selection>() << std::endl;
};