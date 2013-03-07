/*
	Created on 2/10/2013

	Author:		Aaron Cohn
	Course:		Advanced Data Structures
	Instructor: Vinh Phat

	Summary: Compresses/decompresses text files using the Huffman Coding 
	Algorithm.
*/

#include "main.h"

int main(int argc, string argv[])
{	
	string infile, outfile;
	bool oops = true;

	do
	{
		string choice = PromptString("huff, or unhuff? ");
		if (choice == "huff")
		{
			oops = false;
			infile = PromptString("File to compress: ");
			huff(infile);
		}
		else if (choice == "unhuff")
		{
			oops = false;
			infile = PromptString("File to uncompress: ");
			outfile = PromptString("Output file: ");
			unhuff(infile, outfile);
		}
		else
			cout << "To huff, or to unhuff? That is the question. Your answer wasn't recognized." << endl;

	} while (oops);

	cout << "Press any key to continue...";
	cin.get();

	return 0;
}

void huff(const string &filename_in)
{
	// Build the huffman tree
	AsciiMap *histogram = fileHistogram(filename_in);
	AsciiHuff *hufftree = asciiMap2Huff(*histogram);
	
	// Make a mapping of characters to variable width codes
	AsciiHuff::CodeMap *huffcodes = hufftree->generateHuffCodes();
	
	// display minimal codes on the screen
	cout << "Huffcodes written to file: " << endl;
	auto begin = huffcodes->begin(); auto end = huffcodes->end();
	for (auto it = begin; it != end; it++)
	{
		auto ch = it->first;
		auto code = it->second;
		cout << (char)ch << " : " << code2str(code) << endl;
	}
	
	// write the file header, which consists of a pre-order traversal of the Huffman Tree
	string outfile = filename_in + ".hf";
	obstream bstream_out(outfile);
	hufftree->writeFileHeader(bstream_out);
	
	// translate the contents of the file using the Huffman codes and write to output file
	ifstream fstream_in(filename_in.c_str(), ios::in);
	if (fstream_in)
	{
		char ch;
		while (fstream_in.get(ch))
		{
			auto length = (*huffcodes)[ch].first;
			auto code = (*huffcodes)[ch].second;
			bstream_out.writebits(length, code);
		}
		int eof_length = (*huffcodes)[257].first;
		int eof_code = (*huffcodes)[257].second;
		bstream_out.writebits(eof_length, eof_code); // pseudo-EOF
		
		fstream_in.close();
		bstream_out.close();
	}
}

void unhuff(const string &infile, const string &outfile)
{
	// read the file header
	ibstream bstream_in(infile);
	AsciiHuffPtr hufftree = AsciiHuff::readFileHeader(bstream_in);
	AsciiHuff::CodeMap *huffcodes = hufftree->generateHuffCodes();


	// display the huffman codes retrieved from the file header
	cout << endl;
	cout << "Huffcodes read from file: " << endl;
	auto begin = huffcodes->begin(); auto end = huffcodes->end();
	for (auto it = begin; it != end; it++)
	{
		auto ch = it->first;
		auto code = it->second;
		cout << (char)ch << " : " << code2str(code) << endl;
	}

	// continue reading the file body from the input stream
	ofstream fstream_out(outfile);
	hufftree->readFileBody(bstream_in, fstream_out);
}

/*	reads the file specified and returns a mapping of ASCII characters to 
	frequencies of occurrence */
AsciiMap* fileHistogram(const string &filename)
{
	ifstream infile(filename.c_str(), ios::in);
	AsciiMap *amap = NULL;
	char ch;

	if (infile)
	{
		amap = new AsciiMap;
		while (infile.get(ch))
			(*amap)[ch] += 1;

		(*amap)[257] = 1; // Pseudo-EOF character
	}
	return amap;
}

AsciiHuffPtr asciiMap2Huff(const AsciiMap &amap)
{	
	typedef	priority_queue<AsciiHuffPtr, vector<AsciiHuffPtr>, AsciiHuffPtrComparer> AsciiHuffPQ;

	AsciiHuffPQ apq;
	AsciiHuffPtr result = NULL;
	
	// create the initial "forest" of single-node trees
	auto begin = amap.begin(); auto end = amap.end();
	for (auto it = begin; it != end; it++)
	{
		int key = it->second;
		int val = it->first;

		// store a pointer to each tree in the priority queue
		apq.push(new AsciiHuff(key, val));
	}
	
	// build a minimal encoding tree using Huffman's algorithm:
	// Repeat until 1 tree remains in the queue:
	//		pop off the two trees with the minimum keys
	//		merge them together, with the root being the sum of the keys
	//		put the new tree back into the queue.
	while (apq.size() > 1)
	{
		auto u = apq.top();
		apq.pop();
		
		auto v = apq.top();
		apq.pop();
		
		auto z = join(*u, *v);
		apq.push(z);
		
		delete u;
		delete v;
	}

	// the last item in the queue is the final huffman tree
	if (apq.size() == 1)
		result = apq.top();

	return result;
}

string code2str(const AsciiHuff::CodePair &cp)
{
	int length = cp.first;
	int code = cp.second;
	string codestr;
	
	for (int i = 0; i < length; i++)
	{
		if (code & 1) // is right-most bit a 1?
			codestr = "1" + codestr;
		else
			codestr = "0" + codestr;
		code = code >> 1;
	}

	return codestr;
}
