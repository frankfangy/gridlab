#ifndef __scanner__h__
#define __scanner__h__
#include <iostream>
#include <math.h>
#include <float.h>
#include <string.h>

class scanner 
{
public:
    struct scan_item
    {
        double *data; 
        double catched;
        double mindata ;
        double step;
        double min_step;
        int top ; 
        int pos ;
        char name[128];
        scan_item(){}
        scan_item( double *d , double delta , int scan_number , const std::string &sname = "" )
        {
            setup( d ,  delta ,  scan_number , sname );
        }

        void set_min_step(double mstep)
        {
            min_step = mstep;
        }

        void setup( double *d , double delta , int scan_number , const std::string &sname = "" )
        {
            data = d; 
            catched = *d;
            mindata = *d - delta ; 
            step = delta * 2.0 /  (scan_number-1);
            top = scan_number;
            pos = 0;
            min_step = 0.0001;
            strcpy( name, sname.c_str() );
        }

        void relocate_center_on_hold_place( double window_shrink_ratio = 0.25 )
        {
            double w = step * top * window_shrink_ratio;
            mindata = catched - w;
            step = w * 2.0 / (top-1);
            pos = 0;
        }

        void reset() {pos = 0;}

        void hold() { catched = *data; }

        void reset_to_hold() {  *data = catched; }        

        double value() const  { return *data; }

        bool next()
        {
            if( step < min_step )
            {
                *data = mindata + step * top/2;
                pos = 1;
                return false;
            }

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

    void reset_to_hold()
    {
        for(int i=0; i<count; i++)            scan_items[i].reset_to_hold();
    }

    void relocate_center_on_hold_place()
    {
        for(int i=0; i<count; i++)      scan_items[i].relocate_center_on_hold_place();
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