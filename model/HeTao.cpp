/*
The MIT License (MIT)

Copyright (c) 2016 Lancaster University, UK.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/


#include "HeTao.h"
#include "Timer.h"

using namespace codal;

static HeTao *device_instance = NULL;

/**
  * Constructor.
  *
  * Create a representation of a GenuinoZero device, which includes member variables
  * that represent various device drivers used to control aspects of the micro:bit.
  */
HeTao::HeTao() :
        tim4(TIM4, TIM4_IRQn),
        timer(tim4),
        messageBus(),
        io(),
        buttonUp(io.buttonUp, DEVICE_ID_BUTTON_UP, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
        buttonDown(io.buttonDown, DEVICE_ID_BUTTON_DOWN, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
        buttonLeft(io.buttonLeft, DEVICE_ID_BUTTON_LEFT, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up),
        buttonRight(io.buttonRight, DEVICE_ID_BUTTON_RIGHT, DEVICE_BUTTON_ALL_EVENTS, ACTIVE_LOW, PullMode::Up) {
    // Clear our status
    status = 0;

    io.pwr_on.setDigitalValue(1);

    device_instance = this;
}

/**
  * Post constructor initialisation method.
  *
  * This call will initialised the scheduler, memory allocator and Bluetooth stack.
  *
  * This is required as the Bluetooth stack can't be brought up in a
  * static context i.e. in a constructor.
  *
  * @code
  * uBit.init();
  * @endcode
  *
  * @note This method must be called before user code utilises any functionality
  *       contained within the GenuinoZero class.
  */
int HeTao::init() {
    if (status & DEVICE_INITIALIZED)
        return DEVICE_NOT_SUPPORTED;

    status |= DEVICE_INITIALIZED;

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    for (int i = 0; i < DEVICE_COMPONENT_COUNT; i++) {
        if (CodalComponent::components[i])
            CodalComponent::components[i]->init();
    }

    codal_dmesg_set_flush_fn(hetao_dmesg_flush);
    status |= DEVICE_COMPONENT_STATUS_IDLE_TICK;

    return DEVICE_OK;
}

/**
  * A periodic callback invoked by the fiber scheduler idle thread.
  * We use this for any low priority, backgrounf housekeeping.
  *
  */
void HeTao::idleCallback() {
    codal_dmesg_flush();
}

void hetao_dmesg_flush() {
}
