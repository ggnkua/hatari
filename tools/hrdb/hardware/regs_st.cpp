/* Autogenerated file */

#include "regs_st.h"

namespace Regs {
    
    const char* GetString(const StringDef* strings, uint16_t value)
    {
        while (strings->string)
        {
            if (value == strings->value)
                return strings->string;
            ++strings;
        }
        return "?";
    }

const char* GetString(MMU_BANK val)
{
	if (val==MMU_BANK::SIZE_128K) return "128K";
	if (val==MMU_BANK::SIZE_512K) return "512K";
	if (val==MMU_BANK::SIZE_2MB) return "2MB";
	if (val==MMU_BANK::SIZE_RESERVED) return "reserved";
	return "?";
}
const char* GetString(RESOLUTION val)
{
	if (val==RESOLUTION::LOW) return "ST-Low";
	if (val==RESOLUTION::MEDIUM) return "ST-Medium";
	if (val==RESOLUTION::HIGH) return "ST-High";
	return "?";
}
const char* GetString(SYNC_RATE val)
{
	if (val==SYNC_RATE::HZ_60) return "60Hz";
	if (val==SYNC_RATE::HZ_50) return "50Hz";
	return "?";
}
const char* GetString(SYNC_TYPE val)
{
	if (val==SYNC_TYPE::INTERNAL) return "Internal Sync";
	if (val==SYNC_TYPE::EXTERNAL) return "External Sync";
	return "?";
}
const char* GetString(TIMER_MODE_A val)
{
	if (val==TIMER_MODE_A::STOP) return "Stopped";
	if (val==TIMER_MODE_A::DELAY_4) return "Delay mode, divide by 4";
	if (val==TIMER_MODE_A::DELAY_10) return "Delay mode, divide by 10";
	if (val==TIMER_MODE_A::DELAY_16) return "Delay mode, divide by 16";
	if (val==TIMER_MODE_A::DELAY_50) return "Delay mode, divide by 50";
	if (val==TIMER_MODE_A::DELAY_64) return "Delay mode, divide by 64";
	if (val==TIMER_MODE_A::DELAY_100) return "Delay mode, divide by 100";
	if (val==TIMER_MODE_A::DELAY_200) return "Delay mode, divide by 200";
	if (val==TIMER_MODE_A::EVENT) return "Event count mode";
	if (val==TIMER_MODE_A::PULSE_4) return "Pulse extension mode, divide by 4";
	if (val==TIMER_MODE_A::PULSE_10) return "Pulse extension mode, divide by 10";
	if (val==TIMER_MODE_A::PULSE_16) return "Pulse extension mode, divide by 16";
	if (val==TIMER_MODE_A::PULSE_50) return "Pulse extension mode, divide by 50";
	if (val==TIMER_MODE_A::PULSE_64) return "Pulse extension mode, divide by 64";
	if (val==TIMER_MODE_A::PULSE_100) return "Pulse extension mode, divide by 100";
	if (val==TIMER_MODE_A::PULSE_200) return "Pulse extension mode, divide by 200";
	return "?";
}
const char* GetString(TIMER_MODE_B val)
{
	if (val==TIMER_MODE_B::STOP) return "Stopped";
	if (val==TIMER_MODE_B::DELAY_4) return "Delay mode, divide by 4";
	if (val==TIMER_MODE_B::DELAY_10) return "Delay mode, divide by 10";
	if (val==TIMER_MODE_B::DELAY_16) return "Delay mode, divide by 16";
	if (val==TIMER_MODE_B::DELAY_50) return "Delay mode, divide by 50";
	if (val==TIMER_MODE_B::DELAY_64) return "Delay mode, divide by 64";
	if (val==TIMER_MODE_B::DELAY_100) return "Delay mode, divide by 100";
	if (val==TIMER_MODE_B::DELAY_200) return "Delay mode, divide by 200";
	if (val==TIMER_MODE_B::EVENT) return "Event count mode (HBL events)";
	if (val==TIMER_MODE_B::PULSE_4) return "Pulse extension mode, divide by 4";
	if (val==TIMER_MODE_B::PULSE_10) return "Pulse extension mode, divide by 10";
	if (val==TIMER_MODE_B::PULSE_16) return "Pulse extension mode, divide by 16";
	if (val==TIMER_MODE_B::PULSE_50) return "Pulse extension mode, divide by 50";
	if (val==TIMER_MODE_B::PULSE_64) return "Pulse extension mode, divide by 64";
	if (val==TIMER_MODE_B::PULSE_100) return "Pulse extension mode, divide by 100";
	if (val==TIMER_MODE_B::PULSE_200) return "Pulse extension mode, divide by 200";
	return "?";
}
const char* GetString(TIMER_MODE_CD val)
{
	if (val==TIMER_MODE_CD::STOP) return "Stopped";
	if (val==TIMER_MODE_CD::DELAY_4) return "Delay mode, divide by 4";
	if (val==TIMER_MODE_CD::DELAY_10) return "Delay mode, divide by 10";
	if (val==TIMER_MODE_CD::DELAY_16) return "Delay mode, divide by 16";
	if (val==TIMER_MODE_CD::DELAY_50) return "Delay mode, divide by 50";
	if (val==TIMER_MODE_CD::DELAY_64) return "Delay mode, divide by 64";
	if (val==TIMER_MODE_CD::DELAY_100) return "Delay mode, divide by 100";
	if (val==TIMER_MODE_CD::DELAY_200) return "Delay mode, divide by 200";
	return "?";
}
const char* GetString(ENDINT val)
{
	if (val==ENDINT::AUTO) return "Automatic";
	if (val==ENDINT::SOFTWARE) return "Software";
	return "?";
}
const char* GetString(USARTLEN val)
{
	if (val==USARTLEN::LEN_8) return "8 Bits";
	if (val==USARTLEN::LEN_7) return "7 Bits";
	if (val==USARTLEN::LEN_6) return "6 Bits";
	if (val==USARTLEN::LEN_5) return "5 Bits";
	return "?";
}
const StringDef g_enumStringsMMU_BANK[] = {
	{ 0, "128K" },
	{ 1, "512K" },
	{ 2, "2MB" },
	{ 3, "reserved" },
	{ 0, nullptr }
};
const StringDef g_enumStringsRESOLUTION[] = {
	{ 0, "ST-Low" },
	{ 1, "ST-Medium" },
	{ 2, "ST-High" },
	{ 0, nullptr }
};
const StringDef g_enumStringsSYNC_RATE[] = {
	{ 0, "60Hz" },
	{ 1, "50Hz" },
	{ 0, nullptr }
};
const StringDef g_enumStringsSYNC_TYPE[] = {
	{ 0, "Internal Sync" },
	{ 1, "External Sync" },
	{ 0, nullptr }
};
const StringDef g_enumStringsTIMER_MODE_A[] = {
	{ 0, "Stopped" },
	{ 1, "Delay mode, divide by 4" },
	{ 2, "Delay mode, divide by 10" },
	{ 3, "Delay mode, divide by 16" },
	{ 4, "Delay mode, divide by 50" },
	{ 5, "Delay mode, divide by 64" },
	{ 6, "Delay mode, divide by 100" },
	{ 7, "Delay mode, divide by 200" },
	{ 8, "Event count mode" },
	{ 9, "Pulse extension mode, divide by 4" },
	{ 10, "Pulse extension mode, divide by 10" },
	{ 11, "Pulse extension mode, divide by 16" },
	{ 12, "Pulse extension mode, divide by 50" },
	{ 13, "Pulse extension mode, divide by 64" },
	{ 14, "Pulse extension mode, divide by 100" },
	{ 15, "Pulse extension mode, divide by 200" },
	{ 0, nullptr }
};
const StringDef g_enumStringsTIMER_MODE_B[] = {
	{ 0, "Stopped" },
	{ 1, "Delay mode, divide by 4" },
	{ 2, "Delay mode, divide by 10" },
	{ 3, "Delay mode, divide by 16" },
	{ 4, "Delay mode, divide by 50" },
	{ 5, "Delay mode, divide by 64" },
	{ 6, "Delay mode, divide by 100" },
	{ 7, "Delay mode, divide by 200" },
	{ 8, "Event count mode (HBL events)" },
	{ 9, "Pulse extension mode, divide by 4" },
	{ 10, "Pulse extension mode, divide by 10" },
	{ 11, "Pulse extension mode, divide by 16" },
	{ 12, "Pulse extension mode, divide by 50" },
	{ 13, "Pulse extension mode, divide by 64" },
	{ 14, "Pulse extension mode, divide by 100" },
	{ 15, "Pulse extension mode, divide by 200" },
	{ 0, nullptr }
};
const StringDef g_enumStringsTIMER_MODE_CD[] = {
	{ 0, "Stopped" },
	{ 1, "Delay mode, divide by 4" },
	{ 2, "Delay mode, divide by 10" },
	{ 3, "Delay mode, divide by 16" },
	{ 4, "Delay mode, divide by 50" },
	{ 5, "Delay mode, divide by 64" },
	{ 6, "Delay mode, divide by 100" },
	{ 7, "Delay mode, divide by 200" },
	{ 0, nullptr }
};
const StringDef g_enumStringsENDINT[] = {
	{ 0, "Automatic" },
	{ 1, "Software" },
	{ 0, nullptr }
};
const StringDef g_enumStringsUSARTLEN[] = {
	{ 0, "8 Bits" },
	{ 1, "7 Bits" },
	{ 2, "6 Bits" },
	{ 3, "5 Bits" },
	{ 0, nullptr }
};
const FieldDef g_fieldDef_MMU_CONFIG_BANK1 = { Regs::MMU_CONFIG, "BANK1", Regs::MMU_CONFIG_BANK1_SHIFT, Regs::MMU_CONFIG_BANK1_MASK, g_enumStringsMMU_BANK };
const FieldDef g_fieldDef_MMU_CONFIG_BANK0 = { Regs::MMU_CONFIG, "BANK0", Regs::MMU_CONFIG_BANK0_SHIFT, Regs::MMU_CONFIG_BANK0_MASK, g_enumStringsMMU_BANK };
const FieldDef g_fieldDef_VID_SCREEN_HIGH_ALL = { Regs::VID_SCREEN_HIGH, "ALL", Regs::VID_SCREEN_HIGH_ALL_SHIFT, Regs::VID_SCREEN_HIGH_ALL_MASK, nullptr };
const FieldDef g_fieldDef_VID_SCREEN_MID_ALL = { Regs::VID_SCREEN_MID, "ALL", Regs::VID_SCREEN_MID_ALL_SHIFT, Regs::VID_SCREEN_MID_ALL_MASK, nullptr };
const FieldDef g_fieldDef_VID_SCREEN_LOW_STE_ALL = { Regs::VID_SCREEN_LOW_STE, "ALL", Regs::VID_SCREEN_LOW_STE_ALL_SHIFT, Regs::VID_SCREEN_LOW_STE_ALL_MASK, nullptr };
const FieldDef g_fieldDef_VID_SCANLINE_OFFSET_STE_ALL = { Regs::VID_SCANLINE_OFFSET_STE, "ALL", Regs::VID_SCANLINE_OFFSET_STE_ALL_SHIFT, Regs::VID_SCANLINE_OFFSET_STE_ALL_MASK, nullptr };
const FieldDef g_fieldDef_VID_HORIZ_SCROLL_STE_PIXELS = { Regs::VID_HORIZ_SCROLL_STE, "PIXELS", Regs::VID_HORIZ_SCROLL_STE_PIXELS_SHIFT, Regs::VID_HORIZ_SCROLL_STE_PIXELS_MASK, nullptr };
const FieldDef g_fieldDef_VID_SYNC_MODE_TYPE = { Regs::VID_SYNC_MODE, "TYPE", Regs::VID_SYNC_MODE_TYPE_SHIFT, Regs::VID_SYNC_MODE_TYPE_MASK, g_enumStringsSYNC_TYPE };
const FieldDef g_fieldDef_VID_SYNC_MODE_RATE = { Regs::VID_SYNC_MODE, "RATE", Regs::VID_SYNC_MODE_RATE_SHIFT, Regs::VID_SYNC_MODE_RATE_MASK, g_enumStringsSYNC_RATE };
const FieldDef g_fieldDef_VID_PAL_0_BLUE_ST = { Regs::VID_PAL_0, "BLUE_ST", Regs::VID_PAL_0_BLUE_ST_SHIFT, Regs::VID_PAL_0_BLUE_ST_MASK, nullptr };
const FieldDef g_fieldDef_VID_PAL_0_BLUE_STE = { Regs::VID_PAL_0, "BLUE_STE", Regs::VID_PAL_0_BLUE_STE_SHIFT, Regs::VID_PAL_0_BLUE_STE_MASK, nullptr };
const FieldDef g_fieldDef_VID_PAL_0_GREEN_ST = { Regs::VID_PAL_0, "GREEN_ST", Regs::VID_PAL_0_GREEN_ST_SHIFT, Regs::VID_PAL_0_GREEN_ST_MASK, nullptr };
const FieldDef g_fieldDef_VID_PAL_0_GREEN_STE = { Regs::VID_PAL_0, "GREEN_STE", Regs::VID_PAL_0_GREEN_STE_SHIFT, Regs::VID_PAL_0_GREEN_STE_MASK, nullptr };
const FieldDef g_fieldDef_VID_PAL_0_RED_ST = { Regs::VID_PAL_0, "RED_ST", Regs::VID_PAL_0_RED_ST_SHIFT, Regs::VID_PAL_0_RED_ST_MASK, nullptr };
const FieldDef g_fieldDef_VID_PAL_0_RED_STE = { Regs::VID_PAL_0, "RED_STE", Regs::VID_PAL_0_RED_STE_SHIFT, Regs::VID_PAL_0_RED_STE_MASK, nullptr };
const FieldDef g_fieldDef_VID_SHIFTER_RES_RES = { Regs::VID_SHIFTER_RES, "RES", Regs::VID_SHIFTER_RES_RES_SHIFT, Regs::VID_SHIFTER_RES_RES_MASK, g_enumStringsRESOLUTION };
const FieldDef g_fieldDef_FDC_ACCESS_ALL = { Regs::FDC_ACCESS, "ALL", Regs::FDC_ACCESS_ALL_SHIFT, Regs::FDC_ACCESS_ALL_MASK, nullptr };
const FieldDef g_fieldDef_FDC_STATUS_ERROR = { Regs::FDC_STATUS, "ERROR", Regs::FDC_STATUS_ERROR_SHIFT, Regs::FDC_STATUS_ERROR_MASK, nullptr };
const FieldDef g_fieldDef_FDC_STATUS_SECT_CNT = { Regs::FDC_STATUS, "SECT_CNT", Regs::FDC_STATUS_SECT_CNT_SHIFT, Regs::FDC_STATUS_SECT_CNT_MASK, nullptr };
const FieldDef g_fieldDef_FDC_STATUS_DATA_REQ = { Regs::FDC_STATUS, "DATA_REQ", Regs::FDC_STATUS_DATA_REQ_SHIFT, Regs::FDC_STATUS_DATA_REQ_MASK, nullptr };
const FieldDef g_fieldDef_FDC_DMA_BASE_HIGH_ALL = { Regs::FDC_DMA_BASE_HIGH, "ALL", Regs::FDC_DMA_BASE_HIGH_ALL_SHIFT, Regs::FDC_DMA_BASE_HIGH_ALL_MASK, nullptr };
const FieldDef g_fieldDef_FDC_DMA_BASE_MID_ALL = { Regs::FDC_DMA_BASE_MID, "ALL", Regs::FDC_DMA_BASE_MID_ALL_SHIFT, Regs::FDC_DMA_BASE_MID_ALL_MASK, nullptr };
const FieldDef g_fieldDef_FDC_DMA_BASE_LOW_ALL = { Regs::FDC_DMA_BASE_LOW, "ALL", Regs::FDC_DMA_BASE_LOW_ALL_SHIFT, Regs::FDC_DMA_BASE_LOW_ALL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_GPIP_ALL = { Regs::MFP_GPIP, "ALL", Regs::MFP_GPIP_ALL_SHIFT, Regs::MFP_GPIP_ALL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_AER_CENT_BUSY = { Regs::MFP_AER, "CENT_BUSY", Regs::MFP_AER_CENT_BUSY_SHIFT, Regs::MFP_AER_CENT_BUSY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_AER_RS232_DCD = { Regs::MFP_AER, "RS232_DCD", Regs::MFP_AER_RS232_DCD_SHIFT, Regs::MFP_AER_RS232_DCD_MASK, nullptr };
const FieldDef g_fieldDef_MFP_AER_RS232_CTS = { Regs::MFP_AER, "RS232_CTS", Regs::MFP_AER_RS232_CTS_SHIFT, Regs::MFP_AER_RS232_CTS_MASK, nullptr };
const FieldDef g_fieldDef_MFP_AER_KBD_MIDI = { Regs::MFP_AER, "KBD_MIDI", Regs::MFP_AER_KBD_MIDI_SHIFT, Regs::MFP_AER_KBD_MIDI_MASK, nullptr };
const FieldDef g_fieldDef_MFP_AER_FDC_HDC = { Regs::MFP_AER, "FDC_HDC", Regs::MFP_AER_FDC_HDC_SHIFT, Regs::MFP_AER_FDC_HDC_MASK, nullptr };
const FieldDef g_fieldDef_MFP_AER_RS232_RING = { Regs::MFP_AER, "RS232_RING", Regs::MFP_AER_RS232_RING_SHIFT, Regs::MFP_AER_RS232_RING_MASK, nullptr };
const FieldDef g_fieldDef_MFP_AER_MONO = { Regs::MFP_AER, "MONO", Regs::MFP_AER_MONO_SHIFT, Regs::MFP_AER_MONO_MASK, nullptr };
const FieldDef g_fieldDef_MFP_DDR_CENT_BUSY = { Regs::MFP_DDR, "CENT_BUSY", Regs::MFP_DDR_CENT_BUSY_SHIFT, Regs::MFP_DDR_CENT_BUSY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_DDR_RS232_DCD = { Regs::MFP_DDR, "RS232_DCD", Regs::MFP_DDR_RS232_DCD_SHIFT, Regs::MFP_DDR_RS232_DCD_MASK, nullptr };
const FieldDef g_fieldDef_MFP_DDR_RS232_CTS = { Regs::MFP_DDR, "RS232_CTS", Regs::MFP_DDR_RS232_CTS_SHIFT, Regs::MFP_DDR_RS232_CTS_MASK, nullptr };
const FieldDef g_fieldDef_MFP_DDR_KBD_MIDI = { Regs::MFP_DDR, "KBD_MIDI", Regs::MFP_DDR_KBD_MIDI_SHIFT, Regs::MFP_DDR_KBD_MIDI_MASK, nullptr };
const FieldDef g_fieldDef_MFP_DDR_FDC_HDC = { Regs::MFP_DDR, "FDC_HDC", Regs::MFP_DDR_FDC_HDC_SHIFT, Regs::MFP_DDR_FDC_HDC_MASK, nullptr };
const FieldDef g_fieldDef_MFP_DDR_RS232_RING = { Regs::MFP_DDR, "RS232_RING", Regs::MFP_DDR_RS232_RING_SHIFT, Regs::MFP_DDR_RS232_RING_MASK, nullptr };
const FieldDef g_fieldDef_MFP_DDR_MONO = { Regs::MFP_DDR, "MONO", Regs::MFP_DDR_MONO_SHIFT, Regs::MFP_DDR_MONO_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_TIMER_B = { Regs::MFP_IERA, "TIMER_B", Regs::MFP_IERA_TIMER_B_SHIFT, Regs::MFP_IERA_TIMER_B_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_SEND_ERR = { Regs::MFP_IERA, "SEND_ERR", Regs::MFP_IERA_SEND_ERR_SHIFT, Regs::MFP_IERA_SEND_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_SEND_EMPTY = { Regs::MFP_IERA, "SEND_EMPTY", Regs::MFP_IERA_SEND_EMPTY_SHIFT, Regs::MFP_IERA_SEND_EMPTY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_REC_ERR = { Regs::MFP_IERA, "REC_ERR", Regs::MFP_IERA_REC_ERR_SHIFT, Regs::MFP_IERA_REC_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_REC_FULL = { Regs::MFP_IERA, "REC_FULL", Regs::MFP_IERA_REC_FULL_SHIFT, Regs::MFP_IERA_REC_FULL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_TIMER_A = { Regs::MFP_IERA, "TIMER_A", Regs::MFP_IERA_TIMER_A_SHIFT, Regs::MFP_IERA_TIMER_A_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_RS232_RING = { Regs::MFP_IERA, "RS232_RING", Regs::MFP_IERA_RS232_RING_SHIFT, Regs::MFP_IERA_RS232_RING_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERA_MONO_DETECT = { Regs::MFP_IERA, "MONO_DETECT", Regs::MFP_IERA_MONO_DETECT_SHIFT, Regs::MFP_IERA_MONO_DETECT_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_RS232_DTD = { Regs::MFP_IERB, "RS232_DTD", Regs::MFP_IERB_RS232_DTD_SHIFT, Regs::MFP_IERB_RS232_DTD_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_CENT_BUSY = { Regs::MFP_IERB, "CENT_BUSY", Regs::MFP_IERB_CENT_BUSY_SHIFT, Regs::MFP_IERB_CENT_BUSY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_RS232_CTS = { Regs::MFP_IERB, "RS232_CTS", Regs::MFP_IERB_RS232_CTS_SHIFT, Regs::MFP_IERB_RS232_CTS_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_BLITTER = { Regs::MFP_IERB, "BLITTER", Regs::MFP_IERB_BLITTER_SHIFT, Regs::MFP_IERB_BLITTER_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_TIMER_D = { Regs::MFP_IERB, "TIMER_D", Regs::MFP_IERB_TIMER_D_SHIFT, Regs::MFP_IERB_TIMER_D_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_TIMER_C = { Regs::MFP_IERB, "TIMER_C", Regs::MFP_IERB_TIMER_C_SHIFT, Regs::MFP_IERB_TIMER_C_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_IKBD_MIDI = { Regs::MFP_IERB, "IKBD_MIDI", Regs::MFP_IERB_IKBD_MIDI_SHIFT, Regs::MFP_IERB_IKBD_MIDI_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IERB_FDC_HDC = { Regs::MFP_IERB, "FDC_HDC", Regs::MFP_IERB_FDC_HDC_SHIFT, Regs::MFP_IERB_FDC_HDC_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_TIMER_B = { Regs::MFP_IPRA, "TIMER_B", Regs::MFP_IPRA_TIMER_B_SHIFT, Regs::MFP_IPRA_TIMER_B_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_SEND_ERR = { Regs::MFP_IPRA, "SEND_ERR", Regs::MFP_IPRA_SEND_ERR_SHIFT, Regs::MFP_IPRA_SEND_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_SEND_EMPTY = { Regs::MFP_IPRA, "SEND_EMPTY", Regs::MFP_IPRA_SEND_EMPTY_SHIFT, Regs::MFP_IPRA_SEND_EMPTY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_REC_ERR = { Regs::MFP_IPRA, "REC_ERR", Regs::MFP_IPRA_REC_ERR_SHIFT, Regs::MFP_IPRA_REC_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_REC_FULL = { Regs::MFP_IPRA, "REC_FULL", Regs::MFP_IPRA_REC_FULL_SHIFT, Regs::MFP_IPRA_REC_FULL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_TIMER_A = { Regs::MFP_IPRA, "TIMER_A", Regs::MFP_IPRA_TIMER_A_SHIFT, Regs::MFP_IPRA_TIMER_A_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_RS232_RING = { Regs::MFP_IPRA, "RS232_RING", Regs::MFP_IPRA_RS232_RING_SHIFT, Regs::MFP_IPRA_RS232_RING_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRA_MONO_DETECT = { Regs::MFP_IPRA, "MONO_DETECT", Regs::MFP_IPRA_MONO_DETECT_SHIFT, Regs::MFP_IPRA_MONO_DETECT_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_RS232_DTD = { Regs::MFP_IPRB, "RS232_DTD", Regs::MFP_IPRB_RS232_DTD_SHIFT, Regs::MFP_IPRB_RS232_DTD_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_CENT_BUSY = { Regs::MFP_IPRB, "CENT_BUSY", Regs::MFP_IPRB_CENT_BUSY_SHIFT, Regs::MFP_IPRB_CENT_BUSY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_RS232_CTS = { Regs::MFP_IPRB, "RS232_CTS", Regs::MFP_IPRB_RS232_CTS_SHIFT, Regs::MFP_IPRB_RS232_CTS_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_BLITTER = { Regs::MFP_IPRB, "BLITTER", Regs::MFP_IPRB_BLITTER_SHIFT, Regs::MFP_IPRB_BLITTER_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_TIMER_D = { Regs::MFP_IPRB, "TIMER_D", Regs::MFP_IPRB_TIMER_D_SHIFT, Regs::MFP_IPRB_TIMER_D_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_TIMER_C = { Regs::MFP_IPRB, "TIMER_C", Regs::MFP_IPRB_TIMER_C_SHIFT, Regs::MFP_IPRB_TIMER_C_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_IKBD_MIDI = { Regs::MFP_IPRB, "IKBD_MIDI", Regs::MFP_IPRB_IKBD_MIDI_SHIFT, Regs::MFP_IPRB_IKBD_MIDI_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IPRB_FDC_HDC = { Regs::MFP_IPRB, "FDC_HDC", Regs::MFP_IPRB_FDC_HDC_SHIFT, Regs::MFP_IPRB_FDC_HDC_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_TIMER_B = { Regs::MFP_ISRA, "TIMER_B", Regs::MFP_ISRA_TIMER_B_SHIFT, Regs::MFP_ISRA_TIMER_B_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_SEND_ERR = { Regs::MFP_ISRA, "SEND_ERR", Regs::MFP_ISRA_SEND_ERR_SHIFT, Regs::MFP_ISRA_SEND_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_SEND_EMPTY = { Regs::MFP_ISRA, "SEND_EMPTY", Regs::MFP_ISRA_SEND_EMPTY_SHIFT, Regs::MFP_ISRA_SEND_EMPTY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_REC_ERR = { Regs::MFP_ISRA, "REC_ERR", Regs::MFP_ISRA_REC_ERR_SHIFT, Regs::MFP_ISRA_REC_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_REC_FULL = { Regs::MFP_ISRA, "REC_FULL", Regs::MFP_ISRA_REC_FULL_SHIFT, Regs::MFP_ISRA_REC_FULL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_TIMER_A = { Regs::MFP_ISRA, "TIMER_A", Regs::MFP_ISRA_TIMER_A_SHIFT, Regs::MFP_ISRA_TIMER_A_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_RS232_RING = { Regs::MFP_ISRA, "RS232_RING", Regs::MFP_ISRA_RS232_RING_SHIFT, Regs::MFP_ISRA_RS232_RING_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRA_MONO_DETECT = { Regs::MFP_ISRA, "MONO_DETECT", Regs::MFP_ISRA_MONO_DETECT_SHIFT, Regs::MFP_ISRA_MONO_DETECT_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_RS232_DTD = { Regs::MFP_ISRB, "RS232_DTD", Regs::MFP_ISRB_RS232_DTD_SHIFT, Regs::MFP_ISRB_RS232_DTD_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_CENT_BUSY = { Regs::MFP_ISRB, "CENT_BUSY", Regs::MFP_ISRB_CENT_BUSY_SHIFT, Regs::MFP_ISRB_CENT_BUSY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_RS232_CTS = { Regs::MFP_ISRB, "RS232_CTS", Regs::MFP_ISRB_RS232_CTS_SHIFT, Regs::MFP_ISRB_RS232_CTS_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_BLITTER = { Regs::MFP_ISRB, "BLITTER", Regs::MFP_ISRB_BLITTER_SHIFT, Regs::MFP_ISRB_BLITTER_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_TIMER_D = { Regs::MFP_ISRB, "TIMER_D", Regs::MFP_ISRB_TIMER_D_SHIFT, Regs::MFP_ISRB_TIMER_D_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_TIMER_C = { Regs::MFP_ISRB, "TIMER_C", Regs::MFP_ISRB_TIMER_C_SHIFT, Regs::MFP_ISRB_TIMER_C_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_IKBD_MIDI = { Regs::MFP_ISRB, "IKBD_MIDI", Regs::MFP_ISRB_IKBD_MIDI_SHIFT, Regs::MFP_ISRB_IKBD_MIDI_MASK, nullptr };
const FieldDef g_fieldDef_MFP_ISRB_FDC_HDC = { Regs::MFP_ISRB, "FDC_HDC", Regs::MFP_ISRB_FDC_HDC_SHIFT, Regs::MFP_ISRB_FDC_HDC_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_TIMER_B = { Regs::MFP_IMRA, "TIMER_B", Regs::MFP_IMRA_TIMER_B_SHIFT, Regs::MFP_IMRA_TIMER_B_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_SEND_ERR = { Regs::MFP_IMRA, "SEND_ERR", Regs::MFP_IMRA_SEND_ERR_SHIFT, Regs::MFP_IMRA_SEND_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_SEND_EMPTY = { Regs::MFP_IMRA, "SEND_EMPTY", Regs::MFP_IMRA_SEND_EMPTY_SHIFT, Regs::MFP_IMRA_SEND_EMPTY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_REC_ERR = { Regs::MFP_IMRA, "REC_ERR", Regs::MFP_IMRA_REC_ERR_SHIFT, Regs::MFP_IMRA_REC_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_REC_FULL = { Regs::MFP_IMRA, "REC_FULL", Regs::MFP_IMRA_REC_FULL_SHIFT, Regs::MFP_IMRA_REC_FULL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_TIMER_A = { Regs::MFP_IMRA, "TIMER_A", Regs::MFP_IMRA_TIMER_A_SHIFT, Regs::MFP_IMRA_TIMER_A_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_RS232_RING = { Regs::MFP_IMRA, "RS232_RING", Regs::MFP_IMRA_RS232_RING_SHIFT, Regs::MFP_IMRA_RS232_RING_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRA_MONO_DETECT = { Regs::MFP_IMRA, "MONO_DETECT", Regs::MFP_IMRA_MONO_DETECT_SHIFT, Regs::MFP_IMRA_MONO_DETECT_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_RS232_DTD = { Regs::MFP_IMRB, "RS232_DTD", Regs::MFP_IMRB_RS232_DTD_SHIFT, Regs::MFP_IMRB_RS232_DTD_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_CENT_BUSY = { Regs::MFP_IMRB, "CENT_BUSY", Regs::MFP_IMRB_CENT_BUSY_SHIFT, Regs::MFP_IMRB_CENT_BUSY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_RS232_CTS = { Regs::MFP_IMRB, "RS232_CTS", Regs::MFP_IMRB_RS232_CTS_SHIFT, Regs::MFP_IMRB_RS232_CTS_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_BLITTER = { Regs::MFP_IMRB, "BLITTER", Regs::MFP_IMRB_BLITTER_SHIFT, Regs::MFP_IMRB_BLITTER_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_TIMER_D = { Regs::MFP_IMRB, "TIMER_D", Regs::MFP_IMRB_TIMER_D_SHIFT, Regs::MFP_IMRB_TIMER_D_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_TIMER_C = { Regs::MFP_IMRB, "TIMER_C", Regs::MFP_IMRB_TIMER_C_SHIFT, Regs::MFP_IMRB_TIMER_C_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_IKBD_MIDI = { Regs::MFP_IMRB, "IKBD_MIDI", Regs::MFP_IMRB_IKBD_MIDI_SHIFT, Regs::MFP_IMRB_IKBD_MIDI_MASK, nullptr };
const FieldDef g_fieldDef_MFP_IMRB_FDC_HDC = { Regs::MFP_IMRB, "FDC_HDC", Regs::MFP_IMRB_FDC_HDC_SHIFT, Regs::MFP_IMRB_FDC_HDC_MASK, nullptr };
const FieldDef g_fieldDef_MFP_VR_ENDINT = { Regs::MFP_VR, "ENDINT", Regs::MFP_VR_ENDINT_SHIFT, Regs::MFP_VR_ENDINT_MASK, g_enumStringsENDINT };
const FieldDef g_fieldDef_MFP_VR_VEC_BASE_OFFSET = { Regs::MFP_VR, "VEC_BASE_OFFSET", Regs::MFP_VR_VEC_BASE_OFFSET_SHIFT, Regs::MFP_VR_VEC_BASE_OFFSET_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TACR_MODE_TIMER_A = { Regs::MFP_TACR, "MODE_TIMER_A", Regs::MFP_TACR_MODE_TIMER_A_SHIFT, Regs::MFP_TACR_MODE_TIMER_A_MASK, g_enumStringsTIMER_MODE_A };
const FieldDef g_fieldDef_MFP_TBCR_MODE_TIMER_B = { Regs::MFP_TBCR, "MODE_TIMER_B", Regs::MFP_TBCR_MODE_TIMER_B_SHIFT, Regs::MFP_TBCR_MODE_TIMER_B_MASK, g_enumStringsTIMER_MODE_B };
const FieldDef g_fieldDef_MFP_TCDCR_MODE_TIMER_D = { Regs::MFP_TCDCR, "MODE_TIMER_D", Regs::MFP_TCDCR_MODE_TIMER_D_SHIFT, Regs::MFP_TCDCR_MODE_TIMER_D_MASK, g_enumStringsTIMER_MODE_CD };
const FieldDef g_fieldDef_MFP_TCDCR_MODE_TIMER_C = { Regs::MFP_TCDCR, "MODE_TIMER_C", Regs::MFP_TCDCR_MODE_TIMER_C_SHIFT, Regs::MFP_TCDCR_MODE_TIMER_C_MASK, g_enumStringsTIMER_MODE_CD };
const FieldDef g_fieldDef_MFP_TADR_ALL = { Regs::MFP_TADR, "ALL", Regs::MFP_TADR_ALL_SHIFT, Regs::MFP_TADR_ALL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TBDR_ALL = { Regs::MFP_TBDR, "ALL", Regs::MFP_TBDR_ALL_SHIFT, Regs::MFP_TBDR_ALL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TCDR_ALL = { Regs::MFP_TCDR, "ALL", Regs::MFP_TCDR_ALL_SHIFT, Regs::MFP_TCDR_ALL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TDDR_ALL = { Regs::MFP_TDDR, "ALL", Regs::MFP_TDDR_ALL_SHIFT, Regs::MFP_TDDR_ALL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_SCR_ALL = { Regs::MFP_SCR, "ALL", Regs::MFP_SCR_ALL_SHIFT, Regs::MFP_SCR_ALL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_UCR_PARITY_EVEN = { Regs::MFP_UCR, "PARITY_EVEN", Regs::MFP_UCR_PARITY_EVEN_SHIFT, Regs::MFP_UCR_PARITY_EVEN_MASK, nullptr };
const FieldDef g_fieldDef_MFP_UCR_PARITY_IGNORE = { Regs::MFP_UCR, "PARITY_IGNORE", Regs::MFP_UCR_PARITY_IGNORE_SHIFT, Regs::MFP_UCR_PARITY_IGNORE_MASK, nullptr };
const FieldDef g_fieldDef_MFP_UCR_FORMAT = { Regs::MFP_UCR, "FORMAT", Regs::MFP_UCR_FORMAT_SHIFT, Regs::MFP_UCR_FORMAT_MASK, nullptr };
const FieldDef g_fieldDef_MFP_UCR_WORD_LEN = { Regs::MFP_UCR, "WORD_LEN", Regs::MFP_UCR_WORD_LEN_SHIFT, Regs::MFP_UCR_WORD_LEN_MASK, g_enumStringsUSARTLEN };
const FieldDef g_fieldDef_MFP_UCR_CLOCK_DIV = { Regs::MFP_UCR, "CLOCK_DIV", Regs::MFP_UCR_CLOCK_DIV_SHIFT, Regs::MFP_UCR_CLOCK_DIV_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_REC_ENABLE = { Regs::MFP_RSR, "REC_ENABLE", Regs::MFP_RSR_REC_ENABLE_SHIFT, Regs::MFP_RSR_REC_ENABLE_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_STRIP = { Regs::MFP_RSR, "STRIP", Regs::MFP_RSR_STRIP_SHIFT, Regs::MFP_RSR_STRIP_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_MATCH = { Regs::MFP_RSR, "MATCH", Regs::MFP_RSR_MATCH_SHIFT, Regs::MFP_RSR_MATCH_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_FOUND = { Regs::MFP_RSR, "FOUND", Regs::MFP_RSR_FOUND_SHIFT, Regs::MFP_RSR_FOUND_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_FRAME_ERR = { Regs::MFP_RSR, "FRAME_ERR", Regs::MFP_RSR_FRAME_ERR_SHIFT, Regs::MFP_RSR_FRAME_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_PARITY_ERR = { Regs::MFP_RSR, "PARITY_ERR", Regs::MFP_RSR_PARITY_ERR_SHIFT, Regs::MFP_RSR_PARITY_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_OVERRUN_ERR = { Regs::MFP_RSR, "OVERRUN_ERR", Regs::MFP_RSR_OVERRUN_ERR_SHIFT, Regs::MFP_RSR_OVERRUN_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_RSR_BUFFER_FULL = { Regs::MFP_RSR, "BUFFER_FULL", Regs::MFP_RSR_BUFFER_FULL_SHIFT, Regs::MFP_RSR_BUFFER_FULL_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_TRANS_ENABLE = { Regs::MFP_TSR, "TRANS_ENABLE", Regs::MFP_TSR_TRANS_ENABLE_SHIFT, Regs::MFP_TSR_TRANS_ENABLE_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_LOW = { Regs::MFP_TSR, "LOW", Regs::MFP_TSR_LOW_SHIFT, Regs::MFP_TSR_LOW_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_HIGH = { Regs::MFP_TSR, "HIGH", Regs::MFP_TSR_HIGH_SHIFT, Regs::MFP_TSR_HIGH_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_BREAK = { Regs::MFP_TSR, "BREAK", Regs::MFP_TSR_BREAK_SHIFT, Regs::MFP_TSR_BREAK_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_EOT = { Regs::MFP_TSR, "EOT", Regs::MFP_TSR_EOT_SHIFT, Regs::MFP_TSR_EOT_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_AUTO_TURN = { Regs::MFP_TSR, "AUTO_TURN", Regs::MFP_TSR_AUTO_TURN_SHIFT, Regs::MFP_TSR_AUTO_TURN_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_UNDERRUN_ERR = { Regs::MFP_TSR, "UNDERRUN_ERR", Regs::MFP_TSR_UNDERRUN_ERR_SHIFT, Regs::MFP_TSR_UNDERRUN_ERR_MASK, nullptr };
const FieldDef g_fieldDef_MFP_TSR_BUFFER_EMPTY = { Regs::MFP_TSR, "BUFFER_EMPTY", Regs::MFP_TSR_BUFFER_EMPTY_SHIFT, Regs::MFP_TSR_BUFFER_EMPTY_MASK, nullptr };
const FieldDef g_fieldDef_MFP_UDR_ALL = { Regs::MFP_UDR, "ALL", Regs::MFP_UDR_ALL_SHIFT, Regs::MFP_UDR_ALL_MASK, nullptr };
/* Register Field Sets */

const FieldDef* g_regFieldsDef_MMU_CONFIG[] = {
	 &g_fieldDef_MMU_CONFIG_BANK1,
	 &g_fieldDef_MMU_CONFIG_BANK0,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_SCREEN_HIGH[] = {
	 &g_fieldDef_VID_SCREEN_HIGH_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_SCREEN_MID[] = {
	 &g_fieldDef_VID_SCREEN_MID_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_SCREEN_LOW_STE[] = {
	 &g_fieldDef_VID_SCREEN_LOW_STE_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_SCANLINE_OFFSET_STE[] = {
	 &g_fieldDef_VID_SCANLINE_OFFSET_STE_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_HORIZ_SCROLL_STE[] = {
	 &g_fieldDef_VID_HORIZ_SCROLL_STE_PIXELS,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_SYNC_MODE[] = {
	 &g_fieldDef_VID_SYNC_MODE_TYPE,
	 &g_fieldDef_VID_SYNC_MODE_RATE,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_PAL_0[] = {
	 &g_fieldDef_VID_PAL_0_BLUE_ST,
	 &g_fieldDef_VID_PAL_0_BLUE_STE,
	 &g_fieldDef_VID_PAL_0_GREEN_ST,
	 &g_fieldDef_VID_PAL_0_GREEN_STE,
	 &g_fieldDef_VID_PAL_0_RED_ST,
	 &g_fieldDef_VID_PAL_0_RED_STE,
	nullptr
};
const FieldDef* g_regFieldsDef_VID_SHIFTER_RES[] = {
	 &g_fieldDef_VID_SHIFTER_RES_RES,
	nullptr
};
const FieldDef* g_regFieldsDef_FDC_ACCESS[] = {
	 &g_fieldDef_FDC_ACCESS_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_FDC_STATUS[] = {
	 &g_fieldDef_FDC_STATUS_ERROR,
	 &g_fieldDef_FDC_STATUS_SECT_CNT,
	 &g_fieldDef_FDC_STATUS_DATA_REQ,
	nullptr
};
const FieldDef* g_regFieldsDef_FDC_DMA_BASE_HIGH[] = {
	 &g_fieldDef_FDC_DMA_BASE_HIGH_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_FDC_DMA_BASE_MID[] = {
	 &g_fieldDef_FDC_DMA_BASE_MID_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_FDC_DMA_BASE_LOW[] = {
	 &g_fieldDef_FDC_DMA_BASE_LOW_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_GPIP[] = {
	 &g_fieldDef_MFP_GPIP_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_AER[] = {
	 &g_fieldDef_MFP_AER_CENT_BUSY,
	 &g_fieldDef_MFP_AER_RS232_DCD,
	 &g_fieldDef_MFP_AER_RS232_CTS,
	 &g_fieldDef_MFP_AER_KBD_MIDI,
	 &g_fieldDef_MFP_AER_FDC_HDC,
	 &g_fieldDef_MFP_AER_RS232_RING,
	 &g_fieldDef_MFP_AER_MONO,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_DDR[] = {
	 &g_fieldDef_MFP_DDR_CENT_BUSY,
	 &g_fieldDef_MFP_DDR_RS232_DCD,
	 &g_fieldDef_MFP_DDR_RS232_CTS,
	 &g_fieldDef_MFP_DDR_KBD_MIDI,
	 &g_fieldDef_MFP_DDR_FDC_HDC,
	 &g_fieldDef_MFP_DDR_RS232_RING,
	 &g_fieldDef_MFP_DDR_MONO,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_IERA[] = {
	 &g_fieldDef_MFP_IERA_TIMER_B,
	 &g_fieldDef_MFP_IERA_SEND_ERR,
	 &g_fieldDef_MFP_IERA_SEND_EMPTY,
	 &g_fieldDef_MFP_IERA_REC_ERR,
	 &g_fieldDef_MFP_IERA_REC_FULL,
	 &g_fieldDef_MFP_IERA_TIMER_A,
	 &g_fieldDef_MFP_IERA_RS232_RING,
	 &g_fieldDef_MFP_IERA_MONO_DETECT,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_IERB[] = {
	 &g_fieldDef_MFP_IERB_RS232_DTD,
	 &g_fieldDef_MFP_IERB_CENT_BUSY,
	 &g_fieldDef_MFP_IERB_RS232_CTS,
	 &g_fieldDef_MFP_IERB_BLITTER,
	 &g_fieldDef_MFP_IERB_TIMER_D,
	 &g_fieldDef_MFP_IERB_TIMER_C,
	 &g_fieldDef_MFP_IERB_IKBD_MIDI,
	 &g_fieldDef_MFP_IERB_FDC_HDC,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_IPRA[] = {
	 &g_fieldDef_MFP_IPRA_TIMER_B,
	 &g_fieldDef_MFP_IPRA_SEND_ERR,
	 &g_fieldDef_MFP_IPRA_SEND_EMPTY,
	 &g_fieldDef_MFP_IPRA_REC_ERR,
	 &g_fieldDef_MFP_IPRA_REC_FULL,
	 &g_fieldDef_MFP_IPRA_TIMER_A,
	 &g_fieldDef_MFP_IPRA_RS232_RING,
	 &g_fieldDef_MFP_IPRA_MONO_DETECT,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_IPRB[] = {
	 &g_fieldDef_MFP_IPRB_RS232_DTD,
	 &g_fieldDef_MFP_IPRB_CENT_BUSY,
	 &g_fieldDef_MFP_IPRB_RS232_CTS,
	 &g_fieldDef_MFP_IPRB_BLITTER,
	 &g_fieldDef_MFP_IPRB_TIMER_D,
	 &g_fieldDef_MFP_IPRB_TIMER_C,
	 &g_fieldDef_MFP_IPRB_IKBD_MIDI,
	 &g_fieldDef_MFP_IPRB_FDC_HDC,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_ISRA[] = {
	 &g_fieldDef_MFP_ISRA_TIMER_B,
	 &g_fieldDef_MFP_ISRA_SEND_ERR,
	 &g_fieldDef_MFP_ISRA_SEND_EMPTY,
	 &g_fieldDef_MFP_ISRA_REC_ERR,
	 &g_fieldDef_MFP_ISRA_REC_FULL,
	 &g_fieldDef_MFP_ISRA_TIMER_A,
	 &g_fieldDef_MFP_ISRA_RS232_RING,
	 &g_fieldDef_MFP_ISRA_MONO_DETECT,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_ISRB[] = {
	 &g_fieldDef_MFP_ISRB_RS232_DTD,
	 &g_fieldDef_MFP_ISRB_CENT_BUSY,
	 &g_fieldDef_MFP_ISRB_RS232_CTS,
	 &g_fieldDef_MFP_ISRB_BLITTER,
	 &g_fieldDef_MFP_ISRB_TIMER_D,
	 &g_fieldDef_MFP_ISRB_TIMER_C,
	 &g_fieldDef_MFP_ISRB_IKBD_MIDI,
	 &g_fieldDef_MFP_ISRB_FDC_HDC,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_IMRA[] = {
	 &g_fieldDef_MFP_IMRA_TIMER_B,
	 &g_fieldDef_MFP_IMRA_SEND_ERR,
	 &g_fieldDef_MFP_IMRA_SEND_EMPTY,
	 &g_fieldDef_MFP_IMRA_REC_ERR,
	 &g_fieldDef_MFP_IMRA_REC_FULL,
	 &g_fieldDef_MFP_IMRA_TIMER_A,
	 &g_fieldDef_MFP_IMRA_RS232_RING,
	 &g_fieldDef_MFP_IMRA_MONO_DETECT,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_IMRB[] = {
	 &g_fieldDef_MFP_IMRB_RS232_DTD,
	 &g_fieldDef_MFP_IMRB_CENT_BUSY,
	 &g_fieldDef_MFP_IMRB_RS232_CTS,
	 &g_fieldDef_MFP_IMRB_BLITTER,
	 &g_fieldDef_MFP_IMRB_TIMER_D,
	 &g_fieldDef_MFP_IMRB_TIMER_C,
	 &g_fieldDef_MFP_IMRB_IKBD_MIDI,
	 &g_fieldDef_MFP_IMRB_FDC_HDC,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_VR[] = {
	 &g_fieldDef_MFP_VR_ENDINT,
	 &g_fieldDef_MFP_VR_VEC_BASE_OFFSET,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TACR[] = {
	 &g_fieldDef_MFP_TACR_MODE_TIMER_A,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TBCR[] = {
	 &g_fieldDef_MFP_TBCR_MODE_TIMER_B,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TCDCR[] = {
	 &g_fieldDef_MFP_TCDCR_MODE_TIMER_D,
	 &g_fieldDef_MFP_TCDCR_MODE_TIMER_C,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TADR[] = {
	 &g_fieldDef_MFP_TADR_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TBDR[] = {
	 &g_fieldDef_MFP_TBDR_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TCDR[] = {
	 &g_fieldDef_MFP_TCDR_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TDDR[] = {
	 &g_fieldDef_MFP_TDDR_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_SCR[] = {
	 &g_fieldDef_MFP_SCR_ALL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_UCR[] = {
	 &g_fieldDef_MFP_UCR_PARITY_EVEN,
	 &g_fieldDef_MFP_UCR_PARITY_IGNORE,
	 &g_fieldDef_MFP_UCR_FORMAT,
	 &g_fieldDef_MFP_UCR_WORD_LEN,
	 &g_fieldDef_MFP_UCR_CLOCK_DIV,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_RSR[] = {
	 &g_fieldDef_MFP_RSR_REC_ENABLE,
	 &g_fieldDef_MFP_RSR_STRIP,
	 &g_fieldDef_MFP_RSR_MATCH,
	 &g_fieldDef_MFP_RSR_FOUND,
	 &g_fieldDef_MFP_RSR_FRAME_ERR,
	 &g_fieldDef_MFP_RSR_PARITY_ERR,
	 &g_fieldDef_MFP_RSR_OVERRUN_ERR,
	 &g_fieldDef_MFP_RSR_BUFFER_FULL,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_TSR[] = {
	 &g_fieldDef_MFP_TSR_TRANS_ENABLE,
	 &g_fieldDef_MFP_TSR_LOW,
	 &g_fieldDef_MFP_TSR_HIGH,
	 &g_fieldDef_MFP_TSR_BREAK,
	 &g_fieldDef_MFP_TSR_EOT,
	 &g_fieldDef_MFP_TSR_AUTO_TURN,
	 &g_fieldDef_MFP_TSR_UNDERRUN_ERR,
	 &g_fieldDef_MFP_TSR_BUFFER_EMPTY,
	nullptr
};
const FieldDef* g_regFieldsDef_MFP_UDR[] = {
	 &g_fieldDef_MFP_UDR_ALL,
	nullptr
};
} // namespace
