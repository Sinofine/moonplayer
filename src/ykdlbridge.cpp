#include "ykdlbridge.h"
#include "platforms.h"
#include "selectiondialog.h"
#include "settings_network.h"
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QProcess>
#include <QRegularExpression>

YkdlBridge ykdl_bridge;

YkdlBridge::YkdlBridge(QObject *parent) : ParserBridge(parent)
{
    selectionDialog = NULL;
}


void YkdlBridge::runParser(const QString &url)
{
    QStringList args;
    args << "python" << (getAppPath() + "/ykdl_patched.py");
    args << "-t" << "15" << "--json";
    if (!Settings::proxy.isEmpty() &&
            (Settings::proxyType == "http" ||
             (Settings::proxyType == "http_unblockcn" && !url.contains(".youtube.com"))))
        args << "--proxy" << (Settings::proxy + ':' + QString::number(Settings::port));
    args << url;
    process->start("/usr/bin/env", args, QProcess::ReadOnly);
}


void YkdlBridge::parseOutput(const QByteArray &jsonData)
{
    QJsonParseError json_error;
    QJsonObject obj = QJsonDocument::fromJson(jsonData, &json_error).object();
    if (json_error.error != QJsonParseError::NoError)
        return;
    if (obj.contains("streams"))
    {
        result.title = obj["title"].toString();
        QJsonObject streams = obj["streams"].toObject();
        QJsonObject selectedItem;
        QJsonObject::const_iterator i;

        // Select video quality
        // get all available qualities
        QString selected;
        QStringList items;
        for (i = streams.constBegin(); i != streams.constEnd(); i++)
        {
            QString profile = i.value().toObject()["video_profile"].toString();
            items << QString("%1 (%2)").arg(i.key(), profile);
        }

        // show dialog
        selected = selectionDialog->showDialog(items,
                                                   tr("Please select a video quality:"));
        if (selected.isEmpty())
            return;
        selected = selected.section(' ', 0, 0);
        selectedItem = streams[selected].toObject();

        // Write names-urls-list
        if (!selectedItem.isEmpty())
        {
            QJsonArray json_urls = selectedItem["src"].toArray();
            result.container = selectedItem["container"].toString();
            result.referer = obj["extra"].toObject()["referer"].toString();
            result.ua = obj["extra"].toObject()["ua"].toString();
            result.danmaku_url = obj["danmaku_url"].toString();

            if (json_urls.size() == 0)
                return;

            // replace illegal chars in title with .
            static QRegularExpression illegalChars("[\\\\/]");
            result.title.replace(illegalChars, ".");

            // Make file list
            for (int i = 0; i < json_urls.size(); i++)
            {
                result.names << QString("%1_%2.%3").arg(result.title, QString::number(i), result.container);
                result.urls << json_urls[i].toString();
            }
        }
    }
}


