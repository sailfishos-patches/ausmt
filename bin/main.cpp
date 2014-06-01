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

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <notification.h>

static const char *PATCH_FILE = "patch.json";
static const char *PATCHES_DIR = "/usr/share/patchmanager/patches";
static const char *BACKUP_PATCHES_DIR = "/var/lib/patchmanager/ausmt/patches";
static const char *NAME_KEY = "name";

int main(int argc, char **argv)
{
    QCoreApplication app (argc, argv);
    if (app.arguments().count() != 3) {
        return 1;
    }

    QString operation = app.arguments().at(1);
    QString patch = app.arguments().at(2);
    QString name;
    QString summary;
    QString body;

    QDir dir (PATCHES_DIR);
    if (!dir.cd(patch)) {
        dir = QDir(BACKUP_PATCHES_DIR);
        if (!dir.cd(patch)) {
            // Patch won't apply so no need to notify
            return 2;
        }
    }


    QFile file (dir.absoluteFilePath(PATCH_FILE));
    if (!file.open(QIODevice::ReadOnly)) {
        name = patch;
    } else {
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        QString trueName = document.object().value(NAME_KEY).toString();
        if (trueName.isEmpty()) {
            name = patch;
        } else {
            name = trueName;
        }
        file.close();
    }

    if (operation == "in") {
        // Installing
        summary = "Patch installed";
        body = QString("Patch %1 installed").arg(name);
    } else if (operation == "fin") {
        summary = "Failed to install patch";
        body = QString("Patch %1 installation failed").arg(name);
    } else if (operation == "rm") {
        // Removing
        summary = "Patch removed";
        body = QString("Patch %1 removed").arg(name);
    } else if (operation == "frm") {
        summary = "Failed to remove patch";
        body = QString("Patch %1 removal failed").arg(name);
    } else {
        // Should not happen
        return 3;
    }

    Notification notification;
    notification.setHintValue("x-nemo-icon", "icon-l-developer-mode");
    notification.setHintValue("x-nemo-preview-icon", "icon-l-developer-mode");
    notification.setSummary(summary);
    notification.setBody(body);
    notification.setPreviewSummary(summary);
    notification.setPreviewBody(body);
    notification.setTimestamp(QDateTime::currentDateTime());
    notification.publish();

    return 0;
}
