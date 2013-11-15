/*
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_OBJECTDESCRIPTION_H
#define PHONON_OBJECTDESCRIPTION_H

#include "phonon_export.h"
#include "phonondefs.h"

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QSharedData>
#include <QtCore/QString>
#include <QtCore/QVariant>

namespace Phonon
{
    enum ObjectDescriptionType
    {
        AudioOutputDeviceType,
        AudioCaptureDeviceType,
        VideoCaptureDeviceType,
        EffectType,
        AudioChannelType,
        SubtitleType

        //VideoOutputDeviceType,
        //AudioCodecType,
        //VideoCodecType,
        //ContainerFormatType,
        //VisualizationType,
    };

class DescriptionBasePrivate;
#warning drop base suffix? reads terribly...
#warning ongoing qobject derivee discussion
class PHONON_EXPORT DescriptionBase
{
public:
    int index() const;
    QString name() const;
    QString description() const;
#warning need QString iconHint name of icon to use for this device
#warning need Capabilities capabilities Video|Audio (in case a device can do both?)
#warning need QString discoverer or Discoverer discoverer (indicating device of source... in 4 this was either backendname or kde)

    bool isValid() const; /* valid description */

#warning operator impl in derivee?
    DescriptionBase &operator=(const DescriptionBase &other);
    bool operator==(const DescriptionBase &other) const;
    bool operator!=(const DescriptionBase &other) const;

#warning ctor/dtor need to be public if we want qlist<foo>
protected:
    DescriptionBase();
    DescriptionBase(const DescriptionBase &other);
    virtual ~DescriptionBase();

    DescriptionBase(DescriptionBasePrivate &dd);
    DescriptionBasePrivate *const k_ptr;

    P_DECLARE_PRIVATE(DescriptionBase)
};

class DeviceDescriptionBasePrivate;
#warning consider turning devices into one class with properties or templatify it derivees only have boilerplate code
#warning drop base suffix? reads terribly...
class PHONON_EXPORT DeviceDescriptionBase : public DescriptionBase
{
public:
    bool isAvailable() const;

    PHONON_DEPRECATED QVariant property(const char *name) const;
    PHONON_DEPRECATED QList<QByteArray> propertyNames() const;

#warning ctor/dtor need to be public if we want qlist<foo>
protected:
    DeviceDescriptionBase();
    virtual ~DeviceDescriptionBase();

    DeviceDescriptionBase(DeviceDescriptionBasePrivate &dd);

    P_DECLARE_PRIVATE(DeviceDescriptionBase)
};

class PHONON_EXPORT AudioOutputDevice : public DeviceDescriptionBase
{
public:
    AudioOutputDevice();
#warning possibly needs available param
    AudioOutputDevice(int index, QString name, QString description);
    ~AudioOutputDevice();

#warning fromindex boogieman - why here why at all what is the beef with globalconfig...
    static AudioOutputDevice fromIndex(int i);
};

class PHONON_EXPORT AudioCaptureDevice : public DeviceDescriptionBase
{
public:
    AudioCaptureDevice();
    AudioCaptureDevice(int index, QString name, QString description);
    ~AudioCaptureDevice();

#warning fromindex boogieman
    static AudioCaptureDevice fromIndex(int i) { return AudioCaptureDevice(0, "unknown ACD", "unknown ACD description"); }
};

class PHONON_EXPORT VideoCaptureDevice : public DeviceDescriptionBase
{
public:
    VideoCaptureDevice();
    VideoCaptureDevice(int index, QString name, QString description);
    ~VideoCaptureDevice();

#warning fromindex boogieman
    static VideoCaptureDevice fromIndex(int i) { return VideoCaptureDevice(0, "unknown VCD", "unknown VCD description"); }
};

class PHONON_EXPORT EffectDescription : public DescriptionBase
{
public:
    EffectDescription(); /* constructs invalid description */
    EffectDescription(int index, QString name, QString description);
};

#warning sub is a stub
class PHONON_EXPORT SubtitleDescription : public DescriptionBase
{
};

#warning audiochannel is a stub
class PHONON_EXPORT AudioChannelDescription : public DescriptionBase
{
};


} //namespace Phonon

Q_DECLARE_METATYPE(Phonon::AudioOutputDevice)
Q_DECLARE_METATYPE(QList<Phonon::AudioOutputDevice>)

#ifndef QT_NO_PHONON_EFFECT
Q_DECLARE_METATYPE(QList<Phonon::EffectDescription>)
Q_DECLARE_METATYPE(Phonon::EffectDescription)
#endif //QT_NO_PHONON_EFFECT

#endif // PHONON_OBJECTDESCRIPTION_H
