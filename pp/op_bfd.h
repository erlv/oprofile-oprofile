/**
 * @file op_bfd.h
 * Encapsulation of bfd objects
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Philippe Elie <phil_el@wanadoo.fr>
 * @author John Levon <moz@compsoc.man.ac.uk>
 */

#ifndef OP_BFD_H
#define OP_BFD_H

#include <bfd.h>

#include <vector>
#include <string>

#include "op_types.h"

class opp_samples_files;
class op_bfd;

/** all symbol vector indexation use this type */
typedef size_t symbol_index_t;
const symbol_index_t nil_symbol_index = symbol_index_t(-1);

/** a symbol description from a bfd point of view. This duplicate
 * information pointed by an asymbol, we need this duplication in case
 * the symbol is an artificial symbol */
class op_bfd_symbol {
	friend class op_bfd;
public:

	op_bfd_symbol(asymbol const * a, u32 value, u32 filepos, u32 sect_vma,
		      u32 size, std::string name)
		:
		bfd_symbol(a),
		symb_value(value),
		section_filepos(filepos),
		section_vma(sect_vma),
		symb_size(size),
		symb_name(name)
		{}
	u32 vma() const { return symb_value + section_vma; }
	u32 value() const { return symb_value; }
	u32 filepos() const { return symb_value + section_filepos; }
	std::string const & name() const { return symb_name; }
	asymbol const * symbol() const { return bfd_symbol; }
	size_t size() const { return symb_size; }

private:
	/// the original bfd symbol, this can be null if the symbol is an
	/// artificial symbol
	asymbol const * bfd_symbol;
	/// the offset of this symbol relative to the begin of the section's
	/// symbol
	u32 symb_value;
	/// the section filepos for this symbol
	u32 section_filepos;
	/// the section vma for this symbol
	u32 section_vma;
	/// the size of this symbol
	size_t symb_size;
	/// the name of the symbol
	std::string symb_name;
};

/** Encapsulation of a bfd object. Simplify open/close of bfd, enumerating
 * symbols and retrieving informations for symbols or vma. */
class op_bfd {
public:
	/**
	 * @param samples a valid samples file associated with this image
	 * @param filename the name of the image file
	 *
	 * All error are fatal.
	 *
	 */
	op_bfd(bool is_kernel, std::string const & filename);

	/** close an opended bfd image and free all related resource. */
	~op_bfd();

	/**
	 * @param sym_idx index of the symbol
	 * @param offset fentry number
	 * @param filename output parameter to store filename
	 * @param linenr output parameter to store linenr.
	 *
	 * retrieve the relevant finename:linenr information for the sym_idx
	 * at offset. If the lookup fail return false. In some case this
	 * function can retrieve the filename and return true but fail to
	 * retrieve the linenr and so can return zero in linenr
	 */
	bool get_linenr(symbol_index_t sym_idx, uint offset, 
			char const * & filename, unsigned int & linenr) const;

	/**
	 * @param sym_idx symbol index
	 * @param start reference to start var
	 * @param end reference to end var
	 *
	 * Calculates the range of sample file entries covered by sym. start
	 * and end will be filled in appropriately. If index is the last entry
	 * in symbol table, all entries up to the end of the sample file will
	 * be used.  After calculating start and end they are sanitized
	 *
	 * All error are fatal.
	 */
	void get_symbol_range(symbol_index_t sym_idx,
			      u32 & start, u32 & end) const;

	/** 
	 * @param symbol the symbol name
	 *
	 * find and return the index of a symbol else return -1
	 */
	symbol_index_t symbol_index(char const * symbol) const;

	/**
	 * sym_offset - return offset from a symbol's start
	 * @param num_symbols symbol number
	 * @param num number of fentry
	 *
	 * Returns the offset of a sample at position num
	 * in the samples file from the start of symbol sym_idx.
	 */
	u32 sym_offset(symbol_index_t num_symbols, u32 num) const;

	/**
	 * @param start reference to the start vma
	 * @param end reference to the end vma
	 *
	 * return in start, end the vma range for this binary object.
	 */
	void get_vma_range(u32 & start, u32 & end) const;

	/**
	 * return the text section filepos if the bfd object is the kernel
	 * image or a module image, else return 0 */
	const u32 get_start_offset() const { return text_offset; }

	/** Returns true if the underlined bfd object contains debug info */
	bool have_debug_info() const;

	/** return the image name of the underlined binary image */
	std::string get_filename() const;

	// sorted vector by vma of interesting symbol.
	std::vector<op_bfd_symbol> syms;

	// nr of samples.
	uint nr_samples;
private:
	// the bfd object.
	bfd *ibfd;

	// vector of symbol filled by the bfd lib.
	asymbol **bfd_syms;
	// image file such the linux kernel need than all vma are offset
	// by this value.
	u32 text_offset;
	// ctor helper
	void open_bfd_image(std::string const & file_name, bool is_kernel);
	bool get_symbols();

	/**
	 * symbol_size - return the size of a symbol
	 * @param sym_idx symbol index
	 */
	size_t symbol_size(symbol_index_t sym_idx) const;

	/**
	 * create an artificial symbol which cover the vma range start, end
	 */
	void create_artificial_symbol(u32 start, u32 end);
};

#endif /* !OP_BFD_H*/