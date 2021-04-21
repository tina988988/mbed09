#include "mbed.h"
#include "mbed_rpc.h"

/**
 *  This example program has been updated to use the RPC implementation in the new mbed libraries.
 *  This example demonstrates using RPC over serial
**/
DigitalOut led1(LED1);
DigitalOut led3(LED3);
//RpcDigitalOut myled3(LED3,"myled3");
BufferedSerial pc(USBTX, USBRX);
void LEDControl(Arguments *in, Reply *out);
RPCFunction rpcLED(&LEDControl, "LEDControl");
double x, y;
Thread thread;

void blink() {
    //char buffer[200], outbuf[256];
    //char strings[20];
    while(1) {
        if(x) led1 = !led1; 
        if(y) led3 = !led3;
        ThisThread::sleep_for(500ms);
    }
}

int main() {
    //The mbed RPC classes are now wrapped to create an RPC enabled version - see RpcClasses.h so don't add to base class

    // receive commands, and send back the responses
    char buf[256], outbuf[256];

    FILE *devin = fdopen(&pc, "r");
    FILE *devout = fdopen(&pc, "w");
    thread.start(blink);

    while(1) {
        memset(buf, 0, 256);
        for (int i = 0; ; i++) {
            char recv = fgetc(devin);
            if (recv == '\n') {
                printf("\r\n");
                break;
            }
            buf[i] = fputc(recv, devout);
        }
        //Call the static call method on the RPC class
        RPC::call(buf, outbuf);
        printf("%s\r\n", outbuf);
        //void blink();
    }

}

// Make sure the method takes in Arguments and Reply objects.
void LEDControl (Arguments *in, Reply *out)   {
    bool success = true;

    // In this scenario, when using RPC delimit the two arguments with a space.
    x = in->getArg<double>();
   // z = in->getArg<double>();
    y = in->getArg<double>();
    //z1 = in->getArg<double>();

    // Have code here to call another RPC function to wake up specific led or close it.
    char buffer[200], outbuf[256];
    char strings[20];
    int led = x;
    //int on = z;
    int led1 = y;
    //int on1 = z1;
    sprintf(strings, "/%d/%d", led, led1);
    //sprintf(strings, "/myled%d/write %d", led1, on1);
    strcpy(buffer, strings);
    RPC::call(buffer, outbuf);
    if (success) {
        out->putData(buffer);
    } else {
        out->putData("Failed to execute LED control.");
    }
}