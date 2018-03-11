/* This file is part of the KDE project
   Copyright (C) 2017 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QtTest>
#include <KProperty>

class KPropertyTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testNull();
private:
};

void KPropertyTest::testNull()
{
    {
        // isNull
        const KProperty nullProperty;
        QVERIFY(nullProperty.isNull());
        const KProperty nullPropertyCopy(nullProperty);
        QVERIFY(nullPropertyCopy.isNull());
        QVERIFY2(nullProperty == nullPropertyCopy, "two null properties are equal");
        QVERIFY2(nullProperty == KProperty(), "two null properties are equal v2");
        QVERIFY2(!(nullProperty != nullPropertyCopy), "two null properties are equal v2");
    }
    {
        // name
        const KProperty nullProperty;
        const KProperty nullPropertyCopy(nullProperty);
        KProperty notNullProperty("notNull");
        QVERIFY(!notNullProperty.isNull());
        QVERIFY2(!(nullProperty == notNullProperty), "null property is not equal to not null property");
        notNullProperty.setName(QByteArray());
        QVERIFY2(notNullProperty.isNull(), "Not null property becomes null after setting empty name");
        notNullProperty.setName("");
        QVERIFY2(notNullProperty.isNull(), "Not null property becomes null after setting empty name v2");
        notNullProperty.setName("other");
        QVERIFY2(!notNullProperty.isNull(), "Null property becomes not-null after setting non-empty name");
        QVERIFY(nullProperty.name().isEmpty());
        QVERIFY(nullPropertyCopy.name().isEmpty());
    }
    {
        // caption
        const KProperty nullProperty;
        const KProperty nullPropertyCopy(nullProperty);
        KProperty notNullProperty("notNull");
        QVERIFY(nullProperty.caption().isEmpty());
        QVERIFY(nullPropertyCopy.caption().isEmpty());
        QVERIFY(notNullProperty.caption().isEmpty());
        KProperty propertyWithCaption("withCaption", 17, "My caption");
        QVERIFY(!propertyWithCaption.isNull());
        propertyWithCaption.setName(QByteArray());
        QVERIFY2(propertyWithCaption.isNull(), "Property with name and caption is null after setting "
                                               "empty name");
    }
    {
        // caption does not influence null
        KProperty propertyWithCaption("withCaption", 17, "My caption");
        propertyWithCaption.setCaption(QByteArray());
        QVERIFY2(!propertyWithCaption.isNull(), "Property with name and caption is still not null "
                                                "after setting empty caption");
    }
    {
        // description
        KProperty nullProperty;
        nullProperty.setDescription("My desc");
        QVERIFY2(nullProperty.isNull(), "Null property with non-empty description is still null");
    }
    {
        // value
        KProperty nullProperty;
        QTest::ignoreMessage(QtWarningMsg, "COULD NOT SET value to a null property");
        nullProperty.setValue(0.123);
        QTest::ignoreMessage(QtWarningMsg, "COULD NOT SET value to a null property");
        KProperty nullPropertyWithValue(QByteArray(), 177);
        QVERIFY2(nullPropertyWithValue.isNull(), "Null property with value set from ctor is still null");
        QVERIFY2(nullPropertyWithValue.value().isNull(), "Null property with value set from "
                                                                "ctor still has no value");
    }
}

/* TODO
    KProperty::KProperty(KProperty const&)
    KProperty::KProperty(QByteArray const&, KPropertyListData*, QVariant const&, QString const&, QString const&, int, KProperty*)
    KProperty::KProperty(QByteArray const&, QStringList const&, QStringList const&, QVariant const&, QString const&, QString const&, int, KProperty*)
    KProperty::KProperty(QByteArray const&, QVariant const&, QString const&, QString const&, int, KProperty*)
    KProperty::KProperty()
    KProperty::captionForDisplaying() const
    KProperty::caption() const
    KProperty::childValueChanged(KProperty*, QVariant const&, bool)
    KProperty::children() const
    KProperty::child(QByteArray const&)
    KProperty::clearModifiedFlag()
    KProperty::composedProperty() const
    KProperty::description() const
    KProperty::hasOptions() const
    KProperty::iconName() const
    KProperty::isModified() const
OK  KProperty::isNull() const
    KProperty::isReadOnly() const
    KProperty::isStorable() const
    KProperty::isVisible() const
    KProperty::listData() const
    KProperty::name() const
    KProperty::oldValue() const
    KProperty::operator=(KProperty const&)
    KProperty::operator=(QVariant const&)
    KProperty::operator==(KProperty const&) const
    KProperty::option(char const*, QVariant const&) const
    KProperty::parent() const
    KProperty::resetValue()
    KProperty::setCaption(QString const&)
    KProperty::setComposedProperty(KComposedPropertyInterface*)
    KProperty::setDescription(QString const&)
    KProperty::setIconName(QString const&)
    KProperty::setListData(KPropertyListData*)
    KProperty::setListData(QStringList const&, QStringList const&)
    KProperty::setName(QByteArray const&)
    KProperty::setOption(char const*, QVariant const&)
    KProperty::setReadOnly(bool)
    KProperty::setStorable(bool)
    KProperty::setType(int)
    KProperty::setValueSyncPolicy(KProperty::ValueSyncPolicy)
    KProperty::setValue(QVariant const&, QFlags<KProperty::ValueOption>)
    KProperty::setVisible(bool)
    KProperty::type() const
    KProperty::valueEqualsTo(QVariant const&, QFlags<KProperty::ValueOption>) const
    KProperty::valueSyncPolicy() const
    KProperty::value() const

    KPropertyListData::~KPropertyListData()
    KPropertyListData::KPropertyListData(QList<QVariant>, QStringList const&)
    KPropertyListData::KPropertyListData(QStringList const&, QStringList const&)
    KPropertyListData::KPropertyListData()
    KPropertyListData::keysAsStringList() const
    KPropertyListData::setKeysAsStringList(QStringList const&)
*/

/* Full API TODO:

KComposedPropertyCreatorInterface::~KComposedPropertyCreatorInterface()
KComposedPropertyCreatorInterface::KComposedPropertyCreatorInterface()
KComposedPropertyInterface::~KComposedPropertyInterface()
KComposedPropertyInterface::KComposedPropertyInterface(KProperty*)
KPropertySetBuffer::KPropertySetBuffer(KPropertySet const&)
KPropertySetBuffer::KPropertySetBuffer()
KPropertySetBuffer::init(KPropertySet const&)
KPropertySetBuffer::intersectedChanged(KPropertySet&, KProperty&)
KPropertySetBuffer::intersectedReset(KPropertySet&, KProperty&)
KPropertySetBuffer::intersect(KPropertySet const&)
KPropertyFactoryManager::~KPropertyFactoryManager()
KPropertyFactoryManager::KPropertyFactoryManager()
KPropertyFactoryManager::addInitFunction(void (*)())
KPropertyFactoryManager::canConvertValueToText(KProperty const*) const
KPropertyFactoryManager::canConvertValueToText(int) const
KPropertyFactoryManager::createComposedProperty(KProperty*)
KPropertyFactoryManager::propertyValueToLocalizedString(KProperty const*) const
KPropertyFactoryManager::propertyValueToString(KProperty const*) const
KPropertyFactoryManager::registerFactory(KPropertyFactory*)
KPropertyFactoryManager::self()
KPropertyFactoryManager::valueToLocalizedString(int, QVariant const&) const
KPropertyFactoryManager::valueToString(int, QVariant const&) const
KPropertyFactory::~KPropertyFactory()
KPropertyFactory::KPropertyFactory()
KPropertyFactory::addComposedPropertyCreatorInternal(int, KComposedPropertyCreatorInterface*, bool)
KPropertyFactory::addComposedPropertyCreator(int, KComposedPropertyCreatorInterface*)
KPropertyFactory::addDisplayInternal(int, KPropertyValueDisplayInterface*, bool)
KPropertyFactory::addDisplay(int, KPropertyValueDisplayInterface*)
KPropertyFactory::composedPropertyCreators() const
KPropertyFactory::valueDisplays() const
KPropertyUtils::keyForEnumValue(char const*, int)
KPropertyValueDisplayInterface::~KPropertyValueDisplayInterface()
KPropertyValueDisplayInterface::KPropertyValueDisplayInterface()
KPropertyValueDisplayInterface::maxStringValueLength()
KPropertyValueDisplayInterface::valueToLocalizedString(QVariant const&)
*/

QTEST_GUILESS_MAIN(KPropertyTest)

#include "KPropertyTest.moc"
