/********************************************************************************//**
 * \file gpTimer32Bit.h
 * \author Roman Holderried
 *
 * \brief Includes, typedefs, declaration for the General Purpose Timer module
 *
 * <b> History </b>
 *      - 2018-06-04 - Created originally for the RoBoard project. 
 *      - 2021-04-15 - Integrated into RobLib.
 *                     
 ***********************************************************************************/

#ifndef _TIMER32BIT_H_
#define _TIMER32BIT_H_
/************************************************************************************
 * Includes
 ***********************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/************************************************************************************
 * Defines
 ***********************************************************************************/
#define MAX_TIMER_NUMBER  10

#if MAX_TIMER_NUMBER > 32
#error "Maximum number of timers is restricted to 32."
#endif

/************************************************************************************
 * Type definitions
 ***********************************************************************************/
// Callback
typedef void (* tTimer_cb)(void);

// Timervar
typedef struct {
    bool        bOneShot;           // Single timer execution flag
    bool        bActive;            // Timer counting flag
    bool        bUpCounter;         // true: Upcounter, false: Downcounter
    uint8_t     ui8Index;           // Timer index value
    uint32_t    ui32ResetValue;     // Reset value / Maximum counter value
    uint32_t    ui32TimerVal;      // Current timer value
    tTimer_cb pfnTimer_cb;          // Callback
//    void *vUser_data;             // Pointer on userdata, passed at callback execution
}tTIMERVAR32;

#define  tTIMERVAR32_DEFAULTS {false, false, false, 0, 0, 0, NULL, /*NULL*/}

// Timer control structure
typedef struct {
    uint32_t        ui32FreeIndices;
    uint8_t         ui8ActiveTimers;
    tTIMERVAR32    sTimerStruct[MAX_TIMER_NUMBER];
}tTIMER_CTL_32BIT;

#define tTIMER_32BIT_CTL_DEFAULTS { 0xFFFFFFFF, 0, {tTIMERVAR32_DEFAULTS}}


/************************************************************************************
 * Function declarations
 ***********************************************************************************/
/********************************************************************************//**
 * \brief Timer exection routine
 *
 * This function must get called by the ISR of the time base counter. It increments
 * or decrements the counters of all active timer structures and executes the
 * corresponding callback routines (if defined).
 *
 ***********************************************************************************/
int8_t AppendTimer32Bit (tTIMERVAR32 * psTimerConf);

/********************************************************************************//**
 * \brief Appends one timer to the structure and returns its index.
 *
 * @param   psTimerConf Configuration structure
 * @returns index of the timer in the structure. Returns -1 if there is no more free
 *          timer index.
 ***********************************************************************************/
void Timer32BitDestruct (uint8_t index);

/********************************************************************************//**
 * \brief Deletes the Timer on specific index
 *
 * @param ui8Index Index of the timer to delete
 ***********************************************************************************/
void Timer32BitExecute (void);

/********************************************************************************//**
 * \brief Sets the bActive bit of the timer at the specific index
 *
 * @param ui8Index  Index of the timer to delete
 * @param bActive   Active flag of the timer
 ***********************************************************************************/
void Timer32BitSetActive (uint8_t index, bool bActive);

/********************************************************************************//**
 * \brief Sets a new timer value for the timer at the specific index.
 *
 * The counter values are set according to their bUpCounter setting. If configured
 * as Up-Counter, the actual couter value is set to 0. If configured as down Down-
 * Counter, the actual Counter value is set to the desired value.
 *
 * @param ui8Index      Index of the timer to set
 * @param bActive       Active flag of the timer
 * @param ui32TimerVal  Desired timer value
 ***********************************************************************************/
void Timer32BitSetValue (uint8_t index, bool bActive, uint32_t ui32TimerVal);

#endif //_TIMER32BIT_H_
// EOF 
