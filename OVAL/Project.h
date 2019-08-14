#ifndef PROJECT_H
#define PROJECT_H

#include <QString>
#include <QList>
#include <unordered_map>
#include "FileManager.h"

class ProjectManager
{
public:

    QString currentName;
    QString currentPath;

    ProjectManager();

    /**
     * @brief GenBatchFile - Generates a Batch file for a given source file.
     * (Currently supports executing one file at a time)
     * TODO: Look into having generating and executing multiple files.
     *
     * @param path - The path to the directory of the source files.
     * @param name
     * @param commandsToWrite - A list of commands to be written in the batch (in decending order).
     *        The commandsToWrite are equivalent to the keys in the "commands" map.
     */
    void GenBatchFile(QString path, QString name, QList<QString> commandsToWrite);

private:

    FileManager _fileManager;

    /**
     * A lookup of all the commands that might need to be written in the batch file of the project.
     */
    std::unordered_map<std::string, std::string> commands = {
           {"Run","#>name<#.exe"},
           {"Build","g++ #>name<#.cpp -o #><#.exe -lglfw3 -lglew32 -lgdi32 -lopengl32"},
           {"ChangeDirectory","cd #>path<#"}
       };
};

#endif // PROJECT_H
