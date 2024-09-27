
#include <intelfpgaup/KEY.h>
#include "button.h"

int botao;

int buttonPressed(){
    int *data;
    if(KEY_open()){
        if(KEY_read(&data)){
            // if(data != 0){
            //     printf("botao n: %d\n", data);
            // }
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

void buttonRead(){

}