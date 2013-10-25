/*
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2011-2013 Harald Sitter <sitter@kde.org>

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
#include "frontend_p.h"
#include "globalstatic_p.h"
#include "platformplugin.h"
#include "phononnamespace_p.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QPluginLoader>

namespace Phonon {

class FactoryPrivate : public Phonon::Factory::Sender
{
    Q_OBJECT
public:
    FactoryPrivate();
    ~FactoryPrivate();

    bool createBackend();
#warning why is there a createBackend but no createPPlugin... why is it a gateway function....
    PlatformPlugin *platformPlugin();

    QObject *backendObject;
    BackendInterface *interface;

    QList<QObject *> objects;
    QList<FrontendPrivate *> frontendPrivates;

private Q_SLOTS:
    void objectDestroyed(QObject *);
    void objectDescriptionChanged(ObjectDescriptionType);

private:
    // Note that platformPlugin members are private because one must use
    // platformPlugin().
    PlatformPlugin *m_platformPlugin;
    bool m_noPlatformPluginFound; /* Set if initial creation failed. */
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

bool FactoryPrivate::createBackend()
{
    pDebug() << Q_FUNC_INFO << "Phonon" << PHONON_VERSION_STR << "trying to create backend...";
#ifndef QT_NO_LIBRARY
    Q_ASSERT(backendObject == 0);

    // If the user defines a backend with PHONON_BACKEND this overrides the
    // platform plugin (because we cannot influence its lookup priority) and
    // consequently will try to find/load the defined backend manually.
    const QByteArray backendEnv = qgetenv("PHONON_BACKEND");

    PlatformPlugin *f = globalFactory->platformPlugin();
    if (f && backendEnv.isEmpty()) {
        // TODO: it would be very groovy if we could add a param, so that the
        // platform could also try to load the defined backend as preferred choice.
        backendObject = f->createBackend();
    }

    if (!backendObject) {
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
                backendObject = pluginLoader.instance();
                if (backendObject) {
                    break;
                }

                // no backend found, don't leave an unused plugin in memory
                pluginLoader.unload();
            }

            if (backendObject) {
                break;
            }
        }
        if (!backendObject) {
            pWarning() << Q_FUNC_INFO << "phonon backend plugin could not be loaded";
            return false;
        }
    }

    pDebug() << Q_FUNC_INFO
             << "Phonon backend"
             << backendObject->property("backendName").toString()
             << "version"
             << backendObject->property("backendVersion").toString()
             << "loaded";

    connect(backendObject, SIGNAL(objectDescriptionChanged(ObjectDescriptionType)),
            SLOT(objectDescriptionChanged(ObjectDescriptionType)));

    return true;
#else //QT_NO_LIBRARY
    pWarning() << Q_FUNC_INFO << "Trying to use Phonon with QT_NO_LIBRARY defined. "
                                 "That is currently not supported";
    return false;
#endif
}

FactoryPrivate::FactoryPrivate()
    : m_platformPlugin(0)
    , m_noPlatformPluginFound(false)
    , backendObject(0)
    , interface(0)
{
    // Add the post routine to make sure that all other global statics (especially the ones from Qt)
    // are still available. If the FactoryPrivate dtor is called too late many bad things can happen
    // as the whole backend might still be alive.
    qAddPostRoutine(globalFactory.destroy);
}

FactoryPrivate::~FactoryPrivate()
{
    for (int i = 0; i < frontendPrivates.count(); ++i) {
        frontendPrivates.at(i)->deleteBackendObject();
    }
    if (objects.size() > 0) {
        pError() << "The backend objects are not deleted as was requested.";
        qDeleteAll(objects);
    }
    delete backendObject;
    delete m_platformPlugin;
}

void FactoryPrivate::objectDescriptionChanged(ObjectDescriptionType type)
{
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
}

Factory::Sender *Factory::sender()
{
    return globalFactory;
}

void Factory::registerFrontendObject(FrontendPrivate *bp)
{
    globalFactory->frontendPrivates.prepend(bp); // inserted last => deleted first
}

void Factory::deregisterFrontendObject(FrontendPrivate *bp)
{
    // The Factory can already be cleaned up while there are other frontend objects still alive.
    // When those are deleted they'll call deregisterFrontendObject through ~BasePrivate
    if (!globalFactory.isDestroyed()) {
        globalFactory->frontendPrivates.removeAll(bp);
    }
}

void FactoryPrivate::objectDestroyed(QObject * obj)
{
    //pDebug() << Q_FUNC_INFO << obj;
    objects.removeAll(obj);
}

QObject *Factory::createPlayer(QObject *parent)
{
    if (!backend())
        return 0;
    return registerQObject(interface()->createObject(BackendInterface::PlayerClass, parent));
}

QObject *Factory::createEffect(int effectId, QObject *parent)
{
    if (!backend())
        return 0;
    return registerQObject(interface()->createObject(BackendInterface::EffectClass, parent, QList<QVariant>() << effectId));
}

QObject *Factory::createVolumeFaderEffect(QObject *parent)
{
    if (!backend())
        return 0;
    return registerQObject(interface()->createObject(BackendInterface::VolumeFaderEffectClass, parent));
}

QObject *Factory::createAudioOutput(QObject *parent)
{
    if (!backend())
        return 0;
    return registerQObject(interface()->createObject(BackendInterface::AudioOutputClass, parent));
}

QObject *Factory::createVideoWidget(QObject *parent)
{
    if (!backend())
        return 0;
    return registerQObject(interface()->createObject(BackendInterface::VideoWidgetClass, parent));
}

QObject *Factory::createAudioDataOutput(QObject *parent)
{
    if (!backend())
        return 0;
    return registerQObject(interface()->createObject(BackendInterface::AudioDataOutputClass, parent));
}

PlatformPlugin *FactoryPrivate::platformPlugin()
{
    if (m_platformPlugin) {
        return m_platformPlugin;
    }
    if (m_noPlatformPluginFound) {
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
    m_noPlatformPluginFound = true;
    return 0;
}

PlatformPlugin *Factory::platformPlugin()
{
    return globalFactory->platformPlugin();
}

QObject *Factory::backend()
{
    if (globalFactory.isDestroyed())
        return 0;

    if (globalFactory->backendObject == 0) {
        globalFactory->createBackend();
        if (globalFactory->backendObject)
            globalFactory->interface = qobject_cast<BackendInterface *>(globalFactory->backendObject);
    }

    return globalFactory->backendObject;
}

BackendInterface *Factory::interface()
{
    if (globalFactory.isDestroyed())
        return 0;

    return globalFactory->interface;
}

QObject *Factory::registerQObject(QObject *o)
{
    if (o) {
        QObject::connect(o, SIGNAL(destroyed(QObject *)),
                         globalFactory, SLOT(objectDestroyed(QObject *)),
                         Qt::DirectConnection);
        globalFactory->objects.append(o);
    }
    return o;
}

} // namespace Phonon

#include "factory.moc"
#include "moc_factory_p.cpp"
