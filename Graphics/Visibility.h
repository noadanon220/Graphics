#pragma once
#include "Definitions.h"

// Builds a visibility map for one unit, storing values in outV (0..1).
// maxRange limits the LOS radius. Vision stops at stones and trees.
void BuildVisibilityForUnit(int map[MSZ][MSZ], int r0, int c0, double outV[MSZ][MSZ], int maxRange = 20);
