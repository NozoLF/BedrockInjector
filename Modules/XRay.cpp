#include "Modules/XRay.h"
#include <array>
namespace modules {
bool isOre(std::string_view id){
 constexpr std::array<std::string_view,17> ores={"minecraft:diamond_ore","minecraft:deepslate_diamond_ore","minecraft:iron_ore","minecraft:deepslate_iron_ore","minecraft:gold_ore","minecraft:deepslate_gold_ore","minecraft:emerald_ore","minecraft:deepslate_emerald_ore","minecraft:redstone_ore","minecraft:deepslate_redstone_ore","minecraft:lapis_ore","minecraft:deepslate_lapis_ore","minecraft:copper_ore","minecraft:deepslate_copper_ore","minecraft:coal_ore","minecraft:deepslate_coal_ore","minecraft:ancient_debris"};
 for(auto ore:ores)if(id==ore)return true;return false;
}
}
