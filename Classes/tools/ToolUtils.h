//
//  ToolUtils.h
//  KPGranny2
//
//  Created by Bluefir-MAC on 15/3/27.
//
//

#ifndef __KPGranny2__ToolUtils__
#define __KPGranny2__ToolUtils__

#include "cocos2d.h"
#include "cocos-ext.h"
USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;


namespace ToolUtils
{
    UIButton* createButton(int id, std::string title, CCPoint pos);
    class CAdjustCtrls;
    class CItemCtrl : public Layout
    {
    public:
        enum ControlsID
        {
            ID_BTN_CTRL
        };
    public:
        static CItemCtrl *Create(int id, CCNode* pCtrl = NULL, const char* ctrlName = NULL)
        {
            CItemCtrl *pNewPop = new (std::nothrow)CItemCtrl;
            if (pNewPop && pNewPop->init(pCtrl, ctrlName))
            {
                pNewPop->autorelease();
                return pNewPop;
            }
            delete pNewPop;
            return NULL;
        }
        
        void select();
        void unSelect();
        void setSelectedNum(int num);
        
        virtual bool init(CCNode* pCtrl, const char* ctrlName = NULL);
        void btnTouchCallback(CCObject* pSender, TouchEventType eventType);
        
    private:
        bool m_bSelected;
        UIButton *m_pBtnSetCtrl;
        CC_SYNTHESIZE(CCPoint, m_ctrlOldPos, CtrlOldPos);
        CC_SYNTHESIZE(CAdjustCtrls*, m_adjustCtrls, AdjustCtrls);
        CC_SYNTHESIZE(CCNode*, m_ctrl, Ctrl);
        CC_SYNTHESIZE(std::string, m_ctrlName, CtrlName);
    };
    
    class CAdjustCtrls : public CCLayer
    {
    public:
        enum ControlsID
        {
            ID_LIST_CTRL,
            ID_BTN_SHOW_HIDE,
            ID_BTN_SELECT_ALL,
            ID_BTN_UNSELECT_ALL,
            ID_BTN_RESET,
            ID_BTN_PRINT_ALL,
            ID_BTN_LOCK_AXIS,

            ID_BTN_TO_LEFT,
            ID_BTN_TO_RIGHT,
            ID_BTN_TO_TOP,
            ID_BTN_TO_BOTTOM,
            
            ID_BTN_COMBBOX_PARENT,
            // 相对于父节点对齐
            ID_BTN_ALIGN_TOP_LEFT,
            ID_BTN_ALIGN_TOP_CENTER,
            ID_BTN_ALIGN_TOP_RIGHT,
            ID_BTN_ALIGN_LEFT_CENTER,
            ID_BTN_ALIGN_CENTER,
            ID_BTN_ALIGN_RIGHT_CENTER,
            ID_BTN_ALIGN_BOTTOM_LEFT,
            ID_BTN_ALIGN_BOTTOM_CENTER,
            ID_BTN_ALIGN_BOTTOM_RIGHT,
            
            ID_BTN_COMMBOX_GROUP,
            // 组对齐
            ID_BTN_ALIGN_X,
            ID_BTN_ALIGN_TOP,
            ID_BTN_ALIGN_Y,
            ID_BTN_ALIGN_LEFT,
            ID_BTN_ALIGN_BOTTOM,
            ID_BTN_ALIGN_RIGHT,
            
        };
        enum LockAxisType
        {
            NONE_AXIS,
            X_AXIS,
            Y_AXIS,
        };
        
    public:
        static CAdjustCtrls* create();
        bool init();
        void registerWithTouchDispatcher();
        void addCtrl(CCNode* pCtrl, const char* ctrlName = NULL);
        bool ccTouchBegan(CCTouch* touch, CCEvent* event);
        void ccTouchMoved(CCTouch* touch, CCEvent* event);
        void ccTouchEnded(CCTouch* touch, CCEvent* event);
        
        void listItemClick(CItemCtrl* curItemCtrl, int iEventType);
        
    private:
        bool touchSelf(CCPoint pos);
        
        void showHide();
        void selectAll();
        void unSelectAll();
        void reset();
        void printAll();
        void lockAxis();
        void alignCtrlsGroup(ControlsID id);
        void alignCtrlsParent(ControlsID id);
        void combboxParent();
        void combboxGroup();
        void smallMoveCtrls(ControlsID id);
        
        void btnTouchCallback(CCObject* pSender, TouchEventType eventType);
        
    private:
        bool m_bMoveSelf;
        bool m_bMoveCtrls;
        int m_curSelectedNum;
        LockAxisType m_lockAxis;
        
        UILayer *m_uiLayerLeft, *m_uiLayerRight;
        
        UIButton* m_pBtnLockAxis;
        std::list<CItemCtrl*> m_itemCtrlVector;
        std::list<CItemCtrl*> m_itemCtrlSelected;
        CItemCtrl* m_curItemCtrl;
        CCSprite* m_pSprTitle, *m_pSprTitle2;
        UIListView* m_pListCtrl;
        
        UIButton* m_btnAlignParent[9];
        UIButton* m_btnAlignGroup[6];
        UIButton* m_btnShowHide;
        UIButton* m_btnDir[4];
        std::vector<UIButton*> m_buttons;
        bool m_bShowAlignParent;
        bool m_bShowAlignGroup;
    };
}
#endif /* defined(__KPGranny2__ToolUtils__) */
