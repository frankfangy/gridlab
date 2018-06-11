#ifndef __scanner__h__
#define __scanner__h__
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


#endif 