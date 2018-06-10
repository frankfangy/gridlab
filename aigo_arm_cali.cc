#include <iostream>
#include <math.h>
#include <float.h>


class scanner 
{
public:
    struct scan_item
    {
        double *data; 
        double catched;
        double mindata ;
        double step;
        int top ; 
        int pos ;
        scan_item(){}
        scan_item( double *d , double delta , int scan_number )
        {
            setup( d ,  delta ,  scan_number );
        }

        void setup( double *d , double delta , int scan_number )
        {
            data = d; 
            catched = *d;
            mindata = *d - delta ; 
            step = delta * 2.0 /  (scan_number-1);
            top = scan_number;
            pos = 0;
        }

        void reset() {pos = 0;}

        void hold() { catched = *data; }

        double value() const  { return *data; }

        bool next()
        {
            if( pos < top )
            {
                *data = mindata + pos * step;
                pos++;
                return true;
            }
            else 
            {                    
                pos = 0;
                *data = mindata + pos * step;
                pos++;
                return false; // no scan data anymore
            }
        }
    };

    scan_item scan_items[20];
    int count;
    double min_of_loss;

    scanner(): count(0)
    {}

    void log_out_result()
    {
        int i=0;
        std::cout << "best loss "<< min_of_loss << " on param: ";
        for(;i<count-1;i++)
        {
            std::cout << scan_items[i].catched << ",";
        }
        std::cout << scan_items[i].catched << std::endl;
    }

    void add_scan_item( double *d , double delta , int scan_number  )
    {
        scan_items[count].setup( d, delta , scan_number );
	    count++;
    }

                
    void hold()
    {
        for(int i=0; i<count; i++)            scan_items[i].hold();
    }
        
    virtual double loss()
    {
    	std::cout << "running loss on : " ;

        for( int i=0; i< count; i++)
            std::cout <<  scan_items[i].value() << ",";
	    std::cout << std::endl;
        return 1; // add you loss funciton here 
    }
    
    int running_lvl; // 0 -> count
    
    void reset()
    {
        for(int i=0; i<count; i++)            scan_items[i].reset();
        running_lvl = 0;
        for(int i=0; i< count ; i++)
        {
            scan_items[i].next() ;
        }
    }

    bool next()
    {
        int i = 0;
        while( !scan_items[i].next() ) 
        {
            i++;
            if( i >= count  ) return false; // scan over            
        }
        return true;
    }

    void scan()
    {
        reset();
        double l;
        min_of_loss = DBL_MAX ;
        while(true)
        {
            l = loss();
            if( l < min_of_loss )
            {
                min_of_loss = l;
                // mark this point
                hold();
            }
            if( !next() ) break;
        }
        log_out_result();
    }
    
};

class arm_cali: public scanner
{
public:

    double  l1,l2,
            x0,y0,
            board_width , board_height,
            a1_offset,a2_offset;
public:
    struct test_data_package
    {
        int gridx;
        int gridy;
        double deva1;
        double deva2;
        test_data_package(){}
        test_data_package( int gx, int gy , double da1, double da2  )
        {
            setup(gx,gy,da1,da2);
        }

        void setup( int gx, int gy , double da1, double da2  )
        {
            gridx =gx;
            gridy = gy;
            deva1 =da1;
            deva2 =da2;
        }        
    };

    test_data_package test_datas[20]; //max 20
    int test_data_count; 

    arm_cali():scanner()
    {
        l1 = 355;
        l2 = 308;
        x0 = 111;
        y0 = -36.6;
        board_width = 432;
        board_height = 401;
        a1_offset = M_PI/2;
        a2_offset = M_PI;

        add_scan_item(&l1,1.0,5);
        add_scan_item(&l2,2.0,5);   
        add_scan_item(&x0,1.0,5);
        add_scan_item(&y0,1.0,5);
        add_scan_item(&a1_offset,0.3,10);
        add_scan_item(&a2_offset,0.3,10);  

        // prepare input data 
                
        double angle_one[9] = { 2.281, 2.0892, 3.7112, 2.3316, 2.9138, 2.6430, 2.9053, 2.3331, 3.4852 };
        double angle_two[9] = { 5.9579, 4.3549, 5.0544, 3.476, 5.1004, 4.2399, 5.1066, 4.6832, 5.1578 };
        double logic_x[9] = { 0, 18, 0, 18, 9, 3, 15, 15, 3 };
        double logic_y[9] = { 0, 0, 18, 18, 9, 3, 15, 3, 15 };
        test_data_count = 9;
        for(int i=0;i<9;i++)
        {
            test_datas[i].setup( logic_x[i] ,logic_y[i] , angle_one[i], angle_two[i] );
        }
    }
    
    virtual double loss()
    {
        static int scan_count = 0;
        if( ++scan_count % 100 == 0 )
    	scanner::loss(); // log param

        // for each case of param 
        // loss is the std error of position
        double da1,da2,sum = 0;
        double x,y;
        int i;
        for(i=0;i< test_data_count; i++)
        {
            da1 = test_datas[i].deva1 - a1_offset;
            da2 = test_datas[i].deva2 - a2_offset; 
            
            std::cout << i << "> " << da1 << "," <<   da2 ;

            da2 =  da1 - da2; // arm 2 global angle
            x = l1 * cos(da1) + l2 * cos(da2);
            y = l1 * sin(da1) + l2 * sin(da2);
            
            std::cout <<  " - " << x << "," << y ;

            x -= x0 + test_datas[i].gridx * ( board_width / 18.0 );
            y -= y0 + test_datas[i].gridy * ( board_height / 18.0);
            
            std::cout << " -- " <<  x << "," << y << std::endl;
            sum += x*x + y*y;
        }
        sum = sqrt( sum / test_data_count );
        return sum;
    }
    


};

int main(int n, char *argv[])
{
    arm_cali armc;
    armc.scan();
    return 0;
}
