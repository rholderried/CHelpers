/********************************************************************************//**
 * \file gpTimer32Bit.c
 * \author Roman Holderried
 *
 * \brief General Purpose Timer module
 *
 * This module extends the functionality of one hardware timer peripheral to 
 * schedule multiple non-critical timer operations. 
 * It provides up to 32 timer slots which can operate at integer multiples of the
 * time base cycle time.
 *
 * <b> History </b>
 *      - 2018-06-04 - Created originally for the RoBoard project. 
 *      - 2021-04-15 - Integrated into RobLib.
 *                     
 ***********************************************************************************/

/************************************************************************************
 * Includes
 ***********************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "Timer32Bit.h"

/************************************************************************************
 * Defines
 ***********************************************************************************/

/************************************************************************************
 * State variables
 ***********************************************************************************/
static tTIMER_CTL_32BIT sTimerCtl = tTIMER_32BIT_CTL_DEFAULTS;

/************************************************************************************
 * Function definitions
 ***********************************************************************************/

//===================================================================================
// Funktion: timer32BitExecute
//===================================================================================
void Timer32BitExecute (void)
{
    uint8_t ui8CurrentIndex = 0;
    int8_t i8ActiveTimers = 0;
    bool bTimerFinished = false;
    tTIMERVAR32 *pTimer;

    i8ActiveTimers = sTimerCtl.ui8ActiveTimers;

    // Loop over all active timers
    while ((i8ActiveTimers > 0) && (ui8CurrentIndex < MAX_TIMER_NUMBER))
    {
        pTimer = &sTimerCtl.sTimerStruct[ui8CurrentIndex];

        // Skip if the current index is a free timer
        if ((sTimerCtl.ui32FreeIndices & (1 << ui8CurrentIndex))
             || pTimer->bActive == false)
        {
            ui8CurrentIndex++;
            continue;
        }

        /****************************************************************************
         * Handle the Counter value
         ***************************************************************************/
        if (pTimer->bUpCounter)
        {
            if (pTimer->ui32TimerVal < pTimer->ui32ResetValue)
                pTimer->ui32TimerVal++;
            
            if (pTimer->ui32TimerVal == pTimer->ui32ResetValue)
            {
                bTimerFinished = true;
                pTimer->ui32TimerVal = 0;
            }
        }
        else
        {
            if (pTimer->ui32TimerVal > 0)
                pTimer->ui32TimerVal--;

            if (pTimer->ui32TimerVal == 0)
            {
                bTimerFinished = true;
                pTimer->ui32TimerVal = pTimer->ui32ResetValue;
            }
        }
        /****************************************************************************
         * Handle the action on timer finished
         ***************************************************************************/
        
        if (bTimerFinished)
        {
            // Destroy the timer if it was declared as one shot timer
            if (pTimer->bOneShot)
                timer32BitDestruct(ui8CurrentIndex);

            // Call the action procedure
            if (pTimer->pfnTimer_cb != NULL)
                pTimer->pfnTimer_cb(/*pTimer->vUser_data*/);

            // Prepare the var for reuse
            bTimerFinished = false;
        }

        ui8CurrentIndex++;
        i8ActiveTimers--;
    }
}

//===================================================================================
// Funktion: appendTimer32Bit
//===================================================================================
int8_t AppendTimer32Bit (tTIMERVAR32 *sTimerConf)
{
    uint8_t ui8Counter;
    int8_t  returnVal = -1;

    // Freien Index suchen
    for (ui8Counter = 0; ui8Counter < MAX_TIMER_NUMBER; ui8Counter++)
    {
        if (!(sTimerCtl.ui32FreeIndices & (1 << ui8Counter)))
            continue;
        
        else
        {
            sTimerCtl.sTimerStruct[ui8Counter] = *sTimerConf;

            // Flag index position of the timer
            sTimerCtl.ui32FreeIndices &= ~(1 << ui8Counter);

            // Save timer index
            sTimerCtl.sTimerStruct[ui8Counter].ui8Index = ui8Counter;

            // Timer can be active from the beginning
            if (sTimerCtl.sTimerStruct[ui8Counter].bActive == true)
                sTimerCtl.ui8ActiveTimers += 1;

            returnVal = ui8Counter;
            break;
        }

    }
    return returnVal;
}

//==============================================================================
// Funktion: timer32BitDestruct
//==============================================================================
void Timer32BitDestruct (uint8_t ui8Index)
{
    // Free the index
    sTimerCtl.ui32FreeIndices |= (1 << ui8Index);

    // Reset timer activation
    timer32BitSetActive(ui8Index, false);
}

//==============================================================================
// Funktion: timer32BitSetActive
//==============================================================================
void Timer32BitSetActive (uint8_t ui8Index, bool bActive)
{
    // War das bActive - Bit bereits gesetzt?
    bool bActiveOld = sTimerCtl.sTimerStruct[ui8Index].bActive;

    // Aktivieren / Deaktivieren des Timers
    sTimerCtl.sTimerStruct[ui8Index].bActive = bActive;

    // Inkrementieren oder dekrementieren der Anzahl aktiver Timer
    sTimerCtl.ui8ActiveTimers += bActiveOld ? -((uint8_t)(!bActive)) : (uint8_t)bActive;
}

//==============================================================================
// Funktion: ms_timer_16Bit_set_val
//==============================================================================
void Timer32BitSetValue (uint8_t ui8Index, bool bActive, uint32_t ui32TimerVal)
{
    tTIMERVAR32 *pTimer = &sTimerCtl.sTimerStruct[ui8Index];


    if (pTimer->bUpCounter)
    {
        pTimer->ui32ResetValue = ui32TimerVal;
        pTimer->ui32TimerVal = 0;
    }
    else
        pTimer->ui32TimerVal = ui32TimerVal;

    // Aktivieren / Deaktivieren des Timers
    timer32BitSetActive(ui8Index, bActive);
}
// EOF timer.c-----------------------------------------------------------------------
