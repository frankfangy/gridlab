#include "scanner.hpp"
#include <vector>
#include <stdlib.h>
#include <string.h>

class arm_cali: public scanner
{
public:
    struct test_data_package
    {
        int gridx;
        int gridy;
        double deva1;
        double deva2;
        double deva3; 

        test_data_package(){}

        test_data_package(const test_data_package &o)
        {
            memcpy(this, &o , sizeof(o));
        }

        test_data_package &operator=( const test_data_package &o)
        {
            memcpy(this, &o , sizeof(o));
            return *this;
        }

        test_data_package( int gx, int gy , double da1, double da2 ,double da3 )
        {
            setup(gx,gy,da1,da2,da3);
        }

        void setup( int gx, int gy , double da1, double da2 ,double da3  )
        {
            gridx =gx;
            gridy =gy;
            deva1 =da1;
            deva2 =da2;
            deva3 =da3;
        }        
    };


    
public:

    double  l1,l2,
            x0,y0,
            board_width , board_height,
            a1_offset,a2_offset;

    double ellipse_phase ; // angle that get min r for l2
    double ellipse_short_ratio; // min r = l2 * ratio 

    std::vector<test_data_package> test_datas; //max 20

    arm_cali():scanner()
    {
        l1 = 355;
        l2 = 308;
        x0 = 109;
        y0 = -36.6;
        board_width = 432;
        board_height = 401;
        a1_offset = M_PI/2;
        a2_offset = M_PI;

        ellipse_phase = M_PI/2;
        ellipse_short_ratio = 0.95;

        

        add_scan_item(&l1,2.0,5);
        add_scan_item(&l2,2.0,5);   
        add_scan_item(&x0,2.0,5);
        add_scan_item(&y0,3.0,5);
        add_scan_item(&a1_offset,0.3,10);
        add_scan_item(&a2_offset,0.3,10);  
        
        add_scan_item(&ellipse_phase,M_PI/2,10);  
        add_scan_item(&ellipse_short_ratio,0.0499,10);  

        // prepare input data 
        double logic_x[13] = { 0,      18,     18,     0,      3,      15,     15,     3,      9,     9,      15,     9,      3 };
        double logic_y[13] = { 0,      0,      18,     18,     3,      3,      15,     15,     9,     3,      9 ,     15,     9 };
        double angle1[13]  = { 2.2304, 2.0279, 2.2794, 3.6830, 2.7903, 2.3255, 2.6553, 3.4698, 2.9206,2.5893,  2.5479, 3.0633, 3.3041 };
        double angle2[13]  = { 5.9656, 4.3580, 3.4453, 5.0790, 5.7478, 4.7093, 4.2613, 5.1986, 5.1388,5.2768,  4.5651, 4.8243, 5.5591 };
        double angle3[13]  = { 2.8532, 2.8900, 2.8961, 2.8961, 2.8654, 2.8961, 3.1170, 3.0986, 3.0004,2.9881,  3.0863, 3.1109, 3.0188 };
        
        for(int i=0;i<13;i++)
        {
            test_datas.push_back( test_data_package( logic_x[i] ,logic_y[i] , angle1[i], angle2[i], angle3[i] ) );
        }
    }
    
    virtual double loss()
    {
        static int scan_count = 0;
        if( ++scan_count % 1000000 == 0 )
    	scanner::loss(); // log param

        // for each case of param 
        // loss is the std error of position
        double da1,da2,sum = 0;
        double x,y,ll2,da;
        int i;
        for(i=0;i< test_datas.size(); i++)
        {
            da1 = test_datas[i].deva1 - a1_offset;
            da2 = test_datas[i].deva2 - a2_offset; 
            
            da = fabs(da2 - ellipse_phase);
            if(da > M_PI_2 )  da = M_PI - da;            
            ll2 = l2 * (  da * 1.0 + ( M_PI_2 - da ) * ellipse_short_ratio   ) / M_PI_2 ;

            da2 =  da1 - da2; // arm 2 global angle
            x = l1 * cos(da1) + ll2 * cos(da2);
            y = l1 * sin(da1) + ll2 * sin(da2);
            
            //std::cout <<  " - " << x << "," << y ;

            x -= x0 + test_datas[i].gridx * ( board_width / 18.0 );
            y -= y0 + test_datas[i].gridy * ( board_height / 18.0);
            
            //std::cout << " -- " <<  x << "," << y << std::endl;
            sum += x*x + y*y;
        }
        sum = sqrt( sum / test_datas.size() );
        //std::cout << sum << std::endl;
        return sum;
    }
    


};

int main(int n, char *argv[])
{
    arm_cali armc;
    armc.scan();

    for( int i=0 ; i< 10 ;i++)
    {
        armc.relocate_center_on_hold_place();
        armc.scan();
    }

    return 0;
}
