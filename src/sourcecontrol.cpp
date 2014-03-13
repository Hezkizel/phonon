/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Casian Andrei <skeletk13@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), Nokia Corporation
 * (or its successors, if any) and the KDE Free Qt Foundation, which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sourcecontrol.h"
#include "sourcecontrol_p.h"

#include "factory_p.h"

namespace Phonon
{


/*
 * SourceControl
 */

SourceControl::SourceControl(SourceControlPrivate &pd, QObject *parent)
    : QObject(parent)
    , Frontend(pd)
{

}

SourceControl::~SourceControl()
{

}

bool SourceControl::isActive() const
{
    P_D(const SourceControl);
    if (!d->m_scInterface)
        return false;
    return d->m_scInterface->isActive();
}

Source SourceControl::source() const
{
    P_D(const SourceControl);
    return d->m_source;
}

// private

SourceControlPrivate::SourceControlPrivate(Source &source)
    : m_scInterface(0)
    , m_source(source)
{

}

SourceControlPrivate::~SourceControlPrivate()
{

}


/*
 * Video Source Control
 */

VideoSourceControl::VideoSourceControl(Source &source, QObject *parent)
    : SourceControl(*new VideoSourceControlPrivate(source), parent)
{

}

VideoSourceControl::~VideoSourceControl()
{

}

bool VideoSourceControl::supportsMenus() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return false;
    return d->m_interface->supportsMenus();
}

QSet<VideoSourceControl::Menu> VideoSourceControl::availableMenus() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return QSet<Menu>();
    return d->m_interface->availableMenus();
}

VideoSourceControl::Menu VideoSourceControl::currentMenu() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return RootMenu;
    return d->m_interface->currentMenu();
}

void VideoSourceControl::setCurrentMenu(Menu menu)
{
    P_D(VideoSourceControl);
    if (!d->m_interface)
        return;
    d->m_interface->setCurrentMenu(menu);
}

bool VideoSourceControl::supportsChapters() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return false;
    return d->m_interface->supportsChapters();
}

int VideoSourceControl::chapterCount() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->chapterCount();
}

int VideoSourceControl::currentChapter() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->currentChapter();
}

void VideoSourceControl::setCurrentChapter(int chapterNumber)
{
    P_D(VideoSourceControl);
    if (!d->m_interface)
        return;
    d->m_interface->setCurrentChapter(chapterNumber);
}

bool VideoSourceControl::supportsAngles() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return false;
    return d->m_interface->supportsAngles();
}

int VideoSourceControl::angleCount() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->angleCount();
}

int VideoSourceControl::currentAngle()
{
    P_D(VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->currentAngle();
}

void VideoSourceControl::setCurrentAngle(int angleNumber)
{
    P_D(VideoSourceControl);
    if (!d->m_interface)
        return;
    d->m_interface->setCurrentAngle(angleNumber);
}

bool VideoSourceControl::supportsTitles() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return false;
    return d->m_interface->supportsTitles();
}

bool VideoSourceControl::isAutoplayingTitles() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return false;
    return d->m_interface->isAutoplayingTitles();
}

void VideoSourceControl::setAutoplayTitles(bool enable)
{
    P_D(VideoSourceControl);
    if (!d->m_interface)
        return;
    d->m_interface->setAutoplayTitles(enable);
}

int VideoSourceControl::titleCount() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->titleCount();
}

int VideoSourceControl::currentTitle() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->currentTitle();
}

void VideoSourceControl::setCurrentTitle(int titleNumber)
{
    P_D(VideoSourceControl);
    if (!d->m_interface)
        return;
    d->m_interface->setCurrentTitle(titleNumber);
}

bool VideoSourceControl::supportsAudioChannels() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return false;
    return d->m_interface->supportsAudioChannels();
}

int VideoSourceControl::audioChannelCount() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->audioChannelCount();
}

int VideoSourceControl::audioChannel() const
{
    P_D(const VideoSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->audioChannel();
}

void VideoSourceControl::setAudioChannel(int channelNumber)
{
    P_D(VideoSourceControl);
    if (!d->m_interface)
        return;
    return d->m_interface->setAudioChannel(channelNumber);
}

// private

VideoSourceControlPrivate::VideoSourceControlPrivate(Source &source)
    : SourceControlPrivate(source)
    , m_currentMenu(VideoSourceControl::RootMenu)
    , m_currentChapter(0)
    , m_currentAngle(0)
    , m_currentTitle(0)
    , m_autoplayTitlesEnabled(false)
    , m_audioChannel(0)
{

}

VideoSourceControlPrivate::~VideoSourceControlPrivate()
{

}

void VideoSourceControlPrivate::createBackendObject()
{
    if (m_backendObject)
        return;

    P_Q(VideoSourceControl);
    m_backendObject = Factory::createVideoSourceControl(q);
    m_interface = qobject_cast<VideoSourceControlInterface *>(m_backendObject);
    m_scInterface = static_cast<SourceControlInterface *>(m_interface);
    if (m_backendObject && m_interface)
        setupBackendObject();
}

void VideoSourceControlPrivate::setupBackendObject()
{
    P_Q(VideoSourceControl);
    Q_ASSERT(m_backendObject);

    QObject::connect(m_backendObject, SIGNAL(availableMenusChanged()),
                     q, SLOT(availableMenusChanged()),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(currentMenuChanged(VideoSourceControl::Menu)),
                     q, SLOT(currentMenuChanged(VideoSourceControl::Menu)),
                     Qt::QueuedConnection);

    QObject::connect(m_backendObject, SIGNAL(chapterCountChanged(int)),
                     q, SLOT(chapterCountChanged(int)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(currentChapterChanged(int)),
                     q, SLOT(currentChapterChanged(int)),
                     Qt::QueuedConnection);

    QObject::connect(m_backendObject, SIGNAL(angleCountChanged(int)),
                     q, SLOT(angleCountChanged(int)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(currentAngleChanged(int)),
                     q, SLOT(currentAngleChanged(int)),
                     Qt::QueuedConnection);

    QObject::connect(m_backendObject, SIGNAL(titleCountChanged(int)),
                     q, SLOT(titleCountChanged(int)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(currentTitleChanged(int)),
                     q, SLOT(currentTitleChanged(int)),
                     Qt::QueuedConnection);

    QObject::connect(m_backendObject, SIGNAL(audioChannelCountChanged(int)),
                     q, SLOT(audioChannelCountChanged(int)),
                     Qt::QueuedConnection);
}


/*
 * Audio Source Control
 */

AudioSourceControl::AudioSourceControl(Source &source, QObject *parent)
    : SourceControl(*new AudioSourceControlPrivate(source), parent)
{

}

AudioSourceControl::~AudioSourceControl()
{

}

bool AudioSourceControl::supportsTracks() const
{
    P_D(const AudioSourceControl);
    if (!d->m_interface)
        return false;
    return d->m_interface->supportsTracks();
}

int AudioSourceControl::trackCount() const
{
    P_D(const AudioSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->trackCount();
}

int AudioSourceControl::trackNumber() const
{
    P_D(const AudioSourceControl);
    if (!d->m_interface)
        return -1;
    return d->m_interface->trackNumber();
}

void AudioSourceControl::setTrackNumber(int trackNumber)
{
    P_D(AudioSourceControl);
    if (!d->m_interface)
        return;
    d->m_interface->setTrackNumber(trackNumber);
}

// private

AudioSourceControlPrivate::AudioSourceControlPrivate(Source &source)
    : SourceControlPrivate(source)
    , m_currentTrack(0)
{

}

AudioSourceControlPrivate::~AudioSourceControlPrivate()
{

}

void AudioSourceControlPrivate::createBackendObject()
{
    if (m_backendObject)
        return;

    P_Q(AudioSourceControl);
    m_backendObject = Factory::createAudioSourceControl(q);
    m_interface = qobject_cast<AudioSourceControlInterface *>(m_backendObject);
    m_scInterface = static_cast<SourceControlInterface *>(m_interface);
    if (m_backendObject && m_interface)
       setupBackendObject();
}

void AudioSourceControlPrivate::setupBackendObject()
{
    P_Q(AudioSourceControl);
    Q_ASSERT(m_backendObject);

    QObject::connect(m_backendObject, SIGNAL(trackCountChanged(int)),
                     q, SLOT(trackCountChanged(int)),
                     Qt::QueuedConnection);
    QObject::connect(m_backendObject, SIGNAL(currentTrackChanged(int)),
                     q, SLOT(currentTrackChanged(int)),
                     Qt::QueuedConnection);
}

} // Phonon namespace
