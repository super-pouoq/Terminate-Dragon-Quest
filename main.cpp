#include "game.h"
#include "Hero_and_Monster.h"
#include "items.h"
#include "status.h"
#include "equip.h"
#include "battle.h"
#include "backpack.h"
#include "maze.h"
extern Equipment poisoned_blade;
int main() {
    system("chcp 65001 > nul");
    Game game;
    init_items_effects();
    init_equipment_effects();
    game.init(&game);
    return 0;
}