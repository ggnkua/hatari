#include "disasmwidget.h"

#include <iostream>
#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QShortcut>
#include <QFontDatabase>
#include <QMenu>
#include <QContextMenuEvent>
#include <QCheckBox>
#include <QCompleter>

#include "dispatcher.h"
#include "targetmodel.h"
#include "stringparsers.h"
#include "symboltablemodel.h"
#include "memory.h"

//-----------------------------------------------------------------------------
DisasmTableModel::DisasmTableModel(QObject *parent, TargetModel *pTargetModel, Dispatcher* pDispatcher, int windowIndex):
    QAbstractTableModel(parent),
    m_pTargetModel(pTargetModel),
    m_pDispatcher(pDispatcher),
    m_memory(0, 0),
    m_rowCount(25),
    m_requestedAddress(0),
    m_logicalAddr(0),
    m_requestId(0),
    m_bFollowPC(true),
    m_windowIndex(windowIndex)
{
    m_memSlot = (MemorySlot)(windowIndex + MemorySlot::kDisasm0);

    m_breakpointPixmap   = QPixmap(":/images/breakpoint10.png");
    m_breakpointPcPixmap = QPixmap(":/images/pcbreakpoint10.png");
    m_pcPixmap           = QPixmap(":/images/pc10.png");

    connect(m_pTargetModel, &TargetModel::startStopChangedSignal, this, &DisasmTableModel::startStopChangedSlot);
    connect(m_pTargetModel, &TargetModel::memoryChangedSignal, this, &DisasmTableModel::memoryChangedSlot);
    connect(m_pTargetModel, &TargetModel::breakpointsChangedSignal, this, &DisasmTableModel::breakpointsChangedSlot);
    connect(m_pTargetModel, &TargetModel::symbolTableChangedSignal, this, &DisasmTableModel::symbolTableChangedSlot);
    connect(m_pTargetModel, &TargetModel::connectChangedSignal, this, &DisasmTableModel::connectChangedSlot);
    connect(m_pTargetModel, &TargetModel::registersChangedSignal, this, &DisasmTableModel::CalcEAs);
    connect(m_pTargetModel, &TargetModel::otherMemoryChanged,       this, &DisasmTableModel::otherMemoryChangedSlot);
}

int DisasmTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // Find how many pixels a row takes
    return std::max(m_rowCount, 1);
    //return m_disasm.lines.size();
}

int DisasmTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return kColCount;
}

QVariant DisasmTableModel::data(const QModelIndex &index, int role) const
{
    // Always empty if we have no disassembly ready
    uint32_t row = (uint32_t)index.row();
    if (row >= m_disasm.lines.size())
        return QVariant();

    const Disassembler::line& line = m_disasm.lines[row];
    if (role == Qt::DisplayRole)
    {
        if (index.column() == kColSymbol)
        {
            uint32_t addr = line.address;
            Symbol sym;
            if (row == 0)
            {
                // show symbol + offset if necessary for the top line
                if (m_pTargetModel->GetSymbolTable().FindLowerOrEqual(addr, sym))
                {
                    if (addr == sym.address)
                        return QString::fromStdString(sym.name) + ":";
                    else {
                        return QString::asprintf("%s+$%x:", sym.name.c_str(), addr - sym.address);
                    }
                }
            }
            else {
                if (m_pTargetModel->GetSymbolTable().Find(addr, sym))
                    return QString::fromStdString(sym.name) + ":";
            }
        }
        else if (index.column() == kColAddress)
        {
            uint32_t addr = line.address;
            QString addrStr = QString::asprintf("%08x", addr);
            return addrStr;
        }
        else if (index.column() == kColBreakpoint)
        {
            //uint32_t pc = m_pTargetModel->GetPC();
            //if (pc == line.address)
            //    return QString(">");
        }
        else if (index.column() == kColDisasm)
        {
            QString str;
            QTextStream ref(&str);
            Disassembler::print(line.inst,
                    line.address, ref);
            return str;
        }
        else if (index.column() == kColHex)
        {
            QString str;
            for (uint32_t i = 0; i < line.inst.byte_count; ++i)
                str += QString::asprintf("%02x", line.mem[i]);
            return str;
        }
        else if (index.column() == kColComments)
        {
            QString str;
            QTextStream ref(&str);
            Registers regs = m_pTargetModel->GetRegs();
            printEA(line.inst.op0, regs, line.address, ref);
            if (str.size() != 0)
                ref << "  ";
            printEA(line.inst.op1, regs, line.address, ref);
            return str;
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (row >= m_disasm.lines.size())
            return QVariant();

        if (index.column() == kColBreakpoint)
        {
            bool isPc = line.address == m_pTargetModel->GetPC();
            for (size_t i = 0; i < m_breakpoints.m_breakpoints.size(); ++i)
            {
                if (m_breakpoints.m_breakpoints[i].m_pcHack == line.address)
                {
                    return isPc? m_breakpointPcPixmap : m_breakpointPixmap;
                }
            }
            if (isPc)
                return m_pcPixmap;
        }
    }
    return QVariant(); // invalid item
}

QVariant DisasmTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal)
    {
        if (role == Qt::DisplayRole)
        {
            switch (section)
            {
            case kColSymbol: return QString("Symbol");
            case kColAddress: return QString("Address");
            case kColBreakpoint: return QString("");    // Too narrow
            case kColHex: return QString("Hex");    // Too narrow
            case kColDisasm: return QString("Disassembly");
            case kColComments: return QString("");
            }
        }
        if (role == Qt::TextAlignmentRole)
        {
            return Qt::AlignLeft;
        }
    }
    return QVariant();
}

void DisasmTableModel::SetAddress(uint32_t addr)
{
    // Request memory for this region and save the address.
    m_logicalAddr = addr;
    RequestMemory();
    emit addressChanged(m_logicalAddr);
}

// Request enough memory based on m_rowCount and m_logicalAddr
void DisasmTableModel::RequestMemory()
{
    uint32_t addr = m_logicalAddr;
    uint32_t lowAddr = (addr > 100) ? addr - 100 : 0;
    uint32_t size = ((m_rowCount * 10) + 100);
    if (m_pTargetModel->IsConnected())
    {
        m_requestId = m_pDispatcher->RequestMemory(m_memSlot, lowAddr, size);
    }
}

bool DisasmTableModel::GetEA(uint32_t row, int operandIndex, uint32_t &addr)
{
    if (row >= m_opAddresses.size())
        return false;

    if (operandIndex >= 2)
        return false;

    addr = m_opAddresses[row].address[operandIndex];
    return m_opAddresses[row].valid[operandIndex];
}

bool DisasmTableModel::GetInstructionAddr(int row, uint32_t &addr) const
{
    if (row >= m_disasm.lines.size())
        return false;
    addr = m_disasm.lines[row].address;
    return true;
}

bool DisasmTableModel::SetAddress(std::string addrStr)
{
    uint32_t addr;
    if (!StringParsers::ParseExpression(addrStr.c_str(), addr,
                                       m_pTargetModel->GetSymbolTable(), m_pTargetModel->GetRegs()))
    {
        return false;
    }

    SetAddress(addr);
    return true;
}

void DisasmTableModel::MoveUp()
{
    if (m_requestId != 0)
        return; // not up to date

    // Disassemble upwards to see if something sensible appears.
    // Stop at the first valid instruction opcode.
    // Max instruction size is 10 bytes.
    if (m_requestId == 0)
    {
        for (uint32_t off = 2; off <= 10; off += 2)
        {
            uint32_t targetAddr = m_logicalAddr - off;
            // Check valid memory
            if (m_memory.GetAddress() > targetAddr ||
                m_memory.GetAddress() + m_memory.GetSize() <= targetAddr)
            {
                continue;
            }
            // Get memory buffer for this range
            uint32_t offset = targetAddr - m_memory.GetAddress();
            uint32_t size = m_memory.GetSize() - offset;
            buffer_reader disasmBuf(m_memory.GetData() + offset, size);
            instruction inst;
            if (Disassembler::decode_inst(disasmBuf, inst) == 0)
            {
                if (inst.opcode != Opcode::NONE)
                {
                    SetAddress(targetAddr);
                    return;
                }
            }
        }
    }

    if (m_logicalAddr > 2)
        SetAddress(m_logicalAddr - 2);
    else
        SetAddress(0);
}

void DisasmTableModel::MoveDown()
{
    if (m_requestId != 0)
        return; // not up to date

    if (m_disasm.lines.size() > 0)
    {
        // Find our current line in disassembly
        for (size_t i = 0; i < m_disasm.lines.size(); ++i)
        {
            if (m_disasm.lines[i].address == m_logicalAddr)
            {
                // This will go off and request the memory itself
                SetAddress(m_disasm.lines[i].GetEnd());
                return;
            }
        }
        // Default to line 1
        SetAddress(m_disasm.lines[0].GetEnd());
    }
}

void DisasmTableModel::PageUp()
{
    if (m_requestId != 0)
        return; // not up to date

    // TODO we should actually disassemble upwards to see if something sensible appears
    if (m_logicalAddr > 20)
        SetAddress(m_logicalAddr - 20);
    else
        SetAddress(0);
}

void DisasmTableModel::PageDown()
{
    if (m_requestId != 0)
        return; // not up to date

    if (m_disasm.lines.size() > 0)
    {
        // This will go off and request the memory itself
        SetAddress(m_disasm.lines.back().GetEnd());
    }
}

void DisasmTableModel::RunToRow(int row)
{
    if (row >= 0 && row < m_disasm.lines.size())
    {
        Disassembler::line& line = m_disasm.lines[row];
        m_pDispatcher->RunToPC(line.address);
    }
}

void DisasmTableModel::startStopChangedSlot()
{
    // Request new memory for the view
    if (!m_pTargetModel->IsRunning())
    {
        // Decide what to request.
        if (m_bFollowPC)
        {
            // Update to PC position
            SetAddress(m_pTargetModel->GetPC());
        }
        else
        {
            // Just request what we had already.
            RequestMemory();
        }
    }
}

void DisasmTableModel::connectChangedSlot()
{
    if (!m_pTargetModel->IsConnected())
    {
        m_disasm.lines.clear();
        m_rowCount = 1;
        m_memory.Clear();
    }
}

void DisasmTableModel::memoryChangedSlot(int memorySlot, uint64_t commandId)
{
    if (memorySlot != m_memSlot)
        return;

    // Only update for the last request we added
    if (commandId != m_requestId)
        return;

    const Memory* pMemOrig = m_pTargetModel->GetMemory(m_memSlot);
    if (!pMemOrig)
        return;

    if (m_logicalAddr == kInvalid)
    {
        m_logicalAddr = pMemOrig->GetAddress();
        emit addressChanged(m_logicalAddr);
    }

    // Cache it for later
    m_memory = *pMemOrig;
    CalcDisasm();

    // Clear the request, to say we are up to date
    m_requestId = 0;
    emit dataChanged(this->createIndex(0, 0), this->createIndex(m_rowCount - 1, kColCount));
}

void DisasmTableModel::breakpointsChangedSlot(uint64_t commandId)
{
    // Cache data
    m_breakpoints = m_pTargetModel->GetBreakpoints();
    emit dataChanged(this->createIndex(0, 0), this->createIndex(m_rowCount - 1, kColCount));
}

void DisasmTableModel::symbolTableChangedSlot(uint64_t commandId)
{
    // Don't copy here, just force a re-read
    emit dataChanged(this->createIndex(0, 0), this->createIndex(m_rowCount - 1, kColCount));
}

void DisasmTableModel::otherMemoryChangedSlot(uint32_t address, uint32_t size)
{
    // Do a re-request if our memory is touched
    uint32_t ourAddr = m_logicalAddr;
    uint32_t ourSize = ((m_rowCount * 10) + 100);
    if (Overlaps(ourAddr, ourSize, address, size))
        RequestMemory();
}

void DisasmTableModel::CalcDisasm()
{
    // Make sure the data we get back matches our expectations...
    if (m_logicalAddr < m_memory.GetAddress())
        return;
    if (m_logicalAddr >= m_memory.GetAddress() + m_memory.GetSize())
        return;

    // Work out where we need to start disassembling from
    uint32_t offset = m_logicalAddr - m_memory.GetAddress();
    uint32_t size = m_memory.GetSize() - offset;
    buffer_reader disasmBuf(m_memory.GetData() + offset, size);
    m_disasm.lines.clear();
    Disassembler::decode_buf(disasmBuf, m_disasm, m_logicalAddr, m_rowCount);
    CalcEAs();
}

void DisasmTableModel::CalcEAs()
{
    // Precalc EAs
    m_opAddresses.clear();
    m_opAddresses.resize(m_disasm.lines.size());
    const Registers& regs = m_pTargetModel->GetRegs();
    for (size_t i = 0; i < m_disasm.lines.size(); ++i)
    {
        const instruction& inst = m_disasm.lines[i].inst;
        OpAddresses& addrs = m_opAddresses[i];
        bool useRegs = (i == 0);
        addrs.valid[0] = Disassembler::calc_fixed_ea(inst.op0, useRegs, regs, m_disasm.lines[i].address, addrs.address[0]);
        addrs.valid[1] = Disassembler::calc_fixed_ea(inst.op1, useRegs, regs, m_disasm.lines[i].address, addrs.address[1]);
    }

}

void DisasmTableModel::ToggleBreakpoint(int row)
{
    // set a breakpoint
    if (row >= m_disasm.lines.size())
        return;

    Disassembler::line& line = m_disasm.lines[row];
    uint32_t addr = line.address;
    bool removed = false;

    const Breakpoints& bp = m_pTargetModel->GetBreakpoints();
    for (size_t i = 0; i < bp.m_breakpoints.size(); ++i)
    {
        if (bp.m_breakpoints[i].m_pcHack == addr)
        {
            m_pDispatcher->DeleteBreakpoint(bp.m_breakpoints[i].m_id);
            removed = true;
        }
    }
    if (!removed)
    {
        QString cmd = QString::asprintf("pc = $%x", addr);
        m_pDispatcher->SetBreakpoint(cmd.toStdString().c_str(), false);
    }
}

void DisasmTableModel::NopRow(int row)
{
    if (row >= m_disasm.lines.size())
        return;

    Disassembler::line& line = m_disasm.lines[row];
    uint32_t addr = line.address;

    QString command = QString::asprintf("memset %u %u ", addr, line.inst.byte_count);
    for (int i = 0; i <  line.inst.byte_count /2; ++i)
        command += "4e71";

    m_pDispatcher->SendCommandPacket(command.toStdString().c_str());
}

void DisasmTableModel::SetRowCount(int count)
{
    if (count != m_rowCount)
    {
        emit beginResetModel();
        m_rowCount = count;

        // Do we need more data?
        CalcDisasm();
        if (m_disasm.lines.size() < m_rowCount)
        {
            // We need more memory
            RequestMemory();
        }

        emit endResetModel();
    }
}

void DisasmTableModel::SetFollowPC(bool bFollow)
{
    m_bFollowPC = bFollow;
}


void DisasmTableModel::printEA(const operand& op, const Registers& regs, uint32_t address, QTextStream& ref) const
{
    uint32_t ea;

    // Only do a full analysis if this is the PC and therefore the registers are valid...
    if (Disassembler::calc_fixed_ea(op, address == m_pTargetModel->GetPC(), regs, address, ea))
    {
        ea &= 0xffffff; // mask for ST hardware range
        ref << "$" << QString::asprintf("%x", ea);
        Symbol sym;
        if (m_pTargetModel->GetSymbolTable().FindLowerOrEqual(ea, sym))
        {
            uint32_t offset = ea - sym.address;
            if (offset)
                ref << QString::asprintf(" %s+$%x", sym.name.c_str(), offset);
            else
                ref << " " << QString::fromStdString(sym.name);
        }
    };
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DisasmTableView::DisasmTableView(QWidget* parent, DisasmTableModel* pModel, TargetModel* pTargetModel) :
    QTableView(parent),
    m_pTableModel(pModel),
    m_pTargetModel(pTargetModel),
    m_rightClickMenu(this),
    m_rightClickRow(-1),
    m_rightClickInstructionAddr(0)
{
    // Actions for right-click menu
    m_pRunUntilAction = new QAction(tr("Run to here"), this);
    m_pBreakpointAction = new QAction(tr("Toggle Breakpoint"), this);
    m_pNopAction = new QAction(tr("Replace with NOPs"), this);

    m_pMemViewAddress[0] = new QAction("", this);
    m_pMemViewAddress[1] = new QAction("", this);
    m_pMemViewAddress[2] = new QAction(tr("Show instruction memory"), this);
    m_pDisassembleAddress[0] = new QAction("", this);
    m_pDisassembleAddress[1] = new QAction("", this);

    QMenu* pViewMenu = new QMenu("View", this);
    pViewMenu->addAction(m_pMemViewAddress[2]);
    pViewMenu->addAction(m_pMemViewAddress[0]);
    pViewMenu->addAction(m_pMemViewAddress[1]);
    pViewMenu->addAction(m_pDisassembleAddress[0]);
    pViewMenu->addAction(m_pDisassembleAddress[1]);

    QMenu* pEditMenu = new QMenu("Edit", this);
    pEditMenu->addAction(m_pNopAction); //

    m_rightClickMenu.addAction(m_pRunUntilAction);
    m_rightClickMenu.addAction(m_pBreakpointAction);
    m_rightClickMenu.addMenu(pEditMenu);
    m_rightClickMenu.addMenu(pViewMenu);

    new QShortcut(QKeySequence(tr("F3", "Run to cursor")),        this, SLOT(runToCursor()));
    new QShortcut(QKeySequence(tr("F9", "Toggle breakpoint")),    this, SLOT(toggleBreakpoint()));

    connect(m_pRunUntilAction,       &QAction::triggered,                  this, &DisasmTableView::runToCursorRightClick);
    connect(m_pBreakpointAction,     &QAction::triggered,                  this, &DisasmTableView::toggleBreakpointRightClick);
    connect(m_pNopAction,            &QAction::triggered,                  this, &DisasmTableView::nopRightClick);
    connect(m_pMemViewAddress[0],    &QAction::triggered,                  this, &DisasmTableView::memoryViewAddr0);
    connect(m_pMemViewAddress[1],    &QAction::triggered,                  this, &DisasmTableView::memoryViewAddr1);
    connect(m_pMemViewAddress[2],    &QAction::triggered,                  this, &DisasmTableView::memoryViewAddrInst);
    connect(m_pDisassembleAddress[0],&QAction::triggered,                  this, &DisasmTableView::disasmViewAddr0);
    connect(m_pDisassembleAddress[1],&QAction::triggered,                  this, &DisasmTableView::disasmViewAddr1);

    connect(pTargetModel,        &TargetModel::startStopChangedSignal, this, &DisasmTableView::RecalcRowCount);

    // This table gets the focus from the parent docking widget
    setFocus();
}

void DisasmTableView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    if (!index.isValid())
        return;

    m_rightClickRow = index.row();

    m_rightClickInstructionAddr = 0;
    bool vis = m_pTableModel->GetInstructionAddr(m_rightClickRow, m_rightClickInstructionAddr);
    m_pMemViewAddress[2]->setVisible(vis);
    m_pMemViewAddress[2]->setText(QString::asprintf("Show Instruction Memory ($%x)", m_rightClickInstructionAddr));

    // Set up relevant menu items
    for (uint32_t op = 0; op < 2; ++op)
    {
        if (m_pTableModel->GetEA(m_rightClickRow, op, m_rightClickAddr[op]))
        {
            m_pMemViewAddress[op]->setText(QString::asprintf("Show Memory at $%x", m_rightClickAddr[op]));
            m_pMemViewAddress[op]->setVisible(true);
            m_pDisassembleAddress[op]->setText(QString::asprintf("Disassemble at $%x", m_rightClickAddr[op]));
            m_pDisassembleAddress[op]->setVisible(true);
        }
        else
        {
            m_pMemViewAddress[op]->setVisible(false);
            m_pDisassembleAddress[op]->setVisible(false);
        }
    }

    // Run it
    m_rightClickMenu.exec(event->globalPos());
}

void DisasmTableView::runToCursorRightClick()
{
    m_pTableModel->RunToRow(m_rightClickRow);
    m_rightClickRow = -1;
}

void DisasmTableView::toggleBreakpointRightClick()
{
    m_pTableModel->ToggleBreakpoint(m_rightClickRow);
    m_rightClickRow = -1;
}

void DisasmTableView::nopRightClick()
{
    m_pTableModel->NopRow(m_rightClickRow);
    m_rightClickRow = -1;
}

void DisasmTableView::memoryViewAddrInst()
{
    emit m_pTargetModel->addressRequested(1, true, m_rightClickInstructionAddr);
}

void DisasmTableView::memoryViewAddr0()
{
    emit m_pTargetModel->addressRequested(1, true, m_rightClickAddr[0]);
}

void DisasmTableView::memoryViewAddr1()
{
    emit m_pTargetModel->addressRequested(1, true, m_rightClickAddr[1]);
}

void DisasmTableView::disasmViewAddr0()
{
    emit m_pTargetModel->addressRequested(1, false, m_rightClickAddr[0]);
}

void DisasmTableView::disasmViewAddr1()
{
    emit m_pTargetModel->addressRequested(1, false, m_rightClickAddr[1]);
}

void DisasmTableView::runToCursor()
{
    // How do we get the selected row
    QModelIndex i = this->currentIndex();
    m_pTableModel->RunToRow(i.row());
}

void DisasmTableView::toggleBreakpoint()
{
    // How do we get the selected row
    QModelIndex i = this->currentIndex();
    m_pTableModel->ToggleBreakpoint(i.row());
}

QModelIndex DisasmTableView::moveCursor(QAbstractItemView::CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    QModelIndex i = this->currentIndex();

    // Do the override/refill behaviour if we need to scroll our virtual area
    if (cursorAction == QAbstractItemView::CursorAction::MoveUp &&
        i.row() == 0)
    {
        m_pTableModel->MoveUp();
        return i;
    }
    else if (cursorAction == QAbstractItemView::CursorAction::MoveDown &&
             i.row() >= m_pTableModel->GetRowCount() - 1)
    {
        m_pTableModel->MoveDown();
        return i;
    }
    else if (cursorAction == QAbstractItemView::CursorAction::MovePageUp &&
             i.row() == 0)
    {
        m_pTableModel->PageUp();
        return i;
    }
    else if (cursorAction == QAbstractItemView::CursorAction::MovePageDown &&
             i.row() >= m_pTableModel->GetRowCount() - 1)
    {
        m_pTableModel->PageDown();
        return i;
    }

    return QTableView::moveCursor(cursorAction, modifiers);
}

void DisasmTableView::resizeEvent(QResizeEvent* event)
{
    QTableView::resizeEvent(event);
    RecalcRowCount();
}

void DisasmTableView::RecalcRowCount()
{
    // It seems that viewport is updated without this even being called,
    // which means that on startup, "h" == 0.
    int h = this->viewport()->size().height();
    int rowh = this->rowHeight(0);
    if (rowh != 0)
        m_pTableModel->SetRowCount(h / rowh);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

DisasmWidget::DisasmWidget(QWidget *parent, TargetModel* pTargetModel, Dispatcher* pDispatcher, int windowIndex) :
    QDockWidget(parent),
    m_pTargetModel(pTargetModel),
    m_pDispatcher(pDispatcher),
    m_windowIndex(windowIndex)
{
    // Create model early
    m_pTableModel = new DisasmTableModel(this, pTargetModel, pDispatcher, windowIndex);

    this->setWindowTitle("Disassembly");

    m_pTableView = new DisasmTableView(this, m_pTableModel, m_pTargetModel);
    m_pTableView->setModel(m_pTableModel);

    // Top group box
    m_pLineEdit = new QLineEdit(this);
    m_pFollowPC = new QCheckBox("Follow PC", this);
    m_pFollowPC->setTristate(false);
    m_pFollowPC->setChecked(m_pTableModel->GetFollowPC());

    const QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    m_pTableView->setFont(monoFont);
    QFontMetrics fm(monoFont);

    // This is across the top
    int charWidth = fm.width("W");
    m_pTableView->horizontalHeader()->setMinimumSectionSize(12);
    m_pTableView->setColumnWidth(DisasmTableModel::kColSymbol, charWidth * 15);
    m_pTableView->setColumnWidth(DisasmTableModel::kColAddress, charWidth * 12);      // Mac needs most
    m_pTableView->setColumnWidth(DisasmTableModel::kColBreakpoint, charWidth * 4);
    m_pTableView->setColumnWidth(DisasmTableModel::kColHex, charWidth * 4);
    m_pTableView->setColumnWidth(DisasmTableModel::kColDisasm, charWidth * 30);
    m_pTableView->setColumnWidth(DisasmTableModel::kColComments, 300);

    // Down the side
    m_pTableView->verticalHeader()->hide();
    m_pTableView->verticalHeader()->setDefaultSectionSize(fm.height());

    // We don't allow selection. The active key always happens on row 0
    m_pTableView->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
    m_pTableView->setSelectionMode(QAbstractItemView::SelectionMode::NoSelection);
    m_pTableView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    // Layouts
    QVBoxLayout* pMainLayout = new QVBoxLayout;
    QHBoxLayout* pTopLayout = new QHBoxLayout;
    auto pMainRegion = new QWidget(this);   // whole panel
    auto pTopRegion = new QWidget(this);    // top buttons/edits
    //pMainGroupBox->setFlat(true);

    pTopLayout->addWidget(m_pLineEdit);
    pTopLayout->addWidget(m_pFollowPC);

    pMainLayout->addWidget(pTopRegion);
    pMainLayout->addWidget(m_pTableView);

    pTopRegion->setLayout(pTopLayout);
    pMainRegion->setLayout(pMainLayout);
    setWidget(pMainRegion);

    m_pSymbolTableModel = new SymbolTableModel(this, m_pTargetModel->GetSymbolTable());
    QCompleter* pCompl = new QCompleter(m_pSymbolTableModel, this);
    pCompl->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    m_pLineEdit->setCompleter(pCompl);

    // Listen for start/stop, so we can update our memory request
    connect(m_pTableView,   &QTableView::clicked,                 this, &DisasmWidget::cellClickedSlot);
    connect(m_pTableModel,  &DisasmTableModel::addressChanged,    this, &DisasmWidget::UpdateTextBox);
    connect(m_pLineEdit,    &QLineEdit::returnPressed,            this, &DisasmWidget::returnPressedSlot);
    connect(m_pLineEdit,    &QLineEdit::textEdited,               this, &DisasmWidget::textChangedSlot);
    connect(m_pFollowPC,    &QCheckBox::clicked,                  this, &DisasmWidget::followPCClickedSlot);

    this->resizeEvent(nullptr);
}

void DisasmWidget::requestAddress(int windowIndex, bool isMemory, uint32_t address)
{
    if (isMemory)
        return;

    if (windowIndex != m_windowIndex)
        return;

    m_pTableModel->SetAddress(std::to_string(address));
    m_pTableModel->SetFollowPC(false);
    m_pFollowPC->setChecked(false);
    setVisible(true);
}

void DisasmWidget::cellClickedSlot(const QModelIndex &index)
{
    if (index.column() != DisasmTableModel::kColBreakpoint)
        return;
    m_pTableModel->ToggleBreakpoint(index.row());
}

void DisasmWidget::keyDownPressed()
{
    m_pTableModel->MoveDown();
}
void DisasmWidget::keyUpPressed()
{
    m_pTableModel->MoveUp();
}

void DisasmWidget::keyPageDownPressed()
{
    m_pTableModel->PageDown();
}

void DisasmWidget::keyPageUpPressed()
{
    m_pTableModel->PageUp();
}

void DisasmWidget::returnPressedSlot()
{
    QColor col = m_pTableModel->SetAddress(m_pLineEdit->text().toStdString()) ?
                      Qt::white : Qt::red;
    QPalette pal = m_pLineEdit->palette();
    pal.setColor(QPalette::Base, col);
    m_pLineEdit-> setAutoFillBackground(true);
    m_pLineEdit->setPalette(pal);
}

void DisasmWidget::textChangedSlot()
{
    uint32_t addr;
    QColor col = Qt::green;
    if (!StringParsers::ParseExpression(m_pLineEdit->text().toStdString().c_str(), addr,
                                       m_pTargetModel->GetSymbolTable(), m_pTargetModel->GetRegs()))
    {
        col = Qt::red;
    }

    QPalette pal = m_pLineEdit->palette();
    pal.setColor(QPalette::Base, col);
    m_pLineEdit-> setAutoFillBackground(true);
    m_pLineEdit->setPalette(pal);
}

void DisasmWidget::followPCClickedSlot()
{
    m_pTableModel->SetFollowPC(m_pFollowPC->isChecked());
}

void DisasmWidget::UpdateTextBox()
{
    uint32_t addr = m_pTableModel->GetAddress();
    m_pLineEdit->setText(QString::asprintf("$%x", addr));
}

