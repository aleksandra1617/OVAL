#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>

/**
 * @brief The FileManager class Manages file and also handles string manipulation.
 */
class FileManager
{
public:

    FileManager();

    /**
     * @brief CreateFile - Generates a file with the given parameters.
     *
     * If the file alwready exists it will be overriten.
     * If any directories in the path do not exist they will be created.
     *
     * @param name - File name + extention.
     */
    void CreateFile(QString path, QString name, QString dataString);

    /**
     * @brief LoadFile - Loads a file with the given path and name.
     *
     * @param path - The path from the build directory to the file excluding the file name and extention.
     * @param name - The file name and extention.
     *
     * @return A QByteArray Containing all the data in the file.
     */
    QByteArray LoadFile(QString path, QString name);

    /**
     * @brief FindStr - Searches through a QByteArray and returns a QList contining the indeices of all occurences.
     *
     * @param str - The string to find.
     * @param data - The QByteArray to search through.
     *
     * @return The index at which the string was found.
     */
    QList<int> FindStr(QString str, QByteArray data);

    /**
     * @brief GetStrBetween - Returns the a substring of data which is between 2 substrings.
     *
     * @attention Will only return the first occurence of a substring between the 2 given substrings.
     * To get all occurences of substrings between the 2 given substrings call GetAllCasesOfStrBetween
     *
     * @param subStr1 - Substring of data, can be found right before the string of interest.
     * @param subStr2 - Substring of data, can be found right after the string of interest.
     * @param data - Where to look.
     *
     * @return - The string between substring 1 and substring 2.
     */
    QString GetStrBetween(QString subStr1, QString subStr2, QByteArray data);

    QStringList GetAllCasesOfStrBetween(QString subStr1, QString subStr2, QByteArray data);

    void CleanFile(QString fileType, QString path, QString name);

    /**
     * @brief InsertAt - Inserts data into a file at a given index.
     *
     * @param path- The path from the build directory to the file excluding the file name and extention.
     * @param name - The file name and extention.
     * @param dataString - The data to insert.
     * @param index - THe index at which to insert.
     *
     * @return - true if insert is successful, and false if it isn't.
     */
    //bool InsertAt(QString path, QString name, QString dataString, int index);
};

#endif // FILEMANAGER_H
