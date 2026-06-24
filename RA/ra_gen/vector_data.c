/* generated vector source file - do not edit */
        #include "bsp_api.h"
        /* Do not build these data structures if no interrupts are currently allocated because IAR will have build errors. */
        #if VECTOR_DATA_IRQ_COUNT > 0
        BSP_DONT_REMOVE const fsp_vector_t g_vector_table[BSP_ICU_VECTOR_NUM_ENTRIES] BSP_PLACE_IN_SECTION(BSP_SECTION_APPLICATION_VECTORS) =
        {
                        [0] = sci_uart_rxi_isr, /* SCI2 RXI (Receive data full) */
            [1] = sci_uart_txi_isr, /* SCI2 TXI (Transmit data empty) */
            [2] = sci_uart_tei_isr, /* SCI2 TEI (Transmit end) */
            [3] = sci_uart_eri_isr, /* SCI2 ERI (Receive error) */
            [4] = dmac_int_isr, /* DMAC0 INT (DMAC0 transfer end) */
            [5] = sci_uart_rxi_isr, /* SCI3 RXI (Receive data full) */
            [6] = sci_uart_txi_isr, /* SCI3 TXI (Transmit data empty) */
            [7] = sci_uart_tei_isr, /* SCI3 TEI (Transmit end) */
            [8] = sci_uart_eri_isr, /* SCI3 ERI (Receive error) */
            [9] = gpt_counter_overflow_isr, /* GPT0 COUNTER OVERFLOW (Overflow) */
            [10] = adc_scan_end_isr, /* ADC0 SCAN END (End of A/D scanning operation) */
        };
        #if BSP_FEATURE_ICU_HAS_IELSR
        const bsp_interrupt_event_t g_interrupt_event_link_select[BSP_ICU_VECTOR_NUM_ENTRIES] =
        {
            [0] = BSP_PRV_VECT_ENUM(EVENT_SCI2_RXI,GROUP0), /* SCI2 RXI (Receive data full) */
            [1] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TXI,GROUP1), /* SCI2 TXI (Transmit data empty) */
            [2] = BSP_PRV_VECT_ENUM(EVENT_SCI2_TEI,GROUP2), /* SCI2 TEI (Transmit end) */
            [3] = BSP_PRV_VECT_ENUM(EVENT_SCI2_ERI,GROUP3), /* SCI2 ERI (Receive error) */
            [4] = BSP_PRV_VECT_ENUM(EVENT_DMAC0_INT,GROUP4), /* DMAC0 INT (DMAC0 transfer end) */
            [5] = BSP_PRV_VECT_ENUM(EVENT_SCI3_RXI,GROUP5), /* SCI3 RXI (Receive data full) */
            [6] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TXI,GROUP6), /* SCI3 TXI (Transmit data empty) */
            [7] = BSP_PRV_VECT_ENUM(EVENT_SCI3_TEI,GROUP7), /* SCI3 TEI (Transmit end) */
            [8] = BSP_PRV_VECT_ENUM(EVENT_SCI3_ERI,GROUP0), /* SCI3 ERI (Receive error) */
            [9] = BSP_PRV_VECT_ENUM(EVENT_GPT0_COUNTER_OVERFLOW,GROUP1), /* GPT0 COUNTER OVERFLOW (Overflow) */
            [10] = BSP_PRV_VECT_ENUM(EVENT_ADC0_SCAN_END,GROUP2), /* ADC0 SCAN END (End of A/D scanning operation) */
        };
        #endif
        #endif