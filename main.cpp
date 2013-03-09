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
	// define command-line options
	po::options_description desc("Allowed options");
	desc.add_options()
		("h", "produce help message")
		("i", po::value<string>(), "input file path")
		("o", po::value<string>(), "output file path")
	;

	// parse the command-line into a map
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
	} catch (std::exception e) { 
		cout << desc; 
		return 1; 
	}

	po::notify(vm);

	if (vm.count("h"))
	{
		cout << desc << endl;
		return 1;
	}

	string infile, outfile;
	HuffTree::unhuff(infile, outfile);

	return 0;
}