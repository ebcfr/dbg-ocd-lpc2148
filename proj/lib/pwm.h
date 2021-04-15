#ifndef _PWM_H_
#define _PWM_H_

#include "board.h"
#include "timer.h"

/* pwm_init
 *   setup pwm timer period, each tick_ms
 */
int pwm_init(PWM *pwm, uint32_t period_ms, OnTick cb);

/* pwm_channel_enable
 *   set up pwm channel
 */
int pwm_channel_enable(PWM *pwm, uint32_t channel, uint32_t dutycycle, uint32_t oe, uint32_t dbledge);

/* pwm_channel_disable
 *   set up pwm channel
 */
int pwm_channel_disable(PWM *pwm, uint32_t channel);

/* pwm_channel_set
 *   set up dutycycle for pwm channel
 */
int pwm_channel_set(PWM *pwm, uint32_t channel, uint32_t dutycycle);

/* pwm_start
 *   start counting
 */
void pwm_start(PWM *pwm);

/* pwm_stop
 *   stop and reset counting
 */
void pwm_stop(PWM *pwm);

#endif
