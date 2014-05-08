/*
 * Copyright (C) 2014 Lucien XU <sfietkonstantin@free.fr>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * The names of its contributors may not be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include <QtTest/QtTest>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QProcess>

class TestAusmt: public QObject
{
    Q_OBJECT
private:
    bool clear();
    bool prepareSimple();
private slots:
    void testSimple();
    void testReapply();
    void testExternalRemoved();
    void test2Patches();
    void test2PatchesCrossed();
    void cleanupTestCase();
    // Need to write patch for unapplicable patches and conflicting ones
};

bool TestAusmt::clear()
{
    QDir dir ("/opt/ausmt/tests/var");
    if (!dir.removeRecursively()) {
        return false;
    }

    return true;
}

bool TestAusmt::prepareSimple()
{
    if (!clear()) {
        return false;
    }

    if (!QFile::remove("/opt/ausmt/tests/files/simple.qml")) {
        return false;
    }
    if (!QFile::copy("/opt/ausmt/tests/original/simple.qml",
                        "/opt/ausmt/tests/files/simple.qml")) {
        return false;
    }

    return true;
}

void TestAusmt::testSimple()
{
    QVERIFY(prepareSimple());

    // Test the simple patch application stream (apply -> unapply)

    // Apply
    int returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileApply ("/opt/ausmt/tests/files/simple.qml");
    QFile patched ("/opt/ausmt/tests/patched/simple-patch1.qml");
    QVERIFY(fileApply.open(QIODevice::ReadOnly));
    QVERIFY(patched.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply.readAll(), patched.readAll());
    fileApply.close();
    patched.close();

    // Unapply
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply ("/opt/ausmt/tests/files/simple.qml");
    QFile original ("/opt/ausmt/tests/original/simple.qml");
    QVERIFY(fileUnapply.open(QIODevice::ReadOnly));
    QVERIFY(original.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply.readAll(), original.readAll());
    fileUnapply.close();
    original.close();
}

void TestAusmt::testReapply()
{
    QVERIFY(prepareSimple());

    // Test when you try to reapply or reunapply a patch

    // Apply
    int returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileApply ("/opt/ausmt/tests/files/simple.qml");
    QFile patched ("/opt/ausmt/tests/patched/simple-patch1.qml");
    QVERIFY(fileApply.open(QIODevice::ReadOnly));
    QVERIFY(patched.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply.readAll(), patched.readAll());
    fileApply.close();
    patched.close();

    // Reapply
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileApply2 ("/opt/ausmt/tests/files/simple.qml");
    QFile patched2 ("/opt/ausmt/tests/patched/simple-patch1.qml");
    QVERIFY(fileApply2.open(QIODevice::ReadOnly));
    QVERIFY(patched2.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply2.readAll(), patched2.readAll());
    fileApply2.close();
    patched2.close();

    // Unapply
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply ("/opt/ausmt/tests/files/simple.qml");
    QFile original ("/opt/ausmt/tests/original/simple.qml");
    QVERIFY(fileUnapply.open(QIODevice::ReadOnly));
    QVERIFY(original.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply.readAll(), original.readAll());
    fileUnapply.close();
    original.close();

    // Reunapply
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply2 ("/opt/ausmt/tests/files/simple.qml");
    QFile original2 ("/opt/ausmt/tests/original/simple.qml");
    QVERIFY(fileUnapply2.open(QIODevice::ReadOnly));
    QVERIFY(original2.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply2.readAll(), original2.readAll());
    fileUnapply2.close();
    original2.close();
}

void TestAusmt::testExternalRemoved()
{
    QVERIFY(prepareSimple());

    // Test when external source applied or unapplied your patch

    // Perform a false apply (copy file from patched)
    QVERIFY(QFile::remove("/opt/ausmt/tests/files/simple.qml"));
    QVERIFY(QFile::copy("/opt/ausmt/tests/patched/simple-patch1.qml",
                        "/opt/ausmt/tests/files/simple.qml"));

    // Apply
    int returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileApply ("/opt/ausmt/tests/files/simple.qml");
    QFile patched ("/opt/ausmt/tests/patched/simple-patch1.qml");
    QVERIFY(fileApply.open(QIODevice::ReadOnly));
    QVERIFY(patched.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply.readAll(), patched.readAll());
    fileApply.close();
    patched.close();

    // Perform a false unapply (copy file from original)
    QVERIFY(QFile::remove("/opt/ausmt/tests/files/simple.qml"));
    QVERIFY(QFile::copy("/opt/ausmt/tests/original/simple.qml",
                        "/opt/ausmt/tests/files/simple.qml"));

    // Unapply
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply ("/opt/ausmt/tests/files/simple.qml");
    QFile original ("/opt/ausmt/tests/original/simple.qml");
    QVERIFY(fileUnapply.open(QIODevice::ReadOnly));
    QVERIFY(original.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply.readAll(), original.readAll());
    fileUnapply.close();
    original.close();
}

void TestAusmt::test2Patches()
{
    QVERIFY(prepareSimple());

    // Test 2 patches application
    // Apply 1
    int returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileApply1 ("/opt/ausmt/tests/files/simple.qml");
    QFile patched1 ("/opt/ausmt/tests/patched/simple-patch1.qml");
    QVERIFY(fileApply1.open(QIODevice::ReadOnly));
    QVERIFY(patched1.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply1.readAll(), patched1.readAll());
    fileApply1.close();
    patched1.close();

    // Check if orig is here
    QVERIFY(QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages1 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages1.open(QIODevice::ReadOnly));
    QList<QByteArray> allLines = packages1.readAll().trimmed().split('\n');
    packages1.close();
    QCOMPARE(allLines.count(), 1);
    QCOMPARE(allLines.first(), QByteArray("simple-patch1 ausmt-sailfishos-1"));

    QFile fileControl1 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl1.open(QIODevice::ReadOnly));
    allLines = fileControl1.readAll().trimmed().split('\n');
    fileControl1.close();
    QCOMPARE(allLines.count(), 1);
    QList<QByteArray> allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("ausmt-tests"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));

    QFile fileMd5sums1 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums1.open(QIODevice::ReadOnly));
    allLines = fileMd5sums1.readAll().trimmed().split('\n');
    fileMd5sums1.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch1"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    // Apply 2
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch2");
    QCOMPARE(returnCode, 0);
    QFile fileApply2 ("/opt/ausmt/tests/files/simple.qml");
    QFile patched2 ("/opt/ausmt/tests/patched/simple-patch12.qml");
    QVERIFY(fileApply2.open(QIODevice::ReadOnly));
    QVERIFY(patched2.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply2.readAll(), patched2.readAll());
    fileApply2.close();
    patched2.close();

    // Check if orig is here
    QVERIFY(QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages2 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages2.open(QIODevice::ReadOnly));
    allLines = packages2.readAll().trimmed().split('\n');
    packages2.close();
    QCOMPARE(allLines.count(), 2);
    QCOMPARE(allLines.at(0), QByteArray("simple-patch1 ausmt-sailfishos-1"));
    QCOMPARE(allLines.at(1), QByteArray("simple-patch2 ausmt-sailfishos-1"));

    QFile fileControl2 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl2.open(QIODevice::ReadOnly));
    allLines = fileControl2.readAll().trimmed().split('\n');
    fileControl2.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("ausmt-tests"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));

    QFile fileMd5sums2 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums2.open(QIODevice::ReadOnly));
    allLines = fileMd5sums2.readAll().trimmed().split('\n');
    fileMd5sums2.close();
    QCOMPARE(allLines.count(), 2);
    allEntries = allLines.at(0).split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch1"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    allEntries = allLines.at(1).split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch2"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    // Unapply 2
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch2");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply2 ("/opt/ausmt/tests/files/simple.qml");
    QFile unapplied ("/opt/ausmt/tests/patched/simple-patch1.qml");
    QVERIFY(fileUnapply2.open(QIODevice::ReadOnly));
    QVERIFY(unapplied.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply2.readAll(), unapplied.readAll());
    fileUnapply2.close();
    unapplied.close();

    // Check if orig is here
    QVERIFY(QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages3 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages3.open(QIODevice::ReadOnly));
    allLines = packages3.readAll().trimmed().split('\n');
    packages3.close();
    QCOMPARE(allLines.count(), 1);
    QCOMPARE(allLines.first(), QByteArray("simple-patch1 ausmt-sailfishos-1"));

    QFile fileControl3 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl3.open(QIODevice::ReadOnly));
    allLines = fileControl3.readAll().trimmed().split('\n');
    fileControl3.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("ausmt-tests"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));

    QFile fileMd5sums3 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums3.open(QIODevice::ReadOnly));
    allLines = fileMd5sums3.readAll().trimmed().split('\n');
    fileMd5sums3.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch1"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    // Unapply 2
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply1 ("/opt/ausmt/tests/files/simple.qml");
    QFile original ("/opt/ausmt/tests/original/simple.qml");
    QVERIFY(fileUnapply1.open(QIODevice::ReadOnly));
    QVERIFY(original.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply1.readAll(), original.readAll());
    fileUnapply1.close();
    original.close();

    // Check if orig is removed
    QVERIFY(!QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages4 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages4.readAll().trimmed().isEmpty());
    packages4.close();


    QFile fileControl4 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl4.open(QIODevice::ReadOnly));
    QVERIFY(fileControl4.readAll().trimmed().isEmpty());
    fileControl4.close();

    QFile fileMd5sums4 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums4.open(QIODevice::ReadOnly));
    QVERIFY(fileMd5sums4.readAll().trimmed().isEmpty());
    fileMd5sums4.close();
}

void TestAusmt::test2PatchesCrossed()
{
    QVERIFY(prepareSimple());

    // Test 2 patches application (crossed: 1 2 then unapply 1 2)
    // Apply 1
    int returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileApply1 ("/opt/ausmt/tests/files/simple.qml");
    QFile patched1 ("/opt/ausmt/tests/patched/simple-patch1.qml");
    QVERIFY(fileApply1.open(QIODevice::ReadOnly));
    QVERIFY(patched1.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply1.readAll(), patched1.readAll());
    fileApply1.close();
    patched1.close();

    // Check if orig is here
    QVERIFY(QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages1 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages1.open(QIODevice::ReadOnly));
    QList<QByteArray> allLines = packages1.readAll().trimmed().split('\n');
    packages1.close();
    QCOMPARE(allLines.count(), 1);
    QCOMPARE(allLines.first(), QByteArray("simple-patch1 ausmt-sailfishos-1"));

    QFile fileControl1 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl1.open(QIODevice::ReadOnly));
    allLines = fileControl1.readAll().trimmed().split('\n');
    fileControl1.close();
    QCOMPARE(allLines.count(), 1);
    QList<QByteArray> allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("ausmt-tests"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));

    QFile fileMd5sums1 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums1.open(QIODevice::ReadOnly));
    allLines = fileMd5sums1.readAll().trimmed().split('\n');
    fileMd5sums1.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch1"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    // Apply 2
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-install simple-patch2");
    QCOMPARE(returnCode, 0);
    QFile fileApply2 ("/opt/ausmt/tests/files/simple.qml");
    QFile patched2 ("/opt/ausmt/tests/patched/simple-patch12.qml");
    QVERIFY(fileApply2.open(QIODevice::ReadOnly));
    QVERIFY(patched2.open(QIODevice::ReadOnly));
    QCOMPARE(fileApply2.readAll(), patched2.readAll());
    fileApply2.close();
    patched2.close();

    // Check if orig is here
    QVERIFY(QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages2 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages2.open(QIODevice::ReadOnly));
    allLines = packages2.readAll().trimmed().split('\n');
    packages2.close();
    QCOMPARE(allLines.count(), 2);
    QCOMPARE(allLines.at(0), QByteArray("simple-patch1 ausmt-sailfishos-1"));
    QCOMPARE(allLines.at(1), QByteArray("simple-patch2 ausmt-sailfishos-1"));

    QFile fileControl2 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl2.open(QIODevice::ReadOnly));
    allLines = fileControl2.readAll().trimmed().split('\n');
    fileControl2.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("ausmt-tests"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));

    QFile fileMd5sums2 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums2.open(QIODevice::ReadOnly));
    allLines = fileMd5sums2.readAll().trimmed().split('\n');
    fileMd5sums2.close();
    QCOMPARE(allLines.count(), 2);
    allEntries = allLines.at(0).split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch1"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    allEntries = allLines.at(1).split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch2"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    // Unapply 1
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch1");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply2 ("/opt/ausmt/tests/files/simple.qml");
    QFile unapplied ("/opt/ausmt/tests/patched/simple-patch2.qml");
    QVERIFY(fileUnapply2.open(QIODevice::ReadOnly));
    QVERIFY(unapplied.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply2.readAll(), unapplied.readAll());
    fileUnapply2.close();
    unapplied.close();

    // Check if orig is here
    QVERIFY(QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages3 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages3.open(QIODevice::ReadOnly));
    allLines = packages3.readAll().trimmed().split('\n');
    packages3.close();
    QCOMPARE(allLines.count(), 1);
    QCOMPARE(allLines.first(), QByteArray("simple-patch2 ausmt-sailfishos-1"));

    QFile fileControl3 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl3.open(QIODevice::ReadOnly));
    allLines = fileControl3.readAll().trimmed().split('\n');
    fileControl3.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("ausmt-tests"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));

    QFile fileMd5sums3 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums3.open(QIODevice::ReadOnly));
    allLines = fileMd5sums3.readAll().trimmed().split('\n');
    fileMd5sums3.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch2"));
    QCOMPARE(allEntries.at(1), QByteArray("/opt/ausmt/tests/files/simple.qml"));
    QCOMPARE(allEntries.at(2), QByteArray("e07833e21f2bfdabf1786b88d386f3f0985677ebda6f259013214022962469b1"));

    // Unapply 1
    returnCode = QProcess::execute("/opt/ausmt/tests/ausmt-remove simple-patch2");
    QCOMPARE(returnCode, 0);
    QFile fileUnapply1 ("/opt/ausmt/tests/files/simple.qml");
    QFile original ("/opt/ausmt/tests/original/simple.qml");
    QVERIFY(fileUnapply1.open(QIODevice::ReadOnly));
    QVERIFY(original.open(QIODevice::ReadOnly));
    QCOMPARE(fileUnapply1.readAll(), original.readAll());
    fileUnapply1.close();
    original.close();

    // Check if orig is removed
    QVERIFY(!QFile::exists("/opt/ausmt/tests/files/simple.qml.webosinternals.orig"));

    // Also check the metadata
    QFile packages4 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/packages");
    QVERIFY(packages4.readAll().trimmed().isEmpty());
    packages4.close();


    QFile fileControl4 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_control");
    QVERIFY(fileControl4.open(QIODevice::ReadOnly));
    QVERIFY(fileControl4.readAll().trimmed().isEmpty());
    fileControl4.close();

    QFile fileMd5sums4 ("/opt/ausmt/tests/var/lib/patchmanager/ausmt/control/file_md5sums");
    QVERIFY(fileMd5sums4.open(QIODevice::ReadOnly));
    QVERIFY(fileMd5sums4.readAll().trimmed().isEmpty());
    fileMd5sums4.close();
}

void TestAusmt::cleanupTestCase()
{
//    QVERIFY(clear());
}

QTEST_MAIN(TestAusmt)
#include "main.moc"
