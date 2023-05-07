// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#ifndef MAME_EMU_EMUMEM_HEDW_H
#define MAME_EMU_EMUMEM_HEDW_H

#pragma once

// handler_entry_write_dispatch

// dispatches an access among multiple handlers indexed on part of the
// address and when appropriate a selected view

template<int HighBits, int Width, int AddrShift> class handler_entry_write_dispatch : public handler_entry_write<Width, AddrShift>
{
public:
	using uX = typename emu::detail::handler_entry_size<Width>::uX;
	using mapping = typename handler_entry_write<Width, AddrShift>::mapping;

	handler_entry_write_dispatch(address_space *space, const handler_entry::range &init, handler_entry_write<Width, AddrShift> *handler);
	handler_entry_write_dispatch(address_space *space, memory_view &view);
	handler_entry_write_dispatch(handler_entry_write_dispatch<HighBits, Width, AddrShift> *src);
	~handler_entry_write_dispatch();

	void write(offs_t offset, uX data, uX mem_mask) const override;
	u16 write_flags(offs_t offset, uX data, uX mem_mask) const override;
	u16 lookup_flags(offs_t offset, uX mem_mask) const override;
	void *get_ptr(offs_t offset) const override;
	void lookup(offs_t address, offs_t &start, offs_t &end, handler_entry_write<Width, AddrShift> *&handler) const override;

	offs_t dispatch_entry(offs_t address) const override;
	void dump_map(std::vector<memory_entry> &map) const override;

	std::string name() const override;

	void populate_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write<Width, AddrShift> *handler) override;
	void populate_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write<Width, AddrShift> *handler) override;
	void populate_mismatched_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, const memory_units_descriptor<Width, AddrShift> &descriptor, u8 rkey, std::vector<mapping> &mappings) override;
	void populate_mismatched_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, const memory_units_descriptor<Width, AddrShift> &descriptor, std::vector<mapping> &mappings) override;
	void populate_passthrough_nomirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write_passthrough<Width, AddrShift> *handler, std::vector<mapping> &mappings) override;
	void populate_passthrough_mirror(offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write_passthrough<Width, AddrShift> *handler, std::vector<mapping> &mappings) override;
	void detach(const std::unordered_set<handler_entry *> &handlers) override;
	void range_cut_before(offs_t address, int start = COUNT);
	void range_cut_after(offs_t address, int start = -1);

	void enumerate_references(handler_entry::reflist &refs) const override;

	const handler_entry_write<Width, AddrShift> *const *get_dispatch() const override;
	void select_a(int slot) override;
	void select_u(int slot) override;
	void init_handlers(offs_t start_entry, offs_t end_entry, u32 lowbits, offs_t ostart, offs_t oend, handler_entry_write<Width, AddrShift> **dispatch, handler_entry::range *ranges) override;
	handler_entry_write<Width, AddrShift> *dup() override;

private:
	static constexpr int    Level    = emu::detail::handler_entry_dispatch_level(HighBits);
	static constexpr u32    LowBits  = emu::detail::handler_entry_dispatch_lowbits(HighBits, Width, AddrShift);
	static constexpr u32    BITCOUNT = HighBits > LowBits ? HighBits - LowBits : 0;
	static constexpr u32    COUNT    = 1 << BITCOUNT;
	static constexpr offs_t BITMASK  = make_bitmask<offs_t>(BITCOUNT);
	static constexpr offs_t LOWMASK  = make_bitmask<offs_t>(LowBits);
	static constexpr offs_t HIGHMASK = make_bitmask<offs_t>(HighBits) ^ LOWMASK;
	static constexpr offs_t UPMASK   = ~make_bitmask<offs_t>(HighBits);

	class handler_array : public std::array<handler_entry_write<Width, AddrShift> *, COUNT>
	{
	public:
		using std::array<handler_entry_write<Width, AddrShift> *, COUNT>::array;
		handler_array()
		{
			std::fill(this->begin(), this->end(), nullptr);
		}
	};

	class range_array : public std::array<handler_entry::range, COUNT>
	{
	public:
		using std::array<handler_entry::range, COUNT>::array;
		range_array()
		{
			std::fill(this->begin(), this->end(), handler_entry::range{ 0, 0 });
		}
	};

	memory_view *m_view;

	std::vector<handler_array> m_dispatch_array;
	std::vector<range_array> m_ranges_array;

	handler_entry_write<Width, AddrShift> **m_a_dispatch;
	handler_entry::range *m_a_ranges;

	handler_entry_write<Width, AddrShift> **m_u_dispatch;
	handler_entry::range *m_u_ranges;

	void populate_nomirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write<Width, AddrShift> *handler);
	void populate_mirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write<Width, AddrShift> *handler);

	void populate_mismatched_nomirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, const memory_units_descriptor<Width, AddrShift> &descriptor, u8 rkey, std::vector<mapping> &mappings);
	void populate_mismatched_mirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, const memory_units_descriptor<Width, AddrShift> &descriptor, std::vector<mapping> &mappings);
	void mismatched_patch(const memory_units_descriptor<Width, AddrShift> &descriptor, u8 rkey, std::vector<mapping> &mappings, handler_entry_write<Width, AddrShift> *&target);

	void populate_passthrough_nomirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, handler_entry_write_passthrough<Width, AddrShift> *handler, std::vector<mapping> &mappings);
	void populate_passthrough_mirror_subdispatch(offs_t entry, offs_t start, offs_t end, offs_t ostart, offs_t oend, offs_t mirror, handler_entry_write_passthrough<Width, AddrShift> *handler, std::vector<mapping> &mappings);
	void passthrough_patch(handler_entry_write_passthrough<Width, AddrShift> *handler, std::vector<mapping> &mappings, handler_entry_write<Width, AddrShift> *&target);
};

#endif // MAME_EMU_EMUMEM_HEDW_H
