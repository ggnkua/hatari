#ifndef FILEWATCHER_H
#define FILEWATCHER_H

#include <QObject>

class QString;
class QFileSystemWatcher;
class Session;

class FileWatcher : public QObject
{
	Q_OBJECT
public:
    FileWatcher(const Session* pSession);
    ~FileWatcher();

    void handleFileChanged(QString path);
    const Session* m_pSession;
    QFileSystemWatcher* m_pFileSystemWatcher;

    void clear();

    void addPaths(const QStringList &files);

    void addPath(const QString &file);
};

#endif // FILEWATCHER_H
