//#include "CapacitiveSensor.h"

#ifndef DEBUG_H
  #include "debug.h"
  #define DEBUG_H
#endif

#ifndef __PAD_COUNT__
    #define __PAD_COUNT__ 1 
#endif

#ifndef __LOCUS_PAD_COUNT__
    #define __LOCUS_PAD_COUNT__ 1
#endif

// Class controls and interfaces with the evolutionary algorithm, the inputs (cap buttons, remote)
// and the outputs (lights).
class Cap_Buttons 
{
private:
    CapacitiveSensor *pads[__PAD_COUNT__];

public:
    bool LocusPads[__LOCUS_PAD_COUNT__];
    long TimingTotals[__PAD_COUNT__];

public:
    // constructor, pass in the basic crap so we can pass it on to our minions.
    Cap_Buttons() 
    {
        debug("Cap_Buttons CTOR");

        //pads[0] = new CapacitiveSensor(51, 50);
        //pads[1] = new CapacitiveSensor(49, 48);

        pads[0] = new CapacitiveSensor(2, 3);
        pads[0]->set_CS_AutocaL_Millis(0xFFFFFFFF);

//        for (int i = 0; i < __PAD_COUNT__; i++)
//        {
//            pads[i] = new CapacitiveSensor(22, 51 + i);
//        }

        for (int i = 0; i < __LOCUS_PAD_COUNT__; i++)
        {
            LocusPads[i] = false;
        }

        debug("END Cap_Buttons CTOR");
    }

    int get_pad_count()
    {
        return __PAD_COUNT__;
    }

    int get_locus_pad_count()
    {
        return __LOCUS_PAD_COUNT__;
    }


    void update_button_state() {
        long start = millis();
   
        for (int i = 0; i < __PAD_COUNT__; i++) {
            TimingTotals[i] = pads[i]->capacitiveSensor(200);
        }
   
        // Allele Pads
        for (int i = 0; i < __LOCUS_PAD_COUNT__; i++) {
            if (TimingTotals[i] > 300 || TimingTotals[i] == -2) { LocusPads[i] = true; }
            else { LocusPads[i] = false; }
        } 
    }
};
