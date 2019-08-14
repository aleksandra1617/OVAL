#include "Interpreter.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QList>
#include <QRadioButton>

#include <cstdint>
#include <cstring>


const QString Interpreter::NodeTypeToStr(int nodeTypeKey)
{
    if(nodeTypes.size() > nodeTypeKey)
        return nodeTypes[nodeTypeKey];
    else
        return "Unknown Node";
}

Interpreter::Interpreter()
{
    QList <QByteArray> nodeModelsToInterpret= _fileManager->LoadFile("","NodeConstruction.txt").split('@');

    // Gen Node Prototypes
    // Uses nodeModelsToInterpret.size() as the top bound so that
    // if nodes are missing it would not throw an index_out_of_range exception.
    for(int c = 0; c < nodeModelsToInterpret.size(); c++)
    {
        // This if statement is a guard in case the NodeConstruction.txt had less or more then the expected number of nodes.
        //TODO: COME UP WITH A BETTER WAY TO PREVENT THE INDEX OUT OF RANGE ERROR.
        QString codeToInsert = _fileManager->GetStrBetween("{", "}#", nodeModelsToInterpret[c]);
        QString nodeDetailsStr = _fileManager->GetStrBetween(QString::number(c)+"(", ")", nodeModelsToInterpret[c]);

        if(nodeDetailsStr != "[NOT FOUND]")
        {
            ///@example "int::In1", "float::In2", "str::In3", "void::Return"
            QStringList nodeDetails = nodeDetailsStr.split(",");
            _nodePrototypes[c] = BuildNodePrototype(codeToInsert, (int)c, nodeDetails);
        }
    }
}


std::pair<NodeView*, NodeModel*> Interpreter::BuildNodePrototype(QString codeToInsert, int nodeTypeIndex, QStringList nodeDetails)
{
    // Instantiate Node
    std::pair<NodeView*, NodeModel*> node;
    node.first = new NodeView(NodeTypeToStr(nodeTypeIndex));
    node.second = new NodeModel(NodeTypeToStr(nodeTypeIndex).toStdString());
    node.second->codeToInsert = codeToInsert.toStdString();

    for(int c = 0; c < nodeDetails.size(); c++)
    {
        // Start node has no inputs and outputs just a name and a next pin the
        // previous pin is added in the NodeView constructor so it will be removed here.

        ///@example "int::In1", "float::In2", "str::In3", "void::Return"
        QStringList fieldData = nodeDetails[c].split("::");

        QString fieldType = fieldData.at(0).trimmed();
        QString fieldName = fieldData.at(1).trimmed();

        int fieldTypeEnum = NodeModel::StrToReturnType(fieldType.toStdString());

        // Second to last data in nodeDetails is always the return type!
        if(c == nodeDetails.size()-1)
        {
            node.second->returnDataType = (NodeModel::ReturnDataType)fieldTypeEnum;

            if(fieldType != "void")
            {
                node.first->bottomLayout->addWidget(new QLabel(fieldType), c, 0, Qt::AlignRight);
                node.first->bottomLayout->addWidget(new QLabel(fieldName), c, 1, Qt::AlignRight);
                node.first->bottomLayout->addWidget( new QRadioButton(""), c, 2, Qt::AlignRight);
            }
        }
        else
        {
            node.first->bottomLayout->addWidget(new QRadioButton(""), c, 0, Qt::AlignLeft);
            node.first->bottomLayout->addWidget(new QLabel(fieldType), c, 1, Qt::AlignLeft);
            node.first->bottomLayout->addWidget(new QLabel(fieldName), c, 2, Qt::AlignLeft);
            node.second->inputs.insert({fieldName.toStdString(), new NodeModel(fieldName.toStdString(), fieldTypeEnum)});
        }
    }

    return node;
}


std::pair<NodeView*, NodeModel*> Interpreter::CreateNode(int nodeType, int nodeUID)
{
    std::pair<NodeView*, NodeModel*> node;

    if(_nodePrototypes.find(nodeType) != _nodePrototypes.end())
    {
        bool canReturn = _nodePrototypes[nodeType].second->returnDataType != _nodePrototypes[nodeType].second->VOID;

        node.first = new NodeView(*_nodePrototypes[nodeType].first, canReturn, nullptr);
        node.second = new NodeModel(*_nodePrototypes[nodeType].second);

        node.first->UID = nodeUID;
        node.second->UID = nodeUID;

        qDebug() << "CREATING NODE with: ";
        qDebug() << "name: " << node.second->name.c_str();
        qDebug() << "NodeView UID: " << node.first->UID;
        qDebug() << "Second UID: " << node.second->UID;
        qDebug() << "codeToInsert: " << node.second->codeToInsert.c_str();
        qDebug() << "Return Data Type: " << node.second->returnDataType;
        qDebug() << "Inputs: " << node.second->inputs.size() <<"\n";
    }

    return node;
}


bool Interpreter::TraverseModel(NodeModel *start, QString pathToSource, QString sourceName)
{
    bool successfulTraversal = true;

    // A string containing all the nodes' source code in the correct order.
    QString accNodeString = "\n";

    NodeModel *current = start;

    // While the next NodeModel is not null, there is more linked nodes to look through.
    while (current != nullptr)
    {
        qDebug() << "IN TRAVERSAL: "<< current->name.c_str();
        // Check if all it's inputs are set.
        // "input" is an "std::pair" of "std::string" and "NodeModel*"
        for(auto input : current->inputs)
        {
            qDebug() << "AT INPUT: "<< input.second->name.c_str();
            //Found unassigned input
            if(input.second->UID == -1)
            {
                // Cannot build when there are uassigned linked nodes, TODO: display error message.
                return false;
            }
            else
            {
                if(!accVarDeclarations.contains(input.second->codeToInsert.c_str()))
                {
                    // Insert the input code to insert into the source file.
                    qDebug() << input.second->codeToInsert.c_str();
                    accVarDeclarations += "\t"+QString::fromStdString(input.second->codeToInsert)+"\n";
                }
            }
        }

        // Interpret input model and set the node source string
        QString nodeSourceString = InsertNode(current, pathToSource, sourceName);
        accNodeString += nodeSourceString;

        // Done with this node, go to the next.
        current = current->GetNextLink();
    }

    qDebug() << "Accumulated: ";
    qDebug(accNodeString.toStdString().c_str());
    qDebug() << "End str.. ";

    ///INSERT ACCUMULATED NODE SOURCE STRING INTO A SOURCE FILE
    //FIND Index at which to insert after (the first {'s position + 1)
    QByteArray loadedSourceFileData = _fileManager->LoadFile(pathToSource, sourceName +".cpp");

    //TODO: use this only for the first node. Every other node should get the index after the prev node code.
    // Can use ternary conditional to do that Condition ?(if condition true) do this :(else) that
    QList <int> insertIndices = _fileManager->FindStr("{", loadedSourceFileData);

    //INSERT the data into the QByteArray returned after reading the source file and override the file.
    loadedSourceFileData.insert(insertIndices[0]+1, accVarDeclarations+accNodeString);
     _fileManager->CreateFile(pathToSource, sourceName+".cpp", QString::fromStdString(loadedSourceFileData.toStdString()));


     std::string batchFile = sourceName.toStdString()+".bat";
     system(batchFile.c_str());

    return successfulTraversal;
}

QString Interpreter::InsertNode(NodeModel *nodeModel, QString pathToSource, QString sourceName)
{
    QString codeToInsert = QString::fromStdString(nodeModel->codeToInsert);
    QString constructionString = QString::fromStdString(nodeModel->codeToInsert);

    QStringList keysToFindingInput = _fileManager->GetAllCasesOfStrBetween("#>", "<#", constructionString.toUtf8());

    for(QString keyToFindingInput : keysToFindingInput)
    {
        if(keyToFindingInput != "[NOT FOUND]")
        {
            NodeModel *inputModel = nodeModel->inputs.at(keyToFindingInput.toStdString());

            QString defaultInputIndicator = "#>"+ keyToFindingInput+ "<#";
            codeToInsert.replace(defaultInputIndicator, QString::fromStdString(inputModel->name));
        }
    }

    return codeToInsert;
}
