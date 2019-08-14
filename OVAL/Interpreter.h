#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "NodeView.h"
#include "NodeModel.h"
#include "FileManager.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include <QJsonArray>


class Interpreter
{
public:

    Interpreter();
    static Interpreter& Instance()
    {
        static Interpreter instance;
        return instance;
    }

    /// Serves as a dynamic enum.
    QList<QString> nodeTypes = {"Print", "If", "For Each", "While", "End Block", "GlfwWindow", "GlfwWindowClose", "GlfwPollEvents",
                                "GlfwSwapBuffers", "GlGenVertArrays", "GlGenBuffers", "GlBindBuffer", "GlBindVertArray", "GlBufferData",
                                "GlEnableVertAttribArr", "GlVertexAttribPointer", "GlClear", "GlDrawArrays", "GlDelete"};

    QList<QString> listOfOriginalNodes = nodeTypes;
    /**
     * @brief BuildNodePrototype - Called in Interpreter constructor to generate a prototype
     *        for a node of a given node type.
     *
     * @param nodeType
     * @return
     */
    std::pair<NodeView*, NodeModel*> BuildNodePrototype(QString codeToInsert, int nodeType, QStringList nodeDetails);


    /**
     * @brief Converts a given node type to a string.
     *
     * @param nodeType
     */
    const QString NodeTypeToStr(int nodeTypeKey);

    /**
     * @brief Uses the copy constructors for NodeView and NodeModel to generate a new node (std::pair<NodeView*, NodeModel*>).
     * @return NodeView and NodeModel pair if successful.
     */
    std::pair<NodeView*, NodeModel*> CreateNode(int nodeType, int nodeUID);

    /**
     * @brief TraverseModel: Looks trough the inputs of the linked nodes to determine if it is safe to build.
     * Calls inside Interpreter::InsertNode functon to add the 'safe' nodes to the source file.
     *
     * @param start
     * @param pathToSource
     * @param sourceName
     *
     * @return True if it is safe to build source file and false if it isn't.
     */
    bool TraverseModel(NodeModel *start, QString pathToSource, QString sourceName);

    /**
     * @brief Interprets the node model construction string and produces the source code for that node.
     * @returns The source code of the given node.
     */
    QString InsertNode(NodeModel *nodeModel, QString pathToSource, QString sourceName);

    /**
     * @brief _nodePrototypes: A map of all node prototypes, to quickly generate copies of those nodes.
     */
    std::unordered_map<int, std::pair<NodeView*, NodeModel*>> _nodePrototypes;

    void AccVarDeclarations(const QString &str ){accVarDeclarations += str ;}
private:

    FileManager *_fileManager;
    QByteArray nodeModels;
    QString accVarDeclarations;

    /**
     * @brief SetupInterpreter - Runs any setup needed for the interpreter to function.
     */
    void SetupInterpreter();
};

#endif // INTERPRETER_H
