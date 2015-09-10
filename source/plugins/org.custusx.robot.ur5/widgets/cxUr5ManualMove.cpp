#include "cxUr5ManualMove.h"

#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QGroupBox>

namespace cx
{

Ur5ManualMoveTab::Ur5ManualMoveTab(Ur5RobotPtr Ur5Robot,QWidget *parent) :
    QWidget(parent),
    mUr5Robot(Ur5Robot)
{
    setupUi(this);

    connectMovementButtons();

    connect(mUr5Robot.get(),&Ur5Robot::stateUpdated,this,&Ur5ManualMoveTab::updatePositionSlot);
}

Ur5ManualMoveTab::~Ur5ManualMoveTab()
{
}

void Ur5ManualMoveTab::setupUi(QWidget *parent)
{
   mainLayout = new QHBoxLayout(this);

   QWidget *leftColumnWidgets = new QWidget();
   QVBoxLayout *leftColumnLayout = new QVBoxLayout(leftColumnWidgets);

   QWidget *rightColumnWidgets = new QWidget();
   QVBoxLayout *rightColumnLayout = new QVBoxLayout(rightColumnWidgets);

   setMoveToolLayout(leftColumnLayout);
   setCoordInfoWidget(rightColumnLayout);
   setMoveSettingsWidget(leftColumnLayout);
   setJointMoveWidget(rightColumnLayout);

   mainLayout->addWidget(leftColumnWidgets,0,Qt::AlignTop|Qt::AlignLeft);
   mainLayout->addWidget(rightColumnWidgets,0,Qt::AlignTop|Qt::AlignRight);
}

void Ur5ManualMoveTab::setMoveToolLayout(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move tool");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *keyLayout = new QGridLayout();
    group->setLayout(keyLayout);
    keyLayout->setSpacing(0);
    keyLayout->setMargin(0);
    keyLayout->setContentsMargins(0,0,0,0);

    posZButton = new QPushButton(QIcon("/icons/arrow-up-double.png"),"");
    negZButton = new QPushButton(QIcon("/icons/arrow-down-double.png"),"");
    posXButton = new QPushButton(QIcon("/icons/arrow-up.png"),"");
    negXButton = new QPushButton(QIcon("/icons/arrow-down.png"),"");
    posYButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");
    negYButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");

    posZButton->setAutoRepeat(true);
    posZButton->setToolTip("Move in positive Z direction");

    negZButton->setAutoRepeat(true);
    negZButton->setToolTip("Move in negative Z direction");

    posXButton->setAutoRepeat(true);
    posXButton->setToolTip("Move in positive Y direction");

    negXButton->setAutoRepeat(true);
    negXButton->setToolTip("Move in negative Y direction");

    posYButton->setAutoRepeat(true);
    posYButton->setToolTip("Move in positive X direction");

    negYButton->setAutoRepeat(true);
    negYButton->setToolTip("Move in negative X direction");

    rotPosXButton = new QPushButton(QIcon("/icons/edit-redo-7.ico"),"");
    rotNegXButton = new QPushButton(QIcon("/icons/edit-undo-7.ico"),"");
    rotPosYButton = new QPushButton(QIcon("/icons/arrow-up.png"),"");
    rotNegYButton = new QPushButton(QIcon("/icons/arrow-down.png"),"");
    rotPosZButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");
    rotNegZButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");

    rotPosXButton->setAutoRepeat(true);
    rotPosXButton->setToolTip("Rotate counter-clockwise around X axis");

    rotNegXButton->setAutoRepeat(true);
    rotNegXButton->setToolTip("Rotate clockwise around X axis");

    rotPosYButton->setAutoRepeat(true);
    rotPosYButton->setToolTip("Rotate counter-clockwise around Y axis");

    rotNegYButton->setAutoRepeat(true);
    rotNegYButton->setToolTip("Rotate clockwise around Y axis");

    rotPosZButton->setAutoRepeat(true);
    rotPosZButton->setToolTip("Rotate counter-clockwise around Z axis");

    rotNegZButton->setAutoRepeat(true);
    rotNegZButton->setToolTip("Rotate clockwise around Z axis");

    int krow=0;
    keyLayout->addWidget(posZButton, krow, 0, 1, 1);
    keyLayout->addWidget(negZButton, krow,2,1,1);

    krow++;
    keyLayout->addWidget(posXButton,krow,1,1,1);

    krow++;
    keyLayout->addWidget(posYButton,krow,0,1,1);
    keyLayout->addWidget(negYButton,krow,2,1,1);

    krow++;
    keyLayout->addWidget(negXButton,krow,1,1,1);

    krow++;
    keyLayout->addWidget(rotNegXButton, krow, 0, 1, 1);
    keyLayout->addWidget(rotPosXButton, krow,2,1,1);

    krow++;
    keyLayout->addWidget(rotPosYButton,krow,1,1,1);

    krow++;
    keyLayout->addWidget(rotNegZButton,krow,0,1,1);
    keyLayout->addWidget(rotPosZButton,krow,2,1,1);

    krow++;
    keyLayout->addWidget(rotNegYButton,krow,1,1,1);

    // Positive Z Button
    QIcon icon3;
    icon3.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-up-double.png"), QSize(), QIcon::Normal, QIcon::Off);
    posZButton->setIcon(icon3);
    posZButton->setIconSize(QSize(32, 32));
    posZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in positive Z direction", 0));

    // Negative Z Button
    QIcon icon2;
    icon2.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-down-double.png"), QSize(), QIcon::Normal, QIcon::Off);
    negZButton->setIcon(icon2);
    negZButton->setIconSize(QSize(32, 32));
    negZButton->setToolTip(QApplication::translate("Ur5Widget", "Move in negative Z direction", 0));

    // Positive X Button
    QIcon icon4;
    icon4.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-up.png"), QSize(), QIcon::Normal, QIcon::Off);
    posXButton->setIcon(icon4);
    posXButton->setIconSize(QSize(32, 32));

    // Negative Y Button
    QIcon icon5;
    icon5.addFile(QStringLiteral("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-right.png"), QSize(), QIcon::Normal, QIcon::Off);
    negYButton->setIcon(icon5);
    negYButton->setIconSize(QSize(32, 32));

    posYButton->setIcon(QIcon("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-left.png"));
    posYButton->setIconSize(QSize(32, 32));

    negXButton->setIcon(QIcon("C:/Dev/cx/Cx/CX/source/plugins/org.custusx.robot.ur5/icons/arrow-down.png"));
    negXButton->setIconSize(QSize(32, 32));

    // Style sheet for buttons
    posXButton->setStyleSheet("border:none");
    negXButton->setStyleSheet("border:none");
    posYButton->setStyleSheet("border:none");
    negYButton->setStyleSheet("border:none");
    posZButton->setStyleSheet("border:none");
    negZButton->setStyleSheet("border:none");

    rotPosZButton->setIconSize(QSize(32, 32));
    rotPosZButton->setToolTip("Move in rotPositive Z direction");

    rotNegZButton->setIconSize(QSize(32, 32));
    rotNegZButton->setToolTip("Move in rotNegative Z direction");

    rotPosXButton->setIconSize(QSize(32, 32));
    rotNegYButton->setIconSize(QSize(32, 32));
    rotPosYButton->setIconSize(QSize(32, 32));
    rotNegXButton->setIconSize(QSize(32, 32));

    // Style sheet for buttons
    rotPosXButton->setStyleSheet("border:none");
    rotNegXButton->setStyleSheet("border:none");
    rotPosYButton->setStyleSheet("border:none");
    rotNegYButton->setStyleSheet("border:none");
    rotPosZButton->setStyleSheet("border:none");
    rotNegZButton->setStyleSheet("border:none");
}

void Ur5ManualMoveTab::setMoveSettingsWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move settings");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *velAccLayout = new QGridLayout();
    group->setLayout(velAccLayout);

    // Velocity
    velAccLayout->addWidget(new QLabel("Velocity"), 0, 0, 1, 1);
    velocityLineEdit = new QLineEdit();
    velAccLayout->addWidget(velocityLineEdit, 0, 1, 1, 1);
    velocityLineEdit->setText(QApplication::translate("Ur5Widget", "0.1", 0));
    velAccLayout->addWidget(new QLabel("m/s"), 0, 2, 1, 1);

    // Acceleration
    accelerationLineEdit = new QLineEdit();
    velAccLayout->addWidget(accelerationLineEdit, 1, 1, 1, 1);
    accelerationLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));
    velAccLayout->addWidget(new QLabel("Acceleration"), 1, 0, 1, 1);
    velAccLayout->addWidget(new QLabel("m/s^2"), 1, 2, 1, 1);

    // Time
    velAccLayout->addWidget(new QLabel("Time"), 2, 0, 1, 1);
    timeLineEdit = new QLineEdit();
    velAccLayout->addWidget(timeLineEdit, 2, 1, 1, 1);
    timeLineEdit->setText(QApplication::translate("Ur5Widget", "0.5", 0));
    velAccLayout->addWidget(new QLabel("s"), 2, 2, 1, 1);
}

void Ur5ManualMoveTab::setCoordInfoWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Tool position");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *coordInfoLayout = new QGridLayout();
    group->setLayout(coordInfoLayout);

    // Position label
    coordInfoLayout->addWidget(new QLabel("X"), 0, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Y"), 1, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(new QLabel("Z"), 3, 0, 1, 1, Qt::AlignHCenter);

    // mm label
    coordInfoLayout->addWidget(new QLabel("mm"), 0, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 1, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("mm"), 3, 3, 1, 1);

    // Ri orientation label
    coordInfoLayout->addWidget(new QLabel("RX"), 5, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RZ"), 7, 0, 1, 1);
    coordInfoLayout->addWidget(new QLabel("RY"), 6, 0, 1, 1);

    // Rad label
    coordInfoLayout->addWidget(new QLabel("Rad"), 5, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 6, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), 7, 3, 1, 1);

    // X coordinate line edit
    xPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(xPosLineEdit, 0, 2, 1, 1);

    // Y coordinate line edit
    yPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(yPosLineEdit, 1, 2, 1, 1);

    // Line edit for Z position
    zPosLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(zPosLineEdit, 3, 2, 1, 1);

    // Line edit for RX orientation
    rxLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(rxLineEdit, 5, 2, 1, 1);

    // Line edit for RY orientation
    ryLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(ryLineEdit, 6, 2, 1, 1);

    // Line edit for RZ orientation
    rzLineEdit = new QLineEdit();
    coordInfoLayout->addWidget(rzLineEdit, 7, 2, 1, 1);
}

void Ur5ManualMoveTab::setJointMoveWidget(QVBoxLayout *parent)
{
    QGroupBox* group = new QGroupBox("Move joints");
    group->setFlat(true);
    parent->addWidget(group);

    QGridLayout *coordInfoLayout = new QGridLayout();
    group->setLayout(coordInfoLayout);

    coordInfoLayout->setSpacing(5);
    coordInfoLayout->setMargin(5);

    q1PosButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");
    q1NegButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");
    q2PosButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");
    q2NegButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");
    q3PosButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");
    q3NegButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");
    q4PosButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");
    q4NegButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");
    q5PosButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");
    q5NegButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");
    q6PosButton = new QPushButton(QIcon("/icons/arrow-right.png"),"");
    q6NegButton = new QPushButton(QIcon("/icons/arrow-left.png"),"");

    q1PosButton->setAutoRepeat(true);
    q1PosButton->setToolTip("Move in positive Z direction");

    q2PosButton->setAutoRepeat(true);
    q2PosButton->setToolTip("Move in negative Z direction");

    q3PosButton->setAutoRepeat(true);
    q3PosButton->setToolTip("Move in positive Y direction");

    q4PosButton->setAutoRepeat(true);
    q4PosButton->setToolTip("Move in negative Y direction");

    q5PosButton->setAutoRepeat(true);
    q5PosButton->setToolTip("Move in positive X direction");

    q6PosButton->setAutoRepeat(true);
    q6PosButton->setToolTip("Move in negative X direction");

    q1NegButton->setAutoRepeat(true);
    q1NegButton->setToolTip("Move in positive Z direction");

    q2NegButton->setAutoRepeat(true);
    q2NegButton->setToolTip("Move in negative Z direction");

    q3NegButton->setAutoRepeat(true);
    q3NegButton->setToolTip("Move in positive Y direction");

    q4NegButton->setAutoRepeat(true);
    q4NegButton->setToolTip("Move in negative Y direction");

    q5NegButton->setAutoRepeat(true);
    q5NegButton->setToolTip("Move in positive X direction");

    q6NegButton->setAutoRepeat(true);
    q6NegButton->setToolTip("Move in negative X direction");

    q1LineEdit = new QLineEdit();
    q2LineEdit = new QLineEdit();
    q3LineEdit = new QLineEdit();
    q4LineEdit = new QLineEdit();
    q5LineEdit = new QLineEdit();
    q6LineEdit = new QLineEdit();

    int row = 0;
    coordInfoLayout->addWidget(new QLabel("Base"), row, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(q1NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q1PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q1LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Shoulder"), row, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(q2NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q2PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q2LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Elbow"), row, 0, 1, 1, Qt::AlignHCenter);
    coordInfoLayout->addWidget(q3NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q3PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q3LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Wrist 1"), row, 0, 1, 1);
    coordInfoLayout->addWidget(q4NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q4PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q4LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Wrist 2"), row, 0, 1, 1);
    coordInfoLayout->addWidget(q5NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q5PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q5LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);

    row++;
    coordInfoLayout->addWidget(new QLabel("Wrist 3"), row, 0, 1, 1);
    coordInfoLayout->addWidget(q6NegButton,row,1,1,1);
    coordInfoLayout->addWidget(q6PosButton,row,2,1,1);
    coordInfoLayout->addWidget(q6LineEdit, row, 3, 1, 1);
    coordInfoLayout->addWidget(new QLabel("Rad"), row, 4, 1, 1);
}

void Ur5ManualMoveTab::coordButtonPressed(int axis, int sign)
{
    Ur5State velocity;
    velocity.jointVelocity(axis)=(sign)*velocityLineEdit->text().toDouble();
    mUr5Robot->move("speedl",velocity,accelerationLineEdit->text().toDouble(),0,0,timeLineEdit->text().toDouble());
}

void Ur5ManualMoveTab::rotButtonPressed(int angle, int sign)
{
    Ur5State velocity;
    velocity.jointVelocity(angle+3)=(sign)*velocityLineEdit->text().toDouble();
    mUr5Robot->move("speedl",velocity,accelerationLineEdit->text().toDouble(),0,0,timeLineEdit->text().toDouble());
}

void Ur5ManualMoveTab::posZButtonPressedSlot()
{
    coordButtonPressed(2,1);
}

void Ur5ManualMoveTab::negZButtonPressedSlot()
{
    coordButtonPressed(2,-1);
}

void Ur5ManualMoveTab::posYButtonPressedSlot()
{
    coordButtonPressed(1,1);
}

void Ur5ManualMoveTab::negYButtonPressedSlot()
{
    coordButtonPressed(1,-1);
}

void Ur5ManualMoveTab::posXButtonPressedSlot()
{
   coordButtonPressed(0,1);
}

void Ur5ManualMoveTab::negXButtonPressedSlot()
{
    coordButtonPressed(0,-1);
}

void Ur5ManualMoveTab::posRXButtonPressedSlot()
{
    rotButtonPressed(0,1);
}

void Ur5ManualMoveTab::negRXButtonPressedSlot()
{
    rotButtonPressed(0,-1);
}

void Ur5ManualMoveTab::posRYButtonPressedSlot()
{
    rotButtonPressed(1,1);
}

void Ur5ManualMoveTab::negRYButtonPressedSlot()
{
    rotButtonPressed(1,-1);
}

void Ur5ManualMoveTab::posRZButtonPressedSlot()
{
    rotButtonPressed(2,1);
}

void Ur5ManualMoveTab::negRZButtonPressedSlot()
{
    rotButtonPressed(2,-1);
}

void Ur5ManualMoveTab::moveButtonReleasedSlot()
{
    mUr5Robot->stopMove("stopl",accelerationLineEdit->text().toDouble());
}

void Ur5ManualMoveTab::updatePositionSlot()
{
    Ur5State currentState;
    currentState=mUr5Robot->getCurrentState();

    xPosLineEdit->setText(QString::number(1000*(currentState.cartAxis(0)),'f',2));
    yPosLineEdit->setText(QString::number(1000*(currentState.cartAxis(1)),'f',2));
    zPosLineEdit->setText(QString::number(1000*(currentState.cartAxis(2)),'f',2));
    rxLineEdit->setText(QString::number(currentState.cartAngles(0),'f',4));
    ryLineEdit->setText(QString::number(currentState.cartAngles(1),'f',4));
    rzLineEdit->setText(QString::number(currentState.cartAngles(2),'f',4));
}

void Ur5ManualMoveTab::connectMovementButtons()
{
    connect(posXButton,SIGNAL(pressed()),this,SLOT(posXButtonPressedSlot()));
    connect(posXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(negXButton,SIGNAL(pressed()),this,SLOT(negXButtonPressedSlot()));
    connect(negXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(posYButton,SIGNAL(pressed()),this,SLOT(posYButtonPressedSlot()));
    connect(posYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(negYButton,SIGNAL(pressed()),this,SLOT(negYButtonPressedSlot()));
    connect(negYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(posZButton,SIGNAL(pressed()),this,SLOT(posZButtonPressedSlot()));
    connect(posZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(negZButton,SIGNAL(pressed()),this,SLOT(negZButtonPressedSlot()));
    connect(negZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotPosXButton,SIGNAL(pressed()),this,SLOT(posRXButtonPressedSlot()));
    connect(rotPosXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotNegXButton,SIGNAL(pressed()),this,SLOT(negRXButtonPressedSlot()));
    connect(rotNegXButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotPosYButton,SIGNAL(pressed()),this,SLOT(posRYButtonPressedSlot()));
    connect(rotPosYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotNegYButton,SIGNAL(pressed()),this,SLOT(negRYButtonPressedSlot()));
    connect(rotNegYButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotPosZButton,SIGNAL(pressed()),this,SLOT(posRZButtonPressedSlot()));
    connect(rotPosZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));

    connect(rotNegZButton,SIGNAL(pressed()),this,SLOT(negRZButtonPressedSlot()));
    connect(rotNegZButton,SIGNAL(released()),this,SLOT(moveButtonReleasedSlot()));
}

} // cx
