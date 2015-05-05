//
//  GameLayer.cpp
//  test226
//
//  Created by DONG on 15/4/22.
//
//

#include "GameLayer.h"


bool GameLayer::init()
{
    if (!CCLayer::init())
    {
        return false;
    }
    
    return true;
}

void GameLayer::onEnter()
{
    CCLayer::onEnter();
    
    scheduleUpdate();
}

void GameLayer::update(float dt)
{
    
}