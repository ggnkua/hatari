#ifndef SESSION_H
#define SESSION_H

#include <QObject>

class QTcpSocket;
class QTimer;
class QTemporaryFile;

// Shared runtime data about the debugging session used by multiple UI components
// This data isn't persisted over runs (that is saved in Settings)
class Session : public QObject
{
    Q_OBJECT
public:

    // DRAWING LAYOUT OPTIONS
    // Add a 4-pixel offset to shift away from the focus rectangle
    static const int kWidgetBorderX = 6;
    static const int kWidgetBorderY = 4;

    // Standard functions
    Session();
    virtual ~Session();
    void Connect();
    void Disconnect();

    QTcpSocket*      m_pTcpSocket;
    QTemporaryFile*  m_pStartupFile;
    QTemporaryFile*  m_pLoggingFile;

private slots:

    // Called shortly after stop notification received
    void connectTimerCallback();
private:
    QTimer*          m_pTimer;
    bool             m_autoConnect;
};

#endif // SESSION_H
