#include "consolewindow.h"

#include <iostream>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QSettings>

#include "../transport/dispatcher.h"
#include "../models/targetmodel.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ConsoleWindow::ConsoleWindow(QWidget *parent, TargetModel* pTargetModel, Dispatcher* pDispatcher) :
    QDockWidget(parent),
    m_pTargetModel(pTargetModel),
    m_pDispatcher(pDispatcher)
{
    this->setWindowTitle("Console");
    setObjectName("Console");

    m_pLineEdit = new QLineEdit(this);

    // Layouts
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    QHBoxLayout* pTopLayout = new QHBoxLayout;
    auto pMainRegion = new QWidget(this);   // whole panel
    auto pTopRegion = new QWidget(this);      // top buttons/edits

    pTopLayout->addWidget(m_pLineEdit);
    pMainLayout->addWidget(pTopRegion);

    pTopRegion->setLayout(pTopLayout);
    pMainRegion->setLayout(pMainLayout);
    setWidget(pMainRegion);

    loadSettings();

    // Connect text entry
    connect(m_pLineEdit, &QLineEdit::returnPressed,         this, &ConsoleWindow::textEditChangedSlot);
}

void ConsoleWindow::keyFocus()
{
    activateWindow();
    m_pLineEdit->setFocus();
}

void ConsoleWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("Console");

    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void ConsoleWindow::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Console");

    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void ConsoleWindow::textEditChangedSlot()
{
    if (m_pTargetModel->IsConnected() && !m_pTargetModel->IsRunning())
    {
        QString string = "console ";
        string += m_pLineEdit->text();
        m_pDispatcher->SendCommandPacket(string.toStdString().c_str());
    }
    m_pLineEdit->clear();
}
