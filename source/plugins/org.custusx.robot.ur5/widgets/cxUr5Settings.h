#ifndef UR5SETTINGS_H
#define UR5SETTINGS_H

#include "org_custusx_robot_ur5_Export.h"
#include "cxUr5Robot.h"

#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollBar>
#include <QButtonGroup>

namespace cx
{

class org_custusx_robot_ur5_EXPORT Ur5SettingsTab : public QWidget
{
    Q_OBJECT
public:
    Ur5SettingsTab(Ur5RobotPtr Ur5Robot, VisServicesPtr services, QWidget *parent = 0);
    virtual ~Ur5SettingsTab();

private:
    QHBoxLayout *mainLayout;
    void setupUi(QWidget *parent);

    Ur5RobotPtr mUr5Robot;
    VisServicesPtr mServices;

    void setMoveToolLayout(QVBoxLayout *vLayout);
    void setMoveSettingsWidget(QVBoxLayout *vLayout);
    void setCoordInfoWidget(QVBoxLayout *vLayout);
    void setJointMoveWidget(QVBoxLayout *vLayout);
};

} // cx

#endif // UR5SETTINGS_H
