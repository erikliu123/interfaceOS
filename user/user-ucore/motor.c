#include<arch.h>
#include<types.h>

#define speed1 1024*1024-1
#define speed2 768*1024
#define speed3 384*1024
#define speed4 0
#define rb_f 0x00000020
#define lb_f 0x00000040
#define rf_f 0x00000080
#define lf_f 0x00000004
#define rb_b 0x00000010
#define lb_b 0x00000008
#define rf_b 0x00000002
#define lf_b 0x00000001

int gear_lb,gear_lf,gear_rb, gear_rf;
int dir_data;//serial
int state;

#define WHEEL_RF 0x1
#define WHEEL_RB
#define WHEEL_LF 
#define WHEEL_LB

int gear2speed(int *gear)
{
    if (*gear == 1)
    {
        return speed3;
    }
    else if (*gear == 2)
    {
        return speed2;
    }
    else if (*gear == 3)
    {
        return speed1;
    }
    else if (*gear == -1)
    {
        return speed3;
    }
    else if (*gear == -2)
    {
        return speed2;
    }
    else if (*gear == -3)
    {
        return speed1;
    }
    else if (*gear >= 3)
    {
        *gear = 3;
        return speed1;
    }
    else if (*gear <= -3)
    {
        *gear = -3;
        return speed1;
    }
    else
    {
        return 0;
    }
}
void set_gear(int lf, int lb, int rf, int rb)
{
    gear_lb = lb;
    gear_lf = lf;
    gear_rb = rb;
    gear_rf = rf;
    _go();
}
void _go(void)
{
    int direction = 0;
    direction = direction | (gear_rf>=0?rf_f:rf_b);
    direction = direction | (gear_rb>=0?rb_f:rb_b);
    direction = direction | (gear_lf>=0?lf_f:lf_b);
    direction = direction | (gear_lb>=0?lb_f:lb_b);
    *WRITE_IO(dir_data) = direction;
    delay();
    *WRITE_IO(WHEEL_RF) = gear2speed(&gear_rf);
    delay();
    *WRITE_IO(WHEEL_LB) = gear2speed(&gear_lb);
    delay();
    *WRITE_IO(WHEEL_LF) = gear2speed(&gear_lf);
    delay();
    *WRITE_IO(WHEEL_RB) = gear2speed(&gear_rb);
    delay();
}
void speedup(void)
{
//小车当前状态不是直行
    if (state != 0)
    {
        state = 0;
        set_gear(2, 2, 2, 2);
    }
    else
    {
        set_gear(gear_lf + 1, gear_lb + 1, gear_rf + 1, gear_rb + 1);
    }
}
void slowdown(void)
{
    if (state != 0)
    {
        state = 0;
        set_gear(0, 0, 0, 0);
    }
    else
    {
        set_gear(gear_lf - 1, gear_lb - 1, gear_rf - 1, gear_rb - 1);
    }
}
void leftforward(void)
{
    state = 1;
    set_gear(0, 1, 3, 3);
}
void rightforward(void)
{
    state = 1;
    set_gear(3, 3, 0, 1);
}
void turnright(void)
{
    state = 1;
    set_gear(2, 2, -2, -2);
}
void turnleft(void)
{
    state = 1;
    set_gear(-2, -2, 2, 2);
}
void mystop(void)
{
    state = 0;
    set_gear(0, 0, 0, 0);
}

int main(int argc, char const *argv[])
{
    turnleft();
    _go();
    return 0;
}
