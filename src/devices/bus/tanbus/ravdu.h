// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Ralph Allen Colour VDU Card

**********************************************************************/


#ifndef MAME_BUS_TANBUS_RAVDU_H
#define MAME_BUS_TANBUS_RAVDU_H

#pragma once

#include "bus/tanbus/tanbus.h"
#include "video/saa5050.h"
#include "video/mc6845.h"
#include "screen.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class tanbus_ravdu_device :
	public device_t,
	public device_tanbus_interface
{
public:
	static constexpr feature_type imperfect_features() { return feature::GRAPHICS; }

	// construction/destruction
	tanbus_ravdu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

	virtual uint8_t read(offs_t offset, int inhrom, int inhram, int be) override;
	virtual void write(offs_t offset, uint8_t data, int inhrom, int inhram, int be) override;
	virtual void set_inhibit_lines(offs_t offset, int &inhram, int &inhrom) override;

private:
	uint8_t videoram_r(offs_t offset);
	MC6845_UPDATE_ROW(crtc_update_row);
	DECLARE_WRITE_LINE_MEMBER(vsync_changed);

	required_device<screen_device> m_screen;
	required_device<mc6845_device> m_crtc;
	required_device<saa5055_device> m_trom;

	std::unique_ptr<uint8_t[]> m_videoram;
};


// device type definition
DECLARE_DEVICE_TYPE(TANBUS_RAVDU, tanbus_ravdu_device)


#endif // MAME_BUS_TANBUS_RAVDU_H
