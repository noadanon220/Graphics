// Team.cpp
#include "Team.h"
#include "NPC.h"
#include "MedicNPC.h"
#include "SupplierNPC.h"
#include "CommanderNPC.h"
#include "WarriorNPC.h"
#include "WarriorStates.h"
#include "Visibility.h"
#include "AStar.h"

#include <algorithm>
#include <queue>
#include <unordered_set>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>

static const char* TeamName(Team t) {
    // עובד גם כשהגדרת Team היא enum class וגם כ-enum רגיל,
    // בהנחה ש-TEAM_A=0, TEAM_B=1 (כפי שמקובל בפרויקט).
    return (static_cast<int>(t) == 0 ? "A" : "B");
}

TeamSquad::TeamSquad(Team id) : teamId(id), frameCounter(0) {
    std::cout << "TEAM " << TeamName(teamId) << ": ctor" << std::endl;
}

Team TeamSquad::GetId() const {
    return teamId;
}

void TeamSquad::CreateStandardSquad() {
    members.clear();
    members.emplace_back(std::make_unique<CommanderNPC>(teamId));
    members.emplace_back(std::make_unique<WarriorNPC>(teamId));
    members.emplace_back(std::make_unique<WarriorNPC>(teamId));
    members.emplace_back(std::make_unique<MedicNPC>(teamId));
    members.emplace_back(std::make_unique<SupplierNPC>(teamId));
}

bool TeamSquad::IsFreeForNPC(int map[MSZ][MSZ], int r, int c) {
    if (r < 0 || r >= MSZ || c < 0 || c >= MSZ) return false;
    return map[r][c] == SPACE;
}

bool TeamSquad::RandomFreeCellInRegion(int map[MSZ][MSZ],
    int r0, int r1, int c0, int c1,
    int& outR, int& outC, int maxTries)
{
    r0 = std::max(0, r0); c0 = std::max(0, c0);
    r1 = std::min(MSZ - 1, r1); c1 = std::min(MSZ - 1, c1);

    for (int t = 0; t < maxTries; ++t) {
        int rr = r0 + rand() % (r1 - r0 + 1);
        int cc = c0 + rand() % (c1 - c0 + 1);
        if (IsFreeForNPC(map, rr, cc)) { outR = rr; outC = cc; return true; }
    }
    for (int rr = r0; rr <= r1; ++rr)
        for (int cc = c0; cc <= c1; ++cc)
            if (IsFreeForNPC(map, rr, cc)) { outR = rr; outC = cc; return true; }
    return false;
}

bool TeamSquad::SpawnAllInRegion(int map[MSZ][MSZ], int r0, int r1, int c0, int c1)
{
    const int margin = 3;
    r0 += margin; c0 += margin; r1 -= margin; c1 -= margin;

    for (auto& m : members) {
        int rr, cc;
        if (!RandomFreeCellInRegion(map, r0, r1, c0, c1, rr, cc)) return false;
        m->SetXY(cc + 0.5, rr + 0.5);
    }
    return true;
}

void TeamSquad::Draw() const {
    for (const auto& m : members) m->Show();
}

void TeamSquad::Update(int /*map*/[MSZ][MSZ], double /*smap*/[MSZ][MSZ]) {
    for (auto& m : members) m->DoSomeWork();

    // Commander logic ~every 12 frames
    if ((frameCounter++ % 12) == 0) {
        int (*env)[MSZ] = nullptr;
        double (*rs)[MSZ] = nullptr;
        for (auto& m : members) {
            env = m->EnvMap();
            rs = m->EnvSMap();
            if (env && rs) break;
        }
        if (env && rs) {
            RecomputeTeamVisibility(env);
            CommanderBrainTick(env, rs);
        }
    }
}

void TeamSquad::AttachEnvironmentToAll(int map[MSZ][MSZ], double smap[MSZ][MSZ]) {
    for (auto& m : members) m->AttachEnvironment(map, smap);
}

CommanderNPC* TeamSquad::GetCommander() {
    for (auto& m : members) if (auto* x = dynamic_cast<CommanderNPC*>(m.get())) return x;
    return nullptr;
}
MedicNPC* TeamSquad::GetMedic() {
    for (auto& m : members) if (auto* x = dynamic_cast<MedicNPC*>(m.get())) return x;
    return nullptr;
}
SupplierNPC* TeamSquad::GetSupplier() {
    for (auto& m : members) if (auto* x = dynamic_cast<SupplierNPC*>(m.get())) return x;
    return nullptr;
}
std::vector<WarriorNPC*> TeamSquad::GetWarriors() {
    std::vector<WarriorNPC*> out;
    for (auto& m : members) if (auto* x = dynamic_cast<WarriorNPC*>(m.get())) out.push_back(x);
    return out;
}

bool TeamSquad::GetFirstWarriorCell(int& outR, int& outC) const {
    for (auto& m : members) {
        if (m->role == WAR) { outR = m->Row(); outC = m->Col(); return true; }
    }
    return false;
}

void TeamSquad::SetKnownEnemyDepots(const std::vector<std::pair<int, int>>& ammo,
    const std::vector<std::pair<int, int>>& med) {
    enemyAmmoDepots = ammo;
    enemyMedDepots = med;
}
void TeamSquad::SetOwnDepots(const std::vector<std::pair<int, int>>& ammo,
    const std::vector<std::pair<int, int>>& med) {
    ownAmmoDepots = ammo;
    ownMedDepots = med;
}

void TeamSquad::RecomputeTeamVisibility(int map[MSZ][MSZ]) {
    for (int r = 0; r < MSZ; ++r)
        for (int c = 0; c < MSZ; ++c)
            commanderVis[r][c] = 0.0;

    double tmp[MSZ][MSZ];
    for (auto& m : members) {
        if (m->role == WAR || m->role == CMD || m->role == MED || m->role == SUP) {
            BuildVisibilityForUnit(map, m->Row(), m->Col(), tmp, 20);
            for (int r = 0; r < MSZ; ++r)
                for (int c = 0; c < MSZ; ++c)
                    if (tmp[r][c] > commanderVis[r][c])
                        commanderVis[r][c] = tmp[r][c];
        }
    }
}

std::pair<int, int> TeamSquad::FindSafePositionBFS(
    int map[MSZ][MSZ], double smap[MSZ][MSZ],
    int startR, int startC, int searchRadius)
{
    std::queue<std::pair<int, int>> q;
    std::unordered_set<int> visited;
    auto key = [](int r, int c) { return r * MSZ + c; };
    auto inBounds = [](int r, int c) { return r >= 0 && r < MSZ && c >= 0 && c < MSZ; };

    q.push({ startR,startC });
    visited.insert(key(startR, startC));

    std::pair<int, int> best = { startR,startC };
    double bestRisk = smap[startR][startC];

    const int dr[4] = { -1,1,0,0 };
    const int dc[4] = { 0,0,-1,1 };

    while (!q.empty()) {
        auto cur = q.front(); q.pop();
        int r = cur.first, c = cur.second;

        if (!BlocksMovement(map[r][c])) {
            bool hasCover = false;
            for (int rr = -1; rr <= 1 && !hasCover; ++rr)
                for (int cc = -1; cc <= 1; ++cc) {
                    int nr = r + rr, nc = c + cc;
                    if (inBounds(nr, nc) && (map[nr][nc] == STONE || map[nr][nc] == TREE)) { hasCover = true; break; }
                }
            double risk = smap[r][c];
            bool safer = (hasCover && risk <= bestRisk) || (risk < bestRisk * 0.7);
            if (safer) { bestRisk = risk; best = { r,c }; }
        }

        int dist = std::abs(r - startR) + std::abs(c - startC);
        if (dist >= searchRadius) continue;

        for (int i = 0; i < 4; ++i) {
            int nr = r + dr[i], nc = c + dc[i];
            if (inBounds(nr, nc) && visited.insert(key(nr, nc)).second) q.push({ nr,nc });
        }
    }
    return best;
}

void TeamSquad::CommanderBrainTick(int map[MSZ][MSZ], double smap[MSZ][MSZ]) {
    CommanderNPC* cmd = GetCommander();
    if (!cmd) return;

    auto warriors = GetWarriors();

    WarriorNPC* wounded = nullptr;
    for (auto* w : warriors) if (w->IsAlive() && w->IsWounded()) { wounded = w; break; }
    WarriorNPC* lowAmmo = nullptr;
    for (auto* w : warriors) if (w->IsAlive() && w->AmmoLow()) { lowAmmo = w; break; }

    if (wounded && !ownMedDepots.empty()) {
        if (auto* med = GetMedic()) {
            auto dep = ownMedDepots[0];
            med->SetMission(dep.first, dep.second, wounded->Row(), wounded->Col(), 10.0);
            med->setCurrentState(new MedicGoToDepot());
            med->getCurrentState()->OnEnter(med);
        }
    }
    if (lowAmmo && !ownAmmoDepots.empty()) {
        if (auto* sup = GetSupplier()) {
            auto dep = ownAmmoDepots[0];
            sup->SetMission(dep.first, dep.second, lowAmmo->Row(), lowAmmo->Col(), 10.0);
            sup->setCurrentState(new SupplierGoToDepot());
            sup->getCurrentState()->OnEnter(sup);
        }
    }

    int alive = 0;
    for (auto* w : warriors) if (w->IsAlive()) ++alive;

    double cRisk = smap[cmd->Row()][cmd->Col()];
    bool commanderInDanger = (cRisk > 0.1);
    bool hasTarget = !enemyAmmoDepots.empty();
    bool shouldAttack = (alive >= 1) && !commanderInDanger && hasTarget;

    if (shouldAttack) {
        auto goal = enemyAmmoDepots[0];
        const double RISK_W = 6.0, VIS_W = 3.0;

        for (auto* w : warriors) {
            if (!w->IsAlive()) continue;

            if (w->IsWounded() || w->AmmoLow()) {
                auto safe = FindSafePositionBFS(map, smap, w->Row(), w->Col());
                w->SetMission(safe.first, safe.second, WarriorNPC::DEFEND, 15.0);
                w->setCurrentState(new WarriorDefend());
                w->getCurrentState()->OnEnter(w);
                continue;
            }

            auto path = RiskVisAStar(map, smap, commanderVis,
                w->Row(), w->Col(),
                goal.first, goal.second,
                RISK_W, VIS_W);
            if (!path.empty()) {
                w->SetMission(goal.first, goal.second, WarriorNPC::ATTACK, RISK_W);
                w->setCurrentState(new WarriorAttack());
                w->getCurrentState()->OnEnter(w);
            }
            else {
                int midR = (w->Row() + goal.first) / 2;
                int midC = (w->Col() + goal.second) / 2;
                if (midR >= 0 && midR < MSZ && midC >= 0 && midC < MSZ && !BlocksMovement(map[midR][midC])) {
                    w->SetMission(midR, midC, WarriorNPC::MOVE_TO, 8.0);
                    w->setCurrentState(new WarriorMoveTo());
                    w->getCurrentState()->OnEnter(w);
                }
            }
        }

        if (cRisk > 0.05) {
            auto safe = FindSafePositionBFS(map, smap, cmd->Row(), cmd->Col());
            if (safe.first != cmd->Row() || safe.second != cmd->Col()) {
                auto cpath = RiskAwareAStar(map, smap, cmd->Row(), cmd->Col(),
                    safe.first, safe.second, 15.0);
                if (!cpath.empty()) { cmd->SetPathCells(cpath); cmd->setIsMoving(true); }
            }
        }
    }
    else if (commanderInDanger || alive <= 1) {
        for (auto* w : warriors) {
            if (!w->IsAlive()) continue;
            auto safe = FindSafePositionBFS(map, smap, w->Row(), w->Col());
            w->SetMission(safe.first, safe.second, WarriorNPC::DEFEND, 15.0);
            w->setCurrentState(new WarriorDefend());
            w->getCurrentState()->OnEnter(w);
        }
        auto safe = FindSafePositionBFS(map, smap, cmd->Row(), cmd->Col(), 20);
        if (safe.first != cmd->Row() || safe.second != cmd->Col()) {
            auto cpath = RiskAwareAStar(map, smap, cmd->Row(), cmd->Col(),
                safe.first, safe.second, 20.0);
            if (!cpath.empty()) { cmd->SetPathCells(cpath); cmd->setIsMoving(true); }
        }
    }
    else {
        for (auto* w : warriors) {
            if (!w->IsAlive()) continue;
            auto pos = FindSafePositionBFS(map, smap, w->Row(), w->Col(), 10);
            w->SetMission(pos.first, pos.second, WarriorNPC::MOVE_TO, 10.0);
            // זה בטוח עכשיו כי WarriorIdle מוגדר בהדר למטה
            if (!w->getCurrentState() || dynamic_cast<WarriorIdle*>(w->getCurrentState()) == nullptr) {
                w->setCurrentState(new WarriorMoveTo());
                w->getCurrentState()->OnEnter(w);
            }
        }
    }
}
