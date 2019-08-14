#include "Graph.h"
#include "Interpreter.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPen>
#include <QPainter>

Graph::Graph(QFrame* mainFrame, QWidget *parent) : QWidget(parent)
{
    // BUILD START NODE
    Interpreter::Instance().nodeTypes.push_back("Start");
    QString codeToInsert = "", nodeConstrDetails = "";
    std::pair<NodeView*, NodeModel*> startNode = Interpreter::Instance().BuildNodePrototype(codeToInsert,
                                                        Interpreter::Instance().nodeTypes.size()-1, {});

    Interpreter::Instance()._nodePrototypes.insert({Interpreter::Instance().nodeTypes.size()-1, startNode});
    startNode.first->setParent(mainFrame);
    startNode.first->show();

    startNode.first->UID = 0;
    startNode.second->UID = 0;

    _nodes.insert({0, startNode});

    connect(this, SIGNAL(openLink()), this, SLOT(OnLinkOpened()));
    connect(this, SIGNAL(closeLink()), this, SLOT(OnLinkClosed()));
}

#pragma region LINKING NODES
NodeModel* Graph::FindPinModel(QRadioButton *pin)
{
    NodeView *pinParent = qobject_cast<NodeView *>(pin->parent());
    std::pair<NodeView*, NodeModel*> node = _nodes[pinParent->UID];

    return node.second;
}

void Graph::RemovePin(int indexOfNewPin)
{
    /** Works out which pin the new pin is linked to if any,
     *  removes it along with the new pin that was triggered.
     */
    switch(indexOfNewPin % 2)
    {
        case 0:
        if(indexOfNewPin != visualLinkManager.size()-1)
        {
            //VISUAL
            visualLinkManager[indexOfNewPin+1]->setChecked(false);
            QRadioButton *otherPin = visualLinkManager.takeAt(indexOfNewPin+1);
            QRadioButton *newPin = visualLinkManager.takeAt(indexOfNewPin);///@attention unused data

            //LOGIC
            QStringList newPinData = newPin->objectName().split("::"); ///@attention unused data
            QStringList otherPinData = otherPin->objectName().split("::");

            if(otherPinData[0] == "input")
            {
                NodeModel* model = FindPinModel(otherPin);
                //model->ResetInput(otherPinData[2].toStdString());
            }
        }
        else
            visualLinkManager.takeAt(indexOfNewPin);

        break;

        case 1:
        //VISUAL
        visualLinkManager[indexOfNewPin-1]->setChecked(false);
        QRadioButton *newPin = visualLinkManager.takeAt(indexOfNewPin);
        visualLinkManager.takeAt(indexOfNewPin-1); ///@attention unused data

        //LOGIC
        QStringList newPinData = newPin->objectName().split("::");

        if(newPinData[0] == "input")
        {
            NodeModel* model = FindPinModel(newPin);
            //model->ResetInput(newPinData[2].toStdString());
        }

        break;
    }
}

void Graph::SetLink(NodeModel* newNodeModel, QRadioButton *newPin)
{
    // Check if the new link added is aded at an even or an odd position in the visualLinkManager.
    switch(visualLinkManager.size() % 2)
    {
        /**
         * When the position at which the new pin is added is even (0, 2, 4, etc.),
         * a pair is not made because 'newPin' is the first pin from a potential pair.
         */
        case 0:
            visualLinkManager.push_back(newPin);
            qDebug() << "New PIN: " + newPin->objectName();
            qDebug() << "New POS: " << mapToGlobal(newPin->pos()).x() << ",  " <<mapToGlobal(newPin->pos()).y();
            emit openLink();

            break;

        /**
         * When the position at which the new pin is added is odd (1, 3, 5, etc.),
         * a pair is made. The 'newPin' is the second pin from the pair.
         */
        case 1:
            //Get the NodeModel of the other pin (in this case the first pin from the pair).
            QRadioButton *otherPin = visualLinkManager[visualLinkManager.size()-1];
            NodeModel* otherNodeModel = FindPinModel(otherPin);

            // Make sure the two pins are not from the same node.
            if(otherPin->parent() != newPin->parent())
            {
                //Check if the new pin and the previous are compatible using the objectName variable.
                QStringList newPinData = newPin->objectName().split("::");
                QStringList otherPinData = otherPin->objectName().split("::");

                // COMPATIBILITY CHECKS - Pin compatibitlty and data type compatibility.
                if((otherPinData[0] == "prev") && (newPinData[0] == "next"))
                {
                    qDebug() << "Successfully linked!";
                    visualLinkManager.push_back(newPin);
                    newNodeModel->SetNextLink(otherNodeModel);
                    otherNodeModel->SetPrevLink(newNodeModel);
                    emit closeLink();
                }
                else if((newPinData[0] == "prev") && (otherPinData[0] == "next"))
                {
                    qDebug() << "Successfully linked!";
                    visualLinkManager.push_back(newPin);
                    otherNodeModel->SetNextLink(newNodeModel);
                    newNodeModel->SetPrevLink(otherNodeModel);
                    emit closeLink();
                }
                else if((otherPinData[0] == "input") && (newPinData[0] == "return"))
                {
                    // Check that the datatype of the two pins are compatible
                    if(otherPinData[1] == newPinData[1])
                    {
                        qDebug() << "Successfully linked!";
                        visualLinkManager.push_back(newPin);
                        otherNodeModel->AddInput(newNodeModel, otherPinData[0].toStdString());
                        emit closeLink();
                    }
                    else
                        HandleLinkError(newPin, "Data types of input and return do not match! "
                                                "Please make sure to cast the data to the type required.");
                }
                else if((newPinData[0] == "input") && (otherPinData[0] == "return"))
                {
                    // Check that the datatype of the two pins are compatible
                    if(otherPinData[1] == newPinData[1])
                    {
                        qDebug() << "Successfully linked!";
                        visualLinkManager.push_back(newPin);
                        newNodeModel->AddInput(otherNodeModel, newPinData[2].toStdString());
                        emit closeLink();
                    }
                    else
                        HandleLinkError(newPin, "Data types of input and return do not match! "
                                                "\nPlease make sure to cast the data to the type required.");
                }
                else
                    HandleLinkError(newPin, "Pin link cannot be established! Pins are incompatible.");
            }
            else
                HandleLinkError(newPin, "Pins from the same node cannot be linked!");

            break;
    }

    qDebug() << "Size After: " <<visualLinkManager.size() <<"\n\n";
}

void Graph::HandleLinkError(QRadioButton *newPin, QString errMsg)
{
    newPin->setChecked(false);
    visualLinkManager[visualLinkManager.size()-1]->setChecked(false);
    visualLinkManager.removeAt(visualLinkManager.size()-1);
    _exceptionHandler->DisplayErrorMessage(errMsg, ExceptionHandler::UNDEFINED);
}

void Graph::OnLinkOpened()
{
    qDebug() << "Link Opened";
}

void Graph::OnLinkClosed()
{
    qDebug() << "Link Closed";

}
#pragma endregion comment
