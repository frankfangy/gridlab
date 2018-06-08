#include <iostream>
#include <math.h>

double angle_one[9] = { 2.281, 2.0892, 3.7112, 2.3316, 2.9138, 2.6430, 2.9053, 2.3331, 3.4852 };
double angle_two[9] = { 5.9579, 4.3549, 5.0544, 3.476, 5.1004, 4.2399, 5.1066, 4.6832, 5.1578 };

double logic_x[9] = { 0, 18, 0, 18, 9, 3, 15, 15, 3 };
double logic_y[9] = { 0, 0, 18, 18, 9, 3, 15, 3, 15 };

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
                return false; // no scan data anymore
            }
        }
    };

    scan_item scan_items[20];
    int count;

    scanner(): count(0)
    {}

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
    	std::cout << "running loss on : " << count;

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
        double l, lossv = 1e+15;
        while(true)
        {
            l = loss();
            if( l < lossv )
            {
                lossv = l;
                // mark this point
                hold();
            }
            if( !next() ) break;
        }
        std::cout << "scan over" << std::endl ;
    }
    
};

class arm_cali: public scanner
{
public:

    double  l1,l2,
            x0,y0,
            board_width , board_height,
            a1,a2;
public:
    arm_cali():scanner()
    {
        l1 = 355;
        l2 = 308;
        x0 = 111;
        y0 = -36.6;
        board_width = 432;
        board_height = 401;
        a1 = a2 = M_PI/2;

        add_scan_item(&l1,1.0,2);
        add_scan_item(&l2,2.0,3);   
        /*     
        add_scan_item(&x0,1.0,5);
        add_scan_item(&y0,1.0,5);
        add_scan_item(&a1,3.0,10);
        add_scan_item(&a2,3.0,10);  
        */       
    }
    


};

int main(int n, char *argv[])
{
    arm_cali armc;
    armc.scan();
    return 0;
}
