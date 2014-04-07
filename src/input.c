#include <pspkernel.h>
#include <pspctrl.h>

static int triggerCode = 0;
static int triggerStatus = 0;
static SceCtrlData buttonInput;

void updateInput() {
    sceCtrlPeekBufferPositive(&buttonInput, 1); 
    triggerCode = buttonInput.Buttons;
    triggerStatus &= triggerCode;
}
int getAnalogX () {
    return buttonInput.Lx;
}
int getAnalogY () {
    return buttonInput.Ly;
}
int keyTrigger (int keycode) {
    if ((triggerCode & keycode) && !(triggerStatus & keycode)) {
        triggerStatus |= keycode;
        return 1;
    }
    else {
        return 0;
    }
}