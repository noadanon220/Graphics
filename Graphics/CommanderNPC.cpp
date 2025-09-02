// CommanderNPC.cpp - Empty implementation file for CommanderNPC class
#include "CommanderNPC.h"
#include <iostream>

CommanderNPC::CommanderNPC(Team t) : NPC(t, CMD) {
    std::cout << "C: CommanderNPC constructor called for Team " << (t == TEAM_A ? "A" : "B") << std::endl;
}
