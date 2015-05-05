//
//  ToolUtils.cpp
//  KPGranny2
//
//  Created by Bluefir-MAC on 15/3/27.
//
//

#include "ToolUtils.h"
#include "touch_dispatcher/CCTouchDispatcher.h"

#define _EVENT_SELECT_CTRL  0x10000
#define _EVENT_UNSELECT_CTRL 0x10000+1
const float ITEM_WIDTH  = 128.f;
const float ITEM_HEIGHT = 50.f;
const float LIST_HEIGHT = (ITEM_HEIGHT * 4);

#define FONT_NAME "tools/arial.ttf"

namespace ToolUtils
{
    const ccColor3B SELECTED_COLOR = ccBLUE;
    const ccColor3B UN_SELECTED_COLOR = ccBLACK;
    
    UIButton* createButton(int id, std::string title, CCPoint pos)
    {
        auto btn = UIButton::create();
        btn->setTag(id);
        btn->setTitleFontName(FONT_NAME);
        btn->setTitleText(title);
        btn->setTitleFontSize(20);
        btn->setTouchEnabled(true);
        btn->loadTextures("tools/btn_normal.png", "tools/btn_selected.png", "");
        btn->setPosition(pos);
        
        return btn;
    }
    
    //////////////////////////////////  控件项  ////////////////////////////////
    
    bool CItemCtrl::init(CCNode* pCtrl, const char* ctrlName)
    {
        if (!Layout::init())
        {
            return false;
        }
        
        m_ctrl = pCtrl;
        if (ctrlName != NULL)
            m_ctrlName = ctrlName;
        else
            m_ctrlName = "Unknown";
        
        setSize(CCSizeMake(ITEM_WIDTH, ITEM_HEIGHT));
        
        m_pBtnSetCtrl = createButton(ID_BTN_CTRL, m_ctrlName, ccp(ITEM_WIDTH * 0.5f, ITEM_HEIGHT * 0.5f));
        m_pBtnSetCtrl->addTouchEventListener(this, toucheventselector(CItemCtrl::btnTouchCallback));
        addChild(m_pBtnSetCtrl);
        
        if (pCtrl != NULL)
        {
            m_ctrlOldPos = pCtrl->getPosition();
            this->unSelect();
        }
        
        return true;
    }
    
    void CItemCtrl::setSelectedNum(int num)
    {
        m_pBtnSetCtrl->setTitleText(CCString::createWithFormat("%s%d", m_ctrlName.c_str(), num)->getCString());
    }
    
    void CItemCtrl::select()
    {
        if (m_ctrl != NULL)
        {
            m_bSelected = true;
            m_pBtnSetCtrl->setTitleColor(SELECTED_COLOR);
            auto rotBy1 = CCRotateBy::create(0.05f, 45.f);
            auto rotBy2 = CCRotateBy::create(0.05f, -45.f);
            m_ctrl->runAction(CCRepeat::create(CCSequence::create(
                                                                  rotBy1, rotBy1->reverse(), rotBy2, rotBy2->reverse(), NULL), 3));
        }
    }
    
    void CItemCtrl::unSelect()
    {
        if (m_ctrl != NULL)
        {
            m_bSelected = false;
            m_pBtnSetCtrl->setTitleColor(UN_SELECTED_COLOR);
            m_pBtnSetCtrl->setTitleText(m_ctrlName);
        }
    }
    
    void CItemCtrl::btnTouchCallback(CCObject* pSender, TouchEventType eventType)
    {
        UIButton* btn = (UIButton*)pSender;
        
        if (m_ctrl != NULL && eventType == TOUCH_EVENT_ENDED)
        {
            if (btn->getTag() == ID_BTN_CTRL)			// 选中控件
            {
                if (m_bSelected)
                {
                    m_adjustCtrls->listItemClick(this, _EVENT_UNSELECT_CTRL);
                    this->unSelect();
                }
                else
                {
                    m_adjustCtrls->listItemClick(this,  _EVENT_SELECT_CTRL);
                    this->select();
                }
            };
        }
    }
    
    
    //////////////////////////////////  控件位置调整工具  ////////////////////////////////
    
    CAdjustCtrls* CAdjustCtrls::create()
    {
        auto pTool = new CAdjustCtrls;
        if (pTool && pTool->init())
        {
            pTool->autorelease();
            return pTool;
        }
        
        delete pTool;
        return NULL;
        
    }
    
    bool CAdjustCtrls::init()
    {
        if (!CCLayer::init())
        {
            return false;
        }
        setTouchMode(kCCTouchesOneByOne);
        setTouchEnabled(true);
        
        m_itemCtrlVector.clear();
        m_curItemCtrl = NULL;
        m_lockAxis = NONE_AXIS;
        m_curSelectedNum = 0;
        
        // uiLayer
        auto pUILayer = UILayer::create();
        addChild(pUILayer, 200);
        m_uiLayerLeft = pUILayer;
        
        auto pLayerColor = CCLayerColor::create(ccc4(255, 0, 0, 70), ITEM_WIDTH, LIST_HEIGHT);
        pUILayer->addChild(pLayerColor);
        pLayerColor->setPosition(ccp(0, ITEM_HEIGHT));
        
        // title
        m_pSprTitle = CCSprite::create("tools/btn_selected.png");
        m_pSprTitle->setAnchorPoint(CCPointZero);
        m_pSprTitle->setPosition(ccp(0, 0));
        pUILayer->addChild(m_pSprTitle);
        
        auto pLabelTitle = CCLabelTTF::create("Drag", FONT_NAME, 24);
        m_pSprTitle->addChild(pLabelTitle);
        pLabelTitle->setPosition(ccp(m_pSprTitle->getContentSize().width * 0.5f, m_pSprTitle->getContentSize().height * 0.5f));
        
        m_pSprTitle2 = CCSprite::create("tools/btn_selected.png");
        m_pSprTitle2->setAnchorPoint(CCPointZero);
        m_pSprTitle2->setPosition(ccp(0, LIST_HEIGHT + ITEM_HEIGHT * 2));
        pUILayer->addChild(m_pSprTitle2);
        
        auto pLabelTitle2 = CCLabelTTF::create("Drag", FONT_NAME, 24);
        m_pSprTitle2->addChild(pLabelTitle2);
        pLabelTitle2->setPosition(ccp(m_pSprTitle2->getContentSize().width * 0.5f, m_pSprTitle2->getContentSize().height * 0.5f));
        
        // list
        m_pListCtrl = UIListView::create();
        m_pListCtrl->setDirection(SCROLLVIEW_DIR_VERTICAL);
        m_pListCtrl->setTouchEnabled(true);
        m_pListCtrl->setBounceEnabled(true);
        m_pListCtrl->setSize(CCSizeMake(ITEM_WIDTH, LIST_HEIGHT));
        m_pListCtrl->setPosition(ccp(0, ITEM_HEIGHT));
        pUILayer->addWidget(m_pListCtrl);
        
        // button
        float posX = ITEM_WIDTH * 1.5f;
        float posY = ITEM_HEIGHT * 0.5f;
        UIButton* btn;
        
        // show hide
        btn = createButton(ID_BTN_SHOW_HIDE, "Hide", ccp(ITEM_WIDTH * 0.5f, ITEM_HEIGHT * 1.5f + LIST_HEIGHT));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        m_btnShowHide = btn;
        
        // select
        btn = createButton(ID_BTN_SELECT_ALL, "SelectAll", ccp(posX, posY));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        posY += ITEM_HEIGHT;
        m_buttons.push_back(btn);
        
        // unselect
        btn = createButton(ID_BTN_UNSELECT_ALL, "UnSelectAll", ccp(posX, posY));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        posY += ITEM_HEIGHT;
        m_buttons.push_back(btn);
        
        // reset
        btn = createButton(ID_BTN_RESET, "Reset", ccp(posX, posY));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        posY += ITEM_HEIGHT;
        m_buttons.push_back(btn);
        
        // print
        btn = createButton(ID_BTN_PRINT_ALL, "Print", ccp(posX, posY));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        posY += ITEM_HEIGHT;
        m_buttons.push_back(btn);
        
        // 锁定
        m_pBtnLockAxis = createButton(ID_BTN_LOCK_AXIS, "No Lock", ccp(posX, posY));
        m_pBtnLockAxis->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(m_pBtnLockAxis);
        posY += ITEM_HEIGHT;
        m_buttons.push_back(m_pBtnLockAxis);
        
        // 下拉列表按钮
        btn = createButton(ID_BTN_COMBBOX_PARENT, "AlignParent", ccp(posX, posY));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        btn->setZOrder(100);
        m_buttons.push_back(btn);
        
        std::string btnTextParentAlign[] = {
            "TopLeft",
            "TopCenter",
            "TopRight",
            "LeftCenter",
            "Center",
            "RightCenter",
            "BottomLeft",
            "BottomCenter",
            "BottomRight",
        };
        
        // create combbox
        for (int i=0; i<9; ++i)
        {
            m_btnAlignParent[i] = createButton(ID_BTN_ALIGN_TOP_LEFT + i, btnTextParentAlign[i], ccp(posX, posY));
            m_btnAlignParent[i]->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
            pUILayer->addWidget(m_btnAlignParent[i]);
            m_btnAlignParent[i]->setZOrder(50 - i);
            m_btnAlignParent[i]->setVisible(false);
        }
        posY += ITEM_HEIGHT;
                                               
        std::string btnTextGroupAlign[] = {
            "AlignX",
            "AlignTop",
            "AlignY",
            "AlignLeft",
            "AlignBottom",
            "AlignRight",
        };
        
        btn = createButton(ID_BTN_COMMBOX_GROUP, "AlignGroup", ccp(posX, posY));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        btn->setZOrder(100);
        m_buttons.push_back(btn);
        
        for (int i=0; i<6; ++i)
        {
            // 对齐X
            m_btnAlignGroup[i] = createButton(ID_BTN_ALIGN_X + i, btnTextGroupAlign[i], ccp(posX, posY));
            m_btnAlignGroup[i]->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
            pUILayer->addWidget(m_btnAlignGroup[i]);
            m_btnAlignGroup[i]->setVisible(false);
        }
        posY += ITEM_HEIGHT;
        
        m_bShowAlignParent = false;
        m_bShowAlignGroup = false;
        setContentSize(CCSizeMake(ITEM_WIDTH * 2, posY - ITEM_HEIGHT * 0.5f));

        // right layer
        pUILayer = UILayer::create();
        addChild(pUILayer, 200);
        m_uiLayerRight = pUILayer;

        // direction
        auto size = CCDirector::sharedDirector()->getVisibleSize();
        btn = createButton(ID_BTN_TO_RIGHT, "Right", ccp(size.width - ITEM_WIDTH * 0.5f, ITEM_HEIGHT * 1.5f));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        m_buttons.push_back(btn);
        m_btnDir[0] = btn;
        
        btn = createButton(ID_BTN_TO_LEFT, "Left", ccp(size.width - ITEM_WIDTH * 1.5f, ITEM_HEIGHT * 1.5f));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        m_buttons.push_back(btn);
        m_btnDir[1] = btn;
        
        btn = createButton(ID_BTN_TO_TOP, "Top", ccp(size.width - ITEM_WIDTH, ITEM_HEIGHT * 2.5f));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        m_buttons.push_back(btn);
        m_btnDir[2] = btn;
        
        btn = createButton(ID_BTN_TO_BOTTOM, "Bottom", ccp(size.width - ITEM_WIDTH, ITEM_HEIGHT * 0.5f));
        btn->addTouchEventListener(this, toucheventselector(CAdjustCtrls::btnTouchCallback));
        pUILayer->addWidget(btn);
        m_buttons.push_back(btn);
        m_btnDir[3] = btn;
        
        return true;
    }
    
    void CAdjustCtrls::registerWithTouchDispatcher()
    {
        CCTouchDispatcher* pDispatcher = CCDirector::sharedDirector()->getTouchDispatcher();
        
        pDispatcher->addTargetedDelegate(this, INT_MIN + 1, false);
    }
    
    void CAdjustCtrls::addCtrl(CCNode* pCtrl, const char* ctrlName)
    {
        CItemCtrl* itemCtrl = CItemCtrl::Create(-1, pCtrl, ctrlName);
        itemCtrl->setAdjustCtrls(this);
        m_pListCtrl->pushBackCustomItem(itemCtrl);
        m_itemCtrlVector.push_back(itemCtrl);
    }
    
    bool CAdjustCtrls::ccTouchBegan(CCTouch* touch, CCEvent* event)
    {
        CCPoint pos = touch->getLocation();

        if (m_pSprTitle->boundingBox().containsPoint(m_uiLayerLeft->convertToNodeSpace(pos)) ||
            m_pSprTitle2->boundingBox().containsPoint(m_uiLayerLeft->convertToNodeSpace(pos)))
        {
            m_bMoveSelf = true;
        }
        else
        {
            m_bMoveSelf = false;
        }

        if (touchSelf(pos))
        {
            m_bMoveCtrls = false;
        }
        else
        {
            m_bMoveCtrls = true;
        }
        return true;
    }
    
    void CAdjustCtrls::ccTouchMoved(CCTouch* touch, CCEvent* event)
    {
        auto delta = convertToNodeSpace(touch->getLocation()) - convertToNodeSpace(touch->getPreviousLocation());
        if (m_bMoveSelf)
        {
            m_uiLayerLeft->setPosition(m_uiLayerLeft->getPosition() + delta);
        }
        else if (m_bMoveCtrls)
        {
            switch (m_lockAxis)
            {
                case X_AXIS:
                    delta.x = 0.f;
                    break;
                case Y_AXIS:
                    delta.y = 0.f;
                    break;
            }
            for (auto itemCtrl : m_itemCtrlSelected)        // 调整控件位置
            {
                itemCtrl->getCtrl()->setPosition(itemCtrl->getCtrl()->getPosition() + delta);
            }
        }
    }
    
    void CAdjustCtrls::ccTouchEnded(CCTouch* touch, CCEvent* event)
    {
        
    }
    
    bool CAdjustCtrls::touchSelf(CCPoint pos)
    {
        bool bTouchDirBtn = false;
        for (int i=0; i<4; ++i)
        {
            if (m_btnDir[i]->boundingBox().containsPoint(m_uiLayerRight->convertToNodeSpace(pos)))
            {
                bTouchDirBtn = true;
                break;
            }
        }
        CCLog("touch btn: %d, %f, %f", bTouchDirBtn, m_uiLayerRight->convertToNodeSpace(pos).x, m_uiLayerRight->convertToNodeSpace(pos).y);
        CCRect rect = CCRectZero;
        rect.size = getContentSize();
        if (!m_bVisible)
        {
            rect.size.width = ITEM_WIDTH;
        }
        bool touchMe;
        if (rect.containsPoint(m_uiLayerLeft->convertToNodeSpace(pos)) || bTouchDirBtn)
        {
            touchMe = true;
        }
        else
        {
            touchMe = false;
        }
        CCLog("%d", touchMe);
        return touchMe;
    }
    
    void CAdjustCtrls::btnTouchCallback(CCObject* pSender, TouchEventType eventType)
    {
        UIButton* btn = (UIButton*)pSender;
        
        if (eventType == TOUCH_EVENT_ENDED)
        {
            switch (btn->getTag())
            {
                case ID_BTN_SHOW_HIDE:
                    this->showHide();
                    break;
                case ID_BTN_SELECT_ALL:
                    this->selectAll();
                    break;
                case ID_BTN_UNSELECT_ALL:
                    this->unSelectAll();
                    break;
                case ID_BTN_RESET:
                    this->reset();
                    break;
                case ID_BTN_PRINT_ALL:
                    this->printAll();
                    break;
                case ID_BTN_LOCK_AXIS:		// lock
                    this->lockAxis();
                    break;
                case ID_BTN_COMBBOX_PARENT:
                    this->combboxParent();
                    break;
                case ID_BTN_COMMBOX_GROUP:
                    this->combboxGroup();
                    break;
                case ID_BTN_ALIGN_CENTER:
                case ID_BTN_ALIGN_LEFT_CENTER:
                case ID_BTN_ALIGN_RIGHT_CENTER:
                case ID_BTN_ALIGN_TOP_CENTER:
                case ID_BTN_ALIGN_BOTTOM_CENTER:
                case ID_BTN_ALIGN_TOP_LEFT:
                case ID_BTN_ALIGN_TOP_RIGHT:
                case ID_BTN_ALIGN_BOTTOM_LEFT:
                case ID_BTN_ALIGN_BOTTOM_RIGHT:
                    this->alignCtrlsParent((ControlsID)btn->getTag());
                    break;
                case ID_BTN_ALIGN_X:
                case ID_BTN_ALIGN_TOP:
                case ID_BTN_ALIGN_Y:
                case ID_BTN_ALIGN_LEFT:
                case ID_BTN_ALIGN_BOTTOM:
                case ID_BTN_ALIGN_RIGHT:
                    this->alignCtrlsGroup((ControlsID)btn->getTag());
                case ID_BTN_TO_LEFT:
                case ID_BTN_TO_RIGHT:
                case ID_BTN_TO_TOP:
                case ID_BTN_TO_BOTTOM:
                    this->smallMoveCtrls((ControlsID)btn->getTag());
                    break;
            }
        }
    }
    
    void CAdjustCtrls::smallMoveCtrls(ControlsID id)
    {
        std::list<CItemCtrl*>::iterator iter = m_itemCtrlSelected.begin();
        switch (id) {
            case ID_BTN_TO_LEFT:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionX((*iter)->getCtrl()->getPositionX() - 1);
                }
                break;
            case ID_BTN_TO_RIGHT:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionX((*iter)->getCtrl()->getPositionX() + 1);
                }
                break;
            case ID_BTN_TO_TOP:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionY((*iter)->getCtrl()->getPositionY() + 1);
                }
                break;
            case ID_BTN_TO_BOTTOM:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionY((*iter)->getCtrl()->getPositionY() - 1);
                }
                break;
            default:
                break;
        }
    }
    
    void CAdjustCtrls::listItemClick(CItemCtrl* curItemCtrl, int iEventType)
    {
        if (curItemCtrl != NULL)
        {
            if (iEventType == _EVENT_SELECT_CTRL)
            {
                m_itemCtrlSelected.push_back(curItemCtrl);
            }
            else if (iEventType == _EVENT_UNSELECT_CTRL)
            {
                m_itemCtrlSelected.remove(curItemCtrl);
            }
            int curCount = 1;
            for (auto itemCtrl : m_itemCtrlSelected)
            {
                itemCtrl->setSelectedNum(curCount++);
            }
            
        }
    }
    
    void CAdjustCtrls::showHide()
    {
        bool bShow = false;
        if (strcmp(m_btnShowHide->getTitleText(), "Hide") == 0)
        {
            m_btnShowHide->setTitleText("Show");
        }
        else
        {
            m_btnShowHide->setTitleText("Hide");
            bShow = true;
        }
        for (auto btn : m_buttons)
        {
            btn->setVisible(bShow);
        }
        
        if (m_bShowAlignParent)
        {
            this->combboxParent();
        }
        if (m_bShowAlignGroup)
        {
            this->combboxGroup();
        }
        
    }
    
    void CAdjustCtrls::lockAxis()
    {
        switch (m_lockAxis)
        {
            case NONE_AXIS:
                m_pBtnLockAxis->setTitleText("Lock X");
                m_lockAxis = X_AXIS;
                break;
            case X_AXIS:
                m_pBtnLockAxis->setTitleText("Lock Y");
                m_lockAxis = Y_AXIS;
                break;
            case Y_AXIS:
                m_pBtnLockAxis->setTitleText("No Lock");
                m_lockAxis = NONE_AXIS;
                break;
            default:
                break;
        }
    }
    
    void CAdjustCtrls::combboxParent()
    {
        int s = 1;
        m_bShowAlignParent = !m_bShowAlignParent;
        if (m_bShowAlignParent)
            s = -1;
        
        for (int i = 0; i < 9; i++)
        {
            m_btnAlignParent[i]->runAction(CCMoveBy::create(0.2f, ccp(-s * ITEM_WIDTH * (i % 3 + 1), s * ITEM_HEIGHT * ((i / 3) + 1))));
            m_btnAlignParent[i]->setVisible(s == -1);
        }
        if (m_bShowAlignGroup)
        {
            m_bShowAlignGroup = false;
            for (int i = 0; i < 6; i++)
            {
                m_btnAlignGroup[i]->runAction(CCMoveBy::create(0.2f, ccp(-1 * ITEM_WIDTH * (i % 3 + 1), ITEM_HEIGHT * ((i / 3) + 1))));
                m_btnAlignGroup[i]->setVisible(false);
            }
        }
    }
    
    void CAdjustCtrls::combboxGroup()
    {
        int s = 1;
        m_bShowAlignGroup = !m_bShowAlignGroup;
        if (m_bShowAlignGroup)
            s = -1;
        
        for (int i = 0; i < 6; i++)
        {
            m_btnAlignGroup[i]->runAction(CCMoveBy::create(0.2f, ccp(-s * ITEM_WIDTH * (i % 3 + 1), s * ITEM_HEIGHT * ((i / 3) + 1))));
            m_btnAlignGroup[i]->setVisible(s == -1);
        }
        if (m_bShowAlignParent)
        {
            m_bShowAlignParent = false;
            for (int i = 0; i < 9; i++)
            {
                m_btnAlignParent[i]->runAction(CCMoveBy::create(0.2f, ccp(-1 * ITEM_WIDTH * (i % 3 + 1), ITEM_HEIGHT * ((i / 3) + 1))));
                m_btnAlignParent[i]->setVisible(false);
            }
        }
    }
    
    void CAdjustCtrls::alignCtrlsParent(ControlsID id)
    {
        for (auto itemCtrl : m_itemCtrlSelected)
        {
            CCNode* child = itemCtrl->getCtrl();
            CCNode* parent = child->getParent();
            if (parent == NULL)
            {
                continue;
            }
            CCPoint pos = CCPointZero;
            CCSize size = parent->getContentSize();
            switch (id)
            {
                case ID_BTN_ALIGN_CENTER:
                    pos = ccp(size.width * 0.5f, size.height * 0.5f);
                    break;
                case ID_BTN_ALIGN_LEFT_CENTER:
                    pos = ccp(size.width * 0.f, size.height * 0.5f);
                    break;
                case ID_BTN_ALIGN_RIGHT_CENTER:
                    pos = ccp(size.width * 1.f, size.height * 0.5f);
                    break;
                case ID_BTN_ALIGN_TOP_CENTER:
                    pos = ccp(size.width * 0.5f, size.height * 1.f);
                    break;
                case ID_BTN_ALIGN_BOTTOM_CENTER:
                    pos = ccp(size.width * 0.5f, size.height * 0.f);
                    break;
                case ID_BTN_ALIGN_TOP_LEFT:
                    pos = ccp(size.width * 0.f, size.height * 1.f);
                    break;
                case ID_BTN_ALIGN_TOP_RIGHT:
                    pos = ccp(size.width * 1.f, size.height * 1.f);
                    break;
                case ID_BTN_ALIGN_BOTTOM_LEFT:
                    pos = ccp(size.width * 0.f, size.height * 0.f);
                    break;
                case ID_BTN_ALIGN_BOTTOM_RIGHT:
                    pos = ccp(size.width * 1.f, size.height * 0.f);
                    break;
                default:
                    break;
            }
            child->setPosition(pos);
        }
    }
    
    void CAdjustCtrls::alignCtrlsGroup(ControlsID id)
    {
        if (m_itemCtrlSelected.size() == 0)
            return;
        
        auto iter = m_itemCtrlSelected.begin();
        CItemCtrl* itemCtrl = *iter;
        iter++;
        switch (id)
        {
            case ID_BTN_ALIGN_X:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionY(itemCtrl->getCtrl()->getPositionY());
                }
                break;
            case ID_BTN_ALIGN_Y:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionX(itemCtrl->getCtrl()->getPositionX());
                }
                break;
            case ID_BTN_ALIGN_LEFT:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionX(itemCtrl->getCtrl()->boundingBox().getMinX()
                              + (*iter)->getCtrl()->getContentSize().width * (*iter)->getCtrl()->getAnchorPoint().x);
                }
                break;
            case ID_BTN_ALIGN_RIGHT:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionX(itemCtrl->getCtrl()->boundingBox().getMaxX()
                             - (*iter)->getCtrl()->getContentSize().width * (1 - (*iter)->getCtrl()->getAnchorPoint().x));
                }
                break;
            case ID_BTN_ALIGN_TOP:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionY(itemCtrl->getCtrl()->boundingBox().getMaxY()
                            - (*iter)->getCtrl()->getContentSize().height * (1 - (*iter)->getCtrl()->getAnchorPoint().y));
                }
                break;
            case ID_BTN_ALIGN_BOTTOM:
                for (; iter != m_itemCtrlSelected.end(); ++iter)
                {
                    (*iter)->getCtrl()->setPositionY(itemCtrl->getCtrl()->boundingBox().getMinY()
                            + (*iter)->getCtrl()->getContentSize().height * (*iter)->getCtrl()->getAnchorPoint().y);
                }
                break;
            default:
                break;
        }

    }
    
    void CAdjustCtrls::reset()
    {
        for (auto itemCtrl : m_itemCtrlSelected)
        {
            itemCtrl->getCtrl()->setPosition(itemCtrl->getCtrlOldPos());
        }
    }
    
    void CAdjustCtrls::unSelectAll()
    {
        for (auto itemCtrl : m_itemCtrlSelected)
        {
            itemCtrl->unSelect();
        }
        m_itemCtrlSelected.clear();
        m_curSelectedNum = 0;
    }
    
    void CAdjustCtrls::selectAll()
    {
        m_itemCtrlSelected.clear();
        m_itemCtrlSelected = m_itemCtrlVector;
        int curCount = 1;
        for (auto itemCtrl : m_itemCtrlSelected)
        {
            itemCtrl->select();
            itemCtrl->setSelectedNum(curCount++);
        }
    }
    
    void CAdjustCtrls::printAll()
    {
        CCLOG("-------------------------控件位置调整工具------------------------");
        for (auto itemCtrl : m_itemCtrlVector)
        {
            auto pos = itemCtrl->getCtrl()->getPosition();
            auto oldPos = itemCtrl->getCtrlOldPos();
            CCLOG("%s: pos:(%.f,%.f)), oldPos:(%.f,%.f), delta:(%.f,%.f)", itemCtrl->getCtrlName().c_str(),
                  pos.x, pos.y, oldPos.x, oldPos.y, pos.x - oldPos.x, pos.y - oldPos.y);
        }
        CCLOG("-------------------------控件位置调整工具------------------------");
    }
    
}

