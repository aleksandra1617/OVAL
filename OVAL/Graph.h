#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <unordered_map>

#include <QObject>
#include <QRadioButton>
#include <QFrame>

#include "NodeModel.h"
#include "NodeView.h"
#include "ExceptionHandler.h"

class Graph : public QWidget
{
    Q_OBJECT
public:
    explicit Graph(QFrame* mainFrame, QWidget *parent = nullptr);

    ///@brief The first and the second link that were triggered when attempting to link 2 nodes.
    QList<QRadioButton *> visualLinkManager;

    ///@brief The pins that are currently in process of being linked.
    /// Pin at 0 is the pin that was triggered first, pin at 1 is the pin triggered second.
    /// If there are no active pins the size of the list is 0;
    QList <QLine *> linkViews;

    /**
     * @brief _nodes: A map of all the nodes in the currently opened project.
     */
    std::unordered_map<int, std::pair<NodeView*, NodeModel*>> _nodes;

    /**
     * @brief SetLink: Handles the last pin trigger.
     *
     * If that pin alwready exists it will remove it along with the pin that it is linked to if
     * there is one. Also makes sure that a pin will be added only in the right conditions, i.e.
     * after a pin that it can pair with (input and output, prev & next). The two pins must also
     * be from different NodeViews.
     *
     * @param newPin: The last pin that was triggered.
     * @param nodeModel: The model linked to the NodeView of 'newPin'.
     */
    void SetLink(NodeModel* nodeModel, QRadioButton *newPin);

    void RemovePin(int indexOfNewPin);

    /**
     * @brief FindPinModel: Navigates to the NodeModel linked to the NodeView of 'pin'.
     *
     * @param pin: A given a NodeView element.
     *
     * @return The NodeModel linked to the NodeView of 'pin'.
     */
    NodeModel* FindPinModel(QRadioButton *pin);

    void HandleLinkError(QRadioButton *newPin, QString errMsg);

signals:
    void openLink();
    void closeLink();

private slots:

    /**
     * @brief OnLinkOpened - Triggered when the first pin from a pair is activated.
     */
    void OnLinkOpened();

    /**
     * @brief OnLinkClosed - Triggered when the second pin from a pair is activated.
     */
    void OnLinkClosed();

private:
    ExceptionHandler *_exceptionHandler;
};

#endif // GRAPH_H
