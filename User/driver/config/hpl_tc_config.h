/* Auto-generated config file hpl_tc_config.h */
#ifndef HPL_TC_CONFIG_H
#define HPL_TC_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
#include "peripheral_clk_config.h"

#ifndef CONF_TC0_ENABLE
#define CONF_TC0_ENABLE 1
#endif

// <h> Basic settings
// <o> Prescaler
// <0=> No division
// <1=> Divide by 2
// <2=> Divide by 4
// <3=> Divide by 8
// <4=> Divide by 16
// <5=> Divide by 64
// <6=> Divide by 256
// <7=> Divide by 1024
// <i> This defines the prescaler value
// <id> tc_prescaler
#ifndef CONF_TC0_PRESCALER
#define CONF_TC0_PRESCALER 3
#endif
// </h>

// <h> PWM Waveform Output settings
// <o> Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tc_arch_wave_per_val
#ifndef CONF_TC0_WAVE_PER_VAL
#define CONF_TC0_WAVE_PER_VAL 0x3e8
#endif

// <o> Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tc_arch_wave_duty_val
#ifndef CONF_TC0_WAVE_DUTY_VAL
#define CONF_TC0_WAVE_DUTY_VAL 0x1f4
#endif

/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TC0_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC0_CC0                                                                                                   \
	((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 1000000 / (1 << CONF_TC0_PRESCALER) - 1))
#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 64000000 - 1))
#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 256000000 - 1))
#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 1024000000 - 1))
#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)
#endif

// </h>

// <h> Advanced settings
// <y> Mode
// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
// <i> These bits mode
// <id> tc_mode
#ifndef CONF_TC0_MODE
#define CONF_TC0_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

/*  Unused in 16/32 bit PWM mode */
#ifndef CONF_TC0_PER
#define CONF_TC0_PER 0x32
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC0_PRESCSYNC
#define CONF_TC0_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the will continue running in standby sleep mode or not
// <id> tc_arch_runstdby
#ifndef CONF_TC0_RUNSTDBY
#define CONF_TC0_RUNSTDBY 0
#endif

// <q> On-Demand
// <i> Indicates whether the TC0's on-demand mode is on or not
// <id> tc_arch_ondemand
#ifndef CONF_TC0_ONDEMAND
#define CONF_TC0_ONDEMAND 0
#endif

// <o> Auto Lock
// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
// <id> tc_arch_alock
#ifndef CONF_TC0_ALOCK
#define CONF_TC0_ALOCK 0
#endif

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC0_CAPTEN0 0
//#define CONF_TC0_CAPTEN1 0
//#define CONF_TC0_COPEN0  0
//#define CONF_TC0_COPEN1  0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC0_DIR     0
//#define CONF_TC0_ONESHOT 0
//#define CONF_TC0_LUPD    0

// <q> Debug Running Mode
// <i> Indicates whether the Debug Running Mode is enabled or not
// <id> tc_arch_dbgrun
#ifndef CONF_TC0_DBGRUN
#define CONF_TC0_DBGRUN 0
#endif

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC0_EVENT_CONTROL_ENABLE
#define CONF_TC0_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC0_MCEO0
#define CONF_TC0_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC0_MCEO1
#define CONF_TC0_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC0_OVFEO
#define CONF_TC0_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC0_TCEI
#define CONF_TC0_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC0_TCINV
#define CONF_TC0_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC0_EVACT
#define CONF_TC0_EVACT 0
#endif
// </e>

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC0_WAVEGEN   TC_CTRLA_WAVEGEN_MFRQ_Val

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC0_INVEN0 0
//#define CONF_TC0_INVEN1 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC0_PERBUF 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC0_CCBUF0 0
//#define CONF_TC0_CCBUF1 0

// </h>

#include "peripheral_clk_config.h"

#ifndef CONF_TC1_ENABLE
#define CONF_TC1_ENABLE 1
#endif

// <h> Basic settings
// <o> Prescaler
// <0=> No division
// <1=> Divide by 2
// <2=> Divide by 4
// <3=> Divide by 8
// <4=> Divide by 16
// <5=> Divide by 64
// <6=> Divide by 256
// <7=> Divide by 1024
// <i> This defines the prescaler value
// <id> tc_prescaler
#ifndef CONF_TC1_PRESCALER
#define CONF_TC1_PRESCALER 3
#endif
// </h>

// <h> PWM Waveform Output settings
// <o> Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tc_arch_wave_per_val
#ifndef CONF_TC1_WAVE_PER_VAL
#define CONF_TC1_WAVE_PER_VAL 0x3e8
#endif

// <o> Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tc_arch_wave_duty_val
#ifndef CONF_TC1_WAVE_DUTY_VAL
#define CONF_TC1_WAVE_DUTY_VAL 0x1f4
#endif

/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TC1_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC1_CC0                                                                                                   \
	((uint32_t)(((double)CONF_TC1_WAVE_PER_VAL * CONF_GCLK_TC1_FREQUENCY) / 1000000 / (1 << CONF_TC1_PRESCALER) - 1))
#define CONF_TC1_CC1 ((CONF_TC1_CC0 * CONF_TC1_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC1_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC1_CC0 ((uint32_t)(((double)CONF_TC1_WAVE_PER_VAL * CONF_GCLK_TC1_FREQUENCY) / 64000000 - 1))
#define CONF_TC1_CC1 ((CONF_TC1_CC0 * CONF_TC1_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC1_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TC1_CC0 ((uint32_t)(((double)CONF_TC1_WAVE_PER_VAL * CONF_GCLK_TC1_FREQUENCY) / 256000000 - 1))
#define CONF_TC1_CC1 ((CONF_TC1_CC0 * CONF_TC1_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC1_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TC1_CC0 ((uint32_t)(((double)CONF_TC1_WAVE_PER_VAL * CONF_GCLK_TC1_FREQUENCY) / 1024000000 - 1))
#define CONF_TC1_CC1 ((CONF_TC1_CC0 * CONF_TC1_WAVE_DUTY_VAL) / 1000)
#endif

// </h>

// <h> Advanced settings
// <y> Mode
// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
// <i> These bits mode
// <id> tc_mode
#ifndef CONF_TC1_MODE
#define CONF_TC1_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

/*  Unused in 16/32 bit PWM mode */
#ifndef CONF_TC1_PER
#define CONF_TC1_PER 0x32
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC1_PRESCSYNC
#define CONF_TC1_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the will continue running in standby sleep mode or not
// <id> tc_arch_runstdby
#ifndef CONF_TC1_RUNSTDBY
#define CONF_TC1_RUNSTDBY 0
#endif

// <q> On-Demand
// <i> Indicates whether the TC1's on-demand mode is on or not
// <id> tc_arch_ondemand
#ifndef CONF_TC1_ONDEMAND
#define CONF_TC1_ONDEMAND 0
#endif

// <o> Auto Lock
// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
// <id> tc_arch_alock
#ifndef CONF_TC1_ALOCK
#define CONF_TC1_ALOCK 0
#endif

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC1_CAPTEN0 0
//#define CONF_TC1_CAPTEN1 0
//#define CONF_TC1_COPEN0  0
//#define CONF_TC1_COPEN1  0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC1_DIR     0
//#define CONF_TC1_ONESHOT 0
//#define CONF_TC1_LUPD    0

// <q> Debug Running Mode
// <i> Indicates whether the Debug Running Mode is enabled or not
// <id> tc_arch_dbgrun
#ifndef CONF_TC1_DBGRUN
#define CONF_TC1_DBGRUN 0
#endif

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC1_EVENT_CONTROL_ENABLE
#define CONF_TC1_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC1_MCEO0
#define CONF_TC1_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC1_MCEO1
#define CONF_TC1_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC1_OVFEO
#define CONF_TC1_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC1_TCEI
#define CONF_TC1_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC1_TCINV
#define CONF_TC1_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC1_EVACT
#define CONF_TC1_EVACT 0
#endif
// </e>

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC1_WAVEGEN   TC_CTRLA_WAVEGEN_MFRQ_Val

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC1_INVEN0 0
//#define CONF_TC1_INVEN1 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC1_PERBUF 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC1_CCBUF0 0
//#define CONF_TC1_CCBUF1 0

// </h>

#include "peripheral_clk_config.h"

#ifndef CONF_TC2_ENABLE
#define CONF_TC2_ENABLE 1
#endif

// <h> Basic settings
// <o> Prescaler
// <0=> No division
// <1=> Divide by 2
// <2=> Divide by 4
// <3=> Divide by 8
// <4=> Divide by 16
// <5=> Divide by 64
// <6=> Divide by 256
// <7=> Divide by 1024
// <i> This defines the prescaler value
// <id> tc_prescaler
#ifndef CONF_TC2_PRESCALER
#define CONF_TC2_PRESCALER 3
#endif
// </h>

// <h> PWM Waveform Output settings
// <o> Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tc_arch_wave_per_val
#ifndef CONF_TC2_WAVE_PER_VAL
#define CONF_TC2_WAVE_PER_VAL 0x3e8
#endif

// <o> Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tc_arch_wave_duty_val
#ifndef CONF_TC2_WAVE_DUTY_VAL
#define CONF_TC2_WAVE_DUTY_VAL 0x1f4
#endif

/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TC2_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC2_CC0                                                                                                   \
	((uint32_t)(((double)CONF_TC2_WAVE_PER_VAL * CONF_GCLK_TC2_FREQUENCY) / 1000000 / (1 << CONF_TC2_PRESCALER) - 1))
#define CONF_TC2_CC1 ((CONF_TC2_CC0 * CONF_TC2_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC2_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC2_CC0 ((uint32_t)(((double)CONF_TC2_WAVE_PER_VAL * CONF_GCLK_TC2_FREQUENCY) / 64000000 - 1))
#define CONF_TC2_CC1 ((CONF_TC2_CC0 * CONF_TC2_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC2_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TC2_CC0 ((uint32_t)(((double)CONF_TC2_WAVE_PER_VAL * CONF_GCLK_TC2_FREQUENCY) / 256000000 - 1))
#define CONF_TC2_CC1 ((CONF_TC2_CC0 * CONF_TC2_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC2_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TC2_CC0 ((uint32_t)(((double)CONF_TC2_WAVE_PER_VAL * CONF_GCLK_TC2_FREQUENCY) / 1024000000 - 1))
#define CONF_TC2_CC1 ((CONF_TC2_CC0 * CONF_TC2_WAVE_DUTY_VAL) / 1000)
#endif

// </h>

// <h> Advanced settings
// <y> Mode
// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
// <i> These bits mode
// <id> tc_mode
#ifndef CONF_TC2_MODE
#define CONF_TC2_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

/*  Unused in 16/32 bit PWM mode */
#ifndef CONF_TC2_PER
#define CONF_TC2_PER 0x32
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC2_PRESCSYNC
#define CONF_TC2_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the will continue running in standby sleep mode or not
// <id> tc_arch_runstdby
#ifndef CONF_TC2_RUNSTDBY
#define CONF_TC2_RUNSTDBY 0
#endif

// <q> On-Demand
// <i> Indicates whether the TC2's on-demand mode is on or not
// <id> tc_arch_ondemand
#ifndef CONF_TC2_ONDEMAND
#define CONF_TC2_ONDEMAND 0
#endif

// <o> Auto Lock
// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
// <id> tc_arch_alock
#ifndef CONF_TC2_ALOCK
#define CONF_TC2_ALOCK 0
#endif

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC2_CAPTEN0 0
//#define CONF_TC2_CAPTEN1 0
//#define CONF_TC2_COPEN0  0
//#define CONF_TC2_COPEN1  0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC2_DIR     0
//#define CONF_TC2_ONESHOT 0
//#define CONF_TC2_LUPD    0

// <q> Debug Running Mode
// <i> Indicates whether the Debug Running Mode is enabled or not
// <id> tc_arch_dbgrun
#ifndef CONF_TC2_DBGRUN
#define CONF_TC2_DBGRUN 0
#endif

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC2_EVENT_CONTROL_ENABLE
#define CONF_TC2_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC2_MCEO0
#define CONF_TC2_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC2_MCEO1
#define CONF_TC2_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC2_OVFEO
#define CONF_TC2_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC2_TCEI
#define CONF_TC2_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC2_TCINV
#define CONF_TC2_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC2_EVACT
#define CONF_TC2_EVACT 0
#endif
// </e>

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC2_WAVEGEN   TC_CTRLA_WAVEGEN_MFRQ_Val

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC2_INVEN0 0
//#define CONF_TC2_INVEN1 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC2_PERBUF 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC2_CCBUF0 0
//#define CONF_TC2_CCBUF1 0



#include "peripheral_clk_config.h"


#ifndef CONF_TC3_ENABLE
#define CONF_TC3_ENABLE 1
#endif

// <h> Basic settings
// <o> Prescaler
// <0=> No division
// <1=> Divide by 2
// <2=> Divide by 4
// <3=> Divide by 8
// <4=> Divide by 16
// <5=> Divide by 64
// <6=> Divide by 256
// <7=> Divide by 1024
// <i> This defines the prescaler value
// <id> tc_prescaler
#ifndef CONF_TC3_PRESCALER
#define CONF_TC3_PRESCALER 0
#endif
// </h>

// <h> PWM Waveform Output settings
// <o> Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tc_arch_wave_per_val
#ifndef CONF_TC3_WAVE_PER_VAL
#define CONF_TC3_WAVE_PER_VAL 0x3e8
#endif

// <o> Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tc_arch_wave_duty_val
#ifndef CONF_TC3_WAVE_DUTY_VAL
#define CONF_TC3_WAVE_DUTY_VAL 0x1f4
#endif

///* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
//#if CONF_TC0_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
//#define CONF_TC0_CC0                                                                                                   \
//	((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 1000000 / (1 << CONF_TC0_PRESCALER) - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

//#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
//#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 64000000 - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

//#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
//#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 256000000 - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

//#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
//#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 1024000000 - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)
//#endif
/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TC3_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC3_CC0                                                                                                   \
	((uint32_t)(((double)CONF_TC3_WAVE_PER_VAL * CONF_GCLK_TC3_FREQUENCY) / 1000000 / (1 << CONF_TC3_PRESCALER) - 1))
#define CONF_TC3_CC1 ((CONF_TC3_CC0 * CONF_TC3_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC3_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC3_CC0 ((uint32_t)(((double)CONF_TC3_WAVE_PER_VAL * CONF_GCLK_TC3_FREQUENCY) / 64000000 - 1))
#define CONF_TC3_CC1 ((CONF_TC3_CC0 * CONF_TC3_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC3_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TC3_CC0 ((uint32_t)(((double)CONF_TC3_WAVE_PER_VAL * CONF_GCLK_TC3_FREQUENCY) / 256000000 - 1))
#define CONF_TC3_CC1 ((CONF_TC3_CC0 * CONF_TC3_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC3_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TC3_CC0 ((uint32_t)(((double)CONF_TC3_WAVE_PER_VAL * CONF_GCLK_TC3_FREQUENCY) / 1024000000 - 1))
#define CONF_TC3_CC1 ((CONF_TC3_CC0 * CONF_TC3_WAVE_DUTY_VAL) / 1000)
#endif

// </h>

// <h> Advanced settings
// <y> Mode
// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
// <i> These bits mode
// <id> tc_mode
#ifndef CONF_TC3_MODE
#define CONF_TC3_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

/*  Unused in 16/32 bit PWM mode */
#ifndef CONF_TC3_PER
#define CONF_TC3_PER 0x32
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC3_PRESCSYNC
#define CONF_TC3_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the will continue running in standby sleep mode or not
// <id> tc_arch_runstdby
#ifndef CONF_TC3_RUNSTDBY
#define CONF_TC3_RUNSTDBY 0
#endif

// <q> On-Demand
// <i> Indicates whether the TC3's on-demand mode is on or not
// <id> tc_arch_ondemand
#ifndef CONF_TC3_ONDEMAND
#define CONF_TC3_ONDEMAND 0
#endif

// <o> Auto Lock
// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
// <id> tc_arch_alock
#ifndef CONF_TC3_ALOCK
#define CONF_TC3_ALOCK 0
#endif

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC3_CAPTEN0 0
//#define CONF_TC3_CAPTEN1 0
//#define CONF_TC3_COPEN0  0
//#define CONF_TC3_COPEN1  0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC3_DIR     0
//#define CONF_TC3_ONESHOT 0
//#define CONF_TC3_LUPD    0

// <q> Debug Running Mode
// <i> Indicates whether the Debug Running Mode is enabled or not
// <id> tc_arch_dbgrun
#ifndef CONF_TC3_DBGRUN
#define CONF_TC3_DBGRUN 0
#endif

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC3_EVENT_CONTROL_ENABLE
#define CONF_TC3_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC3_MCEO0
#define CONF_TC3_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC3_MCEO1
#define CONF_TC3_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC3_OVFEO
#define CONF_TC3_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC3_TCEI
#define CONF_TC3_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC3_TCINV
#define CONF_TC3_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC3_EVACT
#define CONF_TC3_EVACT 0
#endif
// </e>


#include "peripheral_clk_config.h"

#ifndef CONF_TC4_ENABLE
#define CONF_TC4_ENABLE 1
#endif
// <h> Basic configuration

// <o> Prescaler
// <0x0=> No division
// <0x1=> Divide by 2
// <0x2=> Divide by 4
// <0x3=> Divide by 8
// <0x4=> Divide by 16
// <0x5=> Divide by 64
// <0x6=> Divide by 256
// <0x7=> Divide by 1024
// <i> This defines the prescaler value
// <id> timer_prescaler
#ifndef CONF_TC4_PRESCALER
#define CONF_TC4_PRESCALER 0x0
#endif

// <o> Length of one timer tick in uS <0-4294967295>
// <id> timer_tick
#ifndef CONF_TC4_TIMER_TICK
#define CONF_TC4_TIMER_TICK 1000
#endif
// </h>

// <e> Advanced configuration
// <id> timer_advanced_configuration
#ifndef CONF_TC4__ADVANCED_CONFIGURATION_ENABLE
#define CONF_TC4__ADVANCED_CONFIGURATION_ENABLE 0
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC4_PRESCSYNC
#define CONF_TC4_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the module will continue to run in standby sleep mode
// <id> tc_arch_runstdby
#ifndef CONF_TC4_RUNSTDBY
#define CONF_TC4_RUNSTDBY 0
#endif

// <q> Run in debug mode
// <i> Indicates whether the module will run in debug mode
// <id> tc_arch_dbgrun
#ifndef CONF_TC4_DBGRUN
#define CONF_TC4_DBGRUN 0
#endif

// <q> Run on demand
// <i> Run if requested by some other peripheral in the device
// <id> tc_arch_ondemand
#ifndef CONF_TC4_ONDEMAND
#define CONF_TC4_ONDEMAND 0
#endif

// </e>

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC4_EVENT_CONTROL_ENABLE
#define CONF_TC4_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC4_MCEO0
#define CONF_TC4_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC4_MCEO1
#define CONF_TC4_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC4_OVFEO
#define CONF_TC4_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC4_TCEI
#define CONF_TC4_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC4_TCINV
#define CONF_TC4_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC4_EVACT
#define CONF_TC4_EVACT 0
#endif
// </e>

// Default values which the driver needs in order to work correctly

// Mode set to 32-bit
#ifndef CONF_TC4_MODE
#define CONF_TC4_MODE TC_CTRLA_MODE_COUNT32_Val
#endif

// CC 1 register set to 0
#ifndef CONF_TC4_CC1
#define CONF_TC4_CC1 0
#endif

#ifndef CONF_TC4_ALOCK
#define CONF_TC4_ALOCK 0
#endif

// Not used in 32-bit mode
#define CONF_TC4_PER 0

// Calculating correct top value based on requested tick interval.
#define CONF_TC4_PRESCALE (1 << CONF_TC4_PRESCALER)

// Prescaler set to 64
#if CONF_TC4_PRESCALER > 0x4
#undef CONF_TC4_PRESCALE
#define CONF_TC4_PRESCALE 64
#endif

// Prescaler set to 256
#if CONF_TC4_PRESCALER > 0x5
#undef CONF_TC4_PRESCALE
#define CONF_TC4_PRESCALE 256
#endif

// Prescaler set to 1024
#if CONF_TC4_PRESCALER > 0x6
#undef CONF_TC4_PRESCALE
#define CONF_TC4_PRESCALE 1024
#endif

#ifndef CONF_TC4_CC0
#define CONF_TC4_CC0                                                                                                   \
	(uint32_t)(((float)CONF_TC4_TIMER_TICK / 1000000.f) / (1.f / (CONF_GCLK_TC4_FREQUENCY / CONF_TC4_PRESCALE)))
#endif

	
#include "peripheral_clk_config.h"


#ifndef CONF_TC5_ENABLE
#define CONF_TC5_ENABLE 1
#endif

// <h> Basic settings
// <o> Prescaler
// <0=> No division
// <1=> Divide by 2
// <2=> Divide by 4
// <3=> Divide by 8
// <4=> Divide by 16
// <5=> Divide by 64
// <6=> Divide by 256
// <7=> Divide by 1024
// <i> This defines the prescaler value
// <id> tc_prescaler
#ifndef CONF_TC5_PRESCALER
#define CONF_TC5_PRESCALER 3
#endif
// </h>

// <h> PWM Waveform Output settings
// <o> Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tc_arch_wave_per_val
#ifndef CONF_TC5_WAVE_PER_VAL
#define CONF_TC5_WAVE_PER_VAL 0x3e8
#endif

// <o> Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tc_arch_wave_duty_val
#ifndef CONF_TC5_WAVE_DUTY_VAL
#define CONF_TC5_WAVE_DUTY_VAL 0x1f4
#endif

///* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
//#if CONF_TC0_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
//#define CONF_TC0_CC0                                                                                                   \
//	((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 1000000 / (1 << CONF_TC0_PRESCALER) - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

//#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
//#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 64000000 - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

//#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
//#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 256000000 - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)

//#elif CONF_TC0_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
//#define CONF_TC0_CC0 ((uint32_t)(((double)CONF_TC0_WAVE_PER_VAL * CONF_GCLK_TC0_FREQUENCY) / 1024000000 - 1))
//#define CONF_TC0_CC1 ((CONF_TC0_CC0 * CONF_TC0_WAVE_DUTY_VAL) / 1000)
//#endif
/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TC5_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC5_CC0                                                                                                   \
	((uint32_t)(((double)CONF_TC5_WAVE_PER_VAL * CONF_GCLK_TC5_FREQUENCY) / 1000000 / (1 << CONF_TC5_PRESCALER) - 1))
#define CONF_TC5_CC1 ((CONF_TC5_CC0 * CONF_TC5_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC5_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC5_CC0 ((uint32_t)(((double)CONF_TC5_WAVE_PER_VAL * CONF_GCLK_TC5_FREQUENCY) / 64000000 - 1))
#define CONF_TC5_CC1 ((CONF_TC5_CC0 * CONF_TC5_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC5_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TC5_CC0 ((uint32_t)(((double)CONF_TC5_WAVE_PER_VAL * CONF_GCLK_TC5_FREQUENCY) / 256000000 - 1))
#define CONF_TC5_CC1 ((CONF_TC5_CC0 * CONF_TC5_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC5_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TC5_CC0 ((uint32_t)(((double)CONF_TC5_WAVE_PER_VAL * CONF_GCLK_TC5_FREQUENCY) / 1024000000 - 1))
#define CONF_TC5_CC1 ((CONF_TC5_CC0 * CONF_TC5_WAVE_DUTY_VAL) / 1000)
#endif

// </h>

// <h> Advanced settings
// <y> Mode
// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
// <i> These bits mode
// <id> tc_mode
#ifndef CONF_TC5_MODE
#define CONF_TC5_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

/*  Unused in 16/32 bit PWM mode */
#ifndef CONF_TC5_PER
#define CONF_TC5_PER 0x32
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC5_PRESCSYNC
#define CONF_TC5_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the will continue running in standby sleep mode or not
// <id> tc_arch_runstdby
#ifndef CONF_TC5_RUNSTDBY
#define CONF_TC5_RUNSTDBY 0
#endif

// <q> On-Demand
// <i> Indicates whether the TC5's on-demand mode is on or not
// <id> tc_arch_ondemand
#ifndef CONF_TC5_ONDEMAND
#define CONF_TC5_ONDEMAND 0
#endif

// <o> Auto Lock
// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
// <id> tc_arch_alock
#ifndef CONF_TC5_ALOCK
#define CONF_TC5_ALOCK 0
#endif

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC5_CAPTEN0 0
//#define CONF_TC5_CAPTEN1 0
//#define CONF_TC5_COPEN0  0
//#define CONF_TC5_COPEN1  0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC5_DIR     0
//#define CONF_TC5_ONESHOT 0
//#define CONF_TC5_LUPD    0

// <q> Debug Running Mode
// <i> Indicates whether the Debug Running Mode is enabled or not
// <id> tc_arch_dbgrun
#ifndef CONF_TC5_DBGRUN
#define CONF_TC5_DBGRUN 0
#endif

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC5_EVENT_CONTROL_ENABLE
#define CONF_TC5_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC5_MCEO0
#define CONF_TC5_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC5_MCEO1
#define CONF_TC5_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC5_OVFEO
#define CONF_TC5_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC5_TCEI
#define CONF_TC5_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC5_TCINV
#define CONF_TC5_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC5_EVACT
#define CONF_TC5_EVACT 0
#endif
// </e>

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC5_WAVEGEN   TC_CTRLA_WAVEGEN_MFRQ_Val

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC5_INVEN0 0
//#define CONF_TC5_INVEN1 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC5_PERBUF 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC5_CCBUF0 0
//#define CONF_TC5_CCBUF1 0

// </h>

// <<< end of configuration section >>>
// </h>

//// <h> Advanced settings
//// <y> Mode
//// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
//// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
//// <i> These bits mode
//// <id> tc_mode
//#ifndef CONF_TC0_MODE
//#define CONF_TC0_MODE TC_CTRLA_MODE_COUNT16_Val
//#endif

///*  Unused in 16/32 bit PWM mode */
//#ifndef CONF_TC0_PER
//#define CONF_TC0_PER 0x32
//#endif

//// <y> Prescaler and Counter Synchronization Selection
//// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
//// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
//// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
//// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
//// <id> tc_arch_presync
//#ifndef CONF_TC0_PRESCSYNC
//#define CONF_TC0_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
//#endif

//// <q> Run in standby
//// <i> Indicates whether the will continue running in standby sleep mode or not
//// <id> tc_arch_runstdby
//#ifndef CONF_TC0_RUNSTDBY
//#define CONF_TC0_RUNSTDBY 0
//#endif

//// <q> On-Demand
//// <i> Indicates whether the TC0's on-demand mode is on or not
//// <id> tc_arch_ondemand
//#ifndef CONF_TC0_ONDEMAND
//#define CONF_TC0_ONDEMAND 0
//#endif

//// <o> Auto Lock
//// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
//// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
//// <id> tc_arch_alock
//#ifndef CONF_TC0_ALOCK
//#define CONF_TC0_ALOCK 0
//#endif

///* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
////#define CONF_TC0_CAPTEN0 0
////#define CONF_TC0_CAPTEN1 0
////#define CONF_TC0_COPEN0  0
////#define CONF_TC0_COPEN1  0

///* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
////#define CONF_TC0_DIR     0
////#define CONF_TC0_ONESHOT 0
////#define CONF_TC0_LUPD    0

//// <q> Debug Running Mode
//// <i> Indicates whether the Debug Running Mode is enabled or not
//// <id> tc_arch_dbgrun
//#ifndef CONF_TC0_DBGRUN
//#define CONF_TC0_DBGRUN 0
//#endif

//// <e> Event control
//// <id> timer_event_control
//#ifndef CONF_TC0_EVENT_CONTROL_ENABLE
//#define CONF_TC0_EVENT_CONTROL_ENABLE 0
//#endif

//// <q> Output Event On Match or Capture on Channel 0
//// <i> Enable output of event on timer tick
//// <id> tc_arch_mceo0
//#ifndef CONF_TC0_MCEO0
//#define CONF_TC0_MCEO0 0
//#endif

//// <q> Output Event On Match or Capture on Channel 1
//// <i> Enable output of event on timer tick
//// <id> tc_arch_mceo1
//#ifndef CONF_TC0_MCEO1
//#define CONF_TC0_MCEO1 0
//#endif

//// <q> Output Event On Timer Tick
//// <i> Enable output of event on timer tick
//// <id> tc_arch_ovfeo
//#ifndef CONF_TC0_OVFEO
//#define CONF_TC0_OVFEO 0
//#endif

//// <q> Event Input
//// <i> Enable asynchronous input events
//// <id> tc_arch_tcei
//#ifndef CONF_TC0_TCEI
//#define CONF_TC0_TCEI 0
//#endif

//// <q> Inverted Event Input
//// <i> Invert the asynchronous input events
//// <id> tc_arch_tcinv
//#ifndef CONF_TC0_TCINV
//#define CONF_TC0_TCINV 0
//#endif

//// <o> Event action
//// <0=> Event action disabled
//// <1=> Start, restart or re-trigger TC on event
//// <2=> Count on event
//// <3=> Start on event
//// <4=> Time stamp capture
//// <5=> Period captured in CC0, pulse width in CC1
//// <6=> Period captured in CC1, pulse width in CC0
//// <7=> Pulse width capture
//// <i> Event which will be performed on an event
////<id> tc_arch_evact
//#ifndef CONF_TC0_EVACT
//#define CONF_TC0_EVACT 0
//#endif
//// </e>

///* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
////#define CONF_TC0_WAVEGEN   TC_CTRLA_WAVEGEN_MFRQ_Val

///* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
////#define CONF_TC0_INVEN0 0
////#define CONF_TC0_INVEN1 0

///* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
////#define CONF_TC0_PERBUF 0

///* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
////#define CONF_TC0_CCBUF0 0
////#define CONF_TC0_CCBUF1 0

//// </h>

#include "peripheral_clk_config.h"

#ifndef CONF_TC6_ENABLE
#define CONF_TC6_ENABLE 1
#endif

// <h> Basic settings
// <o> Prescaler
// <0=> No division
// <1=> Divide by 2
// <2=> Divide by 4
// <3=> Divide by 8
// <4=> Divide by 16
// <5=> Divide by 64
// <6=> Divide by 256
// <7=> Divide by 1024
// <i> This defines the prescaler value
// <id> tc_prescaler
#ifndef CONF_TC6_PRESCALER
#define CONF_TC6_PRESCALER 3
#endif
// </h>

// <h> PWM Waveform Output settings
// <o> Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tc_arch_wave_per_val
#ifndef CONF_TC6_WAVE_PER_VAL
#define CONF_TC6_WAVE_PER_VAL 0x3e8
#endif

// <o> Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tc_arch_wave_duty_val
#ifndef CONF_TC6_WAVE_DUTY_VAL
#define CONF_TC6_WAVE_DUTY_VAL 0x1f4
#endif

/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TC6_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC6_CC0                                                                                                   \
	((uint32_t)(((double)CONF_TC6_WAVE_PER_VAL * CONF_GCLK_TC6_FREQUENCY) / 1000000 / (1 << CONF_TC6_PRESCALER) - 1))
#define CONF_TC6_CC1 ((CONF_TC6_CC0 * CONF_TC6_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC6_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC6_CC0 ((uint32_t)(((double)CONF_TC6_WAVE_PER_VAL * CONF_GCLK_TC6_FREQUENCY) / 64000000 - 1))
#define CONF_TC6_CC1 ((CONF_TC6_CC0 * CONF_TC6_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC6_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TC6_CC0 ((uint32_t)(((double)CONF_TC6_WAVE_PER_VAL * CONF_GCLK_TC6_FREQUENCY) / 256000000 - 1))
#define CONF_TC6_CC1 ((CONF_TC6_CC0 * CONF_TC6_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC6_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TC6_CC0 ((uint32_t)(((double)CONF_TC6_WAVE_PER_VAL * CONF_GCLK_TC6_FREQUENCY) / 1024000000 - 1))
#define CONF_TC6_CC1 ((CONF_TC6_CC0 * CONF_TC6_WAVE_DUTY_VAL) / 1000)
#endif

// </h>

// <h> Advanced settings
// <y> Mode
// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
// <i> These bits mode
// <id> tc_mode
#ifndef CONF_TC6_MODE
#define CONF_TC6_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

/*  Unused in 16/32 bit PWM mode */
#ifndef CONF_TC6_PER
#define CONF_TC6_PER 0x32
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC6_PRESCSYNC
#define CONF_TC6_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the will continue running in standby sleep mode or not
// <id> tc_arch_runstdby
#ifndef CONF_TC6_RUNSTDBY
#define CONF_TC6_RUNSTDBY 0
#endif

// <q> On-Demand
// <i> Indicates whether the TC6's on-demand mode is on or not
// <id> tc_arch_ondemand
#ifndef CONF_TC6_ONDEMAND
#define CONF_TC6_ONDEMAND 0
#endif

// <o> Auto Lock
// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
// <id> tc_arch_alock
#ifndef CONF_TC6_ALOCK
#define CONF_TC6_ALOCK 0
#endif

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC6_CAPTEN0 0
//#define CONF_TC6_CAPTEN1 0
//#define CONF_TC6_COPEN0  0
//#define CONF_TC6_COPEN1  0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC6_DIR     0
//#define CONF_TC6_ONESHOT 0
//#define CONF_TC6_LUPD    0

// <q> Debug Running Mode
// <i> Indicates whether the Debug Running Mode is enabled or not
// <id> tc_arch_dbgrun
#ifndef CONF_TC6_DBGRUN
#define CONF_TC6_DBGRUN 0
#endif

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC6_EVENT_CONTROL_ENABLE
#define CONF_TC6_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC6_MCEO0
#define CONF_TC6_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC6_MCEO1
#define CONF_TC6_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC6_OVFEO
#define CONF_TC6_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC6_TCEI
#define CONF_TC6_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC6_TCINV
#define CONF_TC6_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC6_EVACT
#define CONF_TC6_EVACT 0
#endif
// </e>

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC6_WAVEGEN   TC_CTRLA_WAVEGEN_MFRQ_Val

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC6_INVEN0 0
//#define CONF_TC6_INVEN1 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC6_PERBUF 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC6_CCBUF0 0
//#define CONF_TC6_CCBUF1 0

// </h>

#include "peripheral_clk_config.h"

#ifndef CONF_TC7_ENABLE
#define CONF_TC7_ENABLE 1
#endif

// <h> Basic settings
// <o> Prescaler
// <0=> No division
// <1=> Divide by 2
// <2=> Divide by 4
// <3=> Divide by 8
// <4=> Divide by 16
// <5=> Divide by 64
// <6=> Divide by 256
// <7=> Divide by 1024
// <i> This defines the prescaler value
// <id> tc_prescaler
#ifndef CONF_TC7_PRESCALER
#define CONF_TC7_PRESCALER 3
#endif
// </h>

// <h> PWM Waveform Output settings
// <o> Waveform Period Value (uS) <0x00-0xFFFFFFFF>
// <i> The unit of this value is us.
// <id> tc_arch_wave_per_val
#ifndef CONF_TC7_WAVE_PER_VAL
#define CONF_TC7_WAVE_PER_VAL 0x3e8
#endif

// <o> Waveform Duty Value (0.1%) <0x00-0x03E8>
// <i> The unit of this value is 1/1000.
// <id> tc_arch_wave_duty_val
#ifndef CONF_TC7_WAVE_DUTY_VAL
#define CONF_TC7_WAVE_DUTY_VAL 0x1f4
#endif

/* Caculate pwm ccx register value based on WAVE_PER_VAL and Waveform Duty Value */
#if CONF_TC7_PRESCALER < TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC7_CC0                                                                                                   \
	((uint32_t)(((double)CONF_TC7_WAVE_PER_VAL * CONF_GCLK_TC7_FREQUENCY) / 1000000 / (1 << CONF_TC7_PRESCALER) - 1))
#define CONF_TC7_CC1 ((CONF_TC7_CC0 * CONF_TC7_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC7_PRESCALER == TC_CTRLA_PRESCALER_DIV64_Val
#define CONF_TC7_CC0 ((uint32_t)(((double)CONF_TC7_WAVE_PER_VAL * CONF_GCLK_TC7_FREQUENCY) / 64000000 - 1))
#define CONF_TC7_CC1 ((CONF_TC7_CC0 * CONF_TC7_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC7_PRESCALER == TC_CTRLA_PRESCALER_DIV256_Val
#define CONF_TC7_CC0 ((uint32_t)(((double)CONF_TC7_WAVE_PER_VAL * CONF_GCLK_TC7_FREQUENCY) / 256000000 - 1))
#define CONF_TC7_CC1 ((CONF_TC7_CC0 * CONF_TC7_WAVE_DUTY_VAL) / 1000)

#elif CONF_TC7_PRESCALER == TC_CTRLA_PRESCALER_DIV1024_Val
#define CONF_TC7_CC0 ((uint32_t)(((double)CONF_TC7_WAVE_PER_VAL * CONF_GCLK_TC7_FREQUENCY) / 1024000000 - 1))
#define CONF_TC7_CC1 ((CONF_TC7_CC0 * CONF_TC7_WAVE_DUTY_VAL) / 1000)
#endif

// </h>

// <h> Advanced settings
// <y> Mode
// <TC_CTRLA_MODE_COUNT16_Val"> Counter in 16-bit mode
// <TC_CTRLA_MODE_COUNT32_Val"> Counter in 32-bit mode
// <i> These bits mode
// <id> tc_mode
#ifndef CONF_TC7_MODE
#define CONF_TC7_MODE TC_CTRLA_MODE_COUNT16_Val
#endif

/*  Unused in 16/32 bit PWM mode */
#ifndef CONF_TC7_PER
#define CONF_TC7_PER 0x32
#endif

// <y> Prescaler and Counter Synchronization Selection
// <TC_CTRLA_PRESCSYNC_GCLK_Val"> Reload or reset counter on next GCLK
// <TC_CTRLA_PRESCSYNC_PRESC_Val"> Reload or reset counter on next prescaler clock
// <TC_CTRLA_PRESCSYNC_RESYNC_Val"> Reload or reset counter on next GCLK and reset prescaler counter
// <i> These bits select if on retrigger event, the Counter should be cleared or reloaded on the next GCLK_TCx clock or on the next prescaled GCLK_TCx clock.
// <id> tc_arch_presync
#ifndef CONF_TC7_PRESCSYNC
#define CONF_TC7_PRESCSYNC TC_CTRLA_PRESCSYNC_GCLK_Val
#endif

// <q> Run in standby
// <i> Indicates whether the will continue running in standby sleep mode or not
// <id> tc_arch_runstdby
#ifndef CONF_TC7_RUNSTDBY
#define CONF_TC7_RUNSTDBY 0
#endif

// <q> On-Demand
// <i> Indicates whether the TC7's on-demand mode is on or not
// <id> tc_arch_ondemand
#ifndef CONF_TC7_ONDEMAND
#define CONF_TC7_ONDEMAND 0
#endif

// <o> Auto Lock
// <0x0=>The Lock Update bit is not affected on overflow/underflow and re-trigger event
// <0x1=>The Lock Update bit is set on each overflow/underflow or re-trigger event
// <id> tc_arch_alock
#ifndef CONF_TC7_ALOCK
#define CONF_TC7_ALOCK 0
#endif

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC7_CAPTEN0 0
//#define CONF_TC7_CAPTEN1 0
//#define CONF_TC7_COPEN0  0
//#define CONF_TC7_COPEN1  0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC7_DIR     0
//#define CONF_TC7_ONESHOT 0
//#define CONF_TC7_LUPD    0

// <q> Debug Running Mode
// <i> Indicates whether the Debug Running Mode is enabled or not
// <id> tc_arch_dbgrun
#ifndef CONF_TC7_DBGRUN
#define CONF_TC7_DBGRUN 0
#endif

// <e> Event control
// <id> timer_event_control
#ifndef CONF_TC7_EVENT_CONTROL_ENABLE
#define CONF_TC7_EVENT_CONTROL_ENABLE 0
#endif

// <q> Output Event On Match or Capture on Channel 0
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo0
#ifndef CONF_TC7_MCEO0
#define CONF_TC7_MCEO0 0
#endif

// <q> Output Event On Match or Capture on Channel 1
// <i> Enable output of event on timer tick
// <id> tc_arch_mceo1
#ifndef CONF_TC7_MCEO1
#define CONF_TC7_MCEO1 0
#endif

// <q> Output Event On Timer Tick
// <i> Enable output of event on timer tick
// <id> tc_arch_ovfeo
#ifndef CONF_TC7_OVFEO
#define CONF_TC7_OVFEO 0
#endif

// <q> Event Input
// <i> Enable asynchronous input events
// <id> tc_arch_tcei
#ifndef CONF_TC7_TCEI
#define CONF_TC7_TCEI 0
#endif

// <q> Inverted Event Input
// <i> Invert the asynchronous input events
// <id> tc_arch_tcinv
#ifndef CONF_TC7_TCINV
#define CONF_TC7_TCINV 0
#endif

// <o> Event action
// <0=> Event action disabled
// <1=> Start, restart or re-trigger TC on event
// <2=> Count on event
// <3=> Start on event
// <4=> Time stamp capture
// <5=> Period captured in CC0, pulse width in CC1
// <6=> Period captured in CC1, pulse width in CC0
// <7=> Pulse width capture
// <i> Event which will be performed on an event
//<id> tc_arch_evact
#ifndef CONF_TC7_EVACT
#define CONF_TC7_EVACT 0
#endif
// </e>

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC7_WAVEGEN   TC_CTRLA_WAVEGEN_MFRQ_Val

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC7_INVEN0 0
//#define CONF_TC7_INVEN1 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC7_PERBUF 0

/* Commented intentionally. Timer uses fixed value. May be used by other abstractions based on TC. */
//#define CONF_TC7_CCBUF0 0
//#define CONF_TC7_CCBUF1 0

//// <<< end of configuration section >>>

#endif // HPL_TC_CONFIG_H
