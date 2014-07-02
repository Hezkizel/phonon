/*  This file is part of the KDE project
    Copyright (C) 2010 Trever Fischer <tdfischer@fedoraproject.org>

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
#include "player.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include <phonon/audiooutput.h>
#include <phonon/player.h>
#include <phonon/seekslider.h>
#include <phonon/videowidget.h>

Player::Player(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    m_player = new Phonon::Player(this);

    Phonon::AudioOutput* audioOut = new Phonon::AudioOutput(Phonon::VideoCategory, this);
    Phonon::VideoWidget* videoOut = new Phonon::VideoWidget(this);

    videoOut->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //By default, there is no minimum size on a video widget. While the default
    //size is controlled by Qt's layouting system it makes sense to provide a
    //minimum size, so that the widget does not disappear, when the user resizes
    //the window.
    videoOut->setMinimumSize(64, 64);

    //After a MediaSource is loaded, this signal will be emitted to let us know
    //if a video stream was found.
    connect(m_player, SIGNAL(hasVideoChanged(bool)), videoOut, SLOT(setVisible(bool)));

    //This widget will contain the stop/pause buttons
    QWidget *buttonBar = new QWidget(this);

    m_playPause = new QPushButton(tr("Play"), buttonBar);
    m_stop = new QPushButton(tr("Stop"), buttonBar);

    // FIXME seek slider
//    Phonon::SeekSlider *seekSlider = new Phonon::SeekSlider(this);
//    seekSlider->setMediaObject(m_player);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(videoOut);
//    layout->addWidget(seekSlider);
    layout->addWidget(buttonBar);
    setLayout(layout);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonBar);
    buttonLayout->addWidget(m_stop);
    buttonLayout->addWidget(m_playPause);
    buttonBar->setLayout(buttonLayout);

    m_stop->setEnabled(false);

    connect(m_stop, SIGNAL(clicked(bool)), m_player, SLOT(stop()));
    connect(m_playPause, SIGNAL(clicked(bool)), this, SLOT(playPause()));

    //The mediaStateChanged slot will update the GUI elements to reflect what
    //the user can do next
    connect(m_player, SIGNAL(stateChanged(Phonon::State, Phonon::State)), this, SLOT(mediaStateChanged(Phonon::State, Phonon::State)));
}

void Player::playPause()
{
    if (m_player->state() == Phonon::PlayingState) {
        m_player->pause();
    } else {
        if (m_player->source().deviceType() == Phonon::Source::NoDevice)
            load();
        m_player->play();
    }
}

void Player::load(const QUrl &mrl)
{
    if (mrl.scheme().isEmpty())
        m_player->setSource(QUrl::fromLocalFile(mrl.toString()));
    else
        m_player->setSource(mrl);
    m_player->play();
}

void Player::load()
{
    QString url = QFileDialog::getOpenFileName(this);
    if (url.isEmpty())
        return;
    load(QUrl::fromLocalFile(url));
}

void Player::mediaStateChanged(Phonon::State newState, Phonon::State oldState)
{
    Q_UNUSED(oldState);
    switch(newState) {
    case Phonon::StoppedState:
        m_playPause->setText(tr("Play"));
        m_stop->setEnabled(false);
        break;
    case Phonon::PlayingState:
        m_playPause->setText(tr("Pause"));
        m_stop->setEnabled(true);
        break;
    case Phonon::PausedState:
        m_playPause->setText(tr("Play"));
        break;
    }
}
