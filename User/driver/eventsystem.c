#include <stdint.h>
#include "../../Device/instance/evsys.h"
#include "config/peripheral_clk_config.h"
#include "../../Device/samc21n18a.h"
#include "./config/hpl_evsys_config.h"

#define EVSYS_CHANNEL_EDGSEL(value) (EVSYS_CHANNEL_EDGSEL_Msk & ((value) << EVSYS_CHANNEL_EDGSEL_Pos))
#define EVSYS_CHANNEL_PATH(value) (EVSYS_CHANNEL_PATH_Msk & ((value) << EVSYS_CHANNEL_PATH_Pos))
#define EVSYS_CHANNEL_EVGEN(value) (EVSYS_CHANNEL_EVGEN_Msk & ((value) << EVSYS_CHANNEL_EVGEN_Pos))
#define EVSYS_USER_CHANNEL(value) (EVSYS_USER_CHANNEL_Msk & ((value) << EVSYS_USER_CHANNEL_Pos))
/* Event user configuration */
#define USER_MUX_CONF(n) EVSYS_USER_CHANNEL(CONF_CHANNEL_##n)

/* This macro is used for repeat macro: i - unused, n - amount of channels.
 * It contains channel configuration. */
#define CHANNEL_CONF(n) \
    (CONF_ONDEMAND_##n << EVSYS_CHANNEL_ONDEMAND_Pos) | (CONF_RUNSTDBY_##n << EVSYS_CHANNEL_RUNSTDBY_Pos) | EVSYS_CHANNEL_EDGSEL(CONF_EDGSEL_##n) | EVSYS_CHANNEL_PATH(CONF_PATH_##n) | EVSYS_CHANNEL_EVGEN(CONF_EVGEN_##n)

/* This macro is used for repeat macro: i - unused, n - amount of channels
 * It contains interrupts configuration. */
#define INT_CFG(n) \
    (CONF_OVR_##n << (n + EVSYS_INTENSET_OVR0_Pos)) | (CONF_EVD_##n << (n + EVSYS_INTENSET_EVD0_Pos))

void EVSYS_init(void)
{
    GCLK->PCHCTRL[EVSYS_GCLK_ID_2].reg = CONF_GCLK_EVSYS_CHANNEL_2_SRC | (1 << GCLK_PCHCTRL_CHEN_Pos);

    MCLK->APBCMASK.reg |= MCLK_APBCMASK_EVSYS;

    

    /* configure user multiplexers */
    
    /*
    uint8_t i;
	for (i = 0; i < EVSYS_USERS; i++) {
        EVSYS->USER[i].reg = user_mux_confs[i];
	}
    */
    EVSYS->USER[0].reg = USER_MUX_CONF(0);
    EVSYS->USER[1].reg = USER_MUX_CONF(1);
    EVSYS->USER[2].reg = USER_MUX_CONF(2);
    EVSYS->USER[3].reg = USER_MUX_CONF(3);
    EVSYS->USER[4].reg = USER_MUX_CONF(4);
    EVSYS->USER[5].reg = USER_MUX_CONF(5);
    EVSYS->USER[6].reg = USER_MUX_CONF(6);
    EVSYS->USER[7].reg = USER_MUX_CONF(7);
    EVSYS->USER[8].reg = USER_MUX_CONF(8);
    EVSYS->USER[9].reg = USER_MUX_CONF(9);
    EVSYS->USER[10].reg = USER_MUX_CONF(10);
    EVSYS->USER[11].reg = USER_MUX_CONF(11);
    EVSYS->USER[12].reg = USER_MUX_CONF(12);
    EVSYS->USER[13].reg = USER_MUX_CONF(13);
    EVSYS->USER[14].reg = USER_MUX_CONF(14);
    EVSYS->USER[15].reg = USER_MUX_CONF(15);
    EVSYS->USER[16].reg = USER_MUX_CONF(16);
    EVSYS->USER[17].reg = USER_MUX_CONF(17);
    EVSYS->USER[18].reg = USER_MUX_CONF(18);
    EVSYS->USER[19].reg = USER_MUX_CONF(19);
    EVSYS->USER[20].reg = USER_MUX_CONF(20);
    EVSYS->USER[21].reg = USER_MUX_CONF(21);
    EVSYS->USER[22].reg = USER_MUX_CONF(22);
    EVSYS->USER[23].reg = USER_MUX_CONF(23);
    EVSYS->USER[24].reg = USER_MUX_CONF(24);
    EVSYS->USER[25].reg = USER_MUX_CONF(25);
    EVSYS->USER[26].reg = USER_MUX_CONF(26);
    EVSYS->USER[27].reg = USER_MUX_CONF(27);
    EVSYS->USER[28].reg = USER_MUX_CONF(28);
    EVSYS->USER[29].reg = USER_MUX_CONF(29);
    EVSYS->USER[30].reg = USER_MUX_CONF(30);
    EVSYS->USER[31].reg = USER_MUX_CONF(31);
    EVSYS->USER[32].reg = USER_MUX_CONF(32);
    EVSYS->USER[33].reg = USER_MUX_CONF(33);
    EVSYS->USER[34].reg = USER_MUX_CONF(34);
    EVSYS->USER[35].reg = USER_MUX_CONF(35);
    EVSYS->USER[36].reg = USER_MUX_CONF(36);
    EVSYS->USER[37].reg = USER_MUX_CONF(37);
    EVSYS->USER[38].reg = USER_MUX_CONF(38);
    EVSYS->USER[39].reg = USER_MUX_CONF(39);
    EVSYS->USER[40].reg = USER_MUX_CONF(40);
    EVSYS->USER[41].reg = USER_MUX_CONF(41);
    EVSYS->USER[42].reg = USER_MUX_CONF(42);
    EVSYS->USER[43].reg = USER_MUX_CONF(43);
    EVSYS->USER[44].reg = USER_MUX_CONF(44);
    EVSYS->USER[45].reg = USER_MUX_CONF(45);
    EVSYS->USER[46].reg = USER_MUX_CONF(46);
    EVSYS->USER[47].reg = USER_MUX_CONF(47);
    EVSYS->USER[48].reg = USER_MUX_CONF(48);
    EVSYS->USER[49].reg = USER_MUX_CONF(49);

    /* configure channels */
    /*
	for (i = 0; i < EVSYS_CHANNELS; i++) {
        EVSYS->CHANNEL[i].reg = channel_confs[i];
	}
    */
    EVSYS->CHANNEL[0].reg = CHANNEL_CONF(0);
    EVSYS->CHANNEL[1].reg = CHANNEL_CONF(1);
    EVSYS->CHANNEL[2].reg = CHANNEL_CONF(2);
    EVSYS->CHANNEL[3].reg = CHANNEL_CONF(3);
    EVSYS->CHANNEL[4].reg = CHANNEL_CONF(4);
    EVSYS->CHANNEL[5].reg = CHANNEL_CONF(5);
    EVSYS->CHANNEL[6].reg = CHANNEL_CONF(6);
    EVSYS->CHANNEL[7].reg = CHANNEL_CONF(7);
    EVSYS->CHANNEL[8].reg = CHANNEL_CONF(8);
    EVSYS->CHANNEL[9].reg = CHANNEL_CONF(9);
    EVSYS->CHANNEL[10].reg = CHANNEL_CONF(10);
    EVSYS->CHANNEL[11].reg = CHANNEL_CONF(11);

    EVSYS->INTENSET.reg = INT_CFG(0);
    EVSYS->INTENCLR.reg = ~INT_CFG(0);
}