/*
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2011 Harald Sitter <sitter@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Nokia Corporation
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "factory_p.h"

#include "backendinterface.h"
#include "medianode_p.h"
#include "mediaobject.h"
#include "audiooutput.h"
#include "globalstatic_p.h"
#include "objectdescription.h"
#include "platformplugin.h"
#include "phononconfig_p.h"
#include "phononnamespace_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QPluginLoader>
#include <QtCore/QPointer>
#include <QApplication>
#include <QMessageBox>

namespace Phonon
{

class PlatformPlugin;
class FactoryPrivate : public Phonon::Factory::Sender
{
    friend QObject *Factory::backend(bool);
    Q_OBJECT
    public:
        FactoryPrivate();
        ~FactoryPrivate();
        bool createBackend();
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
        PlatformPlugin *platformPlugin();

        PlatformPlugin *m_platformPlugin;
        bool m_noPlatformPlugin;
#endif //QT_NO_PHONON_PLATFORMPLUGIN
        QPointer<QObject> m_backendObject;

        QList<QObject *> objects;
        QList<MediaNodePrivate *> mediaNodePrivateList;

    private Q_SLOTS:
        /**
         * unregisters the backend object
         */
        void objectDestroyed(QObject *);

        void objectDescriptionChanged(ObjectDescriptionType);
};

PHONON_GLOBAL_STATIC(Phonon::FactoryPrivate, globalFactory)

static inline void ensureLibraryPathSet()
{
#ifdef PHONON_LIBRARY_PATH
    static bool done = false;
    if (!done) {
        done = true;
        QCoreApplication::addLibraryPath(QLatin1String(PHONON_LIBRARY_PATH));
    }
#endif // PHONON_LIBRARY_PATH
}

void Factory::setBackend(QObject *b)
{
    Q_ASSERT(globalFactory->m_backendObject == 0);
    globalFactory->m_backendObject = b;
}

bool FactoryPrivate::createBackend()
{
    pDebug() << Q_FUNC_INFO << "Phonon" << PHONON_VERSION_STR << "trying to create backend...";
#ifndef QT_NO_LIBRARY
    Q_ASSERT(m_backendObject == 0);

    // If the user defines a backend with PHONON_BACKEND this overrides the
    // platform plugin (because we cannot influence its lookup priority) and
    // consequently will try to find/load the defined backend manually.
    const QByteArray backendEnv = qgetenv("PHONON_BACKEND");

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    PlatformPlugin *f = globalFactory->platformPlugin();
    if (f && backendEnv.isEmpty()) {
        // TODO: it would be very groovy if we could add a param, so that the
        // platform could also try to load the defined backend as preferred choice.
        m_backendObject = f->createBackend();
    }
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    if (!m_backendObject) {
        ensureLibraryPathSet();

        // could not load a backend through the platform plugin. Falling back to the default
        // (finding the first loadable backend).
        const QLatin1String suffix("/phonon_backend/");
        const QStringList paths = QCoreApplication::libraryPaths();
        for (int i = 0; i < paths.count(); ++i) {
            const QString libPath = paths.at(i) + suffix;
            const QDir dir(libPath);
            if (!dir.exists()) {
                pDebug() << Q_FUNC_INFO << dir.absolutePath() << "does not exist";
                continue;
            }

            QStringList plugins(dir.entryList(QDir::Files));

            if (!backendEnv.isEmpty()) {
                pDebug() << "trying to load:" << backendEnv << "as first choice";
                const int backendIndex = plugins.indexOf(QRegExp(backendEnv + ".*"));
                if (backendIndex != -1)
                    plugins.move(backendIndex, 0);
            }

            foreach (const QString &plugin, plugins) {
                QPluginLoader pluginLoader(libPath + plugin);
                if (!pluginLoader.load()) {
                    pDebug() << Q_FUNC_INFO << "  load failed:"
                             << pluginLoader.errorString();
                    continue;
                }
                pDebug() << pluginLoader.instance();
                m_backendObject = pluginLoader.instance();
                if (m_backendObject) {
                    break;
                }

                // no backend found, don't leave an unused plugin in memory
                pluginLoader.unload();
            }

            if (m_backendObject) {
                break;
            }
        }
        if (!m_backendObject) {
            pWarning() << Q_FUNC_INFO << "phonon backend plugin could not be loaded";
            return false;
        }
    }

    pDebug() << Q_FUNC_INFO
             << "Phonon backend"
             << m_backendObject->property("backendName").toString()
             << "version"
             << m_backendObject->property("backendVersion").toString()
             << "loaded";

    connect(m_backendObject, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
            SLOT(objectDescriptionChanged(ObjectDescriptionType)));

    return true;
#else //QT_NO_LIBRARY
    pWarning() << Q_FUNC_INFO << "Trying to use Phonon with QT_NO_LIBRARY defined. "
                                 "That is currently not supported";
    return false;
#endif
}

FactoryPrivate::FactoryPrivate()
    :
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    m_platformPlugin(0),
    m_noPlatformPlugin(false),
#endif //QT_NO_PHONON_PLATFORMPLUGIN
    m_backendObject(0)
{
    // Add the post routine to make sure that all other global statics (especially the ones from Qt)
    // are still available. If the FactoryPrivate dtor is called too late many bad things can happen
    // as the whole backend might still be alive.
    qAddPostRoutine(globalFactory.destroy);
}

FactoryPrivate::~FactoryPrivate()
{
    for (int i = 0; i < mediaNodePrivateList.count(); ++i) {
        mediaNodePrivateList.at(i)->deleteBackendObject();
    }
    if (objects.size() > 0) {
        pError() << "The backend objects are not deleted as was requested.";
        qDeleteAll(objects);
    }
    delete m_backendObject;
#ifndef QT_NO_PHONON_PLATFORMPLUGIN
    delete m_platformPlugin;
#endif //QT_NO_PHONON_PLATFORMPLUGIN
}

void FactoryPrivate::objectDescriptionChanged(ObjectDescriptionType type)
{
#ifdef PHONON_METHODTEST
    Q_UNUSED(type);
#else
    pDebug() << Q_FUNC_INFO << type;
    switch (type) {
    case AudioOutputDeviceType:
        emit availableAudioOutputDevicesChanged();
        break;
    case AudioCaptureDeviceType:
        emit availableAudioCaptureDevicesChanged();
        break;
    case VideoCaptureDeviceType:
        emit availableVideoCaptureDevicesChanged();
        break;
    default:
        break;
    }
    //emit capabilitiesChanged();
#endif // PHONON_METHODTEST
}

Factory::Sender *Factory::sender()
{
    return globalFactory;
}

void Factory::registerFrontendObject(MediaNodePrivate *bp)
{
    globalFactory->mediaNodePrivateList.prepend(bp); // inserted last => deleted first
}

void Factory::deregisterFrontendObject(MediaNodePrivate *bp)
{
    // The Factory can already be cleaned up while there are other frontend objects still alive.
    // When those are deleted they'll call deregisterFrontendObject through ~BasePrivate
    if (!globalFactory.isDestroyed()) {
        globalFactory->mediaNodePrivateList.removeAll(bp);
    }
}

void FactoryPrivate::objectDestroyed(QObject * obj)
{
    //pDebug() << Q_FUNC_INFO << obj;
    objects.removeAll(obj);
}

#define FACTORY_IMPL(classname) \
QObject *Factory::create ## classname(QObject *parent) \
{ \
    if (backend()) { \
        return registerQObject(qobject_cast<BackendInterface *>(backend())->createObject(BackendInterface::classname##Class, parent)); \
    } \
    return 0; \
}
#define FACTORY_IMPL_1ARG(classname) \
QObject *Factory::create ## classname(int arg1, QObject *parent) \
{ \
    if (backend()) { \
        return registerQObject(qobject_cast<BackendInterface *>(backend())->createObject(BackendInterface::classname##Class, parent, QList<QVariant>() << arg1)); \
    } \
    return 0; \
}

FACTORY_IMPL(Player)
#ifndef QT_NO_PHONON_EFFECT
FACTORY_IMPL_1ARG(Effect)
#endif //QT_NO_PHONON_EFFECT
#ifndef QT_NO_PHONON_VOLUMEFADEREFFECT
FACTORY_IMPL(VolumeFaderEffect)
#endif //QT_NO_PHONON_VOLUMEFADEREFFECT
FACTORY_IMPL(AudioOutput)
#ifndef QT_NO_PHONON_VIDEO
FACTORY_IMPL(VideoWidget)
#endif //QT_NO_PHONON_VIDEO
FACTORY_IMPL(AudioDataOutput)

#undef FACTORY_IMPL

#ifndef QT_NO_PHONON_PLATFORMPLUGIN
PlatformPlugin *FactoryPrivate::platformPlugin()
{
    if (m_platformPlugin) {
        return m_platformPlugin;
    }
    if (m_noPlatformPlugin) {
        return 0;
    }
    Q_ASSERT(QCoreApplication::instance());
    const QByteArray platform_plugin_env = qgetenv("PHONON_PLATFORMPLUGIN");
    if (!platform_plugin_env.isEmpty()) {
        pDebug() << Q_FUNC_INFO << "platform plugin path:" << platform_plugin_env;
        QPluginLoader pluginLoader(QString::fromLocal8Bit(platform_plugin_env.constData()));
        if (pluginLoader.load()) {
            QObject *plInstance = pluginLoader.instance();
            if (!plInstance) {
                pDebug() << Q_FUNC_INFO << "unable to grab root component object for the platform plugin";
            }

            m_platformPlugin = qobject_cast<PlatformPlugin *>(plInstance);
            if (m_platformPlugin) {
                pDebug() << Q_FUNC_INFO << "platform plugin" << m_platformPlugin;
                return m_platformPlugin;
            } else {
                pDebug() << Q_FUNC_INFO << "platform plugin cast fail" << plInstance;
            }
        }
    }
    const QString suffix(QLatin1String("/phonon_platform/"));
    ensureLibraryPathSet();
    QDir dir;
    dir.setNameFilters(
            !qgetenv("KDE_FULL_SESSION").isEmpty() ? QStringList(QLatin1String("kde.*")) :
            (!qgetenv("GNOME_DESKTOP_SESSION_ID").isEmpty() ? QStringList(QLatin1String("gnome.*")) :
             QStringList())
            );
    dir.setFilter(QDir::Files);
    const QStringList libPaths = QCoreApplication::libraryPaths();
    forever {
        for (int i = 0; i < libPaths.count(); ++i) {
            const QString libPath = libPaths.at(i) + suffix;
            dir.setPath(libPath);
            if (!dir.exists()) {
                continue;
            }
            const QStringList files = dir.entryList(QDir::Files);
            for (int i = 0; i < files.count(); ++i) {
                QPluginLoader pluginLoader(libPath + files.at(i));
                if (!pluginLoader.load()) {
                    pDebug() << Q_FUNC_INFO << "  platform plugin load failed:"
                        << pluginLoader.errorString();
                    continue;
                }
                pDebug() << pluginLoader.instance();
                QObject *qobj = pluginLoader.instance();
                m_platformPlugin = qobject_cast<PlatformPlugin *>(qobj);
                pDebug() << m_platformPlugin;
                if (m_platformPlugin) {
                    connect(qobj, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
                            SLOT(objectDescriptionChanged(ObjectDescriptionType)));
                    return m_platformPlugin;
                } else {
                    delete qobj;
                    pDebug() << Q_FUNC_INFO << dir.absolutePath() << "exists but the platform plugin was not loadable:" << pluginLoader.errorString();
                    pluginLoader.unload();
                }
            }
        }
        if (dir.nameFilters().isEmpty()) {
            break;
        }
        dir.setNameFilters(QStringList());
    }
    pDebug() << Q_FUNC_INFO << "platform plugin could not be loaded";
    m_noPlatformPlugin = true;
    return 0;
}

PlatformPlugin *Factory::platformPlugin()
{
    return globalFactory->platformPlugin();
}
#endif // QT_NO_PHONON_PLATFORMPLUGIN

QObject *Factory::backend(bool createWhenNull)
{
    if (globalFactory.isDestroyed()) {
        return 0;
    }
    if (createWhenNull && globalFactory->m_backendObject == 0) {
        globalFactory->createBackend();
    }
    return globalFactory->m_backendObject;
}

#ifndef QT_NO_PROPERTIES
#define GET_STRING_PROPERTY(name) \
QString Factory::name() \
{ \
    if (globalFactory->m_backendObject) { \
        return globalFactory->m_backendObject->property(#name).toString(); \
    } \
    return QString(); \
} \

GET_STRING_PROPERTY(identifier)
GET_STRING_PROPERTY(backendName)
GET_STRING_PROPERTY(backendComment)
GET_STRING_PROPERTY(backendVersion)
GET_STRING_PROPERTY(backendIcon)
GET_STRING_PROPERTY(backendWebsite)
#endif //QT_NO_PROPERTIES
QObject *Factory::registerQObject(QObject *o)
{
    if (o) {
        QObject::connect(o, SIGNAL(destroyed(QObject *)), globalFactory, SLOT(objectDestroyed(QObject *)), Qt::DirectConnection);
        globalFactory->objects.append(o);
    }
    return o;
}

} //namespace Phonon

#include "factory.moc"
#include "moc_factory_p.cpp"
