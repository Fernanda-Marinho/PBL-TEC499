#include <intelfpgaup/KEY.h>
#include "button.h"

/* Checks if a button has been pressed and returns a corresponding 
value: 1 for button 1, 2 for button 2, or 0 if no button has been pressed.  */
int buttonPressed(){
    int *data;

    if(KEY_open()){
        if(KEY_read(&data)){
            if(data == 1){
                KEY_close();
                return 1;

            } else if(data == 2){
                KEY_close();
                return 2;
            }
        }
    }

    KEY_close();
    return 0;

}
