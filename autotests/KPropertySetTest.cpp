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
#include <KPropertySet>

namespace QTest {
//! For convenience.
bool qCompare(const QByteArray &val1, const char* val2, const char *actual,
                                      const char *expected, const char *file, int line)
{
    return QTest::qCompare(val1, QByteArray(val2), actual, expected, file, line);
}
}

class KPropertySetTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testEmpty();
    void testGroupNameForProperty();
private:
    void testPropertiesForEmpty(const KPropertySet &set, const char *context);
};

void KPropertySetTest::testEmpty()
{
    KPropertySet empty;
    testPropertiesForEmpty(empty, "empty");
    empty.clear();
    testPropertiesForEmpty(empty, "after empty.clear()");

    KPropertySet emptyCopy(empty);
    testPropertiesForEmpty(emptyCopy, "emptyCopy");
    emptyCopy.clear();
    testPropertiesForEmpty(emptyCopy, "after emptyCopy.clear()");

    KPropertySet emptyCopy2;
    emptyCopy2 = KPropertySet();
    testPropertiesForEmpty(emptyCopy2, "emptyCopy2");
    emptyCopy2.clear();
    testPropertiesForEmpty(emptyCopy2, "after emptyCopy2.clear()");

    QObject parent;
    const KPropertySet emptyWithParent(&parent);
    QCOMPARE(&parent, emptyWithParent.parent());
}

void KPropertySetTest::testPropertiesForEmpty(const KPropertySet &set, const char *context)
{
    qDebug() << context;
    QVERIFY(!set.parent());
    QVERIFY(set.isEmpty());
    QCOMPARE(set.count(), 0);
    QVERIFY(!set.isReadOnly());
    QVERIFY(!set.hasVisibleProperties());
    QCOMPARE(set.groupNames().count(), 0);
    QVERIFY(set.propertyValues().isEmpty());
}

void KPropertySetTest::testGroupNameForProperty()
{
    {
        KPropertySet set;
        QVERIFY2(set.groupNameForProperty("foo").isEmpty(), "No property in empty");
    }
    {
        KPropertySet set;
        KProperty *foo = new KProperty("foo");
        set.addProperty(foo);
        QCOMPARE(set.groupNameForProperty("foo"), "common");
        QCOMPARE(set.groupNameForProperty(*foo), "common");
        QVERIFY2(set.groupNameForProperty("bar").isEmpty(), "No property in !empty");

        set.removeProperty(foo);
        QVERIFY2(set.groupNameForProperty("foo").isEmpty(), "No property after removal");
        foo = new KProperty("foo");
        set.addProperty(foo);
        set.removeProperty("foo");
        QVERIFY2(set.groupNameForProperty("foo").isEmpty(), "No property after removal 2");

        foo = new KProperty("foo");
        set.addProperty(foo, "someGroup");
        QCOMPARE(set.groupNameForProperty(*foo), "somegroup");
        QCOMPARE(set.groupNameForProperty("foo"), "somegroup");

        KProperty bar("bar");
        QVERIFY2(set.groupNameForProperty(bar).isEmpty(), "No property in !empty 2");
    }
}

/* TODO
    KPropertySelector::KPropertySelector()
    KPropertySetIterator::KPropertySetIterator(KPropertySet const&)
    KPropertySetIterator::KPropertySetIterator(KPropertySet const&, KPropertySelector const&)
    KPropertySetIterator::operator++()
    KPropertySetIterator::setOrder(KPropertySetIterator::Order)
    KPropertySetIterator::skipNotAcceptable()
    KPropertySetPrivate::KPropertySetPrivate(KPropertySet*)
    KPropertySetPrivate::addProperty(KProperty*, QByteArray const&)
    KPropertySetPrivate::addRelatedProperty(KProperty*, KProperty*) const
    KPropertySetPrivate::addToGroup(QByteArray const&, KProperty*)
    KPropertySetPrivate::clear()
    KPropertySetPrivate::copyAttributesFrom(KPropertySetPrivate const&)
    KPropertySetPrivate::copyPropertiesFrom(QList<KProperty*>::const_iterator const&, QList<KProperty*>::const_iterator const&, KPropertySet const&)
    KPropertySetPrivate::hasGroups() const
    KPropertySetPrivate::indexOfPropertyInGroup(KProperty const*) const
    KPropertySetPrivate::indexOfProperty(KProperty const*) const
    KPropertySetPrivate::informAboutClearing(bool*)
    KPropertySetPrivate::removeFromGroup(KProperty*)
    KPropertySetPrivate::removeProperty(KProperty*)
+    KPropertySet::KPropertySet(KPropertySet const&)
OK    KPropertySet::KPropertySet(QObject*)
    KPropertySet::aboutToBeCleared()
    KPropertySet::aboutToBeDeleted()
    KPropertySet::aboutToDeleteProperty(KPropertySet&, KProperty&)
    KPropertySet::addProperty(KProperty*, QByteArray const&)
    KPropertySet::changePropertyIfExists(QByteArray const&, QVariant const&)
    KPropertySet::changeProperty(QByteArray const&, QVariant const&)
    KPropertySet::clear()
    KPropertySet::contains(QByteArray const&) const
    KPropertySet::count(KPropertySelector const&) const
    KPropertySet::count() const
    KPropertySet::debug() const
    KPropertySet::groupCaption(QByteArray const&) const
    KPropertySet::groupIconName(QByteArray const&) const
OK    KPropertySet::groupNameForProperty(const QByteArray &propertyName) const;
OK    KPropertySet::groupNameForProperty(const KProperty &property) const;
    KPropertySet::groupNames() const
    KPropertySet::hasProperties(KPropertySelector const&) const
    KPropertySet::hasVisibleProperties() const
+    KPropertySet::isEmpty() const
    KPropertySet::isReadOnly() const
    KPropertySet::operator=(KPropertySet const&)
    KPropertySet::operator[](QByteArray const&) const
    KPropertySet::previousSelection() const
    KPropertySet::propertyChangedInternal(KPropertySet&, KProperty&)
    KPropertySet::propertyChanged(KPropertySet&, KProperty&)
    KPropertySet::propertyNamesForGroup(QByteArray const&) const
    KPropertySet::propertyReset(KPropertySet&, KProperty&)
    KPropertySet::propertyValues() const
    KPropertySet::propertyValue(QByteArray const&, QVariant const&) const
    KPropertySet::property(QByteArray const&) const
    KPropertySet::readOnlyFlagChanged()
    KPropertySet::removeProperty(KProperty*)
    KPropertySet::removeProperty(QByteArray const&)
    KPropertySet::setGroupCaption(QByteArray const&, QString const&)
    KPropertySet::setGroupIconName(QByteArray const&, QString const&)
    KPropertySet::setPreviousSelection(QByteArray const&)
    KPropertySet::setReadOnly(bool)
*/

QTEST_GUILESS_MAIN(KPropertySetTest)

#include "KPropertySetTest.moc"
