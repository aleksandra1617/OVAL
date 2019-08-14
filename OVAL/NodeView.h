#ifndef NODEVIEW_H
#define NODEVIEW_H
#include <iostream>
#include <vector>

#include <QWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>

///@brief NodeView
/// Describes a node view/UI.
class NodeView : public QWidget
{
    Q_OBJECT

public:
    enum LinkStatus {UNLINKED, START_LINK, LINKED, BROKEN_LINK};
    LinkStatus linkStatus {UNLINKED};

    int UID{-1};
    QLabel* nodeName;

    ///@brief The prev pin and the next pin that will be triggered when attempting to link 2 nodes.
    /// TODO: check that the links are prevPin and nextPin.
    std::vector <QRadioButton*> pins;

    QVBoxLayout* mainNodeLayout; /// Contains both topLayout and bottomLayout.
    QHBoxLayout *topLayout;      /// Contains the Node Name, Previous and Next Links.
    QGridLayout *bottomLayout;   /// Contains the inputs, outputs and/or conditions of the node

    NodeView(QWidget *parent = nullptr);
    NodeView(const NodeView &copy, bool canReturn, QWidget *parent = nullptr);
    NodeView(QString nodeName, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

signals:
    void rightClicked();

private:
    QPoint offset;


    void SetupNodeView(QString nodeNameStr = "[Unassigned]");

private slots:

    /**
     * @brief DragAndDrop
     * Because the nodes should only be able to get dragged and droped in the same widget there
     * is no need to use the QT Drag and Drop functionality, but instead simplify the solution by using the QMouseEvent class.
     *      1. Capture the mouse press event so we know the position we are starting to move.
     *      2. Then, during the mouse move event we’ll check that we’ve moved a certain
     *         distance so we’re sure that the user wants to drag and isn’t just a trembling hand.
     */

    /// @brief  It's triggered when a node linking pin/radio btn has changed it's state.
    //OnCreate() -> Interpreter::Assign()
};

#endif // NODEVIEW_H
