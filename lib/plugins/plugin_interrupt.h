#ifndef PLUGIN_INTERRUPT_H
#define PLUGIN_INTERRUPT_H

class plugin_interrupt
{
    public:
        plugin_interrupt(){}

        virtual bool Interrupt(uint8_t pin) = 0;
};

#endif