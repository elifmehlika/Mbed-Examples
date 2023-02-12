#include "mbed.h"

InterruptIn Adult_Customer(p5);
InterruptIn Child_Customer(p6);
InterruptIn Fire_Alarm(p7);

DigitalOut Wall_Clk(p8);
DigitalOut Barber_Ch1(p9);
DigitalOut Barber_Ch2(p10);
DigitalOut Barber_Ch3(p11);
DigitalOut No_Entry(p12);
BusOut Seats(p13, p14, p15, p16, p17, p18, p19, p20);

int Adult_no=0, Child_no=0, Seat_Pattern= 0b00000000;
int Time_Ch1=0, Time_Ch2=0, Time_Ch3=0, temp=0;

void Adult_ISR(){
    if(Adult_no + Child_no < 8 && No_Entry == 0){
      Adult_no++;
      temp++;
      if(temp > 2){
          temp = 1;
      }
      Seat_Pattern = Seat_Pattern << 1;
      Seat_Pattern = Seat_Pattern | 0b00000001;
    }else{
        No_Entry = 1;
        wait_ms(40);
        No_Entry = 0;
    }
}

void Child_ISR(){
    if(Adult_no + Child_no < 7 && No_Entry == 0){
        Child_no = Child_no + 2;
        Seat_Pattern = Seat_Pattern << 2;
        Seat_Pattern = Seat_Pattern | 0b00000011;
    }else{
        No_Entry = 1;
        wait_ms(40);
        No_Entry = 0;
    }
}

void Fire_Alarm_ISR(){
    No_Entry = 1;
    Barber_Ch1 = Barber_Ch2 = Barber_Ch3 = 0;
    while(Seat_Pattern != 0){
       Seat_Pattern = Seat_Pattern >> 1;
       Seats = Seat_Pattern;
       wait_ms(500);
    }
    Child_no = 0;
    Adult_no = 0;
    wait_ms(10000);
    No_Entry = 0;
}

int main() {
    
    Adult_Customer.rise(&Adult_ISR);
    Child_Customer.rise(&Child_ISR);
    Fire_Alarm.rise(&Fire_Alarm_ISR);
    
    while (1) {
        
        // For Chair 1
        if(Barber_Ch1 == 1){
            Time_Ch1--;
            if(Time_Ch1 == 0){
                Barber_Ch1 = 0;
            }
        }else if(Adult_no != 0 && temp == 1){
            Barber_Ch1 = 1;
            Adult_no--;
            Seat_Pattern = Seat_Pattern >> 1;
            Time_Ch1 = 12;
        }
        
        //For Chair 2
        if(Barber_Ch2 == 1){
            Time_Ch2--;
            if(Time_Ch2 == 0){
                Barber_Ch2 = 0;
            }
        }else if(Adult_no != 0 && temp == 2){
            Barber_Ch2 = 1;
            Adult_no--;
            Seat_Pattern = Seat_Pattern >> 1;
            Time_Ch2 = 12;
        }
        
        // For Chair 3
        if(Barber_Ch3 == 1){
            Time_Ch3--;
            if(Time_Ch3 == 0 && Child_no != 0){
                Barber_Ch3 = 0;
                Child_no--;
                Seat_Pattern = Seat_Pattern >> 1;
            }else if(Time_Ch3 == 0 && Child_no == 0){
                Barber_Ch3 = 0;
            }
        }else if(Child_no != 0){
            Barber_Ch3 = 1;
            Child_no--;
            Seat_Pattern = Seat_Pattern >> 1;
            Time_Ch3 = 12;
        }else if(Child_no == 0  && Adult_no != 0){
            Barber_Ch3 = 1;
            Adult_no--;
            Seat_Pattern = Seat_Pattern >> 1;
            Time_Ch3 = 12;
        }
        
        Seats = Seat_Pattern;
        
        // Clock ticks at every 1 second.
        Wall_Clk = !Wall_Clk;
        wait_ms(1000);
    }
}