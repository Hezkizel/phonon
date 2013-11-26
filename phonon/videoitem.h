/*
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

#ifndef PHONON_VIDEOITEM_H
#define PHONON_VIDEOITEM_H

#include <QQuickItem>

#include "abstractoutput.h"

namespace Phonon {

class VideoItemPrivate;
class PHONON_EXPORT VideoItem : public QQuickItem, public AbstractOutput
{
    Q_OBJECT
public:
    explicit VideoItem(QQuickItem *parent = 0);
    ~VideoItem();

#warning private class
private slots:
    void onFrameReady();

protected:
    virtual QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *updatePaintNodeData);

private:
    P_DECLARE_PRIVATE(VideoItem)
};

} // namespace Phonon

#endif // PHONON_VIDEOITEM_H
