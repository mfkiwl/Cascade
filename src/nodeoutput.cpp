#include "nodeoutput.h"
#include <QMouseEvent>
#include <iostream>
#include <QGraphicsLineItem>
#include <QPen>
#include <QColor>

NodeOutput::NodeOutput(QWidget *parent)
    : QPushButton (parent)
{
    // Keep reference to parent node
    parentNode = static_cast<NodeBase*>(parent);

    this->resize(visualWidth, visualHeight);
}

void NodeOutput::addConnection(Connection *c)
{
    outConnections.push_back(c);
}

void NodeOutput::updateConnections()
{
    foreach(Connection* c, outConnections)
    {
        c->updatePosition();
    }
}

void NodeOutput::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit nodeOutputLeftMouseClicked(this);
        //this->isDragging = true;
    }
}

std::vector<Connection*> NodeOutput::getConnections()
{
    return outConnections;
}

//void NodeOutput::mouseMoveEvent(QMouseEvent *event)
//{
//    if (isDragging)
//    {
//        emit requestConnectionUpdate(parentNode);
//    }
//    Q_UNUSED(event);
//}

//void NodeOutput::mouseReleaseEvent(QMouseEvent *event)
//{
//    if (isDragging)
//    {
//        isDragging = false;
//        emit leftMouseReleaseFromNodeOut(event);
//        //emit requestConnectionAttachment(parentNode);
//    }
//}
