#include "mainwindow.h"

#include <iostream>
#include <QtWidgets>
#include <QShortcut>
#include <QFontDatabase>
#include <QToolBar>

#include "../transport/dispatcher.h"
#include "../models/targetmodel.h"
#include "../models/exceptionmask.h"
#include "../hardware/regs_st.h"

#include "disasmwidget.h"
#include "memoryviewwidget.h"
#include "graphicsinspector.h"
#include "breakpointswidget.h"
#include "consolewindow.h"
#include "hardwarewindow.h"
#include "profilewindow.h"
#include "addbreakpointdialog.h"
#include "exceptiondialog.h"
#include "rundialog.h"
#include "quicklayout.h"
#include "prefsdialog.h"
#include "symboltext.h"

static QString CreateNumberTooltip(uint32_t value, uint32_t prevValue)
{
    uint16_t word = value & 0xffff;
    uint16_t byte = value & 0xff;

    QString final;
    QTextStream ref(&final);

    if (value != prevValue)
    {
        ref << QString::asprintf("Previous value: $%x (%d)\n", prevValue, static_cast<int32_t>(prevValue));
        uint32_t delta = value - prevValue;
        ref << QString::asprintf("Difference from previous: $%x (%d)\n", delta, static_cast<int32_t>(delta));
    }

    if (value & 0x80000000)
        ref << QString::asprintf("Long: %u (%d)\n", value, static_cast<int32_t>(value));
    else
        ref << QString::asprintf("Long: %u\n", value);
    if (value & 0x8000)
        ref << QString::asprintf("Word: %u (%d)\n", word, static_cast<int16_t>(word));
    else
        ref << QString::asprintf("Word: %u\n", word);
    if (value & 0x80)
        ref << QString::asprintf("Byte: %u (%d)\n", byte, static_cast<int8_t>(byte));
    else
        ref << QString::asprintf("Byte: %u\n", byte);

    ref << "Binary: ";
    for (int bit = 31; bit >= 0; --bit)
        ref << ((value & (1U << bit)) ? "1" : "0");
    ref << "\n";

    ref << "ASCII \"";
    for (int bit = 3; bit >= 0; --bit)
    {
        unsigned char val = static_cast<unsigned char>(value >> (bit * 8));
        ref << ((val >= 32 && val < 128) ? QString(val) : ".");
    }
    ref << "\"\n";

    return final;
}

static QString CreateSRTooltip(uint32_t srRegValue, uint32_t registerBit)
{
    uint32_t valSet = (srRegValue >> registerBit) & 1;
    return QString::asprintf("%s = %s", Registers::GetSRBitName(registerBit),
                             valSet ? "TRUE" : "False");
}

static QString MakeBracket(QString str)
{
    return QString("(") + str + ")";
}

RegisterWidget::RegisterWidget(QWidget *parent, Session* pSession) :
    QWidget(parent),
    m_showAddressActions(pSession),
    m_pSession(pSession),
    m_pDispatcher(pSession->m_pDispatcher),
    m_pTargetModel(pSession->m_pTargetModel),
    m_tokenUnderMouseIndex(-1)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Listen for target changes
    connect(m_pTargetModel, &TargetModel::startStopChangedSignal,        this, &RegisterWidget::startStopChangedSlot);
    connect(m_pTargetModel, &TargetModel::registersChangedSignal,        this, &RegisterWidget::registersChangedSlot);
    connect(m_pTargetModel, &TargetModel::connectChangedSignal,          this, &RegisterWidget::connectChangedSlot);
    connect(m_pTargetModel, &TargetModel::memoryChangedSignal,           this, &RegisterWidget::memoryChangedSlot);
    connect(m_pTargetModel, &TargetModel::symbolTableChangedSignal,      this, &RegisterWidget::symbolTableChangedSlot);
    connect(m_pTargetModel, &TargetModel::startStopChangedSignalDelayed, this, &RegisterWidget::startStopDelayedSlot);

    connect(m_pSession, &Session::settingsChanged, this, &RegisterWidget::settingsChangedSlot);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setMouseTracking(true);
    UpdateFont();
}

RegisterWidget::~RegisterWidget()
{
}

void RegisterWidget::paintEvent(QPaintEvent * ev)
{
    QWidget::paintEvent(ev);

    QPainter painter(this);
    painter.setFont(m_monoFont);
    QFontMetrics info(painter.fontMetrics());
    const QPalette& pal = this->palette();

    const QBrush& br = pal.background().color();
    painter.fillRect(this->rect(), br);
    painter.setPen(QPen(pal.dark(), hasFocus() ? 6 : 2));
    painter.drawRect(this->rect());

    for (int i = 0; i < m_tokens.size(); ++i)
    {
        Token& tok = m_tokens[i];

        int x = Session::kWidgetBorderX + tok.x * m_charWidth;
        int y = GetPixelFromRow(tok.y);
        int w = info.horizontalAdvance(tok.text);
        int h = m_lineHeight;
        tok.rect.setRect(x, y, w, h);

        if (tok.colour == TokenColour::kNormal)
            painter.setPen(pal.text().color());
        else if (tok.colour == TokenColour::kChanged)
            painter.setPen(Qt::red);
        else if (tok.colour == TokenColour::kInactive)
            painter.setPen(pal.light().color());
        else if (tok.colour == TokenColour::kCode)
            painter.setPen(Qt::darkGreen);

        if (i == m_tokenUnderMouseIndex && tok.type != TokenType::kNone)
        {
            painter.setBrush(pal.highlight());
            painter.setPen(Qt::NoPen);
            painter.drawRect(tok.rect);
            painter.setPen(pal.highlightedText().color());
        }

        painter.drawText(x, m_yAscent + y, tok.text);
    }
}

void RegisterWidget::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->localPos();
    UpdateTokenUnderMouse();
    QWidget::mouseMoveEvent(event);
    update();
}

void RegisterWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (m_tokenUnderMouseIndex == -1)
        return;

    // Right click menus are instantiated on demand, so we can
    // dynamically add to them
    QMenu menu(this);

    // Add the default actions
    QMenu* pAddressMenu = nullptr;
    if (m_tokenUnderMouse.type == TokenType::kRegister)
    {
        m_addressUnderMouse = m_currRegs.Get(m_tokenUnderMouse.subIndex);
        pAddressMenu = new QMenu("", &menu);
        pAddressMenu->setTitle(QString::asprintf("Address $%08x", m_addressUnderMouse));
    }
    else if (m_tokenUnderMouse.type == TokenType::kSymbol)
    {
        m_addressUnderMouse = m_tokenUnderMouse.subIndex;
        pAddressMenu = new QMenu("", &menu);
        pAddressMenu->setTitle(QString::asprintf("Address $%08x", m_addressUnderMouse));
    }

    if (pAddressMenu)
    {
        m_showAddressActions.addActionsToMenu(pAddressMenu);
        m_showAddressActions.setAddress(m_addressUnderMouse);
        menu.addMenu(pAddressMenu);

        // Run it
        menu.exec(event->globalPos());
    }
}

bool RegisterWidget::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip) {

        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
        int index = m_tokenUnderMouseIndex;
        QString text;
        if (index != -1)
            text = GetTooltipText(m_tokens[index]);

        if (text.size() != 0)
            QToolTip::showText(helpEvent->globalPos(), text);
        else
        {
            QToolTip::hideText();
            event->ignore();
        }
        return true;
    }
    else if (event->type() == QEvent::Leave)
    {
        m_tokenUnderMouseIndex = -1;
        update();
    }
    else if (event->type() == QEvent::Enter)
    {
        QEnterEvent* pEnterEvent = static_cast<QEnterEvent* >(event);
        m_mousePos = pEnterEvent->localPos();
        UpdateTokenUnderMouse();
        update();
    }
    return QWidget::event(event);
}

void RegisterWidget::connectChangedSlot()
{
    PopulateRegisters();
}

void RegisterWidget::startStopChangedSlot()
{
    bool isRunning = m_pTargetModel->IsRunning();

    // Update text here
    if (isRunning)
    {
        // Update our previous values
        // TODO: this is not the ideal way to do this, since there
        // are timing issues. In future, parcel up the stopping updates so
        // that widget refreshes happen as one.
        m_prevRegs = m_pTargetModel->GetRegs();

        // Don't populate display here, since it causes colours to flash
    }
    else
    {
        // STOPPED
        // We don't do any requests here; the MainWindow does them.
        // We just listen for the callbacks.
    }
}

void RegisterWidget::startStopDelayedSlot(int running)
{
    if (m_pTargetModel->IsConnected() && running)
    {
        m_tokens.clear();
        m_tokenUnderMouseIndex = -1;
        AddToken(1, 1, tr("Running, Ctrl+R to break..."), TokenType::kNone, 0);
        update();
    }
}

void RegisterWidget::settingsChangedSlot()
{
    UpdateFont();
    PopulateRegisters();
    UpdateTokenUnderMouse();
    update();
}

void RegisterWidget::registersChangedSlot(uint64_t /*commandId*/)
{
    // Update text here
    PopulateRegisters();
}

void RegisterWidget::memoryChangedSlot(int slot, uint64_t /*commandId*/)
{
    if (slot != MemorySlot::kMainPC)
        return;

    // Disassemble the first instruction
    m_disasm.lines.clear();
    const Memory* pMem = m_pTargetModel->GetMemory(MemorySlot::kMainPC);
    if (!pMem)
        return;

    buffer_reader disasmBuf(pMem->GetData(), pMem->GetSize(), pMem->GetAddress());
    Disassembler::decode_buf(disasmBuf, m_disasm, pMem->GetAddress(), 2);
    PopulateRegisters();
}

void RegisterWidget::symbolTableChangedSlot(uint64_t /*commandId*/)
{
    PopulateRegisters();
}

void RegisterWidget::PopulateRegisters()
{
    m_tokens.clear();
    if (!m_pTargetModel->IsConnected())
    {
        m_tokenUnderMouseIndex = -1;
        AddToken(1, 1, tr("Not connected."), TokenType::kNone, 0);
        update();
        return;
    }

    // Precalc EAs
    const Registers& regs = m_pTargetModel->GetRegs();
    // Build up the text area
    m_currRegs = m_pTargetModel->GetRegs();

    // Row 0 -- PC, and symbol if applicable
    int row = 0;

    AddReg32(0, row, Registers::PC, m_prevRegs, m_currRegs);
    QString sym = FindSymbol(GET_REG(m_currRegs, PC) & 0xffffff);
    if (sym.size() != 0)
        AddToken(14, row, MakeBracket(sym), TokenType::kSymbol, GET_REG(m_currRegs, PC));

    row += 2;

    // Row 1 -- instruction and analysis
    if (m_disasm.lines.size() > 0)
    {
        QString disasmText = ">> ";
        QTextStream ref(&disasmText);

        const instruction& inst = m_disasm.lines[0].inst;
        Disassembler::print_terse(inst, m_disasm.lines[0].address, ref);

        bool branchTaken;
        if (DisAnalyse::isBranch(inst, m_currRegs, branchTaken))
        {
            if (branchTaken)
                ref << " [TAKEN]";
            else
                ref << " [NOT TAKEN]";
        }

        int col = AddToken(0, row, disasmText, TokenType::kNone, 0, TokenColour::kCode) + 5;

        // Comments
        if (m_disasm.lines.size() != 0)
        {
            int i = 0;
            const instruction& inst = m_disasm.lines[i].inst;
            bool prevValid = false;
            for (int opIndex = 0; opIndex < 2; ++opIndex)
            {
                const operand& op = opIndex == 0 ? inst.op0 : inst.op1;
                if (op.type != OpType::INVALID)
                {
                    QString eaText = "";
                    QTextStream eaRef(&eaText);

                    // Separate info
                    if (prevValid)
                        col = AddToken(col, row, " | ", TokenType::kNone, 0, TokenColour::kNormal) + 1;

                    // Operand values
                    prevValid = false;
                    switch (op.type)
                    {
                    case OpType::D_DIRECT:
                        eaRef << QString::asprintf("D%d=$%x", op.d_register.reg, regs.GetDReg(op.d_register.reg));
                        col = AddToken(col, row, eaText, TokenType::kRegister, Registers::D0 + op.d_register.reg, TokenColour::kCode) + 1;
                        prevValid = true;
                        break;
                    case OpType::A_DIRECT:
                        eaRef << QString::asprintf("A%d=$%x", op.a_register.reg, regs.GetAReg(op.a_register.reg));
                        col = AddToken(col, row, eaText, TokenType::kRegister, Registers::A0 + op.a_register.reg, TokenColour::kCode) + 1;
                        prevValid = true;
                        break;
                    case OpType::SR:
                        eaRef << QString::asprintf("SR=$%x", regs.Get(Registers::SR));
                        col = AddToken(col, row, eaText, TokenType::kRegister, Registers::SR, TokenColour::kCode) + 1;
                        prevValid = true;
                        break;
                    case OpType::USP:
                        eaRef << QString::asprintf("USP=$%x", regs.Get(Registers::USP));
                        col = AddToken(col, row, eaText, TokenType::kRegister, Registers::USP, TokenColour::kCode) + 1;
                        prevValid = true;
                        break;
                    case OpType::CCR:
                        eaRef << QString::asprintf("CCR=$%x", regs.Get(Registers::SR));
                        // Todo: can we fix this?
                        col = AddToken(col, row, eaText, TokenType::kRegister, Registers::SR, TokenColour::kCode) + 1;
                        prevValid = true;
                        break;
                    default:
                        break;
                    }

                    uint32_t finalEA;
                    bool valid = Disassembler::calc_fixed_ea(op, true, regs, m_disasm.lines[i].address, finalEA);
                    if (valid)
                    {
                        // Show the calculated EA
                        QString eaAddrText = QString::asprintf("$%x", finalEA);
                        QString sym = FindSymbol(finalEA & 0xffffff);
                        if (sym.size() != 0)
                            eaAddrText = eaAddrText + " (" + sym + ")";
                        col = AddToken(col, row, eaAddrText, TokenType::kSymbol, finalEA, TokenColour::kCode) + 1;
                        prevValid = true;
                    }
                }
            }
        }

        // Add EA analysis
//        ref << "   " << eaText;
    }

    row += 2;
    AddReg16(0, row, Registers::SR, m_prevRegs, m_currRegs);
    AddSR(10, row, m_prevRegs, m_currRegs, Registers::SRBits::kTrace1, "T1");
    AddSR(12, row, m_prevRegs, m_currRegs, Registers::SRBits::kSupervisor, "S");
    AddSR(15, row, m_prevRegs, m_currRegs, Registers::SRBits::kIPL2, "2");
    AddSR(16, row, m_prevRegs, m_currRegs, Registers::SRBits::kIPL1, "1");
    AddSR(17, row, m_prevRegs, m_currRegs, Registers::SRBits::kIPL0, "0");

    AddSR(20, row, m_prevRegs, m_currRegs, Registers::SRBits::kX, "X");
    AddSR(21, row, m_prevRegs, m_currRegs, Registers::SRBits::kN, "N");
    AddSR(22, row, m_prevRegs, m_currRegs, Registers::SRBits::kZ, "Z");
    AddSR(23, row, m_prevRegs, m_currRegs, Registers::SRBits::kV, "V");
    AddSR(24, row, m_prevRegs, m_currRegs, Registers::SRBits::kC, "C");
    QString iplLevel = QString::asprintf("IPL=%u", (m_currRegs.m_value[Registers::SR] >> 8 & 0x7));
    AddToken(26, row, iplLevel, TokenType::kNone);
    row += 1;

    uint32_t ex = GET_REG(m_currRegs, EX);
    if (ex != 0)
        AddToken(0, row, QString::asprintf("EXCEPTION: %s", ExceptionMask::GetName(ex)), TokenType::kNone, 0, TokenColour::kChanged);

    // D-regs // A-regs
    row++;
    for (uint32_t reg = 0; reg < 8; ++reg)
    {
        AddReg32(0, row, Registers::D0 + reg, m_prevRegs, m_currRegs); AddReg32(15, row, Registers::A0 + reg, m_prevRegs, m_currRegs); AddSymbol(28, row, m_currRegs.m_value[Registers::A0 + reg]);
        row++;
    }
    AddReg32(14, row, Registers::USP, m_prevRegs, m_currRegs); AddSymbol(28, row, m_currRegs.m_value[Registers::USP]);
    row++;
    AddReg32(14, row, Registers::ISP, m_prevRegs, m_currRegs); AddSymbol(28, row, m_currRegs.m_value[Registers::ISP]);
    row++;

    // Sundry info
    AddToken(0, row, QString::asprintf("VBL: %10u Frame Cycles: %6u", GET_REG(m_currRegs, VBL), GET_REG(m_currRegs, FrameCycles)), TokenType::kNone);
    row++;
    AddToken(0, row, QString::asprintf("HBL: %10u Line Cycles:  %6u", GET_REG(m_currRegs, HBL), GET_REG(m_currRegs, LineCycles)), TokenType::kNone);
    row++;

    // Tokens have moved, so check again
    UpdateTokenUnderMouse();
    update();
}

void RegisterWidget::UpdateFont()
{
    m_monoFont = m_pSession->GetSettings().m_font;
    QFontMetrics info(m_monoFont);
    m_yAscent = info.ascent();
    m_lineHeight = info.lineSpacing();
    m_charWidth = info.horizontalAdvance("0");
}

QString RegisterWidget::FindSymbol(uint32_t addr)
{
    return DescribeSymbol(m_pTargetModel->GetSymbolTable(), addr & 0xffffff);
}

int RegisterWidget::AddToken(int x, int y, QString text, TokenType type, uint32_t subIndex, TokenColour colour)
{
    Token tok;
    tok.x = x;
    tok.y = y;
    tok.text = text;
    tok.type = type;
    tok.subIndex = subIndex;
    tok.colour = colour;
    m_tokens.push_back(tok);
    // Return X position
    return tok.x + text.size();
}

int RegisterWidget::AddReg16(int x, int y, uint32_t regIndex, const Registers& prevRegs, const Registers& regs)
{
    TokenColour highlight = (regs.m_value[regIndex] != prevRegs.m_value[regIndex]) ? kChanged : kNormal;

    QString label = QString::asprintf("%s:",  Registers::s_names[regIndex]);
    QString value = QString::asprintf("%04x", regs.m_value[regIndex]);
    AddToken(x, y, label, TokenType::kRegister, regIndex, TokenColour::kNormal);
    return AddToken(x + label.size() + 1, y, value, TokenType::kRegister, regIndex, highlight);
}

int RegisterWidget::AddReg32(int x, int y, uint32_t regIndex, const Registers& prevRegs, const Registers& regs)
{
    TokenColour highlight = (regs.m_value[regIndex] != prevRegs.m_value[regIndex]) ? kChanged : kNormal;

    QString label = QString::asprintf("%s:",  Registers::s_names[regIndex]);
    QString value = QString::asprintf("%08x", regs.m_value[regIndex]);
    AddToken(x, y, label, TokenType::kRegister, regIndex, TokenColour::kNormal);
    return AddToken(x + label.size() + 1, y, value, TokenType::kRegister, regIndex, highlight);
}

int RegisterWidget::AddSR(int x, int y, const Registers& /*prevRegs*/, const Registers& regs, uint32_t bit, const char* pName)
{
    uint32_t mask = 1U << bit;
    uint32_t valNew = regs.m_value[Registers::SR] & mask;
    TokenColour highlight = valNew ? TokenColour::kNormal : TokenColour::kInactive;
    char text = pName[0];

    return AddToken(x, y, QString(text), TokenType::kStatusRegisterBit, bit, highlight);
}

int RegisterWidget::AddSymbol(int x, int y, uint32_t address)
{
    QString symText = FindSymbol(address & 0xffffff);
    if (!symText.size())
        return x;

    return AddToken(x, y, MakeBracket(symText), TokenType::kSymbol, address);
}

QString RegisterWidget::GetTooltipText(const RegisterWidget::Token& token)
{
    switch (token.type)
    {
    case TokenType::kRegister:
        {
            uint32_t value = m_currRegs.Get(token.subIndex);
            uint32_t prevValue = m_prevRegs.Get(token.subIndex);
            return CreateNumberTooltip(value, prevValue);
        }
    case TokenType::kSymbol:
        return QString::asprintf("Original address: $%08x", token.subIndex);
    case TokenType::kStatusRegisterBit:
         return CreateSRTooltip(m_currRegs.Get(Registers::SR), token.subIndex);
    default:
        break;
    }
    return "";
}

void RegisterWidget::UpdateTokenUnderMouse()
{
    // Update the token
    m_tokenUnderMouseIndex = -1;
    for (int i = 0; i < m_tokens.size(); ++i)
    {
        const Token& tok = m_tokens[i];
        if (tok.rect.contains(m_mousePos))
        {
            m_tokenUnderMouseIndex = i;
            m_tokenUnderMouse = tok;
            break;
        }
    }
}

int RegisterWidget::GetPixelFromRow(int row) const
{
    return Session::kWidgetBorderY + row * m_lineHeight;
}

int RegisterWidget::GetRowFromPixel(int y) const
{
    if (!m_lineHeight)
        return 0;
    return (y - Session::kWidgetBorderY) / m_lineHeight;
}

MainWindow::MainWindow(Session& session, QWidget *parent)
    : QMainWindow(parent),
      m_session(session)
{
    setObjectName("MainWindow");
    m_pTargetModel = m_session.m_pTargetModel;
    m_pDispatcher = m_session.m_pDispatcher;

    // Creation - done in Tab order
    // Register/status window
    m_pRegisterWidget = new RegisterWidget(this, &m_session);

    // Top row of buttons
    m_pRunningSquare = new QWidget(this);
    m_pRunningSquare->setFixedSize(10, 25);
    m_pRunningSquare->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    m_pStartStopButton = new QPushButton("Break", this);
    m_pStepIntoButton = new QPushButton("Step", this);
    m_pStepOverButton = new QPushButton("Next", this);
    m_pRunToButton = new QPushButton("Run Until:", this);
    m_pRunToCombo = new QComboBox(this);
    m_pRunToCombo->insertItem(0, "RTS");
    m_pRunToCombo->insertItem(1, "RTE");
    m_pRunToCombo->insertItem(2, "Next VBL");
    m_pRunToCombo->insertItem(3, "Next HBL");

    for (int i = 0; i < kNumDisasmViews; ++i)
    {
        m_pDisasmWidgets[i] = new DisasmWindow(this, &m_session, i);
        if (i == 0)
            m_pDisasmWidgets[i]->setWindowTitle("Disassembly 1 (Alt+D)");
        else
            m_pDisasmWidgets[i]->setWindowTitle(QString::asprintf("Disassembly %d", i + 1));
    }

    for (int i = 0; i < kNumMemoryViews; ++i)
    {
        m_pMemoryViewWidgets[i] = new MemoryWindow(this, &m_session, i);
        if (i == 0)
            m_pMemoryViewWidgets[i]->setWindowTitle("Memory 1 (Alt+M)");
        else
            m_pMemoryViewWidgets[i]->setWindowTitle(QString::asprintf("Memory %d", i + 1));
    }

    m_pGraphicsInspector = new GraphicsInspectorWidget(this, &m_session);
    m_pGraphicsInspector->setWindowTitle("Graphics Inspector (Alt+G)");
    m_pBreakpointsWidget = new BreakpointsWindow(this, m_pTargetModel, m_pDispatcher);
    m_pBreakpointsWidget->setWindowTitle("Breakpoints (Alt+B)");
    m_pConsoleWindow = new ConsoleWindow(this, &m_session);

    m_pHardwareWindow = new HardwareWindow(this, &m_session);
    m_pHardwareWindow->setWindowTitle("Hardware (Alt+H)");
    m_pProfileWindow = new ProfileWindow(this, &m_session);

    m_pExceptionDialog = new ExceptionDialog(this, m_pTargetModel, m_pDispatcher);
    m_pRunDialog = new RunDialog(this, &m_session);
    m_pPrefsDialog = new ::PrefsDialog(this, &m_session);

    // https://doc.qt.io/qt-5/qtwidgets-layouts-basiclayouts-example.html
    QVBoxLayout *vlayout = new QVBoxLayout;
    QHBoxLayout *hlayout = new QHBoxLayout;
    auto pTopGroupBox = new QWidget(this);
    auto pMainGroupBox = new QGroupBox(this);

    SetMargins(hlayout);
    hlayout->setAlignment(Qt::AlignLeft);
    hlayout->addWidget(m_pRunningSquare);
    hlayout->addWidget(m_pStartStopButton);
    hlayout->addWidget(m_pStepIntoButton);
    hlayout->addWidget(m_pStepOverButton);
    hlayout->addWidget(m_pRunToButton);
    hlayout->addWidget(m_pRunToCombo);
    hlayout->addStretch();
    //hlayout->setAlignment(m_pRunToCombo, Qt::Align);
    pTopGroupBox->setLayout(hlayout);

    SetMargins(vlayout);
    vlayout->addWidget(pTopGroupBox);
    vlayout->addWidget(m_pRegisterWidget);
    vlayout->setAlignment(Qt::Alignment(Qt::AlignTop));
    pMainGroupBox->setFlat(true);
    pMainGroupBox->setLayout(vlayout);

    setCentralWidget(pMainGroupBox);

    for (int i = 0; i < kNumDisasmViews; ++i)
        this->addDockWidget(Qt::BottomDockWidgetArea, m_pDisasmWidgets[i]);
    for (int i = 0; i < kNumMemoryViews; ++i)
        this->addDockWidget(Qt::RightDockWidgetArea, m_pMemoryViewWidgets[i]);

    this->addDockWidget(Qt::LeftDockWidgetArea, m_pGraphicsInspector);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_pBreakpointsWidget);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_pConsoleWindow);
    this->addDockWidget(Qt::RightDockWidgetArea, m_pHardwareWindow);
    this->addDockWidget(Qt::RightDockWidgetArea, m_pProfileWindow);

    loadSettings();

    // Set up menus (reflecting current state)
    createActions();
    createMenus();
    createToolBar();

    // Listen for target changes
    connect(m_pTargetModel, &TargetModel::startStopChangedSignal, this, &MainWindow::startStopChangedSlot);
    connect(m_pTargetModel, &TargetModel::connectChangedSignal,   this, &MainWindow::connectChangedSlot);
    connect(m_pTargetModel, &TargetModel::memoryChangedSignal,    this, &MainWindow::memoryChangedSlot);

    // Wire up buttons to actions
    connect(m_pStartStopButton, &QAbstractButton::clicked, this, &MainWindow::startStopClicked);
    connect(m_pStepIntoButton, &QAbstractButton::clicked, this, &MainWindow::singleStepClicked);
    connect(m_pStepOverButton, &QAbstractButton::clicked, this, &MainWindow::nextClicked);
    connect(m_pRunToButton, &QAbstractButton::clicked, this, &MainWindow::runToClicked);

    // Wire up menu appearance
    connect(m_pWindowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

	// Keyboard shortcuts
    new QShortcut(QKeySequence("Ctrl+R"),         this, SLOT(startStopClicked()));
    new QShortcut(QKeySequence("Esc"),            this, SLOT(breakPressed()));
    new QShortcut(QKeySequence("S"),              this, SLOT(singleStepClicked()));
    new QShortcut(QKeySequence("Ctrl+S"),         this, SLOT(skipPressed()));
    new QShortcut(QKeySequence("N"),              this, SLOT(nextClicked()));
    new QShortcut(QKeySequence("U"),              this, SLOT(runToClicked()));

    // Try initial connect
    ConnectTriggered();

    // Update everything
    connectChangedSlot();
    startStopChangedSlot();

//    m_pDisasmWidget0->keyFocus();
}

MainWindow::~MainWindow()
{
	delete m_pDispatcher;
    delete m_pTargetModel;
}

void MainWindow::connectChangedSlot()
{
    PopulateRunningSquare();
    updateButtonEnable();

    //if (m_pTargetModel->IsConnected())
    //    m_pDispatcher->SendCommandPacket("profile 1");
}

void MainWindow::startStopChangedSlot()
{
    bool isRunning = m_pTargetModel->IsRunning();

    // Update text here
    if (!isRunning)
    {
        // STOPPED
		// TODO this is where all windows should put in requests for data
        // The Main Window does this and other windows feed from it.

        // Do all the "essentials" straight away.
        m_pDispatcher->ReadRegisters();
        m_pDispatcher->ReadMemory(MemorySlot::kMainPC, m_pTargetModel->GetPC(), 10);

        m_pDispatcher->ReadBreakpoints();
        m_pDispatcher->ReadExceptionMask();

        // Basepage makes things much easier
        m_pDispatcher->ReadMemory(MemorySlot::kBasePage, 0, 0x200);
        // Video memory is generally handy
        m_pDispatcher->ReadMemory(MemorySlot::kVideo, Regs::VID_REG_BASE, 0x70);

        // Only re-request symbols if we didn't find any the first time
        if (m_pTargetModel->GetSymbolTable().GetHatariSubTable().Count() == 0)
            m_pDispatcher->ReadSymbols();
    }
    PopulateRunningSquare();
    updateButtonEnable();
}

void MainWindow::memoryChangedSlot(int slot, uint64_t /*commandId*/)
{
    if (slot != MemorySlot::kMainPC)
        return;

    // Disassemble the first instruction
    m_disasm.lines.clear();
    const Memory* pMem = m_pTargetModel->GetMemory(MemorySlot::kMainPC);
    if (!pMem)
        return;

    // Fetch data and decode the next instruction.
    buffer_reader disasmBuf(pMem->GetData(), pMem->GetSize(), pMem->GetAddress());
    Disassembler::decode_buf(disasmBuf, m_disasm, pMem->GetAddress(), 1);
}

void MainWindow::startStopClicked()
{
    if (!m_pTargetModel->IsConnected())
        return;

    if (m_pTargetModel->IsRunning())
        m_pDispatcher->Break();
	else
        m_pDispatcher->Run();
}

void MainWindow::singleStepClicked()
{
    if (!m_pTargetModel->IsConnected())
        return;

    if (m_pTargetModel->IsRunning())
        return;

    m_pDispatcher->Step();
}

void MainWindow::nextClicked()
{
    if (!m_pTargetModel->IsConnected())
        return;

    if (m_pTargetModel->IsRunning())
        return;

    // Work out where the next PC is
    if (m_disasm.lines.size() == 0)
        return;

    // Bug fix: we can't decide on how to step until the available disassembly matches
    // the PC we are stepping from. This slows down stepping a little (since there is
    // a round-trip). In theory we could send the next instruction opcode as part of
    // the "status" notification if we want it to be faster.
    if(m_disasm.lines[0].address != m_pTargetModel->GetPC())
        return;

    const Disassembler::line& nextInst = m_disasm.lines[0];
    // Either "next" or set breakpoint to following instruction
    bool shouldStepOver = DisAnalyse::isSubroutine(nextInst.inst) ||
                          DisAnalyse::isTrap(nextInst.inst) ||
                          DisAnalyse::isBackDbf(nextInst.inst);
    if (shouldStepOver)
    {
        uint32_t next_pc = nextInst.inst.byte_count + nextInst.address;
        m_pDispatcher->RunToPC(next_pc);
    }
    else
    {
        m_pDispatcher->Step();
    }
}

void MainWindow::skipPressed()
{
    if (!m_pTargetModel->IsConnected())
        return;

    if (m_pTargetModel->IsRunning())
        return;

    // Work out where the next PC is
    if (m_disasm.lines.size() == 0)
        return;

    // Bug fix: we can't decide on how to step until the available disassembly matches
    // the PC we are stepping from. This slows down stepping a little (since there is
    // a round-trip). In theory we could send the next instruction opcode as part of
    // the "status" notification if we want it to be faster.
    if(m_disasm.lines[0].address != m_pTargetModel->GetPC())
        return;

    const Disassembler::line& nextInst = m_disasm.lines[0];
    m_pDispatcher->SetRegister(Registers::PC, nextInst.GetEnd());
}

void MainWindow::runToClicked()
{
    if (!m_pTargetModel->IsConnected())
        return;
    if (m_pTargetModel->IsRunning())
        return;

    if (m_pRunToCombo->currentIndex() == 0)
        m_pDispatcher->SetBreakpoint("(pc).w = $4e75", true);      // RTS
    else if (m_pRunToCombo->currentIndex() == 1)
        m_pDispatcher->SetBreakpoint("(pc).w = $4e73", true);      // RTE
    else if (m_pRunToCombo->currentIndex() == 2)
        m_pDispatcher->SetBreakpoint("VBL ! VBL", true);        // VBL
        //m_pDispatcher->SetBreakpoint("pc = ($70).l", true);        // VBL interrupt code
    else if (m_pRunToCombo->currentIndex() == 3)
        m_pDispatcher->SetBreakpoint("HBL ! HBL", true);        // VBL
    else
        return;
    m_pDispatcher->Run();
}

void MainWindow::addBreakpointPressed()
{
    AddBreakpointDialog dialog(this, m_pTargetModel, m_pDispatcher);
    dialog.exec();
}

void MainWindow::breakPressed()
{
    if (!m_pTargetModel->IsConnected())
        return;

    if (m_pTargetModel->IsRunning())
        m_pDispatcher->Break();
}

// Actions
void MainWindow::LaunchTriggered()
{
    m_pRunDialog->setModal(true);
    m_pRunDialog->show();
    // We can't connect here since the dialog hasn't really run yet.
}

void MainWindow::QuickLaunchTriggered()
{
    LaunchHatari(m_session.GetLaunchSettings(), &m_session);
}

void MainWindow::ConnectTriggered()
{
    m_session.Connect();
}

void MainWindow::DisconnectTriggered()
{
    m_session.Disconnect();
}

void MainWindow::WarmResetTriggered()
{
    m_pDispatcher->ResetWarm();
    // TODO: ideally we should clear out the symbol tables here

    // Restart if in break mode
    if (!m_pTargetModel->IsRunning())
        m_pDispatcher->Run();

}

void MainWindow::ExceptionsDialogTriggered()
{
    m_pExceptionDialog->setModal(true);
    m_pExceptionDialog->show();
}

void MainWindow::PrefsDialogTriggered()
{
    m_pPrefsDialog->setModal(true);
    m_pPrefsDialog->show();
}

void MainWindow::PopulateRunningSquare()
{
    QPalette pal = m_pRunningSquare->palette();

    // set black background
    QColor col = Qt::red;
    if (!m_pTargetModel->IsConnected())
    {
        col = Qt::gray;
    }
    else if (m_pTargetModel->IsRunning())
    {
        col = Qt::green;
    }
    pal.setColor(QPalette::Background, col);
    m_pRunningSquare->setAutoFillBackground(true);
    m_pRunningSquare->setPalette(pal);
}

void MainWindow::updateWindowMenu()
{
    for (int i = 0; i < kNumDisasmViews; ++i)
        m_pDisasmWindowActs[i]->setChecked(m_pDisasmWidgets[i]->isVisible());

    for (int i = 0; i < kNumMemoryViews; ++i)
        m_pMemoryWindowActs[i]->setChecked(m_pMemoryViewWidgets[i]->isVisible());

    m_pGraphicsInspectorAct->setChecked(m_pGraphicsInspector->isVisible());
    m_pBreakpointsWindowAct->setChecked(m_pBreakpointsWidget->isVisible());
    m_pConsoleWindowAct->setChecked(m_pConsoleWindow->isVisible());
    m_pHardwareWindowAct->setChecked(m_pHardwareWindow->isVisible());
    m_pProfileWindowAct->setChecked(m_pProfileWindow->isVisible());
}

void MainWindow::updateButtonEnable()
{
    bool isConnected = m_pTargetModel->IsConnected();
    bool isRunning = m_pTargetModel->IsRunning();

    // Buttons...
    m_pStartStopButton->setEnabled(isConnected);
    m_pStartStopButton->setText(isRunning ? "Break" : "Run");

    m_pStepIntoButton->setEnabled(isConnected && !isRunning);
    m_pStepOverButton->setEnabled(isConnected && !isRunning);
    m_pRunToButton->setEnabled(isConnected && !isRunning);

    // Menu items...
    m_pConnectAct->setEnabled(!isConnected);
    m_pDisconnectAct->setEnabled(isConnected);
    m_pWarmResetAct->setEnabled(isConnected);
    m_pExceptionsAct->setEnabled(isConnected);
}


void MainWindow::loadSettings()
{
    //https://doc.qt.io/qt-5/qsettings.html#details
    QSettings settings;

    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    if(!restoreState(settings.value("windowState").toByteArray()))
    {
        // Default docking status
        for (int i = 0; i < kNumDisasmViews; ++i)
            m_pDisasmWidgets[i]->setVisible(i == 0);
        for (int i = 0; i < kNumMemoryViews; ++i)
            m_pMemoryViewWidgets[i]->setVisible(i == 0);
        m_pGraphicsInspector->setVisible(true);
        m_pBreakpointsWidget->setVisible(true);
        m_pConsoleWindow->setVisible(false);
        m_pHardwareWindow->setVisible(false);
        m_pProfileWindow->setVisible(false);
    }
    else
    {
        QDockWidget* wlist[] =
        {
            m_pBreakpointsWidget, m_pGraphicsInspector,
            m_pConsoleWindow, m_pHardwareWindow,
            m_pProfileWindow,
            nullptr
        };
        QDockWidget** pCurr = wlist;
        while (*pCurr)
        {
            // Fix for docking system: for some reason, we need to manually
            // activate floating docking windows for them to appear
            if ((*pCurr)->isFloating())
            {
                (*pCurr)->activateWindow();
            }
            ++pCurr;
        }
        for (int i = 0; i < kNumDisasmViews; ++i)
            if (m_pDisasmWidgets[i]->isFloating())
                m_pDisasmWidgets[i]->activateWindow();
        for (int i = 0; i < kNumMemoryViews; ++i)
            if (m_pMemoryViewWidgets[i]->isFloating())
                m_pMemoryViewWidgets[i]->activateWindow();
    }

    m_pRunToCombo->setCurrentIndex(settings.value("runto", QVariant(0)).toInt());
    settings.endGroup();
}

void MainWindow::saveSettings()
{
    // enclose in scope so it's saved before widgets are saved
    {
        QSettings settings;
        settings.beginGroup("MainWindow");
        settings.setValue("geometry", saveGeometry());
        settings.setValue("windowState", saveState());
        settings.setValue("runto", m_pRunToCombo->currentIndex());
        settings.endGroup();
    }
    for (int i = 0; i < kNumDisasmViews; ++i)
        m_pDisasmWidgets[i]->saveSettings();
    for (int i = 0; i < kNumMemoryViews; ++i)
        m_pMemoryViewWidgets[i]->saveSettings();
    m_pGraphicsInspector->saveSettings();
    m_pConsoleWindow->saveSettings();
    m_pHardwareWindow->saveSettings();
    m_pProfileWindow->saveSettings();
}

void MainWindow::menuConnect()
{
    ConnectTriggered();
}

void MainWindow::menuDisconnect()
{
    DisconnectTriggered();
}

void MainWindow::about()
{
    QMessageBox box;

    QString text = "<h1>hrdb - Hatari remote debugger GUI</h1>\n"
                   "<p>Released under a GPL licence.</p>"
                  "<p><a href=\"https://github.com/tattlemuss/hatari\">Github Repository</a></p>\n"
                   "<p>Version: " VERSION_STRING "</p>";

    QString gplText =
"This program is free software; you can redistribute it and/or modify"
"<br/>it under the terms of the GNU General Public License as published by"
"<br/>the Free Software Foundation; either version 2 of the License, or"
"<br/>(at your option) any later version."
"<br/>"
"<br/>This program is distributed in the hope that it will be useful,"
"<br/>but WITHOUT ANY WARRANTY; without even the implied warranty of"
"<br/>MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the"
"<br/>GNU General Public License for more details.";

    text += gplText;

    box.setTextFormat(Qt::RichText);
    box.setText(text);
    box.exec();
}

void MainWindow::aboutQt()
{
}

void MainWindow::createActions()
{
    // "File"
    m_pLaunchAct = new QAction(tr("&Launch..."), this);
    m_pLaunchAct->setStatusTip(tr("Launch Hatari"));
    m_pLaunchAct->setShortcut(QKeySequence("Alt+L"));
    connect(m_pLaunchAct, &QAction::triggered, this, &MainWindow::LaunchTriggered);

    // "Quicklaunch"
    m_pQuickLaunchAct = new QAction(tr("&QuickLaunch"), this);
    m_pQuickLaunchAct->setStatusTip(tr("Launch Hatari with previous settings"));
    m_pQuickLaunchAct->setShortcut(QKeySequence("Alt+Q"));
    connect(m_pQuickLaunchAct, &QAction::triggered, this, &MainWindow::QuickLaunchTriggered);

    m_pConnectAct = new QAction(tr("&Connect"), this);
    m_pConnectAct->setStatusTip(tr("Connect to Hatari"));
    connect(m_pConnectAct, &QAction::triggered, this, &MainWindow::ConnectTriggered);

    m_pDisconnectAct = new QAction(tr("&Disconnect"), this);
    m_pDisconnectAct->setStatusTip(tr("Disconnect from Hatari"));
    connect(m_pDisconnectAct, &QAction::triggered, this, &MainWindow::DisconnectTriggered);

    m_pWarmResetAct = new QAction(tr("Warm Reset"), this);
    m_pWarmResetAct->setStatusTip(tr("Warm-Reset the machine"));
    connect(m_pWarmResetAct, &QAction::triggered, this, &MainWindow::WarmResetTriggered);

    m_pExitAct = new QAction(tr("E&xit"), this);
    m_pExitAct->setShortcuts(QKeySequence::Quit);
    m_pExitAct->setStatusTip(tr("Exit the application"));
    connect(m_pExitAct, &QAction::triggered, this, &QWidget::close);

    // Edit
    m_pExceptionsAct = new QAction(tr("&Exceptions..."), this);
    m_pExceptionsAct->setStatusTip(tr("Disconnect from Hatari"));
    connect(m_pExceptionsAct, &QAction::triggered, this, &MainWindow::ExceptionsDialogTriggered);

    m_pPrefsAct = new QAction(tr("&Preferences..."), this);
    m_pPrefsAct->setStatusTip(tr("Set options and preferences"));
    connect(m_pPrefsAct, &QAction::triggered, this, &MainWindow::PrefsDialogTriggered);

    // "Window"
    for (int i = 0; i < kNumDisasmViews; ++i)
    {
        m_pDisasmWindowActs[i] = new QAction(m_pDisasmWidgets[i]->windowTitle(), this);
        m_pDisasmWindowActs[i]->setStatusTip(tr("Show the disassembly window"));
        m_pDisasmWindowActs[i]->setCheckable(true);

        if (i == 0)
            m_pDisasmWindowActs[i]->setShortcut(QKeySequence("Alt+D"));
    }

    for (int i = 0; i < kNumMemoryViews; ++i)
    {
        m_pMemoryWindowActs[i] = new QAction(m_pMemoryViewWidgets[i]->windowTitle(), this);
        m_pMemoryWindowActs[i]->setStatusTip(tr("Show the memory window"));
        m_pMemoryWindowActs[i]->setCheckable(true);

        if (i == 0)
            m_pMemoryWindowActs[i]->setShortcut(QKeySequence("Alt+M"));
    }

    m_pGraphicsInspectorAct = new QAction(tr("&Graphics Inspector"), this);
    m_pGraphicsInspectorAct->setShortcut(QKeySequence("Alt+G"));
    m_pGraphicsInspectorAct->setStatusTip(tr("Show the Graphics Inspector"));
    m_pGraphicsInspectorAct->setCheckable(true);

    m_pBreakpointsWindowAct = new QAction(tr("&Breakpoints"), this);
    m_pBreakpointsWindowAct->setShortcut(QKeySequence("Alt+B"));
    m_pBreakpointsWindowAct->setStatusTip(tr("Show the Breakpoints window"));
    m_pBreakpointsWindowAct->setCheckable(true);

    m_pConsoleWindowAct = new QAction(tr("&Console"), this);
    m_pConsoleWindowAct->setStatusTip(tr("Show the Console window"));
    m_pConsoleWindowAct->setCheckable(true);

    m_pHardwareWindowAct = new QAction(tr("&Hardware"), this);
    m_pHardwareWindowAct->setShortcut(QKeySequence("Alt+H"));
    m_pHardwareWindowAct->setStatusTip(tr("Show the Hardware window"));
    m_pHardwareWindowAct->setCheckable(true);

    m_pProfileWindowAct = new QAction(tr("&Profile"), this);
    m_pProfileWindowAct->setStatusTip(tr("Show the Profile window"));
    m_pProfileWindowAct->setCheckable(true);

    for (int i = 0; i < kNumDisasmViews; ++i)
        connect(m_pDisasmWindowActs[i], &QAction::triggered, this,     [=] () { this->enableVis(m_pDisasmWidgets[i]); m_pDisasmWidgets[i]->keyFocus(); } );

    for (int i = 0; i < kNumMemoryViews; ++i)
        connect(m_pMemoryWindowActs[i], &QAction::triggered, this,     [=] () { this->enableVis(m_pMemoryViewWidgets[i]); m_pMemoryViewWidgets[i]->keyFocus(); } );

    connect(m_pGraphicsInspectorAct, &QAction::triggered, this, [=] () { this->enableVis(m_pGraphicsInspector); m_pGraphicsInspector->keyFocus(); } );
    connect(m_pBreakpointsWindowAct, &QAction::triggered, this, [=] () { this->enableVis(m_pBreakpointsWidget); m_pBreakpointsWidget->keyFocus(); } );
    connect(m_pConsoleWindowAct,     &QAction::triggered, this, [=] () { this->enableVis(m_pConsoleWindow); m_pConsoleWindow->keyFocus(); } );
    connect(m_pHardwareWindowAct,    &QAction::triggered, this, [=] () { this->enableVis(m_pHardwareWindow); m_pHardwareWindow->keyFocus(); } );
    connect(m_pProfileWindowAct,     &QAction::triggered, this, [=] () { this->enableVis(m_pProfileWindow); m_pProfileWindow->keyFocus(); } );

    // "About"
    m_pAboutAct = new QAction(tr("&About"), this);
    m_pAboutAct->setStatusTip(tr("Show the application's About box"));
    connect(m_pAboutAct, &QAction::triggered, this, &MainWindow::about);

    m_pAboutQtAct = new QAction(tr("About &Qt"), this);
    m_pAboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(m_pAboutQtAct, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(m_pAboutQtAct, &QAction::triggered, this, &MainWindow::aboutQt);
}

void MainWindow::createToolBar()
{
    QToolBar* pToolbar = new QToolBar(this);
    pToolbar->addAction(m_pQuickLaunchAct);
    pToolbar->addAction(m_pLaunchAct);
    pToolbar->addSeparator();
    pToolbar->addAction(m_pWarmResetAct);

    this->addToolBar(Qt::ToolBarArea::TopToolBarArea, pToolbar);
}

void MainWindow::createMenus()
{
    // "File"
    m_pFileMenu = menuBar()->addMenu(tr("&File"));
    m_pFileMenu->addAction(m_pQuickLaunchAct);
    m_pFileMenu->addAction(m_pLaunchAct);
    m_pFileMenu->addAction(m_pConnectAct);
    m_pFileMenu->addAction(m_pDisconnectAct);
    m_pFileMenu->addAction(m_pWarmResetAct);
    m_pFileMenu->addSeparator();
    m_pFileMenu->addAction(m_pExitAct);

    m_pEditMenu = menuBar()->addMenu(tr("&Edit"));
    m_pEditMenu->addSeparator();
    m_pEditMenu->addAction(m_pExceptionsAct);
    m_pEditMenu->addSeparator();
    m_pEditMenu->addAction(m_pPrefsAct);

    m_pWindowMenu = menuBar()->addMenu(tr("&Window"));
    for (int i = 0; i < kNumDisasmViews; ++i)
        m_pWindowMenu->addAction(m_pDisasmWindowActs[i]);
    m_pWindowMenu->addSeparator();

    for (int i = 0; i < kNumMemoryViews; ++i)
        m_pWindowMenu->addAction(m_pMemoryWindowActs[i]);
    m_pWindowMenu->addSeparator();

    m_pWindowMenu->addAction(m_pGraphicsInspectorAct);
    m_pWindowMenu->addAction(m_pBreakpointsWindowAct);
    m_pWindowMenu->addAction(m_pConsoleWindowAct);
    m_pWindowMenu->addAction(m_pHardwareWindowAct);
    m_pWindowMenu->addAction(m_pProfileWindowAct);

    m_pHelpMenu = menuBar()->addMenu(tr("Help"));
    m_pHelpMenu->addAction(m_pAboutAct);
    m_pHelpMenu->addAction(m_pAboutQtAct);
}

void MainWindow::enableVis(QWidget* pWidget)
{
    // This used to be a toggle
    pWidget->setVisible(true);
    pWidget->setHidden(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}
