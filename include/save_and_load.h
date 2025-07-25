#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Hero_and_Monster.h"
#include "count.h"
extern Equipment null_weapon, null_armor, null_accessory;

void xorFileBytes(const std::string& filename, unsigned char key = 0xA6) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) {
        std::cerr << "无法打开文件进行读取\n";
        return;
    }
    std::vector<char> buffer((std::istreambuf_iterator<char>(ifs)),
                              std::istreambuf_iterator<char>());
    ifs.close();

    for (auto& byte : buffer) {
        byte ^= key;
    }

    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "无法打开文件进行写入\n";
        return;
    }
    ofs.write(buffer.data(), buffer.size());
    ofs.close();
}

void saveGame(Hero* hero, Shop* shop, const std::string& filename) {
    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "无法打开文件进行保存: " << filename << std::endl;
        return;
    }

    int wp_index = hero->get_weapon()->get_index();
    int ar_index = hero->get_armor()->get_index();
    int ac_index = hero->get_accessory()->get_index();
    
    // 暂时卸下装备保存原始属性
    hero->equip(&null_weapon);
    hero->equip(&null_armor);
    hero->equip(&null_accessory);

    // 保存英雄状态
    outFile << "Name:" << hero->get_name() << "\n"
            << "HP:" << hero->get_HP() << "\n"
            << "Attack:" << hero->get_Attack() << "\n"
            << "Defense:" << hero->get_Defense() << "\n"
            << "Speed:" << hero->get_Speed() << "\n"
            << "Money:" << hero->get_Money() << "\n"
            << "Luck:" << hero->get_Luck() << "\n"
            << "now_weapon_index:" << wp_index << "\n"
            << "now_armor_index:" << ar_index << "\n"
            << "now_accessory_index:" << ac_index << "\n"
            << "count_easy:" << count_easy << "\n"
            << "count_medium:" << count_medium << "\n"
            << "count_hard:" << count_hard << "\n"
            << "count_challenge:" << count_challenge << "\n";

    // 背包中装备
    outFile << "Equipment_backpack:\n";
    for (const auto& eq : hero->get_equipment_backpack()->get_equipment_items()) {
        outFile << type_to_string(eq.get_type()) << " " 
                << eq.get_name() << " "
                << eq.get_num() << " "
                << eq.get_price() << " "
                << eq.get_index() << " "
                << eq.get_is_equipped() << "\n";
    }

    // 普通背包
    outFile << "Backpack:\n";
    for (const auto& item : hero->get_backpack()->get_items()) {
        outFile << item.name << " "
                << item.num << " "
                << item.price << " "
                << item.index << "\n";
    }

    // 商店状态
    outFile << "Shop:\n";
    for (const auto& item : shop->get_items()) {
        outFile << item.name << " "
                << item.num << " "
                << item.price << " "
                << item.index << "\n";
    }

    if (!outFile) {
        std::cerr << "写入文件时发生错误。" << std::endl;
        return;
    }
    Equipment wp = index_to_equipment(wp_index);
    Equipment ar = index_to_equipment(ar_index);
    Equipment ac = index_to_equipment(ac_index);
    hero->get_equipment_backpack()->set_is_equipped_by_index(wp_index);
    hero->get_equipment_backpack()->set_is_equipped_by_index(ar_index);
    hero->get_equipment_backpack()->set_is_equipped_by_index(ac_index);
    if (wp_index) hero->equip(wp.clone());
    if (ar_index) hero->equip(ar.clone());
    if (ac_index) hero->equip(ac.clone());
    outFile.close();
    xorFileBytes(filename); // 加密保存的文件
    std::cout << "游戏已成功保存到文件: " << filename << std::endl;
    std::cout << "感谢您的游戏，按任意键继续！\n";
    getchar();
}


std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }

    return s.substr(start, end - start);
}


void loadGame(Hero* hero, Shop* shop, const std::string& filename) {
    xorFileBytes(filename); // 解密保存的文件
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "无法打开保存文件: " << filename << std::endl;
        return;
    }

    std::string line;
    std::string name;
    int hp, attack, defense, speed, money, luck;
    int wp_index = 0, ar_index = 0, ac_index = 0;

    // 基本属性
    while (std::getline(inFile, line)) {
        size_t pos = line.find(':');
        if (pos != std::string::npos && pos + 1 < line.size()) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            value = trim(value); // 你需要自己实现 trim 函数（去除前后空格）

            if (key == "Name") name = value;
            else if (key == "HP") hp = std::stoi(value);
            else if (key == "Attack") attack = std::stoi(value);
            else if (key == "Defense") defense = std::stoi(value);
            else if (key == "Speed") speed = std::stoi(value);
            else if (key == "Money") money = std::stoi(value);
            else if (key == "Luck") luck = std::stoi(value);
            else if (key == "now_weapon_index") wp_index = std::stoi(value);
            else if (key == "now_armor_index") ar_index = std::stoi(value);
            else if (key == "now_accessory_index") ac_index = std::stoi(value);
            else if (key == "count_easy") count_easy = std::stoi(value);
            else if (key == "count_medium") count_medium = std::stoi(value);
            else if (key == "count_hard") count_hard = std::stoi(value);
            else if (key == "count_challenge") count_challenge = std::stoi(value);
        }
        else if (line == "Equipment_backpack:") {
            break;
        }
    }

    // 设置属性
    hero->set_name(name);
    hero->set_HP(hp);
    hero->set_Attack(attack);
    hero->set_Defense(defense);
    hero->set_Speed(speed);
    hero->set_Money(money);
    hero->set_Luck(luck);

    // 清空当前状态
    hero->get_backpack()->clear();
    hero->get_equipment_backpack()->clear();
    shop->clear(); // 清空商店状态

    // 读取装备背包
    while (std::getline(inFile, line)) {
        if (line == "Backpack:") break;
        std::istringstream iss(line);
        std::string type_str, name;
        unsigned int num, price, index, is_equipped;
        iss >> type_str >> name >> num >> price >> index >> is_equipped;

        EquipmentType type;
        if (type_str == "Weapon") type = EquipmentType::Weapon;
        else if (type_str == "Armor") type = EquipmentType::Armor;
        else if (type_str == "Accessory") type = EquipmentType::Accessory;

        hero->get_equipment_backpack()->add_equipment(name, num, price, type, index);
    }

    // 普通背包
    while (std::getline(inFile, line)) {
        if (line == "Shop:") break; // 开始读取商店
        std::istringstream iss(line);
        std::string name;
        unsigned int num, price, index;
        iss >> name >> num >> price >> index;
        hero->get_backpack()->add_or_stack(name, num, price, index);
    }

    // 商店
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string name;
        unsigned int num, price, index;
        if (!(iss >> name >> num >> price >> index)) break;
        shop->add_or_stack(name, num, price, index);
    }

    // 装备穿上
    Equipment wp = index_to_equipment(wp_index);
    Equipment ar = index_to_equipment(ar_index);
    Equipment ac = index_to_equipment(ac_index);
    hero->get_equipment_backpack()->set_is_equipped_by_index(wp_index);
    hero->get_equipment_backpack()->set_is_equipped_by_index(ar_index);
    hero->get_equipment_backpack()->set_is_equipped_by_index(ac_index);
    if (wp_index) hero->equip(wp.clone());
    if (ar_index) hero->equip(ar.clone());
    if (ac_index) hero->equip(ac.clone());

    inFile.close();
    std::cout << "游戏成功加载自文件: " << filename << std::endl;
    std::cout << "欢迎回来，按任意键进入游戏 " << hero->get_name() << "！\n";
    xorFileBytes(filename);
    getchar();
}


