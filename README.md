[![tag](https://img.shields.io/github/v/tag/lonelywm/ability_system)](https://github.com/lonelywm/ability_system/tags) [![license](https://img.shields.io/github/license/lonelywm/ability_system)](LICENSE) [![compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue)](https://godbolt.org/z/437YEz) 

## Ability System
面向对象编程虽然强大但也有一些缺点，本质上而言，面向对象编程将数据和方法强关联在了一起，程序员不得不面对一个非常臃肿的Class，Class中某些数据和方法是关联的，但并非所有的数据与所有的方法都一一关联，而且在逻辑上，一个类往往拥有很多种能力，在一个Class中加入太多功能会导致可读性和复用性下降。
本框架设计的初衷就是为了将臃肿的大类拆分成若干个小类（结构体），用若干小类组成一个复杂的类，就像搭积木一样。这样可以增加代码的可读性，降低维护成本，拆分出的小类都是单一职责的，很容易理解。
我们采用组件设计模式，一个类的能力取决于它有什么样的组件，这样做的好处是比继承更加灵活且高效（继承的虚标会增加开销），而且借助于C++ Template，我们的组件是静态的，即在编译期实现的，不会产生多余的开销，

## Feature
1. Header-only, Powerful, Tiny
2. Ioc: Inversion of Control
3. Static Component Pattern using C++ template
4. Single Responsibility Principle
5. Self-explanatory and Easy to read

## Basic
本系统主要包含两个模板类，一个是`Ability`，一个是`AbilityContainer`。
```
namespace asr {
    struct A;
    struct B;
    struct C;
}
class A: public Ability<asr::A> {};
class B: public Ability<asr::B, asr::A, asr::C> {};
class C: public Ability<asr::C, asr::A> {};
class ABC: public AbilityContainer<A, B, C> {};
```
- `AbilityContainer`类是`变长模板`的`大类`，由若干`小类`组成，可以接任意个数的小类。
- `Ability`类是`变长模板`的`小类`，各个`小类`之间有依赖关系，依赖关系由其后面的模板参数决定，第一个表示自己的角色，剩余的表示其依赖的角色，所有至少要一个模板（必须标定自己的角色，可以没有依赖）。
- `struct B: public Ability<asr::C, asr::A, asr::C>`的含义是：对于B类，它的角色是`B`，它的依赖是`A`和`C`


## Quick Start
> run it online : [**compiler explorer**](https://godbolt.org/z/oWM8bf) 

假设我们现在要设计一个数据库中的员工信息表
我们需要完成三个功能：
- 描述表头
- 描述表格内容
- 记录表格的选中状态

```
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>

struct User {
    std::string id;
    std::string name;
    std::string sex;
    int age;
};

class Table {
public:
    std::vector<std::string> titles()                { ... }
    std::vector<std::string> keys()                  { ... }
    std::vector<std::string> display(std::string id) { ... }
    bool                     select(std::string id)  { ... }
private:
    ...
};
```
正常情况下，我们可以设计出类似上述代码的接口。


```
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
    std::vector<std::string> asrnvoke() {
        return std::vector<std::string> {"Id", "Name", "Sex", "Age"};
    }
};

struct ASelect: public Ability<asr::Select, asr::Data> {
    std::map<std::string, bool> selected;
    AData* pData;
    void asrnit(AData& Data) { pData = &Data; }
    bool asrnvoke(std::string id) {
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
    void asrnit(AData& Data) { pData = &Data; }
    std::vector<std::string> asrnvoke(std::string id) {
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
    void asrnit(AData& Data) { pData = &Data; }
    std::vector<std::string> asrnvoke() {
        std::vector<std::string> keys;
        for (auto item: pData->userMap) {
            keys.push_back(item.first);
        }
        return keys;
    }
};

class Table: public AbilityContainer<ASelect, ADisplay, AKeys, AData, ATitle> {
public:
    std::vector<std::string> titles()                { return invoke<asr::Title>(); }
    std::vector<std::string> keys()                  { return invoke<asr::Keys>(); }
    std::vector<std::string> display(std::string id) { return invoke<asr::Display>(id); }
    bool                     select(std::string id)  { return invoke<asr::Select>(id); }
};

int main() {
    Table table;
    for (auto data: table.titles()) {
        std::cout << std::setw(8) << data << " ";
    }
    std::cout << std::endl;

    for (auto key: table.keys()) {
        for (auto data: table.display(key)) {
            std::cout << std::setw(8) << data << " ";
        }
        std::cout << std::endl;
    }
}

```

我们的代码还可以进一步简化
```
class Table: public AbilityContainer<ASelect, ADisplay, AKeys, AData, ATitle> {};

int main() {
    Table table;
    for (auto data: table.invoke<asr::Title>()) {
        std::cout << std::setw(8) << data << " ";
    }
    std::cout << std::endl;

    for (auto key: table.keys()) {
        for (auto data: table.invoke<asr::Display>(key)) {
            std::cout << std::setw(8) << data << " ";
        }
        std::cout << std::endl;
    }
}
```

## 查找顺序和层
为什么要引入一个中间变量如`asr::Display`，asr命名空间下面的类都只有声明，仅起到标识的作用。但是引入它们是非常有必要的，主要有两个原因，一是为了避免相互引用，A->B && B->A，相互引用相互依赖是可能发生的；第二是为了给系统加入层的概念，举例说明：
```
struct AFilter: public Ability<asr::Data, asr::Data> {
    std::map<std::string, User> userMap;
    asInit(AData* pData) {
        userMap = pData->userMap；
        userMap["03"] = User{"03", "Hanmei", "f", 16};
    }
};

class Table: public AbilityContainer<ASelect, ADisplay, AKeys, AFilter, AData, ATitle> {};
```
对于每个`Ability`的子类，它们的Ioc顺序是和它们在AbilityContainer中的位置有关的，它会从自己所在位置的后一个开始搜索，一直往后循环一圈（即顺序搜索到最后一个，再跳转到第一个顺序搜索到自己的前一个）。
对于一个`AbilityContainer`出现相同的角色，比如`AFilter`和`AData`它们的角色都是`asr::Data`，那么那一个生效取决于它们的位置，按照搜索顺序，对于`ASelect` `ADisplay` `AKeys` `ATitle`所有查找到的`asr::Data`，实际上都是`AFilter`，如果更换顺序为 `class Table: public AbilityContainer<ASelect, ADisplay, AFilter, AKeys, AData, ATitle> {};` 那么对于`AKeys`她找到的就是`AData`。

## Compiler compatibility
Clang >= 5.0.0
GCC >= 7.1
MSVC >= 19.20