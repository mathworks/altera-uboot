/*
 *  Copyright (C) 2012 Altera Corporation <www.altera.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SCAN_MANAGER_H_
#define _SCAN_MANAGER_H_

typedef unsigned long Scan_mgr_entry_t;

#define NUM_OF_CHAINS     (4)
#define SHIFT_COUNT_32BIT (5)
#define MASK_COUNT_32BIT  (0x1F)

#define SCANMGR_STAT_ADDRESS 0x0
#define SCANMGR_EN_ADDRESS 0x4
#define SCANMGR_FIFOSINGLEBYTE_ADDRESS 0x10
#define SCANMGR_FIFODOUBLEBYTE_ADDRESS 0x14
#define SCANMGR_FIFOQUADBYTE_ADDRESS 0x1c

#define SCANMGR_STAT_ACTIVE_GET(x) (((x) & 0x80000000) >> 31)
#define SCANMGR_STAT_WFIFOCNT_GET(x) (((x) & 0x70000000) >> 28)

/*
 * @enum IOScanChainSelect
 * @brief Definition of enum for IO scan chain ID
 */
typedef enum  {
	IO_SCAN_CHAIN_0 = 0,    /* EMAC_IO and MIXED2_IO */
	IO_SCAN_CHAIN_1,        /* MIXED1_IO and FLASH_IO */
	IO_SCAN_CHAIN_2,        /* General IO */
	IO_SCAN_CHAIN_3,        /* DDR IO */
	IO_SCAN_CHAIN_UNDEFINED
} IOScanChainSelect;

#define IO_SCAN_CHAIN_NUM		NUM_OF_CHAINS
/* Maximum number of IO scan chains */

#define IO_SCAN_CHAIN_128BIT_SHIFT	(7)
/*
 * Shift count to get number of IO scan chain data in granularity
 * of 128-bit ( N / 128 )
 */

#define IO_SCAN_CHAIN_128BIT_MASK	(0x7F)
/*
 * Mask to get residual IO scan chain data in
 * granularity of 128-bit ( N mod 128 )
 */

#define IO_SCAN_CHAIN_32BIT_SHIFT	SHIFT_COUNT_32BIT
/*
 * Shift count to get number of IO scan chain
 * data in granularity of 32-bit ( N / 32 )
 */

#define IO_SCAN_CHAIN_32BIT_MASK	MASK_COUNT_32BIT
/*
 * Mask to get residual IO scan chain data in
 * granularity of 32-bit ( N mod 32 )
 */

#define IO_SCAN_CHAIN_BYTE_MASK		(0xFF)
/* Byte mask */

#define IO_SCAN_CHAIN_PAYLOAD_24BIT	(24)
/* 24-bits (3 bytes) IO scan chain payload definition */

#define TDI_TDO_MAX_PAYLOAD		(127)
/*
 * Maximum length of TDI_TDO packet payload is 128 bits,
 * represented by (length - 1) in TDI_TDO header
 */

#define TDI_TDO_HEADER_FIRST_BYTE	(0x80)
/* TDI_TDO packet header for IO scan chain program */

#define TDI_TDO_HEADER_SECOND_BYTE_SHIFT	(8)
/* Position of second command byte for TDI_TDO packet */

#define SCAN_MGR_IO_SCAN_ENGINE_STATUS_IDLE	(0)
/* IO scan chain engine is idle */

#define SCAN_MGR_IO_SCAN_ENGINE_STATUS_ACTIVE	(1)
/* IO scan chain engine is active */

#define MAX_WAITING_DELAY_IO_SCAN_ENGINE	(100)
/*
 * Maximum polling loop to wait for IO scan chain engine
 * becomes idle to prevent infinite loop
 */

/*
 * scan_mgr_io_scan_chain_prg
 *
 * Program HPS IO Scan Chain
 *
 * io_scan_chain_id @ref IOScanChainSelect [in] - IO scan chain ID with
 *        range of enumIOScanChainSelect *
 * io_scan_chain_len_in_bits uint32_t [in] - IO scan chain length in bits
 * *iocsr_scan_chain @ref Scan_mgr_entry_t [in] - IO scan chain table
 */
extern unsigned long
scan_mgr_io_scan_chain_prg(
	IOScanChainSelect io_scan_chain_id,
	uint32_t io_scan_chain_len_in_bits,
	const unsigned long *iocsr_scan_chain);



/*
 * @fn scan_mgr_io_scan_chain_engine_is_idle
 *
 * @brief Inline function to check IO scan chain engine status and wait if the
 *        engine is active. Poll the IO scan chain engine till maximum iteration
 *        reached.
 *
 * @param max_iter uint32_t [in] - maximum polling loop to revent infinite loop
 */
static inline unsigned long
scan_mgr_io_scan_chain_engine_is_idle(uint32_t max_iter)
{
	uint32_t scanmgr_status;

	scanmgr_status = readl(SCANMGR_STAT_ADDRESS +
		SOCFPGA_SCANMGR_ADDRESS);

	/* Poll the engine until the scan engine is inactive */
	while (SCANMGR_STAT_ACTIVE_GET(scanmgr_status)
		|| (SCANMGR_STAT_WFIFOCNT_GET(scanmgr_status) > 0)) {

		max_iter--;

		if (max_iter > 0) {
			scanmgr_status = readl(
				SOCFPGA_SCANMGR_ADDRESS +
				SCANMGR_STAT_ADDRESS);
		} else {
			return SCAN_MGR_IO_SCAN_ENGINE_STATUS_ACTIVE;
		}
	}
	return SCAN_MGR_IO_SCAN_ENGINE_STATUS_IDLE;
}

extern const unsigned long iocsr_scan_chain0_table[
	((CONFIG_HPS_IOCSR_SCANCHAIN0_LENGTH / 32) + 1)];
extern const unsigned long iocsr_scan_chain1_table[
	((CONFIG_HPS_IOCSR_SCANCHAIN1_LENGTH / 32) + 1)];
extern const unsigned long iocsr_scan_chain2_table[
	((CONFIG_HPS_IOCSR_SCANCHAIN2_LENGTH / 32) + 1)];
extern const unsigned long iocsr_scan_chain3_table[
	((CONFIG_HPS_IOCSR_SCANCHAIN3_LENGTH / 32) + 1)];

#endif	/* _SCAN_MANAGER_H_ */
