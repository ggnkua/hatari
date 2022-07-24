#ifndef ADDBREAKPOINTDIALOG_H
#define ADDBREAKPOINTDIALOG_H

#include <QDialog>

class QCheckBox;
class QLineEdit;
class QComboBox;
class QButtonGroup;

class TargetModel;
class Dispatcher;
class SymbolTableModel;

class AddBreakpointDialog : public QDialog
{
    Q_OBJECT
public:
    AddBreakpointDialog(QWidget* parent, TargetModel* pTargetModel, Dispatcher* pDispatcher);
    virtual ~AddBreakpointDialog();

protected:
    void showEvent(QShowEvent *event);

private slots:
    void okClicked();
    void useClicked();

private:
    TargetModel*        m_pTargetModel;
    Dispatcher*         m_pDispatcher;

    QLineEdit*          m_pExpressionEdit;

    QLineEdit*          m_pMemoryAddressEdit;
    QComboBox*          m_pMemoryConditionCombo;
    QButtonGroup*       m_pMemorySizeButtonGroup;

    QCheckBox*          m_pOnceCheckBox;
    SymbolTableModel*   m_pSymbolTableModel;
};

#endif // ADDBREAKPOINTDIALOG_H
