//
//  GameLayer.h
//  test226
//
//  Created by DONG on 15/4/22.
//
//

#ifndef __test226__GameLayer__
#define __test226__GameLayer__

#include "cocos2d.h"
USING_NS_CC;

class GameLayer : public CCLayer
{
public:
    CREATE_FUNC(GameLayer);
    bool init();
    void onEnter();
    void update(float dt);
};

#endif /* defined(__test226__GameLayer__) */
