#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std ;

#define Modify 1
#define Share 2 
#define Invalid 3 

typedef struct Processor {
    int state[1000] ; 
    int memrange[1000] ;
    int writingnow[1000]; // now writing addr
    int whichblock ;
} Procname ; 

int main (int argc , char * argv[]){
    string Proc, OP , Addr ;
    int cachsize = atoi(argv[3]) ;
    int blocksize = atoi(argv[4]) ;
    int invalidaddr = -1 -blocksize ; 
    int writeinvalid = invalidaddr -  blocksize * 2 ;
 /*   struct Processor P0 = {3,invalidaddr,0,0};
    struct Processor P1 = {3,invalidaddr,0,0};
    struct Processor P2 = {3,invalidaddr,0,0}; */
    Procname P0,P1 , P2  ;
    for (int i = 0 ; i < 1000 ; i ++ ){
        P0.state[i] = 3 ;
        P1.state[i] = 3 ;
        P2.state[i] = 3 ;
        P0.writingnow[i] = writeinvalid ;
        P1.writingnow[i] = writeinvalid ;
        P2.writingnow[i] = writeinvalid ;
    }
    
    P0.whichblock = 0 ;
    P1.whichblock = 0 ;
    P2.whichblock = 0 ;

    
    ifstream inputfile ; 
    inputfile.open(argv[1]);
    inputfile >> Proc ;
    
    ofstream output ;
    output.open(argv[2]) ;

    while(Proc == "P0" || Proc == "P1" || Proc == "P2"){
        inputfile >> OP ;
        inputfile >> Addr ;
        output << Proc << " "<< OP << " "<< Addr <<endl ;
        
        int intaddr = (int)strtol(Addr.c_str(),NULL,16) ;
        int baseaddr = blocksize*(intaddr/blocksize);
        int check = 0 ;
        int tmpstate = 0 ;
        int memstate = 0 ;
       // cout << "real = "<< intaddr << endl ;
        if(Proc == "P0") {
        	check = 0 ;
        	memstate = 0 ;
            for (int s = 0 ; s < P0.whichblock ; s ++ ){
                if(intaddr >= P0.memrange[s] && intaddr < P0.memrange[s]+blocksize){
                    memstate = s ;
                    check = 1 ;
                    break ;
                }
            }
            if(check == 0 ){
            	memstate = P0.whichblock ;
            	P0.state[memstate] = 3 ;
            }
                if(OP == "R"){
               //     P0.writingnow[memstate] = writeinvalid ;
                    if(P0.state[memstate] == 2 ){ // P0 is in shared
                        check = 0 ;
                        for (int w = 0 ; w < P0.whichblock ; w ++ ){
                            if(intaddr >= P0.memrange[w] && intaddr < P0.memrange[w]+blocksize){
                                output << "Read Hit"<< endl ;
                               // output<< "Read Hit" << endl ;
                                P0.state[memstate] = 2 ;
                                check = 1 ;
                                break ;
                            }
                        }
                        if(check == 0 ){
                            output << "Read Miss on bus" << endl ;
                            P0.memrange[P0.whichblock] = baseaddr ;
                            P0.state[P0.whichblock] = 2 ;
                            P0.whichblock++ ;
                            
                            
                        }
                        
                    }
                    else if(P0.state[memstate] == 3){ // P0 is in invalid
                    	output << "Read Miss on bus" << endl ;
                        check = 0 ;
                        for (int w = 0 ; w < P0.whichblock ; w ++ ){
                            if(intaddr >= P0.memrange[w] && intaddr < P0.memrange[w]+blocksize){
                               //cout << "Read Hit"<< endl ;
                                P0.state[memstate] = 2 ;
                                check = 1 ;
                                break ;
                            }
                        }
                        if(check == 0 ){
                           // cout << "Read Miss on bus" << endl ;
                            P0.memrange[P0.whichblock] = baseaddr ;
                            P0.state[P0.whichblock] = 2 ;
                            
                            P0.whichblock++ ;
                            
                        }

                        check = 0 ;
                        tmpstate = 0 ;
                        for (int wr = 0 ; wr < P1.whichblock ; wr ++ ){
                           // cout << "check "<< wr << "  "<< P1.writingnow[wr]<< endl ;
                            if(P1.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P1.state[wr] = 2 ;
                                P1.writingnow[wr] = writeinvalid ;
                            }
     
	                    }
	                    if(check == 1 ){
	                            output<< "Writing Back: P1"<< endl ;
	                        }
                        check = 0 ;
                        tmpstate = 0 ;
                        for (int wr = 0 ; wr < P2.whichblock ; wr ++ ){
	                         if(P2.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P2.state[wr] = 2 ;
                                P2.writingnow[wr] = writeinvalid ;
                            }
      
	                    }
	                    if(check == 1 ){
	                            output<< "Writing Back: P2"<< endl ;
	                            
	                           
	                        }

                    }
                    else if(P0.state[memstate] == 1 ){// P0 is in Modify
                        check = 0 ;
                        for (int w = 0 ; w < P0.whichblock ; w ++ ){
                            if(intaddr >= P0.memrange[w] && intaddr < P0.memrange[w]+blocksize){
                                output << "Read Hit"<< endl ;
                                P0.state[memstate]= 1 ;
                                check = 1 ;
                                break ;
                            }
                        }
                        if(check == 0 ){
                            output << "Read Miss on bus" << endl ;
                            P0.memrange[P0.whichblock] = baseaddr ;
                            P0.state[P0.whichblock] = 1 ;
                            P0.whichblock++ ;
                              
                        }
                    }
                }
                else if(OP == "W"){
                    P0.writingnow[memstate] = baseaddr ;
                  //  P1.writingnow = 0 ;
                  //  P2.writingnow = 0 ;
                    if(P0.state[memstate] == 2) {// P0 is in shared
                        check = 0 ;
                        for (int w = 0 ; w < P0.whichblock ; w ++ ){
                            if(intaddr >= P0.memrange[w] && intaddr < P0.memrange[w]+blocksize){
                                output << "Write Hit"<< endl ;
                                P0.state[memstate] = 1 ;
                                check = 1 ;
                                break ;
                            }
                        }
                        if(check == 0 ){
                            output << "Write Miss on bus" << endl ;
                            P0.memrange[P0.whichblock] = baseaddr ;
                            P0.state[P0.whichblock] = 1 ;  
                            P0.whichblock++ ;
                            
                        }
                        output << "Broadcast invalid messages"<< endl ;
                        
                        check = 0 ;
                        tmpstate = 0 ;
                        for (int w = 0 ; w < P1.whichblock ; w ++ ){
                            if(intaddr >=P1.memrange[w] && intaddr < P1.memrange[w]+blocksize && P1.state[w] != 3 ){
                              //  P1.memrange[w] = invalidaddr ;
                                check = 1 ;
                                P1.state[w] = 3 ;
                                
                            }
                        }
                        if(check == 1 ){
                            output << "Invalidating: P1"<< endl ;
                            
                        }
                        check = 0 ;
                        for (int w = 0 ; w < P2.whichblock ; w ++ ){
                            if(intaddr >=P2.memrange[w] && intaddr < P2.memrange[w]+blocksize && P2.state[w] != 3){
                               // P2.memrange[w] = invalidaddr ;
                                check = 1 ;
                                P2.state[w] = 3 ;
                            }
                        }
                        if(check == 1 ){
                            output << "Invalidating: P2"<< endl ;
                            
                        }

                    }
                    else if(P0.state[memstate] == 3 ){ // P0 is in invalid 
                        output << "Write Miss on bus"<< endl ;
                        output << "Broadcast invalid messages"<< endl ;
                        P0.memrange[P0.whichblock] = baseaddr ;
                        P0.whichblock++ ;
                        P0.state[memstate] = 1 ;
                        check = 0 ;
                        for (int wr = 0 ; wr < P1.whichblock ; wr ++ ){
	                        if(P1.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P1.state[wr] = 2 ;
                                P1.writingnow[wr] = writeinvalid ;
                            }

                            /*
	                        for (int w = 0 ; w < P0.whichblock ; w ++ ){
	                            if(P1.writingnow[wr] >= P0.memrange[w] && P1.writingnow[wr] < P0.memrange[w] + blocksize && P0.state[w]!= 3){
	                                check = 1 ;
	                                P1.state[w] = 2 ;
	                                P1.writingnow[wr] = writeinvalid ;
	                            }
	                        }
	                        */
	                    }
	                    if(check == 1 ){
	                            output<< "Writing Back: P1"<< endl ;
	                            
	                            
	                    }
                        check = 0 ;
                        for (int wr = 0 ; wr < P2.whichblock ; wr ++ ){
	                        if(P2.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P2.state[wr] = 2 ;
                                P2.writingnow[wr] = writeinvalid ;
                            }

                            /* for (int w = 0 ; w < P0.whichblock ; w ++ ){
	                            if(P2.writingnow[wr] >= P0.memrange[w] && P2.writingnow[wr] < P0.memrange[w] + blocksize && P0.state[w]!= 3){
	                                check = 1 ;
	                                P2.state[w] = 2 ;
	                                P2.writingnow[wr] = writeinvalid ;
	                            }
	                        }
                            */
	                    }
                        if(check == 1 ){
                            output<< "Writing Back: P2"<< endl ;
                            
                            
                        }
                        check = 0 ;
                        for (int w = 0 ; w < P1.whichblock ; w ++ ){
                            if(intaddr >=P1.memrange[w] && intaddr < P1.memrange[w]+blocksize && P1.state[w] != 3){
                              //  P1.memrange[w] = invalidaddr ;
                                P1.state[w] = 3 ;
                                check = 1 ;
                            }
                        }
                        if(check == 1 ){
                            output << "Invalidating: P1"<< endl ;
                            
                        }
                        check = 0 ;
                        for (int w = 0 ; w < P2.whichblock ; w ++ ){
                            if(intaddr >=P2.memrange[w] && intaddr < P2.memrange[w]+blocksize &&P2.state[w] != 3){
                              //  P2.memrange[w] = invalidaddr ;
                                P2.state[w] = 3 ;
                                check = 1 ;
                            }
                        }
                        if(check == 1 ){
                            output << "Invalidating: P2"<< endl ;
                            
                        }

                    }
                    else if(P0.state[memstate] == 1 ){ // P0 is in modify
                        check = 0 ;
                        for (int w = 0 ; w < P0.whichblock ; w ++ ){
                            if(intaddr >= P0.memrange[w] && intaddr < P0.memrange[w]+blocksize){
                                output << "Write Hit"<< endl ;
                                P0.state[memstate] = 1 ;
                                check = 1 ;
                                break ;
                            }
                        }
                        if(check == 0 ){
                            output << "Write Miss on bus" << endl ;
                            P0.memrange[P0.whichblock] = baseaddr ;
                            P0.state[P0.whichblock] = 1 ;  
                            P0.whichblock++ ;
                            
                        }
                    }
                }

        }
        //   if is p0 done 
        // if is p1 start 
        else if(Proc == "P1") {

        	check = 0 ;
        	memstate = 0 ;
            for (int s = 0 ; s < P1.whichblock ; s ++ ){
                if(intaddr >= P1.memrange[s] && intaddr < P1.memrange[s]+blocksize){
                    memstate = s ;
                    check = 1 ;
                    break ;
                }
            }


            if(check == 0 ){
            	memstate = P1.whichblock ;
            	P1.state[memstate] = 3 ;
            	
            }
            
            if(OP == "R"){
               // P1.writingnow[memstate] = writeinvalid ;
                if(P1.state[memstate] == 2 ){ // P0 is in shared
                    check = 0 ;
                    for (int w = 0 ; w < P1.whichblock ; w ++ ){
                        if(intaddr >= P1.memrange[w] && intaddr < P1.memrange[w]+blocksize){
                            output << "Read Hit"<< endl ;
                            P1.state[memstate] = 2 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Read Miss on bus" << endl ;
                        P1.memrange[P1.whichblock] = baseaddr ;
                        P1.state[P1.whichblock] = 2 ;
                        P1.whichblock++ ;
                        
                        
                    }

                }
                else if(P1.state[memstate] == 3){ // P0 is in invalid
                	output << "Read Miss on bus" << endl ;
                    check = 0 ;
                    for (int w = 0 ; w < P1.whichblock ; w ++ ){
                        if(intaddr >= P1.memrange[w] && intaddr < P1.memrange[w]+blocksize){
                          //  cout << "Read Hit"<< endl ;
                            P1.state[memstate] = 2 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                       // cout << "Read Miss on bus" << endl ;
                        P1.memrange[P1.whichblock] = baseaddr ;
                        P1.state[P1.whichblock] = 2 ;
                        P1.whichblock++ ;
                        
                        
                    }
                    check = 0 ;
                    for (int wr = 0 ; wr < P0.whichblock ; wr ++ ){
	                    if(P0.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P0.state[wr] = 2 ;
                                P0.writingnow[wr] = writeinvalid ;
                            }

                        /* for (int w = 0 ; w < P1.whichblock ; w ++ ){
	                        if(P0.writingnow[wr] >= P1.memrange[w] && P0.writingnow[wr] < P1.memrange[w] + blocksize && P1.state[w]!= 3){
	                            check = 1 ;
	                            P0.state[w] = 2 ;
	                            P0.writingnow[wr] = writeinvalid ;
	                        }
	                    }*/
	                }
                    if(check == 1 ){
                        output<< "Writing Back: P0"<< endl ;
                        
                        
                    }
                    check = 0 ;
                    for (int wr = 0 ; wr < P2.whichblock ; wr ++ ){
	                  if(P2.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P2.state[wr] = 2 ;
                                P2.writingnow[wr] = writeinvalid ;
                            }

                        /*  for (int w = 0 ; w < P1.whichblock ; w ++ ){
	                        if(P2.writingnow[wr] >= P1.memrange[w] && P2.writingnow[wr] < P1.memrange[w] + blocksize && P1.state[w]!= 3){
	                            check = 1 ;
	                            P2.state[w] = 2 ;
	                            P2.writingnow[wr] = writeinvalid ;
	                        }
	                    }*/
	                }
                    if(check == 1 ){
                        output<< "Writing Back: P2"<< endl ;
                        
                        
                    }
                }
                else if(P1.state[memstate] == 1 ){// P0 is in Modify
                    check = 0 ;
                    for (int w = 0 ; w < P1.whichblock ; w ++ ){
                        if(intaddr >= P1.memrange[w] && intaddr < P1.memrange[w]+blocksize){
                            output << "Read Hit"<< endl ;
                            P1.state[memstate] = 1 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Read Miss on bus" << endl ;
                        P1.memrange[P1.whichblock] = baseaddr ;
                        P1.state[P1.whichblock] = 1 ;  
                        P1.whichblock++ ;
                        
                    }

                }
            }
            else if(OP == "W"){
                P1.writingnow[memstate] = baseaddr ;
              //  P1.writingnow = 0 ;
              //  P2.writingnow = 0 ;
                if(P1.state[memstate] == 2) {// P0 is in shared
                    check = 0 ;
                    for (int w = 0 ; w < P1.whichblock ; w ++ ){
                        if(intaddr >= P1.memrange[w] && intaddr < P1.memrange[w]+blocksize){
                            output << "Write Hit"<< endl ;
                            P1.state[memstate] = 1 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Write Miss on bus" << endl ;
                        P1.memrange[P1.whichblock] = baseaddr ;
                        P1.state[P1.whichblock] = 1 ;
                        P1.whichblock++ ;
                          
                    }
                    output << "Broadcast invalid messages"<< endl ;
                    
                    check = 0 ;
                    for (int w = 0 ; w < P0.whichblock ; w ++ ){
                        if(intaddr >=P0.memrange[w] && intaddr < P0.memrange[w]+blocksize && P0.state[w] != 3){
                          //  P0.memrange[w] = invalidaddr ;
                            check = 1 ;
                            P0.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P0"<< endl ;
                        
                    }
                    check = 0 ;
                    for (int w = 0 ; w < P2.whichblock ; w ++ ){
                        if(intaddr >=P2.memrange[w] && intaddr < P2.memrange[w]+blocksize &&P2.state[w] != 3){
                           // P2.memrange[w] = invalidaddr ;
                            check = 1 ;
                            P2.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P2"<< endl ;
                        
                    }
                }
                else if(P1.state[memstate] == 3 ){ // P0 is in invalid 
                    output << "Write Miss on bus"<< endl ;
                    output << "Broadcast invalid messages"<< endl ;
                    P1.memrange[P1.whichblock] = baseaddr ;
                    P1.whichblock++ ;
                    P1.state[memstate] = 1 ;

                    check = 0 ;
                    for (int wr = 0 ; wr < P0.whichblock ; wr ++ ){
	                    if(P0.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P0.state[wr] = 2 ;
                                P0.writingnow[wr] = writeinvalid ;
                            }

                        /* for (int w = 0 ; w < P1.whichblock ; w ++ ){
	                        if(P0.writingnow[wr] >= P1.memrange[w] && P0.writingnow[wr] < P1.memrange[w] + blocksize && P1.state[w]!= 3){
	                            check = 1 ;
	                            P0.state[w] = 2 ;
	                            P0.writingnow[wr] = writeinvalid ;
	                        }
	                    }*/
	                }
                    if(check == 1 ){
                        output<< "Writing Back: P0"<< endl ;
                        
                        
                    }
                    check = 0 ;
                    for (int wr = 0 ; wr < P2.whichblock ; wr ++ ){
	                   if(P2.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P2.state[wr] = 2 ;
                                P2.writingnow[wr] = writeinvalid ;
                            }

                        /* for (int w = 0 ; w < P1.whichblock ; w ++ ){
	                        if(P2.writingnow[wr] >= P1.memrange[w] && P2.writingnow[wr] < P1.memrange[w] + blocksize && P1.state[w]!= 3){
	                            check = 1 ;
	                            P2.writingnow[wr] = writeinvalid ;
	                            P2.state[w] = 2 ;
	                        }
	                    }*/
	                }
                    if(check == 1 ){
                        output<< "Writing Back: P2"<< endl ;
                        
                        
                    }
                    check = 0 ;
                    for (int w = 0 ; w < P0.whichblock ; w ++ ){
                        if(intaddr >=P0.memrange[w] && intaddr < P0.memrange[w]+blocksize &&P0.state[w] != 3){
                         //   P0.memrange[w] = invalidaddr ;
                            check = 1 ;
                            P0.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P0"<< endl ;
                       
                    }
                    check = 0 ;
                    for (int w = 0 ; w < P2.whichblock ; w ++ ){
                        if(intaddr >=P2.memrange[w] && intaddr < P2.memrange[w]+blocksize &&P2.state[w] != 3){
                           // P2.memrange[w] = invalidaddr ;
                            check = 1 ;
                            P2.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P2"<< endl ;
                        
                    }
                }
                else if(P1.state[memstate] == 1 ){ // P0 is in modify
                    check = 0 ;
                    for (int w = 0 ; w < P1.whichblock ; w ++ ){
                        if(intaddr >= P1.memrange[w] && intaddr < P1.memrange[w]+blocksize){
                            output << "Write Hit"<< endl ;
                            P1.state[memstate] = 1 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Write Miss on bus" << endl ;
                        P1.memrange[P1.whichblock] = baseaddr ;
                        P1.state [P1.whichblock]= 1 ;  
                        P1.whichblock++ ;
                        
                    }
                }
            }
        }
        //if proc is p1 done 
        // else if proc is p2 
        else if(Proc == "P2") {

        	check = 0 ;
        	memstate = 0 ;
            for (int s = 0 ; s < P2.whichblock ; s ++ ){
                if(intaddr >= P2.memrange[s] && intaddr < P2.memrange[s]+blocksize){
                    memstate = s ;
                    check = 1 ;
                    break ;
                }
            }

            if(check == 0 ){
            	memstate = P2.whichblock ;
            	P2.state[memstate] = 3 ;
            }




            if(OP == "R"){
                P2.writingnow[memstate] = writeinvalid ;
                if(P2.state[memstate] == 2 ){ // P0 is in shared
                    check = 0 ;
                    for (int w = 0 ; w < P2.whichblock ; w ++ ){
                        if(intaddr >= P2.memrange[w] && intaddr < P2.memrange[w]+blocksize){
                            output << "Read Hit"<< endl ;
                            P2.state[memstate]= 2 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Read Miss on bus" << endl ;
                        P2.memrange[P2.whichblock] = baseaddr ;
                        P2.state[P2.whichblock] = 2 ;
                        P2.whichblock++ ;
                        
                        
                    }

                }
                else if(P2.state[memstate] == 3){ // P0 is in invalid
                	output << "Read Miss on bus" << endl ;
                    check = 0 ;
                    for (int w = 0 ; w < P2.whichblock ; w ++ ){
                        if(intaddr >= P2.memrange[w] && intaddr < P2.memrange[w]+blocksize){
                          //  cout << "Read Hit"<< endl ;
                            P2.state[memstate] = 2 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                      //  cout << "Read Miss on bus" << endl ;
                        P2.memrange[P2.whichblock] = baseaddr ;
                        P2.state[P2.whichblock] = 2 ;
                        P2.whichblock++ ;
                        
                        
                    }
                    check = 0 ;
                    for (int wr = 0 ; wr < P0.whichblock ; wr ++ ){
	                  if(P0.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P0.state[wr] = 2 ;
                                P0.writingnow[wr] = writeinvalid ;
                            }

                        /*  for (int w = 0 ; w < P2.whichblock ; w ++ ){
	                        if(P0.writingnow[wr] >= P2.memrange[w] && P0.writingnow[wr] < P2.memrange[w] + blocksize && P2.state[w]!= 3){
	                            check = 1 ;
	                            P0.state[w] = 2 ;
	                            P0.writingnow[wr] = writeinvalid ;
	                        }
	                    } */
	                }
                    if(check == 1 ){
                        output<< "Writing Back: P0"<< endl ;
                        
                        
                    }
                    check = 0 ;
                    for (int wr = 0 ; wr < P1.whichblock ; wr ++ ){
	                   if(P1.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P1.state[wr] = 2 ;
                                P1.writingnow[wr] = writeinvalid ;
                            }

                        /* for (int w = 0 ; w < P2.whichblock ; w ++ ){
	                        if(P1.writingnow[wr] >= P2.memrange[w] && P1.writingnow[wr] < P2.memrange[w] + blocksize && P2.state[w]!= 3){
	                            check = 1 ;
	                            P1.state[w] = 2 ;
	                            P1.writingnow[wr] = writeinvalid ;
	                        }
	                    }*/
	                }
                    if(check == 1 ){
                        output<< "Writing Back: P1"<< endl ;
                        
                        
                    }
                }
                else if(P2.state[memstate] == 1 ){// P0 is in Modify
                    check = 0 ;
                    for (int w = 0 ; w < P2.whichblock ; w ++ ){
                        if(intaddr >= P2.memrange[w] && intaddr < P2.memrange[w]+blocksize){
                            output << "Read Hit"<< endl ;
                            P2.state[memstate] = 1 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Read Miss on bus" << endl ;
                        P2.memrange[P2.whichblock] = baseaddr ;
                        P2.state[P2.whichblock] = 1 ; 
                        P2.whichblock++ ;
                         
                    }

                }
            }
            else if(OP == "W"){
                P2.writingnow[memstate] = baseaddr ;

              //  P1.writingnow = 0 ;
              //  P2.writingnow = 0 ;
                if(P2.state[memstate] == 2) {// P0 is in shared
                    check = 0 ;
                    for (int w = 0 ; w < P2.whichblock ; w ++ ){
                        if(intaddr >= P2.memrange[w] && intaddr < P2.memrange[w]+blocksize){
                            output << "Write Hit"<< endl ;
                            P2.state[memstate] = 1 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Write Miss on bus" << endl ;
                        P2.memrange[P2.whichblock] = baseaddr ;
                        P2.state[P2.whichblock] = 1 ;  
                        P2.whichblock++ ;
                        
                    }
                    output << "Broadcast invalid messages"<< endl ;
                    check = 0 ;
                    for (int wr = 0 ; wr < P0.whichblock ; wr ++ ){
	                   if(P0.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P0.state[wr] = 2 ;
                                P0.writingnow[wr] = writeinvalid ;
                            }


	                }
                    if(check == 1 ){
                        output<< "Writing Back: P0"<< endl ;
                        
                        
                    }
                    check = 0 ;
                    for (int wr = 0 ; wr < P1.whichblock ; wr ++ ){
	                    if(P1.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P1.state[wr] = 2 ;
                                P1.writingnow[wr] = writeinvalid ;
                            }

                        /* for (int w = 0 ; w < P2.whichblock ; w ++ ){
	                        if(P1.writingnow[wr] >= P2.memrange[w] && P1.writingnow[wr] < P2.memrange[w] + blocksize&& P2.state[w]!= 3){
	                            check = 1 ;
	                            P1.state[w] = 2 ;
	                             P1.writingnow[wr] = writeinvalid ;
	                        }
	                    }
                        */
	                }
                    if(check == 1 ){
                        output<< "Writing Back: P1"<< endl ;
                        
                       
                    }
                    check = 0 ;
                    for (int w = 0 ; w < P0.whichblock ; w ++ ){
                        if(intaddr >=P0.memrange[w] && intaddr < P0.memrange[w]+blocksize &&P0.state[w] != 3){
                         //  P0.memrange[w] = invalidaddr ;
                            check = 1 ;
                            P0.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P0"<< endl ;
                        
                    }
                    check = 0 ;
                    for (int w = 0 ; w < P1.whichblock ; w ++ ){
                        if(intaddr >=P1.memrange[w] && intaddr < P1.memrange[w]+blocksize &&P1.state[w] != 3){
                         //   P1.memrange[w] = invalidaddr ;
                            check = 1 ;
                             P1.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P1"<< endl ;
                       
                    }

                }
                else if(P2.state[memstate] == 3 ){ // P0 is in invalid 
                    output << "Write Miss on bus"<< endl ;
                    output << "Broadcast invalid messages"<< endl ;
                    P2.memrange[P2.whichblock] = baseaddr ;
                    P2.whichblock++ ;
                    P2.state[memstate] = 1 ;

                   

                    check = 0 ;
                    for (int wr = 0 ; wr < P0.whichblock ; wr ++ ){
	                    if(P0.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P0.state[wr] = 2 ;
                                P0.writingnow[wr] = writeinvalid ;
                            }


	                }
                    if(check == 1 ){
                        output<< "Writing Back: P0"<< endl ;
                        
                        
                    }
                    check = 0 ;
                    for (int wr = 0 ; wr < P1.whichblock ; wr ++ ){
	                  if(P1.writingnow[wr] == baseaddr){
                                check = 1 ;
                                P1.state[wr] = 2 ;
                                P1.writingnow[wr] = writeinvalid ;
                            }

	                }
                    if(check == 1 ){
                        output<< "Writing Back: P1"<< endl ;
                        
                        
                    }
                    check = 0 ;
                    for (int w = 0 ; w < P0.whichblock ; w ++ ){
                        if(intaddr >=P0.memrange[w] && intaddr < P0.memrange[w]+blocksize &&P0.state[w] != 3){
                         //   P0.memrange[w] = invalidaddr ;
                            check = 1 ;
                            P0.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P0"<< endl ;
                        
                    }
                    check = 0 ;
                    for (int w = 0 ; w < P1.whichblock ; w ++ ){
                        if(intaddr >=P1.memrange[w] && intaddr < P1.memrange[w]+blocksize &&P1.state[w] != 3){
                           // P1.memrange[w] = invalidaddr ;
                            check = 1 ;
                            P1.state[w] = 3 ;
                        }
                    }
                    if(check == 1 ){
                        output << "Invalidating: P1"<< endl ;
                        
                    }
                }
                else if(P2.state[memstate] == 1 ){ // P0 is in modify
                    check = 0 ;
                    for (int w = 0 ; w < P2.whichblock ; w ++ ){
                        if(intaddr >= P2.memrange[w] && intaddr < P2.memrange[w]+blocksize){
                            output << "Write Hit"<< endl ;
                            P2.state[memstate] = 1 ;
                            check = 1 ;
                            break ;
                        }
                    }
                    if(check == 0 ){
                        output << "Write Miss on bus" << endl ;
                        P2.memrange[P2.whichblock] = baseaddr ;
                        P2.state[P2.whichblock] = 1 ;  
                        P2.whichblock++ ;
                        
                    }
                }
            }
        }
        Proc = ""; 

        output << endl ;
        inputfile >> Proc ;
    }
    inputfile.close() ;
    output.close() ;

    return 0 ;
}
