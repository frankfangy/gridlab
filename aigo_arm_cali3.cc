#include "aigo_arm_cali.hpp"

int main(int n, char *argv[])
{
    arm_cali armc;


    // prepare input data 
    double rx[13] = { 0,      18,     18,     0,      3,      15,     15,     3,      9,     9,      15,     9,      3 };
    double ry[13] = { 0,      0,      18,     18,     3,      3,      15,     15,     9,     3,      9 ,     15,     9 };
    double angle1[13]  = { 2.2304, 2.0279, 2.2794, 3.6830, 2.7903, 2.3255, 2.6553, 3.4698, 2.9206,2.5893,  2.5479, 3.0633, 3.3041 };
    double angle2[13]  = { 5.9656, 4.3580, 3.4453, 5.0790, 5.7478, 4.7093, 4.2613, 5.1986, 5.1388,5.2768,  4.5651, 4.8243, 5.5591 };
    double angle3[13]  = { 2.8532, 2.8900, 2.8961, 2.8961, 2.8654, 2.8961, 3.1170, 3.0986, 3.0004,2.9881,  3.0863, 3.1109, 3.0188 };
    

    //    board_width = 432;
    //    board_height = 401;
    double xstep = 432 /18.0;
    double ystep = 401 / 18.0;

    for(int i=0;i<13;i++)
    {
        armc.insert_test_data( rx[i] * xstep  ,ry[i] * ystep , angle1[i], angle2[i], angle3[i] );
    }

    armc.scan();

    for( int i=0 ; i< 2 ;i++)
    {
        armc.relocate_center_on_hold_place();
        armc.scan();
    }

    armc.reset_to_hold();

    double a1,a2;
    armc.calc_a1a2_from_xy(200,200,a1,a2);

    return 0;
}
