#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QString>
#include <QList>
#include <unordered_map>
#include "FileManager.h"

class ProjectManager
{
public:

    static ProjectManager& Instance()
    {
        static ProjectManager instance;
        return instance;
    }

    /// @brief currentName - The currently opened project name.
    /// @brief currentPath - The project directory of the currently opened project.
    QString currentName{"UNKNOWN"}, currentPath{"UNKNOWN"};

    int currentUID{0}; //if currrentUID is 0 there are no nodes in the project other than start. TODO: Add in exception handler.

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
           {"ChangeDirectory","cd #>path<#"},
           {"Build","g++ #>name<#.cpp -o #>name<#.exe -lglfw3 -lglew32 -lgdi32 -lopengl32"},
           {"Run","#>name<#.exe"},

       };
};

#endif // PROJECTMANAGER_H
