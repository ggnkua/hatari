#include "filewatcher.h"
#include <QString>
#include <QFileSystemWatcher>
#include <QObject>
#include "transport/dispatcher.h"
#include "models/targetmodel.h"
#include "session.h"

FileWatcher::FileWatcher(const Session* pSession):m_pSession(pSession)
{
    m_pFileSystemWatcher=new QFileSystemWatcher();
    QObject::connect( m_pFileSystemWatcher, &QFileSystemWatcher::fileChanged, this, &FileWatcher::handleFileChanged );
}

FileWatcher::~FileWatcher()
{
    delete m_pFileSystemWatcher;
}

void FileWatcher::clear()
{
    if(m_pFileSystemWatcher)
        m_pFileSystemWatcher->removePaths(m_pFileSystemWatcher->directories());
}

void FileWatcher::addPaths(const QStringList &files)
{
    if(m_pFileSystemWatcher)
        m_pFileSystemWatcher->addPaths(files);
}

void FileWatcher::addPath(const QString &file)
{
    if(m_pFileSystemWatcher)
        m_pFileSystemWatcher->addPath(file);
}

void FileWatcher::handleFileChanged(QString /*path*/)
{
    //@FIXME:nope.
    ((Session*)m_pSession)->resetWarm();
}
