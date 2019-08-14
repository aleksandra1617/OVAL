#include "FileManager.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QList>

FileManager::FileManager()
{

}

void FileManager::CreateFile(QString path, QString name, QString dataString)
{
    // The path will intentionally be set to "" whenever the file needs to be sent ot the build directory.
    // In those cases QDir sends a warning, this if statement is set as a guard to prevent any of those warnings.
    if(path != "")
    {
        QDir dir;

        if (!dir.exists(path))
            dir.mkpath(path);
    }

    QFile::remove(path + name);
    QFile file(path + name);
    file.open(QIODevice::ReadWrite);

    QTextStream stream(&file);
    stream << dataString;
    file.close();
}


QByteArray FileManager::LoadFile(QString path, QString name)
{
    QString fullPath = path+name;
    QFile file(path+name);

    if (!file.open(QIODevice::ReadWrite))
    {
        qWarning("Couldn't open file.");
        return nullptr;
    }

    return file.readAll();
}


void FileManager::CleanFile(QString fileType, QString path, QString name)
{
    if(fileType == "main source")
    {
        QString mainString = "#include <iostream>\n"
                             "#include <vector>\n"
                             "\n"
                             "#define GLEW_STATIC\n"
                             "#include <GL/glew.h>\n"
                             "#include <GLFW/glfw3.h>\n\n"
                             "\n"
                             "using namespace std;\n\n"
                             "int main()\n{\n"
                             "\treturn 0;\n}\n";

        // GENERATE base source file with main
        CreateFile(path, name, mainString);
    }
}

QList<int> FileManager::FindStr(QString str, QByteArray data)
{
    QList<int> indices;

    int c = 0;
    while ((c = data.indexOf(str, c)) != -1) {
        //qDebug() << "Found "<<str <<"at index position " << c << endl;
        indices.append(c);
        ++c;
    }

    return indices;
}


QString FileManager::GetStrBetween(QString subStr1, QString subStr2, QByteArray data)
{
    QString subStrOfInterest = "[NOT FOUND]";
    QList<int> data1 = FindStr(subStr2, data); /// @attention possibly redundant !!!!

    int nodeStartPos = ! FindStr(subStr1, data).isEmpty()
                       ? FindStr(subStr1, data)[0] : -1;

    int nodeEndPos = ! FindStr(subStr2, data).isEmpty()
                     ? FindStr(subStr2, data)[0] : -1;

    int offset = subStr1.size();
    nodeStartPos += offset;

    if((nodeStartPos != -1) && (nodeEndPos != -1))
        subStrOfInterest = data.mid(nodeStartPos, nodeEndPos-nodeStartPos);

    return  subStrOfInterest;
}


QStringList FileManager::GetAllCasesOfStrBetween(QString subStr1, QString subStr2, QByteArray data)
{
    QStringList allStrBetween;
    int offset = subStr1.size();

    //GET ALL OCCURENCES OF subStr1
    QList<int> subStr1Positions = FindStr(subStr1, data);


    //GET ALL OCCURENCES OF subStr2
    QList<int> subStr2Positions = FindStr(subStr2, data);

    //COMPOSE A LIST OF STRINGS BETWEEN THE GIVEN POSITIONS
    for(int c = 0; c < subStr1Positions.size(); c++)
    {
        int startPos = subStr1Positions[c] += offset;
        int endPos = subStr2Positions[c]-startPos;

        allStrBetween.push_back(data.mid(startPos, endPos));
    }

    qDebug("\n\n");
    qDebug() << "Num Inputs: " << allStrBetween.size();
    qDebug() << "In " <<data;

    return allStrBetween;
}
