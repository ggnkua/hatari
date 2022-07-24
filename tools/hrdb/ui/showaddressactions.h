#ifndef SHOWADDRESSACTIONS_H
#define SHOWADDRESSACTIONS_H

#include <QMenu>
#include <QLabel>
#include "../models/memory.h"

class QAction;
class Session;

// Contains the QActions to do "Show in Disassembly 1", "Show in Memory 2" etc,
// plus handling their callbacks/triggers
class ShowAddressActions : public QObject
{
    Q_OBJECT
public:
    ShowAddressActions(Session* pSession);
    virtual ~ShowAddressActions();

    void addActionsToMenu(QMenu* pMenu) const;
    void setAddress(uint32_t address);

private slots:
    // Callbacks when "show in Memory X" etc is selected
    void disasmViewTrigger(int windowIndex);
    void memoryViewTrigger(int windowIndex);
    void graphicsInspectorTrigger();

private:
    // What address will be set to the Window chosen
    uint32_t     m_activeAddress;

    // Actions to add to the menus
    QAction*     m_pDisasmWindowActions[kNumDisasmViews];
    QAction*     m_pMemoryWindowActions[kNumMemoryViews];
    QAction*     m_pGraphicsInspectorAction;

    // Pointer for signal sending
    Session*     m_pSession;
};

class ShowAddressLabel : public QLabel
{
public:
    ShowAddressLabel(Session* pSession);
    ~ShowAddressLabel();

    void SetAddress(uint32_t address);
    void contextMenuEvent(QContextMenuEvent *event);

    ShowAddressActions*      m_pActions;
};

#endif // SHOWADDRESSACTIONS_H
