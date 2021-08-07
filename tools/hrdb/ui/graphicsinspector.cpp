#include "graphicsinspector.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QCompleter>
#include <QSpinBox>
#include <QShortcut>
#include <QKeyEvent>
#include <QCheckBox>
#include <QComboBox>

#include <QPainter>
#include <QStyle>
#include <QFontDatabase>
#include <QSettings>

#include "../transport/dispatcher.h"
#include "../models/targetmodel.h"
#include "../models/symboltablemodel.h"
#include "../models/stringparsers.h"

NonAntiAliasImage::NonAntiAliasImage(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
}

void NonAntiAliasImage::paintEvent(QPaintEvent* ev)
{
    QPainter painter(this);

    const QFont monoFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    if (m_pixmap.width() != 0)
    {
        painter.setRenderHint(QPainter::Antialiasing, false);
        style()->drawItemPixmap(&painter, rect(), Qt::AlignCenter, m_pixmap.scaled(rect().size()));

        if (this->underMouse())
        {
            painter.setFont(monoFont);
            painter.drawText(10, 10,
                  QString::asprintf("X:%d Y:%d\n",
                                (int) m_mousePos.x(),
                                (int) m_mousePos.y()));
        }
    }
    else {
        painter.setFont(monoFont);
        painter.drawText(10, 10, "Not connected");
    }
    const QPalette& pal = this->palette();

    painter.setPen(QPen(pal.dark(), hasFocus() ? 6 : 2));
    painter.drawRect(this->rect());

    QWidget::paintEvent(ev);
}

void NonAntiAliasImage::mouseMoveEvent(QMouseEvent *event)
{
    m_mousePos = event->localPos();
    if (this->underMouse())
        update();

    QWidget::mouseMoveEvent(event);
}


GraphicsInspectorWidget::GraphicsInspectorWidget(QWidget *parent,
                                                 TargetModel* pTargetModel, Dispatcher* pDispatcher) :
    QDockWidget(parent),
    m_pTargetModel(pTargetModel),
    m_pDispatcher(pDispatcher),
    m_mode(k4Bitplane),
    m_address(0U),
    m_width(20),
    m_height(200),
    m_requestIdBitmap(0U),
    m_requestIdPalette(0U)
{
    QString name("GraphicsInspector");
    this->setObjectName(name);
    this->setWindowTitle(name);
    this->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    m_pImageWidget = new NonAntiAliasImage(this);
    m_pImageWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    //m_pPictureLabel->setFixedSize(640, 400);
    //m_pPictureLabel->setScaledContents(true);

    m_pLineEdit = new QLineEdit(this);
    m_pSymbolTableModel = new SymbolTableModel(this, m_pTargetModel->GetSymbolTable());
    QCompleter* pCompl = new QCompleter(m_pSymbolTableModel, this);
    pCompl->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    m_pLineEdit->setCompleter(pCompl);

    m_pModeComboBox = new QComboBox(this);
    m_pModeComboBox->addItem(tr("4 Plane"), Mode::k4Bitplane);
    m_pModeComboBox->addItem(tr("2 Plane"), Mode::k2Bitplane);
    m_pModeComboBox->addItem(tr("1 Plane"), Mode::k1Bitplane);

    m_pWidthSpinBox = new QSpinBox(this);
    m_pWidthSpinBox->setRange(1, 32);
    m_pWidthSpinBox->setValue(m_width);

    m_pHeightSpinBox = new QSpinBox(this);
    m_pHeightSpinBox->setRange(16, 256);
    m_pHeightSpinBox->setValue(m_height);
    m_pLockToVideoCheckBox = new QCheckBox(tr("Follow Video Pointer"), this);

    auto pMainGroupBox = new QWidget(this);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->addWidget(m_pLineEdit);
    hlayout->addWidget(m_pModeComboBox);
    hlayout->addWidget(new QLabel(tr("Width"), this));
    hlayout->addWidget(m_pWidthSpinBox);
    hlayout->addWidget(new QLabel(tr("Height"), this));
    hlayout->addWidget(m_pHeightSpinBox);
    QWidget* pTopContainer = new QWidget(this);
    pTopContainer->setLayout(hlayout);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(pTopContainer);
    vlayout->addWidget(m_pLockToVideoCheckBox);
    vlayout->addWidget(m_pImageWidget);
    vlayout->setAlignment(Qt::Alignment(Qt::AlignTop));
    pMainGroupBox->setLayout(vlayout);

    setWidget(pMainGroupBox);
    m_pLockToVideoCheckBox->setChecked(true);

    connect(m_pTargetModel,  &TargetModel::connectChangedSignal,          this, &GraphicsInspectorWidget::connectChangedSlot);
    connect(m_pTargetModel,  &TargetModel::startStopChangedSignalDelayed, this, &GraphicsInspectorWidget::startStopChangedSlot);
    connect(m_pTargetModel,  &TargetModel::memoryChangedSignal,           this, &GraphicsInspectorWidget::memoryChangedSlot);
    connect(m_pTargetModel,  &TargetModel::otherMemoryChanged,            this, &GraphicsInspectorWidget::otherMemoryChangedSlot);

    connect(m_pLineEdit,     &QLineEdit::returnPressed,                   this, &GraphicsInspectorWidget::textEditChangedSlot);
    connect(m_pLockToVideoCheckBox,
                             &QCheckBox::stateChanged,                    this, &GraphicsInspectorWidget::followVideoChangedSlot);

    connect(m_pModeComboBox, SIGNAL(currentIndexChanged(int)),            SLOT(modeChangedSlot(int)));
    connect(m_pWidthSpinBox, SIGNAL(valueChanged(int)),                   SLOT(widthChangedSlot(int)));
    connect(m_pHeightSpinBox,SIGNAL(valueChanged(int)),                   SLOT(heightChangedSlot(int)));

    loadSettings();
    UpdateCheckBoxes();
    DisplayAddress();
}

GraphicsInspectorWidget::~GraphicsInspectorWidget()
{

}

void GraphicsInspectorWidget::keyFocus()
{
    activateWindow();
    m_pImageWidget->setFocus();
}

void GraphicsInspectorWidget::loadSettings()
{
    QSettings settings;
    settings.beginGroup("GraphicsInspector");

    restoreGeometry(settings.value("geometry").toByteArray());
    m_width = settings.value("width", QVariant(20)).toInt();
    m_height = settings.value("height", QVariant(200)).toInt();
    m_mode = static_cast<Mode>(settings.value("mode", QVariant(200)).toInt());

    m_pWidthSpinBox->setValue(m_width);
    m_pHeightSpinBox->setValue(m_height);
    m_pLockToVideoCheckBox->setChecked(settings.value("lockToVideo", QVariant(true)).toBool());
    m_pModeComboBox->setCurrentIndex(m_mode);
    settings.endGroup();
}

void GraphicsInspectorWidget::saveSettings()
{
    QSettings settings;
    settings.beginGroup("GraphicsInspector");

    settings.setValue("geometry", saveGeometry());
    settings.setValue("width", m_width);
    settings.setValue("height", m_height);
    settings.setValue("lockToVideo", m_pLockToVideoCheckBox->isChecked());
    settings.setValue("mode", static_cast<int>(m_mode));
    settings.endGroup();
}

void GraphicsInspectorWidget::keyPressEvent(QKeyEvent* ev)
{
    int offset = 0;
    int32_t bytes = BytesPerMode(m_mode);

    if (ev->key() == Qt::Key::Key_Up)
        offset = -m_width * bytes;
    else if (ev->key() == Qt::Key::Key_Down)
        offset = +m_width * bytes;
    else if (ev->key() == Qt::Key::Key_PageUp)
        offset = m_height * -m_width * bytes;
    else if (ev->key() == Qt::Key::Key_PageDown)
        offset = m_height * m_width * bytes;
    else if (ev->key() == Qt::Key::Key_Left)
        offset = -2;
    else if (ev->key() == Qt::Key::Key_Right)
        offset = 2;

    if (offset && m_requestIdBitmap == 0)
    {
        // Going up or down by a small amount is OK
        if (offset > 0 || m_address > -offset)
        {
            m_address += offset;
        }
        else {
            m_address = 0;
        }
        m_pLockToVideoCheckBox->setChecked(false);
        RequestMemory();
        DisplayAddress();

        return;
    }
    QDockWidget::keyPressEvent(ev);

}

void GraphicsInspectorWidget::connectChangedSlot()
{
    if (!m_pTargetModel->IsConnected())
    {
        QPixmap empty;
        m_pImageWidget->setPixmap(empty);
    }
}

void GraphicsInspectorWidget::startStopChangedSlot()
{
    // Request new memory for the view
    if (!m_pTargetModel->IsRunning())
    {
        if (m_pLockToVideoCheckBox->isChecked())
            SetAddressFromVideo();

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

        int32_t bytesPerChunk = BytesPerMode(m_mode);
        // Uncompress
        int required = m_width * bytesPerChunk * m_height;

        // Ensure we have the right size memory
        if (pMemOrig->GetSize() < required)
            return;

        // Need to redraw here
        uint8_t* pBitmap = new uint8_t[m_width * 16 * m_height];

        const uint8_t* pChunk = pMemOrig->GetData();
        uint8_t* pDestPixels = pBitmap;

        if (m_mode == k4Bitplane)
        {
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
        }
        else if (m_mode == k2Bitplane)
        {
            for (int i = 0; i < m_width * m_height; ++i)
            {
                uint16_t pSrc[2];
                pSrc[1] = (pChunk[0] << 8) | pChunk[1];
                pSrc[0] = (pChunk[2] << 8) | pChunk[3];
                for (int pix = 15; pix >= 0; --pix)
                {
                    uint8_t val;
                    val  = (pSrc[0] & 1); val <<= 1;
                    val |= (pSrc[1] & 1); val <<= 1;
                    pDestPixels[pix] = val;
                    pSrc[0] >>= 1;
                    pSrc[1] >>= 1;
                }
                pChunk += 4;
                pDestPixels += 16;
            }
        }
        else if (m_mode == k1Bitplane)
        {
            for (int i = 0; i < m_width * m_height; ++i)
            {
                uint16_t pSrc[1];
                pSrc[0] = (pChunk[0] << 8) | pChunk[1];
                for (int pix = 15; pix >= 0; --pix)
                {
                    uint8_t val;
                    val  = (pSrc[0] & 1); val <<= 1;
                    pDestPixels[pix] = val;
                    pSrc[0] >>= 1;
                }
                pChunk += 2;
                pDestPixels += 16;
            }
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
    m_pLockToVideoCheckBox->setChecked(false);
    RequestMemory();
}

void GraphicsInspectorWidget::followVideoChangedSlot()
{
    bool m_bLockToVideo = m_pLockToVideoCheckBox->isChecked();
    if (m_bLockToVideo)
    {
        SetAddressFromVideo();
        RequestMemory();
    }
}

void GraphicsInspectorWidget::modeChangedSlot(int index)
{
    m_mode = static_cast<Mode>(m_pModeComboBox->currentIndex());
    RequestMemory();
}

void GraphicsInspectorWidget::otherMemoryChangedSlot(uint32_t address, uint32_t size)
{
    // Do a re-request if our memory is touched
    uint32_t ourSize = m_height * m_width * BytesPerMode(m_mode);
    if (Overlaps(m_address, ourSize, address, size))
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

// Request enough memory based on m_rowCount and m_logicalAddr
void GraphicsInspectorWidget::RequestMemory()
{
    if (!m_pTargetModel->IsConnected())
        return;

    // Palette first
    m_requestIdPalette = m_pDispatcher->RequestMemory(MemorySlot::kGraphicsInspectorPalette, 0xff8240, 32);

    int size = m_height * m_width * BytesPerMode(m_mode);
    m_requestIdBitmap = m_pDispatcher->RequestMemory(MemorySlot::kGraphicsInspector, m_address, size);
}

bool GraphicsInspectorWidget::SetAddressFromVideo()
{
    // Update to current video regs
    const Memory* pVideoRegs = m_pTargetModel->GetMemory(MemorySlot::kVideo);
    if (pVideoRegs->GetSize() > 0)
    {
        uint8_t hi = pVideoRegs->ReadAddressByte(0xff8201);
        uint8_t mi = pVideoRegs->ReadAddressByte(0xff8203);
        uint8_t lo = pVideoRegs->ReadAddressByte(0xff820d);
        if (m_pTargetModel->GetMachineType() == MACHINE_ST)
            lo = 0;
        if (m_pTargetModel->GetMachineType() == MACHINE_MEGA_ST)
            lo = 0;

        m_address = (hi << 16) | (mi << 8) | lo;
        DisplayAddress();
        return true;
    }
    return false;
}

void GraphicsInspectorWidget::DisplayAddress()
{
    m_pLineEdit->setText(QString::asprintf("$%x", m_address));
}

void GraphicsInspectorWidget::UpdateCheckBoxes()
{
}

int32_t GraphicsInspectorWidget::BytesPerMode(GraphicsInspectorWidget::Mode mode)
{
    switch (mode)
    {
    case k4Bitplane: return 8;
    case k2Bitplane: return 4;
    case k1Bitplane: return 2;
    }
    return 0;
}

