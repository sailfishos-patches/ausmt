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
#include <QtCore/QStandardPaths>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

static const char *AUSMTSRC_SUBDIR = "ausmtsrc";
static const char *AUSMT_SUBDIR = "ausmt";
static const char *AUSMT_VAR_SUBDIR = "var";
static const char *AUSMT_PATCHES_SUBDIR = "patches";
static const char *AUSMT_FILES_SUBDIR = "files";
static const char *CONSTANTS_ROOT_SH = "constants_root.sh";
static const char *CONSTANTS_EXEC_SH = "constants_exec.sh";
static const char *PYTHON = "python3";
static const char *BUILD_SH = "build";
static const char *MAKE_PATCH_SH = "make_patch.sh";
static const char *GEN_MD5SUM_SH = "gen_md5sum.sh";

static const char *AUSMT_INSTALL = "ausmt-install";
static const char *AUSMT_REMOVE = "ausmt-remove";

class TestAusmt: public QObject
{
    Q_OBJECT
private:
    void prepareSimple();
    void checkSimple1Applied();
    void checkSimple2Applied();
    void checkSimple12Applied();
    void checkUnapplied();
private slots:
    void initTestCase();
    void testSimple();
    void testReapply();
    void testExternalRemoved();
    void test2Patches();
    void test2PatchesCrossed();
    void cleanupTestCase();
    // Need to write patch for unapplicable patches and conflicting ones
};

#define GET_DIR \
    QDir dir (QStandardPaths::writableLocation(QStandardPaths::TempLocation)); \
    if (!dir.exists(AUSMT_SUBDIR)) { \
        QVERIFY(dir.mkdir(AUSMT_SUBDIR)); \
    } \
    QVERIFY(dir.cd(AUSMT_SUBDIR))

#define GET_VAR_DIR \
    QDir varDir (dir); \
    if (!varDir.exists(AUSMT_VAR_SUBDIR)) { \
        QVERIFY(varDir.mkdir(AUSMT_VAR_SUBDIR)); \
    } \
    QVERIFY(varDir.cd(AUSMT_VAR_SUBDIR))

#define GET_PATCHES_DIR \
    QDir patchesDir (dir); \
    if (!patchesDir.exists(AUSMT_PATCHES_SUBDIR)) { \
        QVERIFY(patchesDir.mkdir(AUSMT_PATCHES_SUBDIR)); \
    } \
    QVERIFY(patchesDir.cd(AUSMT_PATCHES_SUBDIR))

#define GET_FILES_DIR \
    QDir filesDir (dir); \
    if (!filesDir.exists(AUSMT_FILES_SUBDIR)) { \
        QVERIFY(filesDir.mkdir(AUSMT_FILES_SUBDIR)); \
    } \
    QVERIFY(filesDir.cd(AUSMT_FILES_SUBDIR))

void TestAusmt::initTestCase()
{
    // Dirs
    QDir srcDir (QStandardPaths::writableLocation(QStandardPaths::TempLocation));
    if (!srcDir.exists(AUSMTSRC_SUBDIR)) {
        QVERIFY(srcDir.mkdir(AUSMTSRC_SUBDIR));
    }
    QVERIFY(srcDir.cd(AUSMTSRC_SUBDIR));

    GET_DIR;
    GET_VAR_DIR;
    GET_PATCHES_DIR;
    GET_FILES_DIR;

    // Dump AUSMT into src dir
    QDir ausmtResDir (":/ausmtsrc/");
    foreach (const QString &fileName, ausmtResDir.entryList(QDir::Files)) {
        QFile file (ausmtResDir.absoluteFilePath(fileName));
        if (srcDir.exists(fileName)) {
            QVERIFY(QFileInfo(srcDir.absoluteFilePath(fileName)).isFile());
            QVERIFY(srcDir.remove(fileName));
        }
        QVERIFY(file.copy(srcDir.absoluteFilePath(fileName)));
    }

    QDir ausmtTestResDir (":/ausmtsrc-test/");
    foreach (const QString &fileName, ausmtTestResDir.entryList(QDir::Files)) {
        QFile file (ausmtTestResDir.absoluteFilePath(fileName));
        if (srcDir.exists(fileName)) {
            QVERIFY(QFileInfo(srcDir.absoluteFilePath(fileName)).isFile());
            QVERIFY(srcDir.remove(fileName));
        }
        QVERIFY(file.copy(srcDir.absoluteFilePath(fileName)));
    }

    // Make a better constants_root.sh
    QFile constantsRoot (srcDir.absoluteFilePath(CONSTANTS_ROOT_SH));
    QVERIFY(constantsRoot.open(QIODevice::WriteOnly));
    QTextStream constantsRootStream (&constantsRoot);
    constantsRootStream << QString("AUSMT_VAR_DIR=%1\n").arg(varDir.absolutePath());
    constantsRootStream << QString("PATCH_ROOT_DIR=%1\n").arg(patchesDir.absolutePath());
    constantsRootStream << QString("FILES_DIR=%1\n").arg(filesDir.absolutePath());
    constantsRootStream << QString("NOTIFY_WRAPPER_EXEC=echo\n");
    constantsRoot.close();

    // Build AUSMT
    QString buildName = srcDir.absoluteFilePath(BUILD_SH);
    QFile build (buildName);
    QVERIFY(build.exists());
    QVERIFY(build.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ExeUser));
    QProcess buildProcess;
    buildProcess.setWorkingDirectory(srcDir.absolutePath());
    buildProcess.start(PYTHON, QStringList() << buildName);
    buildProcess.waitForFinished(-1);
    QCOMPARE(buildProcess.exitCode(), 0);

    foreach (const QString &fileName, dir.entryList(QDir::Files)) {
        QFile file (dir.absoluteFilePath(fileName));
        QVERIFY(file.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ExeUser));
    }

    // Remove src
    QVERIFY(srcDir.removeRecursively());

    // Dump patches
    QDir patchesResDir (":/patches/");
    foreach (const QString &fileName, patchesResDir.entryList(QDir::Files)) {
        QFile file (patchesResDir.absoluteFilePath(fileName));
        if (patchesDir.exists(fileName)) {
            QVERIFY(QFileInfo(patchesDir.absoluteFilePath(fileName)).isFile());
            QVERIFY(patchesDir.remove(fileName));
        }
        QVERIFY(file.copy(patchesDir.absoluteFilePath(fileName)));
    }

    foreach (const QString &dirName, patchesResDir.entryList(QDir::Dirs)) {
        if (patchesDir.exists(dirName)) {
            QVERIFY(QFileInfo(patchesDir.absoluteFilePath(dirName)).isDir());
            QDir subDir (patchesDir);
            QVERIFY(subDir.cd(dirName));
            QVERIFY(subDir.removeRecursively());
        }

        patchesDir.mkdir(dirName);
        QDir patchesSubDir (patchesDir);
        QVERIFY(patchesSubDir.cd(dirName));

        QDir patchesResSubDir (patchesResDir);
        QVERIFY(patchesResSubDir.cd(dirName));

        foreach (const QString &fileName, patchesResSubDir.entryList(QDir::Files)) {
            QFile file (patchesResSubDir.absoluteFilePath(fileName));
            QVERIFY(file.copy(patchesSubDir.absoluteFilePath(fileName)));
        }
    }

    // Update patches
    QString makePatchName = patchesDir.absoluteFilePath(MAKE_PATCH_SH);
    QFile makePatch (makePatchName);
    QVERIFY(makePatch.exists());
    QVERIFY(makePatch.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ExeUser));
    QProcess makePatchProcess;
    makePatchProcess.setWorkingDirectory(patchesDir.absolutePath());
    makePatchProcess.start(patchesDir.absoluteFilePath(MAKE_PATCH_SH),
                           QStringList() << filesDir.absolutePath());
    makePatchProcess.waitForFinished(-1);
    QCOMPARE(makePatchProcess.exitCode(), 0);
}

void TestAusmt::prepareSimple()
{
    GET_DIR;
    GET_VAR_DIR;
    GET_PATCHES_DIR;
    GET_FILES_DIR;

    if (varDir.exists()) {
        QVERIFY(varDir.removeRecursively());
    }

    if (filesDir.exists()) {
        QVERIFY(filesDir.removeRecursively());
    }

    QVERIFY(QDir::root().mkpath(varDir.absolutePath()));
    QVERIFY(QDir::root().mkpath(filesDir.absolutePath()));

    QDir filesResDir (":/files");
    foreach (const QString &fileName, filesResDir.entryList(QDir::Files))  {
        QFile file (filesResDir.absoluteFilePath(fileName));
        if (filesDir.exists(fileName)) {
            QVERIFY(QFileInfo(filesDir.absoluteFilePath(fileName)).isFile());
            QVERIFY(filesDir.remove(fileName));
        }
        QVERIFY(file.copy(filesDir.absoluteFilePath(fileName)));
        QFile destFile (filesDir.absoluteFilePath(fileName));
        QVERIFY(destFile.exists());
        destFile.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser);
    }

    // Generate the md5sums
    QString genMd5sumFile = filesDir.absoluteFilePath(GEN_MD5SUM_SH);
    QFile genMd5sum (genMd5sumFile);
    QVERIFY(genMd5sum.exists());
    QVERIFY(genMd5sum.setPermissions(QFileDevice::ReadUser | QFileDevice::WriteUser | QFileDevice::ExeUser));
    QProcess genMd5sumProcess;
    genMd5sumProcess.setWorkingDirectory(filesDir.absolutePath());
    genMd5sumProcess.start(filesDir.absoluteFilePath(GEN_MD5SUM_SH));
    genMd5sumProcess.waitForFinished(-1);
    QCOMPARE(genMd5sumProcess.exitCode(), 0);
}

void TestAusmt::checkSimple1Applied()
{
    GET_DIR;
    GET_VAR_DIR;
    GET_FILES_DIR;

    // Check files
    QFile file (filesDir.absoluteFilePath("simple.qml"));
    QFile patched (":/patched/simple-patch1.qml");
    QVERIFY(file .open(QIODevice::ReadOnly));
    QVERIFY(patched .open(QIODevice::ReadOnly));
    QCOMPARE(file .readAll(), patched .readAll());
    file .close();
    patched .close();

    // Check orig
    QVERIFY(QFile::exists(filesDir.absoluteFilePath("simple.qml.webosinternals.orig")));

    // Check metadata
    QFile packages (varDir.absoluteFilePath("packages"));
    QVERIFY(packages.open(QIODevice::ReadOnly));
    QList<QByteArray> allLines = packages.readAll().trimmed().split('\n');
    packages.close();
    QCOMPARE(allLines.count(), 1);
    QCOMPARE(allLines.first(), QByteArray("simple-patch1 ausmt-sailfishos-1"));

    QFile fileControl (varDir.absoluteFilePath("control/file_control"));
    QVERIFY(fileControl.open(QIODevice::ReadOnly));
    allLines = fileControl.readAll().trimmed().split('\n');
    fileControl.close();
    QCOMPARE(allLines.count(), 1);
    QList<QByteArray> allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("test"));
    QCOMPARE(allEntries.at(1), filesDir.absoluteFilePath("simple.qml").toLocal8Bit());

    QFile fileMd5sums (varDir.absoluteFilePath("control/file_md5sums"));
    QVERIFY(fileMd5sums.open(QIODevice::ReadOnly));
    allLines = fileMd5sums.readAll().trimmed().split('\n');
    fileMd5sums.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch1"));
    QCOMPARE(allEntries.at(1), filesDir.absoluteFilePath("simple.qml").toLocal8Bit());
    QCOMPARE(allEntries.at(2), QByteArray("62a1d5ed1773a6a8dec1f5d0be3de388"));
}

void TestAusmt::checkSimple2Applied()
{
    GET_DIR;
    GET_VAR_DIR;
    GET_FILES_DIR;

    // Check files
    QFile file (filesDir.absoluteFilePath("simple.qml"));
    QFile patched (":/patched/simple-patch2.qml");
    QVERIFY(file .open(QIODevice::ReadOnly));
    QVERIFY(patched .open(QIODevice::ReadOnly));
    QCOMPARE(file .readAll(), patched .readAll());
    file .close();
    patched .close();

    // Check orig
    QVERIFY(QFile::exists(filesDir.absoluteFilePath("simple.qml.webosinternals.orig")));

    // Check metadata
    QFile packages (varDir.absoluteFilePath("packages"));
    QVERIFY(packages.open(QIODevice::ReadOnly));
    QList<QByteArray> allLines = packages.readAll().trimmed().split('\n');
    packages.close();
    QCOMPARE(allLines.count(), 1);
    QCOMPARE(allLines.first(), QByteArray("simple-patch2 ausmt-sailfishos-1"));

    QFile fileControl (varDir.absoluteFilePath("control/file_control"));
    QVERIFY(fileControl.open(QIODevice::ReadOnly));
    allLines = fileControl.readAll().trimmed().split('\n');
    fileControl.close();
    QCOMPARE(allLines.count(), 1);
    QList<QByteArray> allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("test"));
    QCOMPARE(allEntries.at(1), filesDir.absoluteFilePath("simple.qml").toLocal8Bit());

    QFile fileMd5sums (varDir.absoluteFilePath("control/file_md5sums"));
    QVERIFY(fileMd5sums.open(QIODevice::ReadOnly));
    allLines = fileMd5sums.readAll().trimmed().split('\n');
    fileMd5sums.close();
    QCOMPARE(allLines.count(), 1);
    allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch2"));
    QCOMPARE(allEntries.at(1), filesDir.absoluteFilePath("simple.qml").toLocal8Bit());
    QCOMPARE(allEntries.at(2), QByteArray("62a1d5ed1773a6a8dec1f5d0be3de388"));
}

void TestAusmt::checkSimple12Applied()
{
    GET_DIR;
    GET_VAR_DIR;
    GET_FILES_DIR;

    // Check files
    QFile file (filesDir.absoluteFilePath("simple.qml"));
    QFile patched (":/patched/simple-patch12.qml");
    QVERIFY(file .open(QIODevice::ReadOnly));
    QVERIFY(patched .open(QIODevice::ReadOnly));
    QCOMPARE(file .readAll(), patched .readAll());
    file .close();
    patched .close();

    // Check orig
    QVERIFY(QFile::exists(filesDir.absoluteFilePath("simple.qml.webosinternals.orig")));

    // Check metadata
    QFile packages (varDir.absoluteFilePath("packages"));
    QVERIFY(packages.open(QIODevice::ReadOnly));
    QList<QByteArray> allLines = packages.readAll().trimmed().split('\n');
    packages.close();
    QCOMPARE(allLines.count(), 2);
    QCOMPARE(allLines.at(0), QByteArray("simple-patch1 ausmt-sailfishos-1"));
    QCOMPARE(allLines.at(1), QByteArray("simple-patch2 ausmt-sailfishos-1"));

    QFile fileControl (varDir.absoluteFilePath("control/file_control"));
    QVERIFY(fileControl.open(QIODevice::ReadOnly));
    allLines = fileControl.readAll().trimmed().split('\n');
    fileControl.close();
    QCOMPARE(allLines.count(), 1);
    QList<QByteArray> allEntries = allLines.first().split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("test"));
    QCOMPARE(allEntries.at(1), filesDir.absoluteFilePath("simple.qml").toLocal8Bit());

    QFile fileMd5sums (varDir.absoluteFilePath("control/file_md5sums"));
    QVERIFY(fileMd5sums.open(QIODevice::ReadOnly));
    allLines = fileMd5sums.readAll().trimmed().split('\n');
    fileMd5sums.close();
    QCOMPARE(allLines.count(), 2);
    allEntries = allLines.at(0).split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch1"));
    QCOMPARE(allEntries.at(1), filesDir.absoluteFilePath("simple.qml").toLocal8Bit());
    QCOMPARE(allEntries.at(2), QByteArray("62a1d5ed1773a6a8dec1f5d0be3de388"));
    allEntries = allLines.at(1).split(' ');
    QCOMPARE(allEntries.count(), 3);
    QCOMPARE(allEntries.at(0), QByteArray("simple-patch2"));
    QCOMPARE(allEntries.at(1), filesDir.absoluteFilePath("simple.qml").toLocal8Bit());
    QCOMPARE(allEntries.at(2), QByteArray("62a1d5ed1773a6a8dec1f5d0be3de388"));
}

void TestAusmt::checkUnapplied()
{
    GET_DIR;
    GET_VAR_DIR;
    GET_FILES_DIR;

    // Check files
    QFile file (filesDir.absoluteFilePath("simple.qml"));
    QFile original (":/files/simple.qml");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QVERIFY(original.open(QIODevice::ReadOnly));
    QCOMPARE(file.readAll(), original.readAll());
    file.close();
    original.close();

    // Check orig
    QVERIFY(!QFile::exists(filesDir.absoluteFilePath("simple.qml.webosinternals.orig")));

    // Check metadata
    QFile packages (varDir.absoluteFilePath("packages"));
    QVERIFY(packages.open(QIODevice::ReadOnly));
    QVERIFY(packages.readAll().trimmed().isEmpty());
    packages.close();


    QFile fileControl (varDir.absoluteFilePath("control/file_control"));
    QVERIFY(fileControl.open(QIODevice::ReadOnly));
    QVERIFY(fileControl.readAll().trimmed().isEmpty());
    fileControl.close();

    QFile fileMd5sums (varDir.absoluteFilePath("control/file_md5sums"));
    QVERIFY(fileMd5sums.open(QIODevice::ReadOnly));
    QVERIFY(fileMd5sums.readAll().trimmed().isEmpty());
    fileMd5sums.close();
}

void TestAusmt::testSimple()
{
    prepareSimple();
    GET_DIR;

    // Test the simple patch application stream (apply -> unapply)

    // Apply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch1"), 0);
    checkSimple1Applied();

    // Unapply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch1"), 0);
    checkUnapplied();
}

void TestAusmt::testReapply()
{
    prepareSimple();
    GET_DIR;

    // Test when you try to reapply or reunapply a patch

    // Apply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch1"), 0);
    checkSimple1Applied();

    // Reapply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch1"), 0);
    checkSimple1Applied();

    // Unapply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch1"), 0);
    checkUnapplied();

    // Reunapply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch1"), 0);
    checkUnapplied();
}

void TestAusmt::testExternalRemoved()
{
    prepareSimple();
    GET_DIR;
    GET_FILES_DIR;

    // Test when external source applied or unapplied your patch

    // Perform a false apply (copy file from patched)
    QVERIFY(QFile::remove(filesDir.absoluteFilePath("simple.qml")));
    QVERIFY(QFile::copy(":/patched/simple-patch1.qml",
                        filesDir.absoluteFilePath("simple.qml")));

    // Apply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch1"), 0);
    checkSimple1Applied();

    // Perform a false unapply (copy file from original)
    QVERIFY(QFile::remove(filesDir.absoluteFilePath("simple.qml")));
    QVERIFY(QFile::copy(":/files/simple.qml",
                        filesDir.absoluteFilePath("simple.qml")));

    // Unapply
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch1"), 0);
    checkUnapplied();
}

void TestAusmt::test2Patches()
{
    prepareSimple();
    GET_DIR;

    // Test 2 patches application
    // Apply 1
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch1"), 0);
    checkSimple1Applied();

    // Apply 2
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch2"), 0);
    checkSimple12Applied();

    // Unapply 2
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch2"), 0);
    checkSimple1Applied();

    // Unapply 1
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch1"), 0);
    checkUnapplied();
}

void TestAusmt::test2PatchesCrossed()
{
    prepareSimple();
    GET_DIR;

    // Test 2 patches application (crossed: 1 2 then unapply 1 2)
    // Apply 1
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch1"), 0);
    checkSimple1Applied();

    // Apply 2
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_INSTALL), QStringList() << "simple-patch2"), 0);
    checkSimple12Applied();

    // Unapply 1
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch1"), 0);
    checkSimple2Applied();

    // Unapply 2
    QCOMPARE(QProcess::execute(dir.absoluteFilePath(AUSMT_REMOVE), QStringList() << "simple-patch2"), 0);
    checkUnapplied();
}

void TestAusmt::cleanupTestCase()
{
    // TODO: clear the test
}

QTEST_MAIN(TestAusmt)
#include "main.moc"
