#include "Project.h"
#include <QDebug>

ProjectManager::ProjectManager()
{
    currentName = "Demo1";
    currentPath = "OpenGL-Project-Files/";
}


void ProjectManager::GenBatchFile(QString path, QString name, QList<QString> commandsToWrite)
{
    // Process Commands
    QString processedCommandsStr;

    for(QString &key : commandsToWrite)
    {
        QString strToProcess = QString::fromStdString(commands[key.toStdString()]);
        qDebug() << strToProcess;
    }

    _fileManager.CreateFile(path, name, processedCommandsStr);
}
