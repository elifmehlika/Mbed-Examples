#include "mbed.h"

// Define inputs
DigitalIn START(p5); // Start button pressed = 1
DigitalIn STOP(p6); // Stop button pressed = 1
DigitalIn GUARD(p7); // Safety guard is closed = 1
DigitalIn TEMP(p8); // Excess temperature = 1

// Define outputs
DigitalOut READY(p9); // Machine is ready to run (White LED)
DigitalOut RUNNING(p10); // Machine is running (Red LED)
DigitalOut FAULT(p11); // Safety Guard is ON (Blue LED)
DigitalOut EXCESS(p12); // Excess temperature (Yellow LED)

int main() {
    while(1){
        
        // Not Ready State
        while(GUARD == 0 || TEMP == 1){
            READY = !READY;
            wait_ms(500);
        }
        
        // Ready State
        READY = 1;
        while(START == 0){
            if(GUARD == 0 || TEMP == 1){
                break;
            }
            wait_ms(500);
        }
        
        // Running State
        while(GUARD == 1 && TEMP == 0 && STOP == 0){
            READY = 0;
            RUNNING = 1;
            wait_ms(500);
            
            if(GUARD == 0){
                RUNNING = 0;
                FAULT = 1;
                wait_ms(500);
                FAULT = 0;
            }
            if(TEMP == 1){
                RUNNING = 0;
                EXCESS = 1;
                wait_ms(500);
                EXCESS = 0;
            }
            if(STOP == 1){
                RUNNING = 0;
            }
        }
    }
}
