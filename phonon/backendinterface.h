/*
    Copyright (C) 2005 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2013 Harald Sitter <sitter@kde.org>

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

#ifndef PHONON_BACKENDINTERFACE_H
#define PHONON_BACKENDINTERFACE_H

#include "phonon_export.h"
#include "objectdescription.h"

#include <QtCore/QtGlobal>
#include <QtCore/QSet>

class QVariant;

namespace Phonon {

class BackendInterface
{
public:
    virtual ~BackendInterface() {}

    enum Class {
        AudioDataOutputClass,
        AudioOutputClass,
        AudioSourceControlClass,
        EffectClass,
        PlayerClass,
        VideoDataOutputClass,
        VideoSurfaceOutputClass,
        VideoSourceControlClass,
        VideoWidgetClass,
        VisualizationClass,
        VolumeFaderEffectClass
    };

    virtual QObject *createObject(Class c, QObject *parent, const QList<QVariant> &args = QList<QVariant>()) = 0;

    virtual QList<AudioOutputDevice> audioOutputDevices() const = 0;
    virtual QList<AudioCaptureDevice> audioCaptureDevices() const = 0;
    virtual QList<VideoCaptureDevice> videoCaptureDevices() const = 0;

Q_SIGNALS:
    virtual void audioOutputDevicesChanged() = 0;
    virtual void audioCaptureDevicesChanged() = 0;
    virtual void videoCaptureDevicesChanged() = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::BackendInterface, "org.kde.phonon.BackendInterface/5.0")

#endif // PHONON_BACKENDINTERFACE_H
