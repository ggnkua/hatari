#ifndef GRAPHICSINSPECTOR_H
#define GRAPHICSINSPECTOR_H

#include <QDockWidget>
#include <QObject>

// Forward declarations
class QLabel;
class QLineEdit;
class QAbstractItemModel;
class QSpinBox;
class QCheckBox;
class QComboBox;

class TargetModel;
class Dispatcher;

// Taken from https://forum.qt.io/topic/94996/qlabel-and-image-antialiasing/5
class NonAntiAliasImage : public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(NonAntiAliasImage)
public:
    explicit NonAntiAliasImage(QWidget* parent = Q_NULLPTR);
    const QPixmap& pixmap() const
    {
        return m_pixmap;
    }
    void setPixmap(const QPixmap& px)
    {
        m_pixmap = px;
        update();
    }
protected:
    virtual void paintEvent(QPaintEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
private:
    QPixmap m_pixmap;
    QPointF m_mousePos;
};


class GraphicsInspectorWidget : public QDockWidget
{
    Q_OBJECT
public:
    GraphicsInspectorWidget(QWidget *parent,
                            TargetModel* pTargetModel, Dispatcher* pDispatcher);
    ~GraphicsInspectorWidget();

    // Grab focus and point to the main widget
    void keyFocus();
    void loadSettings();
    void saveSettings();
private:
    void connectChangedSlot();
    void startStopChangedSlot();
    void memoryChangedSlot(int memorySlot, uint64_t commandId);
    void otherMemoryChangedSlot(uint32_t address, uint32_t size);
    void textEditChangedSlot();
    void followVideoChangedSlot();

private slots:
    void modeChangedSlot(int index);
    void widthChangedSlot(int width);
    void heightChangedSlot(int height);
protected:
    virtual void keyPressEvent(QKeyEvent *ev);

private:
    enum Mode
    {
        k4Bitplane,
        k2Bitplane,
        k1Bitplane
    };

    void UpdateCheckBoxes();
    void RequestMemory();
    bool SetAddressFromVideo();
    void DisplayAddress();

    static int32_t BytesPerMode(Mode mode);

    QLineEdit*      m_pLineEdit;
    QComboBox*      m_pModeComboBox;
    QSpinBox*       m_pWidthSpinBox;
    QSpinBox*       m_pHeightSpinBox;
    QCheckBox*      m_pLockToVideoCheckBox;

    NonAntiAliasImage*         m_pImageWidget;

    TargetModel*    m_pTargetModel;
    Dispatcher*     m_pDispatcher;
    QAbstractItemModel* m_pSymbolTableModel;
    QVector<QRgb>   m_colours;

    Mode            m_mode;
    uint32_t        m_address;
    int             m_width;
    int             m_height;

    uint64_t        m_requestIdBitmap;
    uint64_t        m_requestIdPalette;
};

#endif // GRAPHICSINSPECTOR_H
