
#ifndef HUFFMANDEC_H
#define HUFFMANDEC_H
#include <string>
#include <queue>
#include <vector>
#include <fstream>
using namespace std;

struct huffmand_node
{
	char id;																				//character
	int freq;																				//frequency of the character
	string code;																			//huffman code for the character
	huffmand_node* left;
	huffmand_node* right;
	huffmand_node()
	{
		left = right = NULL;
	}
};
typedef huffmand_node* dnode_ptr;

class huffmandec
{
protected:
	dnode_ptr dnode_array[128];																//array for 128characters in the Ascii Table
	fstream din_file, dout_file;
	dnode_ptr dchild, dparent, droot;
	char did;
	string in_file_name2;
	class compare
	{//a object funtion to set comparing rule of priority queue
	public:
		bool operator()(const dnode_ptr& c1, const dnode_ptr& c2) const
		{
			return c1->freq > c2->freq;
		}
	};
	priority_queue<dnode_ptr, vector<dnode_ptr>, compare> dpq;									//priority queue of frequency from high to low
	void create_dnode_array();
	void dtraverse(dnode_ptr, string);														//traverse the huffman tree and get huffman code for a character
	string decimal_to_binary(int);															//convert a decimal integer to a 8-bit 0/1 string of binary code
	inline void build_tree(string&, char);													//build the huffman tree according to information from file

public:
	huffmandec (string);
	void decoding_save();
	void recreate_huffman_tree();
};

#endif


