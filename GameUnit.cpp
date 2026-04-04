#include "GameUnit.hpp"
#include "GlobalUnit.hpp"
#include "MsxMath.hpp"
#include "SoundUnit.hpp"
#include <algorithm>
#include <cmath>

TBase::TBase(TBaseList* aList, TRealTimeModel* aModel) 
    : position({0,0,0}), angle(0), speed(0), size(1.0f), modelsize(1.0f), life(4), dead(false), pushable(true), list(aList) {
    anim = std::make_unique<TAnimationHandler>(aModel);
    // Randomize starting animation offset to desync units
    anim->Update((float)(rand() % 100) / 100.0f);
    anim->Start(ANIM_STAND, 3.0f);
}

void TBase::Update(TFloat delta) {
    TVector displacement = {0, speed * delta, 0};
    // Rotate 2D-style (on floor)
    TFloat s = std::sin(angle);
    TFloat c = std::cos(angle);
    TVector rotated = { displacement.x * c + displacement.y * s, -displacement.x * s + displacement.y * c, 0 };
    
    position.x += rotated.x;
    position.y += rotated.y;
}

void TBase::DoAttack(TFloat delta) {
    for (auto& obj : list->objects) {
        if (obj.get() != this && IsValidTarget(obj.get())) {
            obj->DoHit();
        }
    }
}

bool TBase::IsValidTarget(TBase* a) {
    // Ignore targets that are already dead or in the process of dying (life <= 0)
    if (a->dead || a->life <= 0) return false;
    TFloat dx = position.x - a->position.x;
    TFloat dy = position.y - a->position.y;
    TFloat dist = std::sqrt(dx * dx + dy * dy);
    if (dist < 5.0f) {
        // Calculate "front" target point based on size and angle
        TVector targetOffset = {0, size + 1.0f, 0};
        TFloat s = std::sin(angle);
        TFloat c = std::cos(angle);
        TVector targetPos = {
            position.x + (targetOffset.x * c + targetOffset.y * s),
            position.y + (-targetOffset.x * s + targetOffset.y * c),
            0
        };
        
        TFloat tdx = a->position.x - targetPos.x;
        TFloat tdy = a->position.y - targetPos.y;
        TFloat tdist = std::sqrt(tdx * tdx + tdy * tdy);
        return tdist < a->size;
    }
    return false;
}

void TViking::Update(TFloat delta) {
    if (dead) return;

    if (global.keystate->left) angle += 4.0f * delta;
    if (global.keystate->right) angle -= 4.0f * delta;

    // Only allow state changes if the player isn't busy with a non-interruptible animation
    int curAnim = anim->GetCurrentAnim();
    if (curAnim != ANIM_HIT && curAnim != ANIM_DIE && curAnim != ANIM_ATTACK && curAnim != ANIM_ATTACKB) {
        if (global.keystate->ctrl || global.keystate->space) {
            speed = 0;
            if (curAnim != ANIM_ATTACK && curAnim != ANIM_ATTACKB) {
                if (rand() % 2 == 1) anim->Start(ANIM_ATTACK, 0.6f);
                else anim->Start(ANIM_ATTACKB, 0.6f);
            }
        } else if (global.keystate->up) {
            speed = 6.5f;
            if (curAnim != ANIM_WALK) anim->Start(ANIM_WALK, 1.0f);
        } else if (global.keystate->down) {
            speed = -6.5f;
            if (curAnim != ANIM_WALK) anim->Start(ANIM_WALK, 1.0f);
        } else {
            speed = 0;
            if (curAnim != ANIM_STAND) anim->Start(ANIM_STAND, 3.0f);
        }
    }

    // Handle attack triggers
    if (anim->GetCurrentAnim() == ANIM_ATTACK || anim->GetCurrentAnim() == ANIM_ATTACKB) {
        if (anim->Trigger()) {
            global.man->getSoundByName("swhoosh0")->play();
            DoAttack(delta);
        }
    }

    if (anim->Update(delta)) {
        int finishedAnim = anim->GetCurrentAnim();
        if (finishedAnim == ANIM_HIT && life <= 0) {
            anim->Start(ANIM_DIE, 1.0f);
        } else if (finishedAnim == ANIM_DIE) {
            dead = true;
            speed = 0;
            return; // Exit early to ensure no movement or further state changes occur
        } else {
            anim->Start(ANIM_STAND, 3.0f);
        }
    }
    
    if (!dead) TBase::Update(delta);
}

void TViking::DoHit() {
    if (dead || life <= 0) return;
    if (anim->GetCurrentAnim() != ANIM_HIT && anim->GetCurrentAnim() != ANIM_DIE) {
        speed = 0;
        anim->Start(ANIM_HIT, 0.3f);
        life--;
        global.man->getSoundByName("hit0")->play();
        if (life == 0) global.man->getSoundByName("scream")->play();
    }
}

void TSheep::Update(TFloat delta) {
    if (anim->GetCurrentAnim() == ANIM_WALK) angle -= 0.2f * delta;

    if (anim->Update(delta)) {
        if (anim->GetCurrentAnim() == ANIM_HIT) {
            if (life <= 0) anim->Start(ANIM_DIE, 0.3f);
            else ChooseActivity();
        }
        else if (anim->GetCurrentAnim() == ANIM_DIE) {
            dead = true;
            list->score += 5;
        } else {
            ChooseActivity();
        }
    }
    TBase::Update(delta);
}

void TSheep::ChooseActivity() {
    if (rand() % 10 == 9) {
        anim->Start(ANIM_STAND, 2.0f);
        speed = 0;
    } else {
        anim->Start(ANIM_WALK, 1.0f);
        speed = 2.0f;
    }
}

void TSheep::DoHit() {
    if (anim->GetCurrentAnim() != ANIM_HIT && anim->GetCurrentAnim() != ANIM_DIE) {
        speed = 0;
        anim->Start(ANIM_HIT, 0.3f);
        life--;
        global.man->getSoundByName("hit0")->play();
        if (life == 0) global.man->getSoundByName("sheep")->play();
    }
}

void TWolf::Update(TFloat delta) {
    int curAnim = anim->GetCurrentAnim();

    // Aggression check: If idling, check if player is close enough to wake up
    TFloat wolfDistFromHome = Distance(position, patrol);
    TFloat playerDistFromHome = Distance(list->player->position, patrol);
    
    // Predator behavior: Wake up if player enters territory
    if (curAnim == ANIM_STAND && playerDistFromHome <= patrolarea) {
        anim->Start(ANIM_WALK, 1.0f);
        speed = 2.0f;
        curAnim = ANIM_WALK;
    }

    if (curAnim == ANIM_WALK) {
        TVector target;
        // Chase if player is in territory, otherwise return to patrol
        if (wolfDistFromHome <= patrolarea) {
            target = list->player->position;
        } else {
            target = patrol;
        }

        // Ensure movement speed is set to walk speed
        if (speed == 0) speed = 2.0f;

        // Calculate target angle and turn towards it
        TFloat targetAngle = atan2(target.x - position.x, target.y - position.y);
        TFloat aDist = targetAngle - angle;
        while (aDist > M_PI) aDist -= 2 * M_PI;
        while (aDist < -M_PI) aDist += 2 * M_PI;

        TFloat d = (aDist > 0 ? 1 : -1) * delta;
        if (std::abs(d) > std::abs(aDist)) d = aDist;
        angle += d;

        if (IsValidTarget(list->player)) {
            anim->Start(ANIM_ATTACK, 1.0f);
            speed = 0;
        }
    }

    if (anim->GetCurrentAnim() == ANIM_ATTACK) {
        if (anim->Trigger()) {
            global.man->getSoundByName("rambite")->play();
            DoAttack(delta);
        }
    }

    if (anim->Update(delta)) {
        if (anim->GetCurrentAnim() == ANIM_HIT) {
            if (life <= 0) anim->Start(ANIM_DIE, 0.3f);
            else ChooseActivity();
        } else if (anim->GetCurrentAnim() == ANIM_DIE) {
            dead = true;
            list->score += 15;
        } else {
            ChooseActivity();
        }
    }
    TBase::Update(delta);
}

void TWolf::DoHit() {
    if (anim->GetCurrentAnim() != ANIM_HIT && anim->GetCurrentAnim() != ANIM_DIE) {
        speed = 0;
        anim->Start(ANIM_HIT, 0.3f);
        life--;
        global.man->getSoundByName("hit0")->play();
        if (life == 0) global.man->getSoundByName("ramdie")->play();
    }
}

void TWolf::ChooseActivity() {
    if (rand() % 10 == 9) {
        anim->Start(ANIM_STAND, 2.0f);
        speed = 0;
    } else {
        anim->Start(ANIM_WALK, 1.0f);
        speed = 2.0f;
    }
}

TBaseList::TBaseList() : level(0), score(0), numSheep(0) {
    animViking = new TRealTimeModel();
    animViking->load("media/viking_weapon.model", global.man);
    animSheep = new TRealTimeModel();
    animSheep->load("media/sheep.model", global.man);
    animTree = new TRealTimeModel();
    animTree->load("media/tree1.model", global.man);
    animWolf = new TRealTimeModel();
    animWolf->load("media/ram.model", global.man);
    
    animFence = new TRealTimeModel();
    animFence->load("media/fence.model", global.man);

    player = nullptr;
}

TBaseList::~TBaseList() {
    delete animViking;
    delete animSheep;
    delete animTree;
    delete animWolf;
    delete animFence;
}

void TBaseList::Restart() {
    player = nullptr; // Reset pointer first so NextLevel defaults to starting health
    level = 0;
    score = 0;
    numSheep = 0;
    NextLevel();      // NextLevel calls objects.clear() internally
}

void TBaseList::NextLevel() {
    int lastLevelLife = 5;
    if (player != nullptr) {
        lastLevelLife = player->life;
    }

    objects.clear();
    numSheep = 0;
    level++;

    auto vPlayer = std::make_unique<TViking>(this, animViking);
    vPlayer->position.x = rand() % (TILESIZE * AREASIZE);
    vPlayer->position.y = rand() % (TILESIZE * AREASIZE);
    vPlayer->size = 1.1f;
    vPlayer->modelsize = 2.0f;
    vPlayer->life = lastLevelLife;
    player = vPlayer.get();
    Add(std::move(vPlayer));

    for (int i = 0; i < 4; ++i) {
        auto tree = std::make_unique<TTree>(this, animTree);
        tree->position.x = rand() % (TILESIZE * AREASIZE);
        tree->position.y = rand() % (TILESIZE * AREASIZE);
        tree->size = 1.1f;
        tree->modelsize = 4.0f;
        tree->pushable = false;
        tree->speed = 0;
        Add(std::move(tree));
    }

    for (int i = 0; i < level * 2 + 2; ++i) {
        auto sheep = std::make_unique<TSheep>(this, animSheep);
        sheep->position.x = rand() % (TILESIZE * AREASIZE);
        sheep->position.y = rand() % (TILESIZE * AREASIZE);
        sheep->size = 1.4f;
        sheep->modelsize = 1.8f;
        sheep->angle = (rand() % 360) / 180.0f * M_PI;
        Add(std::move(sheep));
    }

    for (int i = 0; i <= level; ++i) {
        auto wolf = std::make_unique<TWolf>(this, animWolf);
        wolf->position.x = rand() % (TILESIZE * AREASIZE);
        wolf->position.y = rand() % (TILESIZE * AREASIZE);
        wolf->patrol.x = rand() % (TILESIZE * AREASIZE);
        wolf->patrol.y = rand() % (TILESIZE * AREASIZE);
        wolf->patrol.z = 0.0f; // Initialize Z to ensure distance checks are 2D/reliable
        wolf->patrolarea = 20.0f;
        wolf->size = 1.4f;
        wolf->modelsize = 1.8f;
        wolf->pushable = true;
        wolf->life = 8;
        wolf->speed = 0;
        wolf->angle = (rand() % 360) / 180.0f * M_PI;
        Add(std::move(wolf));
    }

    if (objects.size() >= 2) {
        TWolf* w1 = dynamic_cast<TWolf*>(objects[objects.size() - 1].get());
        if (w1) w1->patrolarea = 1000.0f;
        TWolf* w2 = dynamic_cast<TWolf*>(objects[objects.size() - 2].get());
        if (w2) w2->patrolarea = 1000.0f;
    }
}

void TBaseList::Add(std::unique_ptr<TBase> item) {
    if (dynamic_cast<TSheep*>(item.get())) numSheep++;
    objects.push_back(std::move(item));
}

void TBaseList::Update(TFloat delta) {
    for (auto& obj : objects) {
        obj->Update(delta);

        // Boundary checks from Pascal: avoid exit from the play area
        TFloat bounds = TILESIZE * AREASIZE;
        if (obj->position.x < obj->size) obj->position.x = obj->size;
        if (obj->position.x > bounds - obj->size) obj->position.x = bounds - obj->size;
        if (obj->position.y < obj->size) obj->position.y = obj->size;
        if (obj->position.y > bounds - obj->size) obj->position.y = bounds - obj->size;
    }

    // Simple O(N^2) collision
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            TBase* a = objects[i].get();
            TBase* b = objects[j].get();
            
            // Use 2D Distance (X, Y) to match Pascal vector2d logic
            TFloat dx = a->position.x - b->position.x;
            TFloat dy = a->position.y - b->position.y;
            TFloat dist = std::sqrt(dx * dx + dy * dy);
            
            TFloat minDist = a->size + b->size;
            if (dist < minDist) {
                AdjustCollision(a, b, minDist - dist);
            }
        }
    }

    // Cleanup dead
    objects.erase(std::remove_if(objects.begin(), objects.end(), 
        [this](const std::unique_ptr<TBase>& b) {
            if (b->dead && b.get() != this->player) {
                if (dynamic_cast<TSheep*>(b.get())) numSheep--;
                return true;
            }
            return false;
        }), objects.end());
}

void TBaseList::AdjustCollision(TBase* a, TBase* b, TFloat comp) {
    // Direction vector on ground (X, Y in logic)
    float dx = a->position.x - b->position.x;
    float dy = a->position.y - b->position.y;
    float mag = std::sqrt(dx * dx + dy * dy);
    
    if (mag < 0.0001f) { dx = 1.0f; dy = 0.0f; mag = 1.0f; }
    dx /= mag; dy /= mag;

    if (a->pushable == b->pushable) {
        // Both pushable or both static: split the repulsion
        float move = (comp / 2.0f);
        a->position.x += dx * move;
        a->position.y += dy * move;
        b->position.x -= dx * move;
        b->position.y -= dy * move;
    } else {
        // Only one is pushable: move only the pushable one by the full amount
        if (a->pushable) {
            a->position.x += dx * comp;
            a->position.y += dy * comp;
        } else {
            b->position.x -= dx * comp;
            b->position.y -= dy * comp;
        }
    }
}
