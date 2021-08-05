#include "rundialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QSettings>
#include <QCloseEvent>
#include <QFileDialog>
#include <QPushButton>

#include "../models/session.h"
#include "quicklayout.h"

RunDialog::RunDialog(QWidget *parent, Session* pSession) :
    QDialog(parent),
    m_pSession(pSession)
{
    this->setObjectName("RunDialog");
    this->setWindowTitle(tr("Run Hatari"));

    // Bottom OK/Cancel buttons
    QPushButton* pOkButton = new QPushButton("&OK", this);
    pOkButton->setDefault(true);
    QPushButton* pCancelButton = new QPushButton("&Cancel", this);
    QHBoxLayout* pHLayout = new QHBoxLayout(this);
    pHLayout->addStretch(20);
    pHLayout->addWidget(pOkButton);
    pHLayout->addWidget(pCancelButton);
    pHLayout->addStretch(20);
    QWidget* pButtonContainer = new QWidget(this);
    pButtonContainer->setLayout(pHLayout);

    // Options grid box
    QGroupBox* gridGroupBox = new QGroupBox(tr("Launch options"));
    QGridLayout *gridLayout = new QGridLayout;

    QPushButton* pExeButton = new QPushButton(tr("Browse..."), this);
    QPushButton* pWDButton = new QPushButton(tr("Browse..."), this);

    m_pExecutableTextEdit = new QLineEdit("hatari", this);
    m_pArgsTextEdit = new QLineEdit("", this);
    m_pWorkingDirectoryTextEdit = new QLineEdit("", this);

    gridLayout->addWidget(new QLabel(tr("Executable:"), this), 0, 0);
    gridLayout->addWidget(m_pExecutableTextEdit, 0, 2);
    gridLayout->addWidget(pExeButton, 0, 4);

    gridLayout->addWidget(new QLabel("Arguments:", this), 1, 0);
    gridLayout->addWidget(m_pArgsTextEdit, 1, 2);

    gridLayout->addWidget(new QLabel(tr("Working Directory:"), this), 2, 0);
    gridLayout->addWidget(m_pWorkingDirectoryTextEdit, 2, 2);
    gridLayout->addWidget(pWDButton, 2, 4);

    gridLayout->setColumnStretch(2, 20);
    gridGroupBox->setLayout(gridLayout);

    // Overall layout (options at top, buttons at bottom)
    QVBoxLayout* pLayout = new QVBoxLayout(this);
    pLayout->addWidget(gridGroupBox);
    pLayout->addWidget(pButtonContainer);

    connect(pExeButton, &QPushButton::clicked, this, &RunDialog::exeClicked);
    connect(pWDButton, &QPushButton::clicked, this, &RunDialog::workingDirectoryClicked);
    connect(pOkButton, &QPushButton::clicked, this, &RunDialog::okClicked);
    connect(pOkButton, &QPushButton::clicked, this, &RunDialog::accept);
    connect(pCancelButton, &QPushButton::clicked, this, &RunDialog::reject);
    loadSettings();
    this->setLayout(pLayout);
}

RunDialog::~RunDialog()
{

}

void RunDialog::loadSettings()
{
    QSettings settings;
    settings.beginGroup("RunDialog");

    restoreGeometry(settings.value("geometry").toByteArray());
    m_pExecutableTextEdit->setText(settings.value("exe", QVariant("hatari")).toString());
    m_pArgsTextEdit->setText(settings.value("args", QVariant("")).toString());
    m_pWorkingDirectoryTextEdit->setText(settings.value("workingDirectory", QVariant("")).toString());

    settings.endGroup();
}

void RunDialog::saveSettings()
{
    QSettings settings;
    settings.beginGroup("RunDialog");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("exe", m_pExecutableTextEdit->text());
    settings.setValue("args", m_pArgsTextEdit->text());
    settings.setValue("workingDirectory", m_pWorkingDirectoryTextEdit->text());
    settings.endGroup();
}

void RunDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
}

void RunDialog::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void RunDialog::okClicked()
{
    QProcess proc;
    QStringList args = m_pArgsTextEdit->text().split(" ");

    proc.setProgram(m_pExecutableTextEdit->text());
    proc.setArguments(args);
    proc.setWorkingDirectory(m_pWorkingDirectoryTextEdit->text());
    proc.startDetached();

    saveSettings();
}

void RunDialog::exeClicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
          tr("Choose Hatari executable"));
    if (filename.size() != 0)
        m_pExecutableTextEdit->setText(filename);
    saveSettings();
}

void RunDialog::workingDirectoryClicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);

    QStringList fileNames;
    if (dialog.exec())
    {
        fileNames = dialog.selectedFiles();
        if (fileNames.length() > 0)
            m_pWorkingDirectoryTextEdit->setText(fileNames[0]);
        saveSettings();
    }
}

