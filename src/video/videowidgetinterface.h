/*
    Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies). <thierry.bastian@trolltech.com>
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

#ifndef PHONON_VIDEOWIDGETINTERFACE_H
#define PHONON_VIDEOWIDGETINTERFACE_H

#include "videowidget.h"

namespace Phonon {

class VideoWidgetInterface
{
public:
    virtual ~VideoWidgetInterface() {}

    virtual Phonon::VideoWidget::AspectRatio aspectRatio() const = 0;
    virtual void setAspectRatio(Phonon::VideoWidget::AspectRatio) = 0;

    virtual qreal brightness() const = 0;
    virtual void setBrightness(qreal) = 0;

    virtual Phonon::VideoWidget::ScaleMode scaleMode() const = 0;
    virtual void setScaleMode(Phonon::VideoWidget::ScaleMode) = 0;

    virtual qreal contrast() const = 0;
    virtual void setContrast(qreal) = 0;

    virtual qreal hue() const = 0;
    virtual void setHue(qreal) = 0;

    virtual qreal saturation() const = 0;
    virtual void setSaturation(qreal) = 0;

    // FIXME: for the oddest of reasons you cannot move this anywhere else, even
    //        when rebuilding everything it fails to set the size of the widget.
    virtual QWidget *widget() = 0;

    virtual QImage snapshot() const = 0;
};

} // namespace Phonon

Q_DECLARE_INTERFACE(Phonon::VideoWidgetInterface, "org.kde.phonon.VideoWidgetInterface/5.0")

#endif // PHONON_VIDEOWIDGETINTERFACE_H
