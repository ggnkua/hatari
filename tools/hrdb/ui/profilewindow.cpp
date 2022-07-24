#include "profilewindow.h"

#include <iostream>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QMenu>

#include "../transport/dispatcher.h"
#include "../models/targetmodel.h"
#include "../models/session.h"
#include "../models/profiledata.h"
#include "quicklayout.h"

//-----------------------------------------------------------------------------
//      Sorting comparators
//-----------------------------------------------------------------------------
bool CompCyclesAsc(ProfileTableModel::Entry& m1, ProfileTableModel::Entry& m2)
{
    return m1.cycleCount < m2.cycleCount;
}
bool CompCyclesDesc(ProfileTableModel::Entry& m1, ProfileTableModel::Entry& m2)
{
    return m1.cycleCount > m2.cycleCount;
}

bool CompCountAsc(ProfileTableModel::Entry& m1, ProfileTableModel::Entry& m2)
{
    return m1.instructionCount < m2.instructionCount;
}
bool CompCountDesc(ProfileTableModel::Entry& m1, ProfileTableModel::Entry& m2)
{
    return m1.instructionCount > m2.instructionCount;
}

bool CompAddressAsc(ProfileTableModel::Entry& m1, ProfileTableModel::Entry& m2)
{
    return m1.text < m2.text;
}
bool CompAddressDesc(ProfileTableModel::Entry& m1, ProfileTableModel::Entry& m2)
{
    return m1.text > m2.text;
}

//-----------------------------------------------------------------------------
ProfileTableModel::ProfileTableModel(QObject *parent, TargetModel *pTargetModel, Dispatcher* pDispatcher) :
    QAbstractTableModel(parent),
    m_pTargetModel(pTargetModel),
    m_pDispatcher(pDispatcher),
    m_sortColumn(kColCycles),
    m_sortOrder(Qt::DescendingOrder),
    m_grouping(kGroupingSymbol)
{
    connect(m_pTargetModel, &TargetModel::profileChangedSignal,     this, &ProfileTableModel::profileChangedSlot);
    connect(m_pTargetModel, &TargetModel::symbolTableChangedSignal, this, &ProfileTableModel::symbolChangedSlot);
}

//-----------------------------------------------------------------------------
int ProfileTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return entries.size();
}

//-----------------------------------------------------------------------------
int ProfileTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return kColCount;
}

//-----------------------------------------------------------------------------
QVariant ProfileTableModel::data(const QModelIndex &index, int role) const
{
    int32_t row = index.row();
    const Entry& ent = entries[row];

    if (role == Qt::DisplayRole)
    {
        if (index.column() == kColAddress)
            return ent.text;
        else if (index.column() == kColInstructionCount)
            return ent.instructionCount;
        else if (index.column() == kColCycles)
            return QVariant(static_cast<qlonglong>(ent.cycleCount));
    }
    if (role == Qt::TextAlignmentRole)
    {
        if (index.column() == kColAddress)
            return Qt::AlignLeft;
        return Qt::AlignRight;

    }
    return QVariant(); // invalid item
}

//-----------------------------------------------------------------------------
QVariant ProfileTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            switch (section)
            {
            case kColAddress:           return QString(tr("Name"));
            case kColInstructionCount:  return QString(tr("Instructions"));
            case kColCycles:            return QString(tr("Cycles"));
            }
        }
        if (role == Qt::TextAlignmentRole)
        {
            if (section == kColAddress)
                return Qt::AlignLeft;
            return Qt::AlignRight;
        }
    }
    return QVariant();
}

//-----------------------------------------------------------------------------
void ProfileTableModel::sort(int column, Qt::SortOrder order)
{
    switch (column)
    {
    case kColCycles:
        std::sort(entries.begin(), entries.end(), order == Qt::SortOrder::AscendingOrder ? CompCyclesAsc : CompCyclesDesc);
        populateFromEntries();
        break;
    case kColInstructionCount:
        std::sort(entries.begin(), entries.end(), order == Qt::SortOrder::AscendingOrder ? CompCyclesAsc : CompCyclesDesc);
        populateFromEntries();
        break;
    case kColAddress:
        std::sort(entries.begin(), entries.end(), order == Qt::SortOrder::AscendingOrder ? CompAddressAsc : CompAddressDesc);
        populateFromEntries();
        break;
    }
    m_sortColumn = column;
    m_sortOrder = order;
}

//-----------------------------------------------------------------------------
void ProfileTableModel::profileChangedSlot()
{
    rebuildEntries();
}

//-----------------------------------------------------------------------------
void ProfileTableModel::symbolChangedSlot()
{
    rebuildEntries();
}

//-----------------------------------------------------------------------------
void ProfileTableModel::rebuildEntries()
{
    map.clear();
    QString text;
    const ProfileData& data = m_pTargetModel->GetRawProfileData();
    const SymbolTable& symbols = m_pTargetModel->GetSymbolTable();
    Symbol result;

    uint32_t bits = 8;
    uint32_t mask = 0xffffffff << bits;
    uint32_t rest = 0xffffffff ^ mask;

    // This converts the entry to a label or a memory block
    for (const ProfileData::Pair ent : data.m_entries)
    {
        uint32_t addr = 0;
        QString label;

        if (m_grouping == kGroupingSymbol)
        {
            if (!symbols.FindLowerOrEqual(ent.first, result))
                continue;

            addr = result.address;
            label = QString::fromStdString(result.name);
        }
        else if (m_grouping == kGroupingAddress256)
        {
            addr = ent.first & mask;
            label = QString::asprintf("$%08x-$%08x", addr, addr + rest);
        }

        QMap<uint32_t, Entry>::iterator it = map.find(addr);
        if (it == map.end())
        {
            Entry entry;
            entry.address = addr;
            entry.text = label;
            entry.instructionCount = ent.second.count;
            entry.cycleCount = ent.second.cycles;
            map.insert(addr, entry);
        }
        else {
            it->instructionCount += ent.second.count;
            it->cycleCount += ent.second.cycles;
        }
    }
    entries.clear();
    for (auto it : map)
    {
        entries.push_back(it);
    }

    sort(m_sortColumn, m_sortOrder);
    populateFromEntries();
}

//-----------------------------------------------------------------------------
void ProfileTableModel::populateFromEntries()
{
    // This is rubbish and will lose cursor position etc
    emit beginResetModel();
    emit endResetModel();
}

//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ProfileTableView::ProfileTableView(QWidget* parent, ProfileTableModel* pModel, Session* pSession) :
    QTableView(parent),
    m_pTableModel(pModel),
    m_pSession(pSession),
    m_rightClickRow(-1),
    m_showAddressActions(pSession)
{
    // This table gets the focus from the parent docking widget
    setFocus();
}

//-----------------------------------------------------------------------------
ProfileTableView::~ProfileTableView()
{

}

//-----------------------------------------------------------------------------
void ProfileTableView::contextMenuEvent(QContextMenuEvent *event)
{
    // Right click menus are instantiated on demand, so we can
    // dynamically add to them
    QMenu menu(this);

    // Add the default actions
    QMenu* pAddressMenu = nullptr;
    int rowIdx = this->rowAt(event->y());
    if (rowIdx >= 0)
    {
        const ProfileTableModel::Entry& ent = m_pTableModel->GetEntry(rowIdx);

        pAddressMenu = new QMenu("", &menu);
        pAddressMenu->setTitle(QString::asprintf("Address $%08x", ent.address));

        m_showAddressActions.addActionsToMenu(pAddressMenu);
        m_showAddressActions.setAddress(ent.address);
        menu.addMenu(pAddressMenu);

        // Run it
        menu.exec(event->globalPos());
    }
}

//-----------------------------------------------------------------------------
void ProfileTableView::mouseDoubleClickEvent(QMouseEvent *event)
{
    int rowIdx = this->rowAt(event->y());
    if (rowIdx >= 0)
    {
        const ProfileTableModel::Entry& ent = m_pTableModel->GetEntry(rowIdx);
        emit m_pSession->addressRequested(Session::kDisasmWindow, 0, ent.address);
    }
}

//-----------------------------------------------------------------------------
ProfileWindow::ProfileWindow(QWidget *parent, Session* pSession) :
    QDockWidget(parent),
    m_pSession(pSession),
    m_pTargetModel(pSession->m_pTargetModel),
    m_pDispatcher(pSession->m_pDispatcher)
{
    this->setWindowTitle("Profile");
    setObjectName("Profile");

    // Layouts
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    QHBoxLayout* pTopLayout = new QHBoxLayout;
    auto pMainRegion = new QWidget(this);   // whole panel
    auto pTopRegion = new QWidget(this);      // top buttons/edits

    m_pStartStopButton = new QPushButton("Start", this);
    m_pClearButton = new QPushButton("Clear", this);
    m_pTableModel = new ProfileTableModel(this, m_pTargetModel, m_pDispatcher);
    m_pTableView = new ProfileTableView(this, m_pTableModel, m_pSession);
    m_pTableView->setModel(m_pTableModel);
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    m_pTableView->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    m_pTableView->verticalHeader()->hide();
    m_pTableView->verticalHeader()->setTextElideMode(Qt::TextElideMode::ElideRight);
    m_pTableView->setWordWrap(false);
    m_pTableView->setSortingEnabled(true);
    //m_pTableView->setShowGrid(false);
    m_pTableView->sortByColumn(ProfileTableModel::kColCycles, Qt::SortOrder::DescendingOrder);

    m_pTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    m_pTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    pTopLayout->addWidget(m_pStartStopButton);
    pTopLayout->addWidget(m_pClearButton);
    pMainLayout->addWidget(pTopRegion);
    pMainLayout->addWidget(m_pTableView);

    SetMargins(pTopLayout);
    SetMargins(pMainLayout);

    pTopRegion->setLayout(pTopLayout);
    pMainRegion->setLayout(pMainLayout);
    setWidget(pMainRegion);

    loadSettings();

    connect(m_pTargetModel,     &TargetModel::connectChangedSignal,     this, &ProfileWindow::connectChangedSlot);
    connect(m_pTargetModel,     &TargetModel::startStopChangedSignal,   this, &ProfileWindow::startStopChangedSlot);
    connect(m_pTargetModel,     &TargetModel::profileChangedSignal,     this, &ProfileWindow::profileChangedSlot);
    connect(m_pSession,         &Session::settingsChanged,              this, &ProfileWindow::settingsChangedSlot);

    connect(m_pStartStopButton, &QAbstractButton::clicked,              this, &ProfileWindow::startStopClicked);
    connect(m_pClearButton,     &QAbstractButton::clicked,              this, &ProfileWindow::resetClicked);

    // Refresh enable state
    connectChangedSlot();

    // Refresh font
    settingsChangedSlot();
}

ProfileWindow::~ProfileWindow()
{
}

void ProfileWindow::keyFocus()
{
    activateWindow();
}

void ProfileWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("Profile");

    restoreGeometry(settings.value("geometry").toByteArray());
    settings.endGroup();
}

void ProfileWindow::saveSettings()
{
    QSettings settings;
    settings.beginGroup("Profile");

    settings.setValue("geometry", saveGeometry());
    settings.endGroup();
}

void ProfileWindow::connectChangedSlot()
{
    bool enable = m_pTargetModel->IsConnected() && !m_pTargetModel->IsRunning();
    m_pStartStopButton->setEnabled(enable);
    m_pClearButton->setEnabled(enable);
}

void ProfileWindow::startStopChangedSlot()
{
    bool enable = m_pTargetModel->IsConnected() && !m_pTargetModel->IsRunning();
    m_pStartStopButton->setEnabled(enable);
    m_pClearButton->setEnabled(enable);
}

void ProfileWindow::profileChangedSlot()
{
    if (m_pTargetModel->IsProfileEnabled())
    {
        m_pStartStopButton->setText("Stop");
    }
    else {
        m_pStartStopButton->setText("Start");
    }
}

void ProfileWindow::settingsChangedSlot()
{
    QFontMetrics fm(m_pSession->GetSettings().m_font);

    // Down the side
    m_pTableView->setFont(m_pSession->GetSettings().m_font);
    m_pTableView->verticalHeader()->setDefaultSectionSize(fm.height());
}

void ProfileWindow::startStopClicked()
{
    if (!m_pTargetModel->IsConnected())
        return;

    m_pDispatcher->SetProfileEnable(m_pTargetModel->IsProfileEnabled());
}

void ProfileWindow::resetClicked()
{
    m_pTargetModel->ProfileReset();
}
