#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <unordered_map>

#include <QMainWindow>
#include <QLayout>
#include <QPushButton>
#include <QList>
#include <QDebug>
#include <QScrollBar>
#include <QRadioButton>

#include "ProjectManager.h"
#include "NodeView.h"
#include "NodeModel.h"
#include "Interpreter.h"

class Graph;
class FileManager;
class ProjectManager;
class ExceptionHandler;

namespace Ui
{
    class MainWindow;
}

/**
 * @brief The MainWindow class manages user input and the overall UI.
 * @details Manages User Input and Interface. The widgets of the same type are grouped by name to
 *          make it more efficient and readable.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

     virtual void paintEvent(QPaintEvent * event) override;

protected:
    /**
     * @brief resizeEvent - Handles resizing the inner widgets on window resize event triggered.
     * @param event
     */
    void resizeEvent(QResizeEvent* event) override;

private slots:

    // MENUBAR SLOTS
    /// @brief OnNewFileActionTriggered: Creates a .cpp for the new file
    void OnNewFileActionTriggered();
    void OnRunAndBuildActionTriggered();
    void OnCleanBuildActionTriggered();

    // NODE SLOTS
    void DetectGenVarBtnClick();
    void DetectGenArrayBtnClick();
    void OnPinTriggered();

    /// @brief Calls the interpreter CreateNode to generate the node view and model.
    void DetectCreateNodeBtnClick();

    /// @brief Displays information about the clicked node onto the top-right side frame.
    void onRightClickedNodeBtn();

    /// @brief Handles Scroll Bars.
    void OnScrollBarValueChanged(int value);

    //PROJECT SLOTS
    void OnInteractionWithSourceFile();

private:
    Ui::MainWindow *ui;

    Graph *_graph;

    //TO MAKE INTO SINGLETONS
    FileManager *_fileManager;
    ExceptionHandler *_exceptionHandler;

    QList <int> prevScrollLocs;
    QSize prevWindowScale;

    /**
     * @details Fetches all QPushButtons in the app, filters out the ones used to create nodes and
     *          connects them to the CreateNode Slot.
     * @attention Add generics to this function so that it can connect all widgets of the same type
     *            to the appropriate slot.
     */
    void ConnectWidgets();

    void HandleScrollBar(int &currLoc, int &prevLoc, QScrollBar *scrollBar, QWidget *widgetToScroll);

    /**
     * @brief MaintainSize - Resizes the given window widget appropriately.
     *        Called on window resize.
     */
    void AdjustSizeTo(QWidget *widgetToAdjust, QSize &byPixels);

    bool ExistsInLayout(QLayout *layout, QString objectName);

    /**
     * @brief Stores all the create btn widgets in a list wit the same order as the NodeType enum
     *        in Interpreter. That way it is easier to identify which node needs to be created.
     */
    void OrganiseCreateNodeBtns(std::vector<QLayout*> layouts);

    /**
     * @brief Searches through a layout for the sender and returns its position in the layout.
     */
    int IdentifyBtnClick(QLayout *layout, QPushButton *sender);

    void GenNode(QString name, QString dataType, QString data, QString codeToInsert, QString constrDetails);

};

#endif // MAINWINDOW_H
