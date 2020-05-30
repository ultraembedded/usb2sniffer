#ifndef USB_CAPTURE_H
#define USB_CAPTURE_H

#include <stdint.h>
#include <stdlib.h>

#include "usb_sniffer_defs.h"
#include "ftdi_axi_driver.h"

#define CFG_BASE_ADDR       0x80000000

//---------------------------------------------------------------------
// usb_capture: Driver for USB capture HW
//---------------------------------------------------------------------
class usb_capture
{
public:
    usb_capture(ftdi_axi_driver * hw)
    {
        m_hw      = hw;
        m_cfg_reg = 0;
    }
    //-----------------------------------------------------------------
    // configure_buffer: Configure storage buffer details
    //-----------------------------------------------------------------
    bool configure_buffer(uint32_t base, uint32_t size)
    {
        bool ok = true;
        ok &= m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_BASE, base);
        ok &= m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_END,  size);
        return ok;
    }
    //-----------------------------------------------------------------
    // match_device: Configure device ID based filtering
    //-----------------------------------------------------------------
    bool match_device(int dev, int exclude)
    {
        m_cfg_reg &= ~(USB_BUFFER_CFG_DEV_MASK         << USB_BUFFER_CFG_DEV_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_MATCH_DEV_MASK   << USB_BUFFER_CFG_MATCH_DEV_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_EXCLUDE_DEV_MASK << USB_BUFFER_CFG_EXCLUDE_DEV_SHIFT);

        if (dev >= 0)
        {
            m_cfg_reg |= (dev << USB_BUFFER_CFG_DEV_SHIFT);

            if (exclude)
                m_cfg_reg |= (1   << USB_BUFFER_CFG_EXCLUDE_DEV_SHIFT);
            else
                m_cfg_reg |= (1   << USB_BUFFER_CFG_MATCH_DEV_SHIFT);
        }

        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // match_endpoint: Configure endpoint based filtering
    //-----------------------------------------------------------------
    bool match_endpoint(int ep, int exclude)
    {
        m_cfg_reg &= ~(USB_BUFFER_CFG_EP_MASK         << USB_BUFFER_CFG_EP_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_MATCH_EP_MASK   << USB_BUFFER_CFG_MATCH_EP_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_EXCLUDE_EP_MASK << USB_BUFFER_CFG_EXCLUDE_EP_SHIFT);

        if (ep >= 0)
        {
            m_cfg_reg |= (ep << USB_BUFFER_CFG_EP_SHIFT);

            if (exclude)
                m_cfg_reg |= (1  << USB_BUFFER_CFG_EXCLUDE_EP_SHIFT);
            else
                m_cfg_reg |= (1  << USB_BUFFER_CFG_MATCH_EP_SHIFT);
        }

        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // drop_sof: Configure SOF filtering
    //-----------------------------------------------------------------
    bool drop_sof(int enable)
    {
        if (enable)
            m_cfg_reg |= (USB_BUFFER_CFG_IGNORE_SOF_MASK << USB_BUFFER_CFG_IGNORE_SOF_SHIFT);
        else
            m_cfg_reg &= ~(USB_BUFFER_CFG_IGNORE_SOF_MASK << USB_BUFFER_CFG_IGNORE_SOF_SHIFT);

        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // drop_in_nak: Drop IN-NAK pairs (polled reads with no data)
    //-----------------------------------------------------------------
    bool drop_in_nak(int enable)
    {
        if (enable)
            m_cfg_reg |= (USB_BUFFER_CFG_IGNORE_IN_NAK_MASK << USB_BUFFER_CFG_IGNORE_IN_NAK_SHIFT);
        else
            m_cfg_reg &= ~(USB_BUFFER_CFG_IGNORE_IN_NAK_MASK << USB_BUFFER_CFG_IGNORE_IN_NAK_SHIFT);

        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // set_speed: Configure PHY USB speed
    //-----------------------------------------------------------------
    bool set_speed(uint32_t speed)
    {
        m_cfg_reg &= ~(USB_BUFFER_CFG_SPEED_MASK << USB_BUFFER_CFG_SPEED_SHIFT);
        m_cfg_reg |=  (speed                     << USB_BUFFER_CFG_SPEED_SHIFT);

        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // set_phy: Manual control of the ULPI PHY pullups, mode
    //-----------------------------------------------------------------
    bool set_phy(int xcvrselect, int termsel, int opmode, int dp_pull, int dm_pull)
    {
        m_cfg_reg &= ~(USB_BUFFER_CFG_SPEED_MASK          << USB_BUFFER_CFG_SPEED_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_PHY_DMPULLDOWN_MASK << USB_BUFFER_CFG_PHY_DMPULLDOWN_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_PHY_DPPULLDOWN_MASK << USB_BUFFER_CFG_PHY_DPPULLDOWN_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_PHY_TERMSELECT_MASK << USB_BUFFER_CFG_PHY_TERMSELECT_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_PHY_XCVRSELECT_MASK << USB_BUFFER_CFG_PHY_XCVRSELECT_SHIFT);
        m_cfg_reg &= ~(USB_BUFFER_CFG_PHY_OPMODE_MASK     << USB_BUFFER_CFG_PHY_OPMODE_SHIFT);

        m_cfg_reg |= (USB_SPEED_MANUAL << USB_BUFFER_CFG_SPEED_SHIFT);
        m_cfg_reg |= (dm_pull          << USB_BUFFER_CFG_PHY_DMPULLDOWN_SHIFT);
        m_cfg_reg |= (dp_pull          << USB_BUFFER_CFG_PHY_DPPULLDOWN_SHIFT);
        m_cfg_reg |= (termsel          << USB_BUFFER_CFG_PHY_TERMSELECT_SHIFT);
        m_cfg_reg |= (xcvrselect       << USB_BUFFER_CFG_PHY_XCVRSELECT_SHIFT);
        m_cfg_reg |= (opmode           << USB_BUFFER_CFG_PHY_OPMODE_SHIFT);

        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // start_capture: Start capturing data
    //-----------------------------------------------------------------
    bool start_capture(void)
    {
        m_cfg_reg |= (USB_BUFFER_CFG_ENABLED_MASK << USB_BUFFER_CFG_ENABLED_SHIFT);
        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // stop_capture: Stop capturing data
    //-----------------------------------------------------------------
    bool stop_capture(void)
    {
        m_cfg_reg &= ~(USB_BUFFER_CFG_ENABLED_MASK << USB_BUFFER_CFG_ENABLED_SHIFT);
        return m_hw->write32(CFG_BASE_ADDR + USB_BUFFER_CFG, m_cfg_reg);
    }
    //-----------------------------------------------------------------
    // triggered: Has the capture triggered
    //-----------------------------------------------------------------
    bool triggered(void)
    {
        uint32_t status = 0;

        if (!m_hw->read32(CFG_BASE_ADDR + USB_BUFFER_STS, status))
        {
            fprintf(stderr, "ERROR: Failed to read status\n");
            return 0;
        }

        return (status & (1 << USB_BUFFER_STS_TRIG_SHIFT)) != 0;
    }
    //-----------------------------------------------------------------
    // has_lost_data: Buffer overflow - data lost
    //-----------------------------------------------------------------
    bool has_lost_data(void)
    {
        uint32_t status = 0; 

        if (!m_hw->read32(CFG_BASE_ADDR + USB_BUFFER_STS, status))
        {
            fprintf(stderr, "ERROR: Failed to read status\n");
            return 0;
        }

        return (status & (1 << USB_BUFFER_STS_DATA_LOSS_SHIFT)) != 0;
    }
    //-----------------------------------------------------------------
    // get_level: Get buffer level
    //-----------------------------------------------------------------
    uint32_t get_level(void)
    {
        uint32_t value = 0;
        m_hw->read32(CFG_BASE_ADDR + USB_BUFFER_CURRENT, value);
        return value;
    }

    typedef enum
    {
        USB_SPEED_HS,
        USB_SPEED_FS,
        USB_SPEED_LS,
        USB_SPEED_MANUAL,
    } tUsbSpeed;

protected:
    ftdi_axi_driver *m_hw;
    uint32_t         m_cfg_reg;
};

#endif