#ifndef NODEMODEL_H
#define NODEMODEL_H

#include <iostream>
#include <unordered_map>

class NodeModel
{
public:
    enum ReturnDataType{VOID, INT, FLOAT, BOOL, STRING, GLUINT, GLENUM};

    int UID {-1};

    /// INPUTS
    std::unordered_map<std::string, NodeModel*> inputs;

    /// RETURN
    std::string name{"UNASSIGNED"}, codeToInsert{"UNASSIGNED"};
    ReturnDataType returnDataType;

    NodeModel();
    NodeModel(const NodeModel &copy);
    NodeModel(std::string name);
    NodeModel(std::string name, int returnType);

    // Getters and Setters
    void SetNextLink(NodeModel* modelLink){next = modelLink;}
    NodeModel* GetNextLink(){return next;}
    void SetPrevLink(NodeModel* modelLink){prev = modelLink;}
    NodeModel* GetPrevLink(){return prev;}

    void AddInput(NodeModel *in, std::string inName);
    void ResetInput(std::string inputKey);

    static NodeModel::ReturnDataType StrToReturnType(std::string returnType)
    {
        if (returnType == "int")
            return NodeModel::ReturnDataType::INT;
        else if (returnType == "float")
            return NodeModel::ReturnDataType::FLOAT;
        else if (returnType == "bool")
            return NodeModel::ReturnDataType::BOOL;
        else if (returnType == "string")
            return NodeModel::ReturnDataType::STRING;
        else if (returnType == "GLuint") {
            return NodeModel::ReturnDataType::GLUINT;
        }

        return NodeModel::ReturnDataType::VOID;
    }

private:
    NodeModel* prev;
    NodeModel* next;
};

#endif // NODEMODEL_H
