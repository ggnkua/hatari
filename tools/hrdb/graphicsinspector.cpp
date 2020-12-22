#include "graphicsinspector.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QCompleter>
#include <QSpinBox>
#include <QShortcut>

#include <QPainter>
#include <QStyle>

#include "dispatcher.h"
#include "targetmodel.h"
#include "symboltablemodel.h"
#include "stringparsers.h"


void NonAntiAliasImage::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    style()->drawItemPixmap(&painter, rect(), Qt::AlignCenter, m_pixmap.scaled(rect().size()));
}


GraphicsInspectorWidget::GraphicsInspectorWidget(QWidget *parent,
                                                 TargetModel* pTargetModel, Dispatcher* pDispatcher) :
    QDockWidget(parent),
    m_pTargetModel(pTargetModel),
    m_pDispatcher(pDispatcher),
    m_address(0U),
    m_width(20),
    m_height(200),
    m_requestIdBitmap(0U),
    m_requestIdPalette(0U)
{
    QString name("Graphics Inspector");
    this->setObjectName(name);
    this->setWindowTitle(name);

    m_pImageWidget = new NonAntiAliasImage(this);
    m_pImageWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    m_pImageWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    //m_pPictureLabel->setFixedSize(640, 400);
    //m_pPictureLabel->setScaledContents(true);

    m_pLineEdit = new QLineEdit(this);
    m_pSymbolTableModel = new SymbolTableModel(this, m_pTargetModel->GetSymbolTable());
    QCompleter* pCompl = new QCompleter(m_pSymbolTableModel, this);
    pCompl->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    m_pLineEdit->setCompleter(pCompl);

    m_pWidthSpinBox = new QSpinBox(this);
    m_pWidthSpinBox->setRange(1, 32);
    m_pWidthSpinBox->setValue(m_width);

    m_pHeightSpinBox = new QSpinBox(this);
    m_pHeightSpinBox->setRange(16, 256);
    m_pHeightSpinBox->setValue(m_height);
    auto pMainGroupBox = new QGroupBox(this);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(m_pLineEdit);
    vlayout->addWidget(m_pWidthSpinBox);
    vlayout->addWidget(m_pHeightSpinBox);
    vlayout->addWidget(m_pImageWidget);
    vlayout->setAlignment(Qt::Alignment(Qt::AlignTop));
    pMainGroupBox->setFlat(true);
    pMainGroupBox->setLayout(vlayout);

    setWidget(pMainGroupBox);

    connect(m_pTargetModel,  &TargetModel::startStopChangedSignalDelayed, this, &GraphicsInspectorWidget::startStopChangedSlot);
    connect(m_pTargetModel,  &TargetModel::memoryChangedSignal,           this, &GraphicsInspectorWidget::memoryChangedSlot);
    connect(m_pLineEdit,     &QLineEdit::returnPressed,                   this, &GraphicsInspectorWidget::textEditChangedSlot);
    connect(m_pWidthSpinBox, SIGNAL(valueChanged(int)),                   SLOT(widthChangedSlot(int)));
    connect(m_pHeightSpinBox,SIGNAL(valueChanged(int)),                   SLOT(heightChangedSlot(int)));

    new QShortcut(QKeySequence(QKeySequence::StandardKey::MoveToPreviousPage), this, SLOT(pageUp()));
    new QShortcut(QKeySequence(QKeySequence::StandardKey::MoveToNextPage    ), this, SLOT(pageDown()));
    new QShortcut(QKeySequence(QKeySequence::StandardKey::MoveToPreviousLine), this, SLOT(lineUp()));
    new QShortcut(QKeySequence(QKeySequence::StandardKey::MoveToNextLine    ), this, SLOT(lineDown()));
    new QShortcut(QKeySequence("Left"), this, SLOT(moveLeft()));
    new QShortcut(QKeySequence("Right"), this, SLOT(moveRight()));
}

GraphicsInspectorWidget::~GraphicsInspectorWidget()
{

}

void GraphicsInspectorWidget::startStopChangedSlot()
{
    // Request new memory for the view
    if (!m_pTargetModel->IsRunning())
    {
        // Just request what we had already.
        RequestMemory();
    }
}

void GraphicsInspectorWidget::memoryChangedSlot(int /*memorySlot*/, uint64_t commandId)
{
    // Only update for the last request we added
    if (commandId == m_requestIdBitmap)
    {
        const Memory* pMemOrig = m_pTargetModel->GetMemory(MemorySlot::kGraphicsInspector);
        if (!pMemOrig)
            return;

        // Uncompress
        int required = m_width * 8 * m_height;

        // Ensure we have the right size memory
        if (pMemOrig->GetSize() < required)
            return;

        // Need to redraw here
        uint8_t* pBitmap = new uint8_t[m_width * 16 * m_height];

        const uint8_t* pChunk = pMemOrig->GetData();
        uint8_t* pDestPixels = pBitmap;
        for (int i = 0; i < m_width * m_height; ++i)
        {
            uint16_t pSrc[4];
            pSrc[3] = (pChunk[0] << 8) | pChunk[1];
            pSrc[2] = (pChunk[2] << 8) | pChunk[3];
            pSrc[1] = (pChunk[4] << 8) | pChunk[5];
            pSrc[0] = (pChunk[6] << 8) | pChunk[7];
            for (int pix = 15; pix >= 0; --pix)
            {
                uint8_t val;
                val  = (pSrc[0] & 1); val <<= 1;
                val |= (pSrc[1] & 1); val <<= 1;
                val |= (pSrc[2] & 1); val <<= 1;
                val |= (pSrc[3] & 1);

                pDestPixels[pix] = val;
                pSrc[0] >>= 1;
                pSrc[1] >>= 1;
                pSrc[2] >>= 1;
                pSrc[3] >>= 1;
            }
            pChunk += 8;
            pDestPixels += 16;
        }

        // Update image in the widget
        QImage img(pBitmap, m_width * 16, m_height, QImage::Format_Indexed8);
        img.setColorTable(m_colours);
        QPixmap pm = QPixmap::fromImage(img);
        m_pImageWidget->setPixmap(pm);

        delete [] pBitmap;
        m_requestIdBitmap = 0;
        return;
    }
    else if (commandId == m_requestIdPalette)
    {
        const Memory* pMemOrig = m_pTargetModel->GetMemory(MemorySlot::kGraphicsInspectorPalette);
        if (!pMemOrig)
            return;

        if (pMemOrig->GetSize() != 32)
            return;

        // Colours are ARGB
        m_colours.clear();
        for (int i = 0; i < 16; ++i)
        {
            uint8_t  r = pMemOrig->GetData()[i * 2];
            uint8_t gb = pMemOrig->GetData()[i * 2 + 1];

            uint32_t colour = 0U;
            colour |= ( r & 0x07) << (24 - 3);
            colour |= (gb & 0x70) << (16 - 4 - 3);
            colour |= (gb & 0x07) << (8 - 3);
            colour |= 0xff000000;
            m_colours.append(colour);
        }
        m_requestIdPalette = 0;
    }
}

void GraphicsInspectorWidget::textEditChangedSlot()
{
    std::string expression = m_pLineEdit->text().toStdString();
    uint32_t addr;
    if (!StringParsers::ParseExpression(expression.c_str(), addr,
                                        m_pTargetModel->GetSymbolTable(),
                                        m_pTargetModel->GetRegs()))
    {
        return;
    }
    m_address = addr;
    RequestMemory();
}

void GraphicsInspectorWidget::widthChangedSlot(int value)
{
    m_width = value;
    RequestMemory();
}

void GraphicsInspectorWidget::heightChangedSlot(int value)
{
    m_height = value;
    RequestMemory();
}

void GraphicsInspectorWidget::pageUp()
{
    if (m_requestIdBitmap != 0)
        return;
    int size = m_height * m_width * 8;
    m_address -= size;
    RequestMemory();
    DisplayAddress();
}

void GraphicsInspectorWidget::pageDown()
{
    if (m_requestIdBitmap != 0)
        return;
    int size = m_height * m_width * 8;
    m_address += size;
    RequestMemory();
    DisplayAddress();
}

void GraphicsInspectorWidget::lineUp()
{
    if (m_requestIdBitmap != 0)
        return;
    int size = m_width * 8;
    m_address -= size;
    RequestMemory();
    DisplayAddress();
}

void GraphicsInspectorWidget::lineDown()
{
    if (m_requestIdBitmap != 0)
        return;
    int size = m_width * 8;
    m_address += size;
    RequestMemory();
    DisplayAddress();
}

void GraphicsInspectorWidget::moveLeft()
{
    if (m_requestIdBitmap != 0)
        return;
    m_address -= 2;
    RequestMemory();
    DisplayAddress();
}

void GraphicsInspectorWidget::moveRight()
{
    if (m_requestIdBitmap != 0)
        return;
    m_address += 2;
    RequestMemory();
    DisplayAddress();
}

// Request enough memory based on m_rowCount and m_logicalAddr
void GraphicsInspectorWidget::RequestMemory()
{
    // Palette first
    m_requestIdPalette = m_pDispatcher->RequestMemory(MemorySlot::kGraphicsInspectorPalette, 0xff8240, 32);

    int size = m_height * m_width * 8;
    m_requestIdBitmap = m_pDispatcher->RequestMemory(MemorySlot::kGraphicsInspector, m_address, size);
}

void GraphicsInspectorWidget::DisplayAddress()
{
    m_pLineEdit->setText(QString::asprintf("$%x", m_address));
}