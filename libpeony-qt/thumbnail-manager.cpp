#include "thumbnail-manager.h"

#include "file-info-manager.h"

#include "file-watcher.h"

#include <QtConcurrent>
#include <QIcon>
#include <QUrl>

using namespace Peony;

static ThumbnailManager *global_instance = nullptr;

ThumbnailManager::ThumbnailManager(QObject *parent) : QObject(parent)
{

}

ThumbnailManager *ThumbnailManager::getInstance()
{
    if (!global_instance)
        global_instance = new ThumbnailManager;
    return global_instance;
}

void ThumbnailManager::createThumbnail(const QString &uri, FileWatcher *watcher)
{
    //NOTE: we should do createThumbnail() after we have queried the file's info.
    auto info = FileInfo::fromUri(uri);
    if (!info->mimeType().startsWith("image/"))
        return;

    //async
    QtConcurrent::run([=]() {
        QIcon thumbnail;
        QUrl url = uri;
        thumbnail.addFile(url.path());
        if (!thumbnail.isNull()) {
            //add lock
            m_mutex.lock();
            m_hash.remove(uri);
            m_hash.insert(uri, thumbnail);
            auto info = FileInfo::fromUri(uri);
            Q_EMIT info->updated();
            if (watcher) {
                watcher->fileChanged(uri);
            }
            m_mutex.unlock();
        }
    });
}

void ThumbnailManager::releaseThumbnail(const QString &uri)
{
    m_mutex.lock();
    m_hash.remove(uri);
    m_mutex.unlock();
}

const QIcon ThumbnailManager::tryGetThumbnail(const QString &uri)
{
    m_mutex.lock();
    auto icon = m_hash.value(uri);
    m_mutex.unlock();
    return icon;
}
