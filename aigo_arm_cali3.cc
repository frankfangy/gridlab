#include "scanner.hpp"
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <x/extype/taged_xvalue.hpp>

class arm_cali: public scanner
{
public:
    struct test_data_package
    {
        double relative_x;  // 采样点相对 棋盘 0,0 的 x 坐标
        double relative_y;  // 采样点相对 棋盘 0,0 的 y 坐标
        double deva1;       // 舵机1 的机械采集值
        double deva2;       // 舵机2 的机械采集值
        double deva3;       // 舵机3 的机械采集值

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

        test_data_package( double rx , double ry , double da1, double da2 ,double da3 )
        {
            setup( rx , ry,da1,da2,da3);
        }

        void setup( double rx , double ry , double da1, double da2 ,double da3  )
        {
            relative_x =rx;
            relative_y =ry;
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
    double ellipse_phase1 ; // angle that get min r for l1
    double ellipse_short_ratio1; // min r = l1 * ratio 

    // 被加入用来训练参数的 测试数据
    std::vector<test_data_package> test_datas;  

    // 构造函数，初始化参数中心值 ， 初始化扫描范围 ，密度
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
        ellipse_phase1 = M_PI/2;
        ellipse_short_ratio1 = 0.95;

        add_scan_item(&l1,2.0,5, "arm1 len ");
        add_scan_item(&l2,2.0,5, "arm2 len ");   
        add_scan_item(&x0,2.0,5, "x0 " );
        add_scan_item(&y0,3.0,5, "y0 ");
        add_scan_item(&a1_offset,0.3,5, "eng1 offset ");
        add_scan_item(&a2_offset,0.3,5, "eng2 offset ");  
        
        add_scan_item(&ellipse_phase,M_PI/2,5 , "ellipse_phase 2 ");  
        add_scan_item(&ellipse_short_ratio,0.0499,5, "ellipse_short_ratio 2 ");  
        add_scan_item(&ellipse_phase1,M_PI/2,5,"ellipse_phase 1 ");  
        add_scan_item(&ellipse_short_ratio1,0.0499,5,"ellipse_short_ratio 1 ");  

    }

    /// 加入测试采样数据，供模型进行拟合 
    void insert_test_data( double relative_x , double relative_y , double da1, double da2 , double da3  )
    {
        test_datas.push_back(  test_data_package( relative_x , relative_y , da1 , da2, da3 ) );
    }
    
    /// 计算 当前参数下，拟合的误差值， 越小越好
    virtual double loss()
    {
        static int scan_count = 0;
        if( ++scan_count % 1000000 == 0 )
    	scanner::loss(); // log param

        // for each case of param 
        // loss is the std error of position
        double da1,da2,sum = 0;
        double x,y,ll2,ll1,da;
        int i;
        for(i=0;i< test_datas.size(); i++)
        {
            da1 = test_datas[i].deva1 - a1_offset;
            da2 = test_datas[i].deva2 - a2_offset; 
            
            da = fabs(da1 - ellipse_phase1);
            if(da > M_PI_2 )  da = M_PI - da;            
            ll1 = l1 * (  da * 1.0 + ( M_PI_2 - da ) * ellipse_short_ratio1   ) / M_PI_2 ;
            
            da = fabs(da2 - ellipse_phase);
            if(da > M_PI_2 )  da = M_PI - da;            
            ll2 = l2 * (  da * 1.0 + ( M_PI_2 - da ) * ellipse_short_ratio   ) / M_PI_2 ;

            da2 =  da1 - da2; // arm 2 global angle
            x = ll1 * cos(da1) + ll2 * cos(da2);
            y = ll1 * sin(da1) + ll2 * sin(da2);
            
            x -= x0 + test_datas[i].relative_x ;
            y -= y0 + test_datas[i].relative_y ;
            
            //std::cout << " -- " <<  x << "," << y << std::endl;
            sum += x*x + y*y;
        }
        sum = sqrt( sum / test_datas.size() );
        //std::cout << sum << std::endl;
        return sum;
    }
    
    /// 从 目标位置 x,y 单位毫米 ， 计算出 应该采用的 a1,a2 
    void calc_a1a2_from_xy_raw( double x , double y , double &dev_a1 , double &dev_a2    )
    {
        double r = sqrt( x*x + y*y );
        double a3 = acos( x / r );
        double logic_a2 = acos( (r - l1 )/l2 );
        double a4 = asin( l2 * sin( logic_a2 ) / r );
        double logic_a1 = a3 + a4;         
        dev_a1 = logic_a1 + a1_offset;
        dev_a2 = logic_a2 + a2_offset;
    }

    /// 已知 deva1,a2 , 计算处放置的位置
    void calc_xy_from_a1a2( double deva1 , double deva2, double &x , double &y   )
    {
        double da1,da2,da,ll1,ll2;
        da1 = deva1 - a1_offset;
        da2 = deva2 - a2_offset; 
        
        da = fabs(da1 - ellipse_phase1);
        if(da > M_PI_2 )  da = M_PI - da;            
        ll1 = l1 * (  da * 1.0 + ( M_PI_2 - da ) * ellipse_short_ratio1   ) / M_PI_2 ;
        
        da = fabs(da2 - ellipse_phase);
        if(da > M_PI_2 )  da = M_PI - da;            
        ll2 = l2 * (  da * 1.0 + ( M_PI_2 - da ) * ellipse_short_ratio   ) / M_PI_2 ;

        da2 =  da1 - da2; // arm 2 global angle
        x = ll1 * cos(da1) + ll2 * cos(da2);
        y = ll1 * sin(da1) + ll2 * sin(da2);
    }

    void calc_a1a2_from_xy( double x , double y , double &dev_a1 , double &dev_a2    )
    {
        double da1,da2 , xx,yy,dx,dy;
        std::cout << "input: x,y " << x << "," << y << std::endl;
        calc_a1a2_from_xy_raw( x,y,da1,da2 );
        std::cout <<"x,y -> a1 a2 :" << da1 << "," << da2 << std::endl;

        calc_xy_from_a1a2(da1,da2, xx, yy);
        dx = xx - x;
        dy = yy - y;
        std::cout << "1st calc : dxdy: " << dx << "," << dy << std::endl ;
        calc_a1a2_from_xy_raw( x-dx,y-dy,da1,da2 );

        calc_xy_from_a1a2(da1,da2, xx, yy);
        dx = xx - x;
        dy = yy - y;
        std::cout << "2nd calc : dxdy: " << dx << "," << dy << std::endl ;
    }

};

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
