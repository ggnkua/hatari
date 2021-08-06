#ifndef RUNDIALOG_H
#define RUNDIALOG_H

#include <QDialog>

class QLineEdit;
class TargetModel;
class Dispatcher;
class Session;

class RunDialog : public QDialog
{
   Q_OBJECT
public:
    RunDialog(QWidget* parent, Session* pSession);
    virtual ~RunDialog();

    // Settings
    void loadSettings();
    void saveSettings();

protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void okClicked();
    void exeClicked();
    void workingDirectoryClicked();

private:
    QLineEdit*      m_pExecutableTextEdit;
    QLineEdit*      m_pArgsTextEdit;
    QLineEdit*      m_pWorkingDirectoryTextEdit;

    TargetModel*    m_pTargetModel;
    Dispatcher*     m_pDispatcher;
    Session*        m_pSession;
};

#endif // RUNDIALOG_H
