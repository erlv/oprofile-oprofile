/**
 * @file op_to_source_options.cpp
 * Annotated source output command line options
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Philippe Elie <phil_el@wanadoo.fr>
 * @author John Levon <moz@compsoc.man.ac.uk>
 */

#include "op_to_source_options.h"
#include "popt_options.h"

using std::vector;
using std::string;
using std::cerr;
using std::endl;

namespace options {
	int with_more_than_samples;
	int until_more_than_samples;
	int sort_by_counter = -1;
	string source_dir;
	string output_dir;
	string output_filter;
	string no_output_filter;
	bool assembly;
	bool source_with_assembly;
	string samplefile;
	string imagefile;
	bool verbose;
	bool demangle;
	vector<string> exclude_symbols;
};

namespace {
 
option options_array[] = {
	option(options::samplefile, "samples-file", 'f', "image sample file", "file"),
	option(options::imagefile, "image-file", 'i', "image file", "file"),
	option(options::verbose, "verbose", 'V', "verbose output"),
	option(options::demangle, "demangle", 'd', "demangle GNU C++ symbol names"),
	option(options::with_more_than_samples, "with-more-than-samples", 'w',
		"show all source file if the percent of samples in this file is more than argument", "[0-100]"),
	option(options::until_more_than_samples, "until-more-than-samples", 'm',
		"show all source files until the percent of samples specified is reached", "[0-100]"),
	option(options::sort_by_counter, "sort-by-counter", 'c', "sort by counter", "counter nr"),
	option(options::source_dir, "source-dir", '\0', "source directory", "directory name"),
	option(options::output_dir, "output-dir", '\0', "output directory", "directory name"),
	option(options::output_filter, "output", '\0', "output filename filter", "filter string"),
	option(options::no_output_filter, "no-output", '\0', "no output filename filter", "filter string"),
	option(options::assembly, "assembly", 'a', "output assembly code"),
	option(options::source_with_assembly, "source-with-assembly", 's', "output assembly code mixed with source"),
	option(options::exclude_symbols, "exclude-symbol", 'e', "exclude these comma separated symbols", "symbol_name"),
};

}

/**
 * get_options - process command line
 * @param argc program arg count
 * @param argv program arg array
 *
 * Process the arguments, fatally complaining on error.
 */
string const get_options(int argc, char const * argv[])
{
	string arg;

	parse_options(argc, argv, arg);

	if (options::with_more_than_samples
		&& options::until_more_than_samples) {
		cerr << "op_to_source: --with-more-than-samples and "
			<< "--until-more-than-samples can not be "
			<< "specified together" << endl;
		exit(EXIT_FAILURE);
	}

	if (options::output_filter.empty())
		options::output_filter = "*";

	return arg;
}
