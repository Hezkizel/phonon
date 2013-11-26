/*
    Copyright (C) 2004-2007 Matthias Kretz <kretz@kde.org>
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

#ifndef PHONON_FACTORY_P_H
#define PHONON_FACTORY_P_H

class QObject;

namespace Phonon {

class BackendInterface;

namespace Factory {

#warning consider creating an int command...
QObject *createPlayer(QObject *parent = 0);
QObject *createEffect(int effectId, QObject *parent = 0);
QObject *createVolumeFaderEffect(QObject *parent = 0);
QObject *createAudioOutput(QObject *parent = 0);
QObject *createVideoWidget(QObject *parent = 0);
QObject *createAudioDataOutput(QObject *parent = 0);

QObject *backend();
BackendInterface *interface();

} // namespace Factory
} // namespace Phonon

#endif // PHONON_FACTORY_P_H
