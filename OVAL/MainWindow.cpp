#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "Graph.h"

#include <QPalette>
#include <QList>
#include <QPainter>

#include <iostream>
#include <string>


void MainWindow::ConnectWidgets()
{
    // LOAD PROJECT
    ui->ConsoleText->append(_fileManager->LoadFile(ProjectManager::Instance().currentPath,
                                                   ProjectManager::Instance().currentName + ".cpp"));

    ui->ConsoleText->setDocumentTitle(ProjectManager::Instance().currentName + ".cpp");
    connect(ui->ConsoleText, SIGNAL(cursorPositionChanged()), this, SLOT(OnInteractionWithSourceFile()));

    //GET all QPushBtns on the window
    QList<QPushButton*> allQPushBtns = this->findChildren<QPushButton*>();

    //Filter the ones that are used to create a node
    foreach(QPushButton* btn, allQPushBtns)
    {
        //All btns used to create a node are named Create...btn -> Example CreateIfBtn
        QString btnName = btn->objectName();

        //Check if the first 6 characters of the btn name are "Create"
        btnName.resize(6);
        if (btnName == "Create")
        {
            connect(btn, SIGNAL(clicked()), this, SLOT(DetectCreateNodeBtnClick()));
        }
    }

    connect(ui->GenVariableBtn, SIGNAL(clicked()), this, SLOT(DetectGenVarBtnClick()));
    connect(ui->GenArrayBtn, SIGNAL(clicked()), this, SLOT(DetectGenArrayBtnClick()));

    // Connect Menu Bar Actions
    connect(ui->NewFileAction, SIGNAL(triggered()), this, SLOT(OnNewFileActionTriggered()));
    connect(ui->BuildAndRunAction, SIGNAL(triggered()), this, SLOT(OnRunAndBuildActionTriggered()));
    connect(ui->CleanAction, SIGNAL(triggered()), this, SLOT(OnCleanBuildActionTriggered()));

    // Connect start pin
    QRadioButton *next = qobject_cast<QRadioButton *>(_graph->_nodes[0].first->topLayout->itemAt(1)->widget());
    connect(next, SIGNAL(clicked()), this, SLOT(OnPinTriggered()));

    // Setup Scrollbars
    prevScrollLocs = {0, 0}; // the number of ints in this list needsto be the same as the numebr of scrollbars connected.
    connect(ui->scrollBar0, SIGNAL(valueChanged(int)), this, SLOT(OnScrollBarValueChanged(int)));
    connect(ui->scrollBar1, SIGNAL(valueChanged(int)), this, SLOT(OnScrollBarValueChanged(int)));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    prevWindowScale = this->size();

    ui->setupUi(this);

    _graph = new Graph(ui->MainFrame);
    _fileManager = new FileManager();
    _exceptionHandler = new ExceptionHandler();

    this->setCentralWidget(ui->horizontalLayoutWidget);

    ConnectWidgets();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnInteractionWithSourceFile()
{
    ui->Console->setStyleSheet("");
}

void MainWindow::AdjustSizeTo(QWidget *widgetToAdjust, QSize &byPixels)
{
    QSize newSize = widgetToAdjust->size() + byPixels;
    widgetToAdjust->resize(newSize);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // Calculate by how many pixcelt the window has grown.
    QSize newWinScale = this->size() - prevWindowScale;

    // Adjust Docked Console to Window
    AdjustSizeTo(ui->ConsoleText, newWinScale);

    //Adjust Inner Frames' Widgets
    QSize scrollBarHeight(0, ui->scrollBar1->size().height()); //Needed to properly set the MainFrame height
    QSize mainFrameHeight(0, ui->MainFrame->size().height());  //Needed to properly set the ScrollBar Position

    QSize mainFrameSizeDiff = ui->Frame->size() - ui->MainFrame->size() - scrollBarHeight;

    AdjustSizeTo(ui->MainFrame, mainFrameSizeDiff);

    ui->scrollBar1->setGeometry(ui->scrollBar1->x(),
                                mainFrameHeight.height(),
                                ui->Frame->size().width(),
                                ui->Frame->size().height());

    /// @attention the size difference is wrong the first time it is set until it is resized.
    //qDebug() << mainFrameSizeDiff;

    prevWindowScale = this->size();
}

int MainWindow::IdentifyBtnClick(QLayout *layout, QPushButton *sender)
{
    for(int c = 0; c < layout->count(); c++)
    {
        //Fetch the button using it's index in the grid.
        QPushButton* btn = (QPushButton*)layout->itemAt(c)->widget();

        //Check which Create btn was clicked
        if (sender == btn)
        {
            return c;
        }
    }
    return -1;
}

bool MainWindow::ExistsInLayout(QLayout *layout, QString objectName)
{
    bool exist = false;
    for (int c = 0; c < layout->count(); c++)
    {
        // QT Dynamic Cast - useful to test if the cast can occur, and stop the program from crashing.
        if(qobject_cast<QPushButton*>(layout->itemAt(c)->widget()) != 0)
        {
            QPushButton *btn = qobject_cast<QPushButton*>(layout->itemAt(c)->widget());

            if(btn->objectName() == objectName)
            {
                exist = true;
            }
        }
    }
    return exist;
}

#pragma region NODE SLOTS
void MainWindow::DetectCreateNodeBtnClick()
{
    int nodeID = -1;

    // Needed to offset the fact that start node does not belong to a layout
    // as there should not be a btn to create a start node becasue it should
    // be the only existing one.
    QHBoxLayout *filler =  new QHBoxLayout();
    filler->addWidget(new QWidget());
    filler->addWidget(new QWidget());

    ///@attention The order at which the layouts are added should match the NodeType enum and vise versa.
    std::vector<QLayout*> layoutsToSearchThrough = {ui->CppNodeMenuLayout, ui->OGLNodeMenuLayout, filler,
                                                    ui->VarNodeMenuLayout};

    for(unsigned int c = 0; c < layoutsToSearchThrough.size(); c++)
    {
        QLayout* layout = layoutsToSearchThrough[c];
        if(IdentifyBtnClick(layout, (QPushButton*)sender()) != -1)
        {
            nodeID = IdentifyBtnClick(layout, (QPushButton*)sender());

            // Offset nodeID by the size of the previous layouts -1 so that it is the index of the last widget.
            for(unsigned int count = 0; count < c; count++)
                nodeID += layoutsToSearchThrough[count]->count()-1;
        }
    }

    if(nodeID != -1)
    {
        // Then dismantle the object name by removing the first 6 char (Create), the last 3 characters (btn) and any spaces
        // this will help match the name of the btn with the enum and the node name.

        ProjectManager::Instance().currentUID += 1;

        /**
         * @brief Calls the interpreter CreateNode to generate the node view and model.
         */
        std::pair<NodeView*, NodeModel*> newNode = Interpreter::Instance().CreateNode(nodeID,
                                                                           ProjectManager::Instance().currentUID);

        if(newNode.first != nullptr && newNode.second != nullptr)
        {
            QRadioButton *prev = qobject_cast<QRadioButton *>(newNode.first->topLayout->itemAt(0)->widget());
            QRadioButton *next = qobject_cast<QRadioButton *>(newNode.first->topLayout->itemAt(2)->widget());

            if(prev != nullptr && next != nullptr)
            {
                connect(prev, SIGNAL(clicked()), this, SLOT(OnPinTriggered()));
                connect(next, SIGNAL(clicked()), this, SLOT(OnPinTriggered()));
            }else
                _exceptionHandler->DisplayErrorMessage("Node link/s are null!", ExceptionHandler::UNDEFINED);

            for(int c = 0; c < newNode.first->bottomLayout->count(); c++)
            {
                 QRadioButton *field = qobject_cast<QRadioButton *>(newNode.first->bottomLayout->itemAt(c)->widget()); //itematPos

                 if(field != nullptr)
                     connect(field, SIGNAL(clicked()), this, SLOT(OnPinTriggered()));

            }
            connect(newNode.first, SIGNAL(rightClicked()), this, SLOT(onRightClickedNodeBtn()));

            newNode.first->setParent(ui->MainFrame);
            newNode.first->show();

            _graph->_nodes.insert({newNode.second->UID, newNode});
        }else
            _exceptionHandler->DisplayErrorMessage("Unable to create node, missing prototype.", ExceptionHandler::INDEX_OUT_OF_RANGE);
    }else
        _exceptionHandler->DisplayErrorMessage("Node type ID not set.", ExceptionHandler::UNDEFINED);
}

void MainWindow::onRightClickedNodeBtn()
{
    NodeView* nodeRightClicked = qobject_cast<NodeView*>(sender());

    if(nodeRightClicked != nullptr)
    {
        qDebug()<< "Node "<<nodeRightClicked->nodeName->text()<<"Right clicked";
        QByteArray allNodeInfo = _fileManager->LoadFile("","NodeInfo.txt");
        QList <QByteArray> nodeInfo = allNodeInfo.split('@');
        qDebug() << Interpreter::Instance().nodeTypes.lastIndexOf(nodeRightClicked->nodeName->text());

        QByteArray thisNodeInfo = nodeInfo[Interpreter::Instance().nodeTypes.lastIndexOf(nodeRightClicked->nodeName->text())];
        ui->Information->setText(thisNodeInfo);
    }

}

void MainWindow::GenNode(QString name, QString dataType, QString data, QString codeToInsert, QString constrDetails)
{
    // Check if is undefined.
    if((name == "") || (name == ""))
        _exceptionHandler->DisplayErrorMessage("\nVariable Undefined! Please fill in all the fields. \n", ExceptionHandler::UNDEFINED);
    else
    {
        QPushButton *btnToInsert = new QPushButton(name);
        btnToInsert->setObjectName("Create"+name);

        // Check if the variable btn exists in the VarNodeMenuLayout so that there is no repetition.
        if(!ExistsInLayout(ui->VarNodeMenuLayout, btnToInsert->objectName()))
        {
            //TODO: Check if variable name starts with an underscore or a letter use the try catch from exception handler.

            // Check if the variable type matchas the data.
            if(_exceptionHandler->TryCatch(dataType, data))
            {
                // Add a btn for creating this variable
                int secondToLastIndex = ui->VarNodeMenuLayout->count()-1;
                ui->VarNodeMenuLayout->insertWidget(secondToLastIndex, btnToInsert);
                btnToInsert->setStyleSheet("QPushButton{\
                                           color: #addcff;\
                                           border-style: outset;\
                                           background-color: #447fba;\
                                           border-width: 2px;\
                                           border-radius: 10px;\
                                           border-color: #e3e9f2;\
                                           font: bold 12px;\
                                           min-width: 4em;\
                                           padding: 4px;\
                                           }");
                connect(btnToInsert, SIGNAL(clicked()), this, SLOT(DetectCreateNodeBtnClick()));
                connect(btnToInsert, SIGNAL(pressed()), this, SLOT(onRightClickedNodeBtn()));
                //TODO DETECT MIDDLE BTN CLICK

                // Create variable prototype
                Interpreter::Instance().nodeTypes.push_back(name);


                std::pair<NodeView*, NodeModel*> node = Interpreter::Instance().BuildNodePrototype(codeToInsert,
                                                                                        Interpreter::Instance().nodeTypes.size()-1,
                                                                                        constrDetails.split(","));

                //Remove prev and next pins as the variables should not have any.
                Interpreter::Instance()._nodePrototypes.insert({Interpreter::Instance().nodeTypes.size()-1, node});
            }
        }
    }
}

void MainWindow::DetectGenVarBtnClick()
{
    QString newVariableName = ui->varName->toPlainText();
    QString newVariableData = ui->varData->toPlainText();
    QString variableDataType = ui->varDataType->currentText();

    QString codeToInsert;

    // GENERATE THE VARIABLE NODE
    if(variableDataType == "string")
        codeToInsert = "\n\t" + variableDataType +" "+newVariableName+" = \""+newVariableData +"\";";
    else
        codeToInsert = "\n\t"+variableDataType +" "+newVariableName+" = "+newVariableData +";";

    QString constrDetails = variableDataType+"::return";
    GenNode(newVariableName, variableDataType, newVariableData, codeToInsert, constrDetails);

    Interpreter::Instance().AccVarDeclarations(codeToInsert);

    // GENERATE A SETTER NODE
    codeToInsert = "\n\t"+newVariableName+"="+"#>set<#;";
    constrDetails = variableDataType+"::set, "+"void::return";
    GenNode("set_"+newVariableName, variableDataType, newVariableData, codeToInsert, constrDetails);

}

void MainWindow::DetectGenArrayBtnClick()
{
    QString newArrayName = ui->arrayName->toPlainText();
    QString newArrayData = ui->arrayData->toPlainText();
    QString arrayDataType = ui->arrayDataType->currentText();

    QString codeToInsert = arrayDataType +" "+newArrayName+"[] = \n\t{\n"+newArrayData+"\n\t};";
    QString constrDetails = arrayDataType+"::return";

    GenNode(newArrayName, arrayDataType, newArrayData, codeToInsert, constrDetails);
}

void MainWindow::OnPinTriggered()
{
    //Find the NodeModel of the pin
    QRadioButton* pin = qobject_cast<QRadioButton *>(sender());
    NodeModel* nodeModel = _graph->FindPinModel(pin);

    // Check if newPin exists in the visualLinkManager.
    int indexOfNewPin = _graph->visualLinkManager.indexOf(pin);

    // if newPin found, remove it else add the new pin to the link manager.
    if(indexOfNewPin == -1)
        _graph->SetLink(nodeModel, pin);
    else
        _graph->RemovePin(indexOfNewPin);

}
#pragma endregion comment

#pragma region PRIVATE MENU BAR SLOTS
void MainWindow::OnNewFileActionTriggered()
{
    _fileManager->CleanFile("main source", "OpenGL-Project-Files/", "Demo1.cpp");
}
#pragma endregion comment

void MainWindow::OnRunAndBuildActionTriggered()
{
    if((ProjectManager::Instance().currentName == "UNKNOWN") || (ProjectManager::Instance().currentPath == "UNKNOWN"))
    {
        _exceptionHandler->DisplayErrorMessage("\nUnknown Project name or path! \n", ExceptionHandler::UNDEFINED);
    }
    else
    {
        QList<QString> listOfBatchCommands = {"ChangeDirectory", "Build", "Run"};
        ProjectManager::Instance().GenBatchFile(ProjectManager::Instance().currentPath,
                                                ProjectManager::Instance().currentName,
                                                listOfBatchCommands);

        std::string batchFile = ProjectManager::Instance().currentName.toStdString()+".bat";
        system(batchFile.c_str());
    }
}

void MainWindow::OnCleanBuildActionTriggered()
{
    bool canBuild = true;
    QString accSourceString;
    NodeModel *start = nullptr;

    _fileManager->CleanFile("main source", "OpenGL-Project-Files/", "Demo1.cpp");

    // Find and set the start node.
    for(unsigned int c = 0; c < _graph->_nodes.size(); c++)
    {
        std::pair<NodeView*, NodeModel*> current = _graph->_nodes.at(c);
        qDebug() << current.second->name.c_str();

        if (current.second->name == "Start")
        {
            start = current.second;
        }
    }

    // Check if a project is opened/setup.
    if((ProjectManager::Instance().currentPath == "UNKNOWN") || (ProjectManager::Instance().currentPath == "UNKNOWN"))
        _exceptionHandler->DisplayErrorMessage("\nUnknown Project name or path!  \n", ExceptionHandler::UNDEFINED);
    else
    {
        // TRAVERSE THE LINKED LIST
        canBuild = Interpreter::Instance().TraverseModel(start, ProjectManager::Instance().currentPath,
                                                         ProjectManager::Instance().currentName);

        // Generate batch file and executable
        if (canBuild != false)
        {
            QList<QString> listOfBatchCommands = {"ChangeDirectory", "Build"};
            ProjectManager::Instance().GenBatchFile(ProjectManager::Instance().currentPath,
                                                    ProjectManager::Instance().currentName,
                                                    listOfBatchCommands);

            std::string batchFile = ProjectManager::Instance().currentName.toStdString()+".bat";
            system(batchFile.c_str());
        }
        else
            _exceptionHandler->DisplayErrorMessage("Build failed! Linked node is not assigned!", ExceptionHandler::UNDEFINED);

        // RELOAD PROJECT CPP
        ui->ConsoleText->setText(_fileManager->LoadFile(ProjectManager::Instance().currentPath,
                                                       ProjectManager::Instance().currentName + ".cpp"));

        ui->Console->setStyleSheet("QDockWidget{background-color : green}");
    }
}

void MainWindow::OnScrollBarValueChanged(int value)
{
    int *prevScrollLoc = nullptr;
    QWidget* widgetToScroll = nullptr;

    // Find the widget to scroll given a list of its parent's children
    QObjectList children = sender()->parent()->children();
    for (int c = 0; c < children.size(); c++)
    {
        if(children[c]->metaObject()->className() != sender()->metaObject()->className())
            widgetToScroll = static_cast<QWidget*>(children[c]);
    }

    // Set prevScrollLoc - scrollBar0, scrollBar1
    //qDebug() << sender()->objectName().back().digitValue();
    prevScrollLoc = &prevScrollLocs[sender()->objectName().back().digitValue()];


    //the prevLoc - mainFrameScrollPrevLoc;
    HandleScrollBar(value, *prevScrollLoc, static_cast<QScrollBar *>(sender()), widgetToScroll);
}

void MainWindow::HandleScrollBar(int &currLoc, int &prevLoc, QScrollBar *scrollBar, QWidget *widgetToScroll)
{
    int x = widgetToScroll->x();
    int y = widgetToScroll->y();
    int w = widgetToScroll->width();
    int h = widgetToScroll->height();

    if(scrollBar->width() < scrollBar->height()) // Scrollbar is veritcal
    {
        if(currLoc > prevLoc)
            widgetToScroll->setGeometry(x, y - (currLoc - prevLoc), w, h + currLoc);
        else
            widgetToScroll->setGeometry(x, y + (prevLoc - currLoc), w, h - currLoc);
    }
    else
    {
        if(currLoc > prevLoc)
            widgetToScroll->setGeometry(x - (currLoc - prevLoc), y, w + currLoc, h);
        else
            widgetToScroll->setGeometry(x + (prevLoc - currLoc), y, w - currLoc, h);
    }

    prevLoc = currLoc;

    if(currLoc >= scrollBar->maximum()/2)
        scrollBar->setMaximum(scrollBar->maximum() + 10);
}

void MainWindow::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    QPen pen(Qt::black, 8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    //QPixmap pm("VisualLink.png");
    //QBrush brush(pm.toImage());
    painter.setPen(pen);
    //painter.setBrush(brush);

    if (!_graph->visualLinkManager.empty())
    {
        for (int c = 0; c < _graph->visualLinkManager.size()-1; c+=2)
        {
            QRadioButton *firstPin = _graph->visualLinkManager[c];
            QRadioButton *secondPin = _graph->visualLinkManager[c+1];
            //firstPin->update();
            //secondPin->update();

            QWidget* firstNodeView = static_cast<QWidget*>(firstPin->parent());
            QWidget* secondNodeView = static_cast<QWidget*>(secondPin->parent());
            QPoint offset(195 - ui->scrollBar1->value(), 40);

            QLine line (firstNodeView->pos()+firstPin->pos() + offset,
                        secondNodeView->pos()+secondPin->pos() + offset);

            painter.drawLine(line);
            this->update();
        }
    }

}
