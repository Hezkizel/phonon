/*
 * This file was generated by dbusidl2cpp version 0.3
 * when processing input file org.kde.MixerIface.xml
 *
 * dbusidl2cpp is Copyright (C) 2006 Trolltech AS. All rights reserved.
 *
 * This is an auto-generated file.
 */

#include "mixeradaptor.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

/*
 * Implementation of adaptor class MixerIfaceAdaptor
 */

MixerIfaceAdaptor::MixerIfaceAdaptor(QObject *parent)
   : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

MixerIfaceAdaptor::~MixerIfaceAdaptor()
{
    // destructor
}

double MixerIfaceAdaptor::volume() const
{
    // get the value of property volume
    return qvariant_cast< double >(object()->property("volume"));
}

void MixerIfaceAdaptor::setVolume(double value)
{
    // set the value of property volume
    object()->setProperty("volume", value);
}

QString MixerIfaceAdaptor::categoryName()
{
    // handle method call org.kde.Phonon.MixerIface.categoryName
    QString categoryName;
    QMetaObject::invokeMethod(object(), "categoryName", Q_RETURN_ARG(QString, categoryName));

    // Alternative:
    //categoryName = static_cast<YourObjectType *>(object())->categoryName();
    return categoryName;
}

QString MixerIfaceAdaptor::name()
{
    // handle method call org.kde.Phonon.MixerIface.name
    QString name;
    QMetaObject::invokeMethod(object(), "name", Q_RETURN_ARG(QString, name));

    // Alternative:
    //name = static_cast<YourObjectType *>(object())->name();
    return name;
}


#include "mixeradaptor.moc"
