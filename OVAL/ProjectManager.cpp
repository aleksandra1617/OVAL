#include "ProjectManager.h"
#include <QDebug>

ProjectManager::ProjectManager()
{
    currentName = "Demo1";
    currentPath = "OpenGL-Project-Files/";
}

void ProjectManager::GenBatchFile(QString path, QString name, QList<QString> commandsToWrite)
{
    // Process Commands
    QString processedCommandsStr = "";

    for(QString &key : commandsToWrite)
    {
        QString strToProcess = QString::fromStdString(commands[key.toStdString()]);
        strToProcess.replace("#>path<#", path);
        strToProcess.replace("#>name<#", name);

        processedCommandsStr += strToProcess +"\n";
    }

    _fileManager.CreateFile("", name+".bat", processedCommandsStr);
}
