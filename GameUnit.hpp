#ifndef GAMEUNIT_HPP
#define GAMEUNIT_HPP

#include "MsxMath.hpp"
#include "AnimationHandler.hpp"
#include <vector>
#include <memory>

// Animation constants from source
const int ANIM_STAND = 0;
const int ANIM_WALK = 1;
const int ANIM_ATTACK = 2;
const int ANIM_HIT = 3;
const int ANIM_DIE = 4;
const int ANIM_ATTACKB = 5;

const int TILESIZE = 4;
const int AREASIZE = 12;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

class TBaseList;

class TBase {
public:
    TVector position;
    TFloat angle;
    TFloat speed;
    TFloat size, modelsize;
    int life;
    bool dead;
    bool pushable;
    
    std::unique_ptr<TAnimationHandler> anim;
    TBaseList* list;

    TBase(TBaseList* aList, TRealTimeModel* aModel);
    virtual ~TBase() = default;

    virtual void Update(TFloat delta);
    virtual void DoHit() = 0;
    void DoAttack(TFloat delta);
    bool IsValidTarget(TBase* a);
};

class TViking : public TBase {
public:
    using TBase::TBase;
    virtual void Update(TFloat delta) override;
    virtual void DoHit() override;
private:
    void HandleInput();
};

class TSheep : public TBase {
public:
    using TBase::TBase;
    virtual void Update(TFloat delta) override;
    virtual void DoHit() override;
    void ChooseActivity();
};

class TTree : public TBase {
public:
    using TBase::TBase;
    virtual void DoHit() override {}
};

class TWolf : public TBase {
public:
    TVector patrol;
    TFloat patrolarea;
    using TBase::TBase;
    virtual void Update(TFloat delta) override;
    void ChooseActivity();
    virtual void DoHit() override;
};

class TBaseList {
public:
    std::vector<std::unique_ptr<TBase>> objects;
    TViking* player;
    int level;
    int score;
    int numSheep;

    // Pre-loaded models
    TRealTimeModel *animViking, *animSheep, *animTree, *animWolf, *animFence;

    TBaseList();
    ~TBaseList();

    void Restart();
    void NextLevel();
    void Update(TFloat delta);
    void Add(std::unique_ptr<TBase> item);
    
private:
    void AdjustCollision(TBase* a, TBase* b, TFloat comp);
};

#endif