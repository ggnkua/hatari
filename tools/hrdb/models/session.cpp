#include "session.h"
#include <QtNetwork>
#include <QTimer>

Session::Session() :
    QObject(),
    m_autoConnect(true)
{
    m_pStartupFile = new QTemporaryFile(this);
    m_pLoggingFile = new QTemporaryFile(this);

    // Create the core data models, since other object want to connect to them.
    m_pTcpSocket = new QTcpSocket();

    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &Session::connectTimerCallback);

    m_pTimer->start(500);
}

Session::~Session()
{
    m_pLoggingFile->close();
    delete m_pTcpSocket;
    delete m_pTimer;
}

void Session::Connect()
{
    m_autoConnect = true;
    // Have a first go immediately, just in case
    connectTimerCallback();
}

void Session::Disconnect()
{
    m_autoConnect = false;
    m_pTcpSocket->disconnectFromHost();
}

void Session::connectTimerCallback()
{
    if (m_autoConnect && m_pTcpSocket->state() == QAbstractSocket::UnconnectedState)
    {
        QHostAddress qha(QHostAddress::LocalHost);
        m_pTcpSocket->connectToHost(qha, 56001);
    }
}

