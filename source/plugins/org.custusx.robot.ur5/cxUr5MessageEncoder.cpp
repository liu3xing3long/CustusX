#include "cxUr5MessageEncoder.h"


namespace cx
{

QString Ur5MessageEncoder::movej(Ur5State p,double a, double v,double r)
{
    return QString("movej(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8,r=%9)")
            .arg(p.cartAxis(0)).arg(p.cartAxis(1)).arg(p.cartAxis(2)).arg(p.cartAngles(0))
            .arg(p.cartAngles(1)).arg(p.cartAngles(2)).arg(a).arg(v).arg(r);
}

QString Ur5MessageEncoder::movel(Ur5State p,double a, double v)
{
    return QString("movel(p[%1,%2,%3,%4,%5,%6],a=%7,v=%8)")
            .arg(p.cartAxis(0)).arg(p.cartAxis(1)).arg(p.cartAxis(2)).arg(p.cartAngles(0))
            .arg(p.cartAngles(1)).arg(p.cartAngles(2)).arg(a).arg(v);
}

QString Ur5MessageEncoder::movec(Ur5State pose_via,Ur5State pose_to, double a, double v, double r)
{
    return QString("movec(p[%1,%2,%3,%4,%5,%6],p[%7,%8,%9,%10,%11,%12],a=%13,v=%14,r=%15")
            .arg(pose_via.cartAxis(0)).arg(pose_via.cartAxis(1)).arg(pose_via.cartAxis(2))
            .arg(pose_via.cartAngles(0)).arg(pose_via.cartAngles(1)).arg(pose_via.cartAngles(2))
            .arg(pose_to.cartAxis(0)).arg(pose_to.cartAxis(1)).arg(pose_to.cartAxis(2))
            .arg(pose_to.cartAngles(0)).arg(pose_to.cartAngles(1)).arg(pose_to.cartAngles(2))
            .arg(a).arg(v).arg(r);
}

QString Ur5MessageEncoder::speedj(Ur5State p, double a, double t)
{
    return QString("speedj([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(p.jointVelocity(0)).arg(p.jointVelocity(1)).arg(p.jointVelocity(2)).arg(p.jointVelocity(3))
            .arg(p.jointVelocity(4)).arg(p.jointVelocity(5)).arg(a).arg(t);
}

QString Ur5MessageEncoder::speedl(Ur5State p, double a, double t)
{
    return QString("speedl([%1,%2,%3,%4,%5,%6],a=%7,t_min=%8)")
            .arg(p.jointVelocity(0)).arg(p.jointVelocity(1)).arg(p.jointVelocity(2)).arg(p.jointVelocity(3))
            .arg(p.jointVelocity(4)).arg(p.jointVelocity(5)).arg(a).arg(t);
}

QString Ur5MessageEncoder::set_tcp(Ur5State p)
{
    return QString("set_tcp(p[%1,%2,%3,%4,%5,%6])")
            .arg(p.cartAxis(0)).arg(p.cartAxis(1)).arg(p.cartAxis(2)).arg(p.cartAngles(0))
            .arg(p.cartAngles(1)).arg(p.cartAngles(2));
}

QString Ur5MessageEncoder::stopl(double a)
{
    return QString("stopl(%1)").arg(a);
}

QString Ur5MessageEncoder::stopj(double a)
{
    return QString("stopj(%1)").arg(a);
}

QString Ur5MessageEncoder::powerdown()
{
    return QString("powerdown()");
}




} // cx
