#pragma once
#include "NPC.h"

// Commander does not fight; he plans and issues orders.
class CommanderNPC : public NPC {
public:
    explicit CommanderNPC(Team t);
};
