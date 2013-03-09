/*
	Created on 2/10/2013

	Author:		Aaron Cohn
	Course:		Advanced Data Structures
	Instructor: Vinh Phat

	Summary: Compresses/decompresses text files using the Huffman Coding 
	Algorithm.
*/

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "prompt.h"
#include "hufftree.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char **argv)
{	
	string infile, outfile;

	// define command-line options
	po::options_description desc("Allowed options");
	desc.add_options()
		("h", "produce help message")
		("i", po::value<string>(), "input file path")
		("o", po::value<string>(), "output file path")
	;

	// parse the command-line into a map
	try {
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		if (vm.count("h"))
		{
			cout << desc << endl;
			return 1;
		}
		if (vm.count("i"))
			infile = vm["i"].as<string>();
		if (vm.count("o"))
			outfile = vm["o"].as<string>();
	} 
	catch (std::exception e) { 
		cout << "Error in command line. See description below.\n" 
			 << desc << endl; 
		return 1; 
	}

	if (infile.empty())
		infile = PromptString("Enter path of file to be compressed: ");
	
	if (outfile.empty())
	{	// use same name as infile, replace extension with .hf
		outfile = infile;
		auto dot = outfile.find_last_of('.');
		outfile.replace(dot, infile.length() - 1, ".hf");
	}

	if (!HuffTree::huff(infile, outfile))
	{
		cout << "There was a problem reading the input file.";
		return 1;
	}
	
	return 0;
}