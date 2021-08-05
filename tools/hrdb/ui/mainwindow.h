#ifndef MAINWINDOW_H
#define MAINWINDOW_H

/*
	Main GUI Window.

	Currently everything is controlled and routed through here.
*/

#include <QMainWindow>
#include "../models/targetmodel.h"
#include "../models/disassembler.h"
#include "../models/session.h"

class QPushButton;
class QLabel;
class QTcpSocket;
class QTextEdit;
class QActionGroup;
class QComboBox;

class Dispatcher;
class TargetModel;

class DisasmWindow;
class MemoryWindow;
class GraphicsInspectorWidget;
class BreakpointsWindow;
class ConsoleWindow;
class ExceptionDialog;
class RunDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void connectChangedSlot();
	void startStopChangedSlot();
    void registersChangedSlot(uint64_t commandId);
    void memoryChangedSlot(int slot, uint64_t commandId);
    void symbolTableChangedSlot(uint64_t commandId);
    void startStopDelayedSlot(int running);

    void startStopClicked();
    void singleStepClicked();
    void nextClicked();
    void runToClicked();
    void addBreakpointPressed();
    void breakPressed();

    // Menu item callbacks
    void menuConnect();
    void menuDisconnect();

    void about();
    void aboutQt();
private:
    void updateWindowMenu();
    // File Menu
    void Run();
    void Connect();
    void Disconnect();

    // Exception Menu
    void ExceptionsDialog();

	// Populaters
	void PopulateRegisters();
    QString FindSymbol(uint32_t addr);
    void PopulateRunningSquare();
    void updateButtonEnable();

    // Settings
    void loadSettings();
    void saveSettings();

    // Our UI widgets
    QPushButton*	m_pStartStopButton;
    QPushButton*	m_pStepIntoButton;
    QPushButton*	m_pStepOverButton;
    QPushButton*	m_pRunToButton;
    QComboBox*      m_pRunToCombo;
	QTextEdit*		m_pRegistersTextEdit;
    QWidget*        m_pRunningSquare;

    // Dialogs
    ExceptionDialog*    m_pExceptionDialog;
    RunDialog*          m_pRunDialog;

    // Docking windows
    DisasmWindow*           m_pDisasmWidget0;
    DisasmWindow*           m_pDisasmWidget1;
    MemoryWindow*           m_pMemoryViewWidget0;
    MemoryWindow*           m_pMemoryViewWidget1;
    GraphicsInspectorWidget*    m_pGraphicsInspector;
    BreakpointsWindow*          m_pBreakpointsWidget;
    ConsoleWindow*              m_pConsoleWindow;

    // Low-level data
    Session                     m_session;
    Dispatcher*             	m_pDispatcher;
    TargetModel*                m_pTargetModel;

    // Shown data
    Registers                   m_prevRegs;
    Disassembler::disassembly   m_disasm;

    // Menus
    void createActions();
    void createMenus();
    void enableVis(QWidget *pWidget);
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;

    QAction *runAct;
    QAction *connectAct;
    QAction *disconnectAct;
    QAction *exitAct;

    QAction *exceptionsAct;

    QAction *disasmWindowAct0;
    QAction *disasmWindowAct1;
    QAction *memoryWindowAct0;
    QAction *memoryWindowAct1;
    QAction *graphicsInspectorAct;
    QAction *breakpointsWindowAct;
    QAction *consoleWindowAct;

    QAction *aboutAct;
    QAction *aboutQtAct;
};
#endif // MAINWINDOW_H
