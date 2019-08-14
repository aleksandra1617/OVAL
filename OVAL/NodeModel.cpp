#include "NodeModel.h"

NodeModel::NodeModel():inputs()
{
    next = nullptr;
    prev = nullptr;
}

NodeModel::NodeModel(const NodeModel &copy)
{
    this->name = copy.name;
    this->codeToInsert = copy.codeToInsert;
    this->inputs = copy.inputs;
    this->returnDataType = copy.returnDataType;
    next = nullptr;
    prev = nullptr;
}

NodeModel::NodeModel(std::string name)
{
    this->name = name;
    next = nullptr;
    prev = nullptr;
}

NodeModel::NodeModel(std::string name, int returnType)
{
    this->name = name;
    this->returnDataType = (ReturnDataType)returnType;
    next = nullptr;
    prev = nullptr;
}

void NodeModel::AddInput(NodeModel *in, std::string inName)
{
    for (auto model : inputs)
    {
        if(model.first == inName)
        {
            inputs[model.first] = in;
        }
    }
}

void NodeModel::ResetInput(std::string inputKey)
{
    inputs.erase(inputKey);
    NodeModel *inputToReset = new NodeModel();
    inputToReset->UID = -1;

   inputToReset->name = "UNASSIGNED";
   inputToReset->codeToInsert = "UNASSIGNED";
   inputs[inputKey] = inputToReset;


}
