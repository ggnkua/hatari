#ifndef GRAPHICSINSPECTOR_H
#define GRAPHICSINSPECTOR_H

#include <QDockWidget>
#include <QObject>

// Forward declarations
#include "../models/session.h"
#include "nonantialiasimage.h"

class QLabel;
class QLineEdit;
class QAbstractItemModel;
class QSpinBox;
class QCheckBox;
class QComboBox;

class TargetModel;
class Dispatcher;

class GraphicsInspectorWidget : public QDockWidget
{
    Q_OBJECT
public:
    GraphicsInspectorWidget(QWidget *parent,
                            Session* pSession);
    ~GraphicsInspectorWidget();

    // Grab focus and point to the main widget
    void keyFocus();
    void loadSettings();
    void saveSettings();
private:
    void connectChangedSlot();
    void startStopChangedSlot();
    void startStopDelayedChangedSlot();
    void memoryChangedSlot(int memorySlot, uint64_t commandId);
    void otherMemoryChangedSlot(uint32_t address, uint32_t size);
    void runningRefreshTimerSlot();

    void textEditChangedSlot();
    void lockAddressToVideoChangedSlot();
    void lockFormatToVideoChangedSlot();
    void lockPaletteToVideoChangedSlot();

private slots:
    void modeChangedSlot(int index);
    void paletteChangedSlot(int index);
    void widthChangedSlot(int width);
    void heightChangedSlot(int height);
    void paddingChangedSlot(int height);
    void tooltipStringChangedSlot();
    void requestAddress(Session::WindowType type, int windowIndex, uint32_t address);
protected:
    virtual void keyPressEvent(QKeyEvent *ev);

private:
    enum Mode
    {
        k4Bitplane,
        k2Bitplane,
        k1Bitplane,
    };

    enum Palette
    {
        kGreyscale,
        kContrast1,
        kBitplane0,
        kBitplane1,
        kBitplane2,
        kBitplane3,
    };

    void UpdateUIElements();

    // This requests the video regs, which ultimately triggers RequestBitmapMemory
    void StartMemoryRequests();
    // This requests the bitmap area, so assumes that the address/size etc is correctly set
    void RequestBitmapMemory();

    bool SetAddressFromVideo();
    void DisplayAddress();

    // Either copy from registers, or use the user settings
    void UpdatePaletteFromSettings();

    // Copy format from video regs if required
    void UpdateFormatFromSettings();

    struct EffectiveData
    {
        int width;
        int height;
        Mode mode;
        int bytesPerLine;
        int requiredSize;
    };

    // Get the effective data by checking the "lock to" flags and
    // using them if necessary.
    GraphicsInspectorWidget::Mode GetEffectiveMode() const;
    int GetEffectiveWidth() const;
    int GetEffectiveHeight() const;
    int GetEffectivePadding() const;
    void GetEffectiveData(EffectiveData& data) const;

    static int32_t BytesPerMode(Mode mode);

    QLineEdit*      m_pAddressLineEdit;
    QComboBox*      m_pModeComboBox;
    QSpinBox*       m_pWidthSpinBox;
    QSpinBox*       m_pHeightSpinBox;
    QSpinBox*       m_pPaddingSpinBox;
    QCheckBox*      m_pLockAddressToVideoCheckBox;
    QCheckBox*      m_pLockFormatToVideoCheckBox;
    QCheckBox*      m_pLockPaletteToVideoCheckBox;
    QComboBox*      m_pPaletteComboBox;
    QLabel*         m_pInfoLabel;

    NonAntiAliasImage*         m_pImageWidget;

    Session*        m_pSession;
    TargetModel*    m_pTargetModel;
    Dispatcher*     m_pDispatcher;

    QAbstractItemModel* m_pSymbolTableModel;

    Mode            m_mode;
    uint32_t        m_address;
    int             m_width;            // in "chunks"
    int             m_height;
    int             m_padding;          // in bytes

    uint64_t        m_requestIdBitmap;
    uint64_t        m_requestIdVideoRegs;
};

#endif // GRAPHICSINSPECTOR_H
