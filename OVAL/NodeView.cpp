#include "NodeView.h"
#include <QMouseEvent>
#include <QPoint>
#include <QLabel>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QColor>
#include <QDebug>

#include <cstdint>
#include <cstring>

void NodeView::SetupNodeView(QString nodeNameStr)
{
    //COLOUR SCHEME
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(68, 127, 186, 220));

    this->setStyleSheet("QFrame{ border-style: outset;\
                         color: #e3e9f2;\
                         border-width: 2px;\
                         border-radius: 10px;\
                         border-color: beige;\
                         font: bold 12px;\
                         min-width: 3em;\
                         padding: 3px;}");

    this->setAutoFillBackground(true);
    this->setPalette(pal);

    //NODE Sequence pins
    QRadioButton *prev = new QRadioButton("");
    QRadioButton *next = new QRadioButton("");
    prev->setAutoExclusive(false);
    next->setAutoExclusive(false);
    prev->setGeometry(0,0,16,16);
    next->setGeometry(0,0,16,16);
    prev->setObjectName("prev");
    next->setObjectName("next");

    //NODE FIELDS
    nodeName = new QLabel(nodeNameStr);
    nodeName->setAlignment(Qt::AlignHCenter);
    nodeName->setGeometry(0, 0, 80, 80);
    nodeName->setStyleSheet("QLabel{background-color : #447FBA}");

    //SUB LAYOUT
    topLayout = new QHBoxLayout();

    if(nodeNameStr != "Start")
        topLayout->addWidget(prev);

    topLayout->addWidget(nodeName, 100);
    topLayout->addWidget(next);

    bottomLayout = new QGridLayout();

    //MAIN LAYOUT
    mainNodeLayout = new QVBoxLayout(this);
    mainNodeLayout->addLayout(topLayout);
    this->setLayout(mainNodeLayout);

}

NodeView::NodeView(const NodeView &copy, bool canReturn, QWidget *parent) : QWidget(parent)
{
    //COLOUR SCHEME
    QPalette pal;
    pal.setColor(QPalette::Background, QColor(68, 127, 186, 220));

    SetupNodeView(copy.nodeName->text());

    for(int c = 0; c < copy.bottomLayout->rowCount(); c++)
    {
        if (canReturn && (c == copy.bottomLayout->rowCount()-1))
        {
            QLabel* fieldType = (QLabel*)copy.bottomLayout->itemAtPosition(c, 0)->widget();
            QLabel* fieldName = (QLabel*)copy.bottomLayout->itemAtPosition(c, 1)->widget();
            QLabel *copyfieldType = new QLabel(fieldType->text());
            QLabel *copyfieldName = new QLabel(fieldName->text());
            copyfieldType->setStyleSheet("QLabel{background-color: white; color : #447FBA;}");
            copyfieldName->setStyleSheet("QLabel{background-color: white; color : #447FBA;}");

            QRadioButton* rb = new QRadioButton("");
            rb->setAutoExclusive(false);
            rb->setObjectName("return::"+fieldType->text()+"::"+fieldName->text());

            this->bottomLayout->addWidget(copyfieldType, c, 0, Qt::AlignRight);
            this->bottomLayout->addWidget(copyfieldName, c, 1, Qt::AlignRight);
            this->bottomLayout->addWidget(rb, c, 2, Qt::AlignRight);
        }
        else
        {
            QLabel* fieldType = (QLabel*)copy.bottomLayout->itemAtPosition(c, 1)->widget();
            QLabel* fieldName = (QLabel*)copy.bottomLayout->itemAtPosition(c, 2)->widget();
            QLabel *copyfieldType = new QLabel(fieldType->text());
            QLabel *copyfieldName = new QLabel(fieldName->text());
            copyfieldType->setStyleSheet("QLabel{background-color: white; color : #447FBA;}");
            copyfieldName->setStyleSheet("QLabel{background-color: white; color : #447FBA;}");

            QRadioButton* rb = new QRadioButton("");
            rb->setAutoExclusive(false);
            rb->setObjectName("input::"+fieldType->text()+"::"+fieldName->text());

            this->bottomLayout->addWidget(rb, c, 0, Qt::AlignLeft);
            this->bottomLayout->addWidget(copyfieldType, c, 1, Qt::AlignLeft);
            this->bottomLayout->addWidget(copyfieldName, c, 2, Qt::AlignLeft);

        }
    }

    mainNodeLayout->addLayout(bottomLayout);
    this->setLayout(mainNodeLayout);

}

NodeView::NodeView(QWidget *parent) : QWidget(parent) //TODO: Pass a different colour based on the node type
{
    SetupNodeView();
}

NodeView::NodeView(QString nodeNameStr, QWidget *parent) : QWidget(parent)
{
    SetupNodeView(nodeNameStr);
}

#pragma region [DRAG AND DROP]
void NodeView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        offset = event->pos();
    }

    if(event->button() == Qt::RightButton)
    {
        emit rightClicked();
    }
}
void NodeView::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton))
    {
        this->move(mapToParent(event->pos() - offset));
        this->updateGeometry();
        this->update();
    }
}
#pragma endregion
