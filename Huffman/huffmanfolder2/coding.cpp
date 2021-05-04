
#include "huffman.h"
#include "huffmandec.h"
#include <fstream>
#include<sstream>
#include <string.h>
#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

static const char alphanum[] =
    "0123456789"
    "!@#$%^&*"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

int stringLength = sizeof(alphanum) - 1;


void huffman::create_node_array()
{
    for (int i = 0; i < 128; i++)
    {
        node_array[i] = new huffman_node;
        node_array[i]->id = i;
        node_array[i]->freq = 0;
    }
}
void huffmandec::create_dnode_array()
{
    for (int i = 0; i < 128; i++)
    {
        dnode_array[i] = new huffmand_node;
        dnode_array[i]->id = i;
        dnode_array[i]->freq = 0;
    }
}

void huffman::traverse(node_ptr node, string code)
{
    if (node->left == NULL && node->right == NULL)
    {
        node->code = code;
        cout<<"\nCharacter:"<<node->id;
        cout<<"\tCode:"<<node->code;
    }
    else
    {
        traverse(node->left, code + '0');
        traverse(node->right, code + '1');
    }
}
void huffmandec::dtraverse(dnode_ptr node, string code)
{
    if (node->left == NULL && node->right == NULL)
    {
        node->code = code;

    }
    else
    {
        dtraverse(node->left, code + '0');
        dtraverse(node->right, code + '1');
    }
}

int huffman::binary_to_decimal(string in)
{
    int result = 0;
    for (int i = 0; i < in.size(); i++)
        result = result * 2 + in[i] - '0';
    return result;
}

string huffmandec::decimal_to_binary(int in)
{
    string temp = "";
    string result = "";
    while (in)
    {
        temp += ('0' + in % 2);
        in /= 2;
    }
    result.append(8 - temp.size(), '0');													//append '0' ahead to let the result become fixed length of 8
    for (int i = temp.size() - 1; i >= 0; i--)
    {
        result += temp[i];
    }
    return result;
}

inline void huffmandec::build_tree(string& path, char a_code)
{
    //build a new branch according to the input code and ignore the already existed branches
    dnode_ptr current = droot;
    for (int i = 0; i < path.size(); i++)
    {
        if (path[i] == '0')
        {
            if (current->left == NULL)
                current->left = new huffmand_node;
            current = current->left;
        }
        else if (path[i] == '1')
        {
            if (current->right == NULL)
                current->right = new huffmand_node;
            current = current->right;
        }
    }
    current->id = a_code;																	//attach id to the leaf
}

huffman::huffman(vector<string> in, string out, int c)
{
    for (int i=0; i<c; i++)
    {
        in_file_name.push_back(in[i]);
    }
    out_file_name = out;
    files=c;
    create_node_array();
}
huffmandec::huffmandec(string in)
{
    in_file_name2=in;
    create_dnode_array();
}

void huffman::create_pq()
{

    for(int j=0; j<files; j++)
    {
        int n = in_file_name[j].length();
        char in_array[n + 1];
        strcpy(in_array, in_file_name[j].c_str());
        in_file.open(in_array, std::fstream::in);
        in_file.get(id);
        while (!in_file.eof())
        {
            node_array[id]->freq++;
            in_file.get(id);
        }
        in_file.close();

    }
    for (int i = 0; i < 128; i++)
    {
        if (node_array[i]->freq)
        {

            pq.push(node_array[i]);

        }
    }

}


void huffman::create_huffman_tree()
{
    priority_queue<node_ptr, vector<node_ptr>, compare> temp(pq);
    while (temp.size() > 1)
    {
        //create the huffman tree with highest frequecy characher being leaf from bottom to top
        root = new huffman_node;
        root->freq = 0;
        root->left = temp.top();
        root->freq += temp.top()->freq;
        temp.pop();
        root->right = temp.top();
        root->freq += temp.top()->freq;
        temp.pop();
        temp.push(root);
    }
}


void huffman::calculate_huffman_codes()
{
    traverse(root, "");
}


void huffman::coding_save()
{
    string in = "", s = "";

    in += (char)pq.size();																	//the first byte saves the size of the priority queue
    priority_queue<node_ptr, vector<node_ptr>, compare> temp(pq);
    while (!temp.empty())
    {
        //get all characters and their huffman codes for output
        node_ptr current = temp.top();
        in += current->id;
        s.assign(127 - current->code.size(), '0');//set the codes with a fixed 128-bit string form[000¡­¡­1 + real code]
        s += '1';//'1' indicates the start of huffman code
        s.append(current->code);
        in += (char)binary_to_decimal(s.substr(0, 8));
        for (int i = 0; i < 15; i++)
        {
            //cut into 8-bit binary codes that can convert into saving char needed for binary file
            s = s.substr(8);
            in += (char)binary_to_decimal(s.substr(0, 8));
        }
        temp.pop();
    }


    for(int m=0; m<files; m++)
    {
        s.clear();
        int n = in_file_name[m].length();
        char in_array[n + 1];
        strcpy(in_array, in_file_name[m].c_str());
        in_file.open(in_array, std::fstream::in);

        in_file.get(id);
        while (!in_file.eof())
        {
            //get the huffman code
            s += node_array[id]->code;
            while (s.size() > 8)
            {
                //cut into 8-bit binary codes that can convert into saving char needed for binary file
                in += (char)binary_to_decimal(s.substr(0, 8));
                s = s.substr(8);
            }
            in_file.get(id);
        }
        int count = 8 - s.size();
        if (s.size() < 8)
        {
            //append number of 'count' '0' to the last few codes to create the last byte of text
            s.append(count, '0');
        }
        in += (char)binary_to_decimal(s);															//save number of 'count' at last
        in += (char)count;
        in+="eof";
        in_file.close();
    }

    int n2 = out_file_name.length();
    char out_array[n2 + 1];
    strcpy(out_array, out_file_name.c_str());
    out_file.open(out_array, ios::out|ios::binary);
    out_file.write(in.c_str(), in.size());
    out_file.close();
}

void huffmandec::recreate_huffman_tree()
{
    int n = in_file_name2.length();
    char in_array[n + 1];
    strcpy(in_array, in_file_name2.c_str());
    din_file.open(in_array, ios::in| ios::binary);
    unsigned char size;//unsigned char to get number of node of huffman tree
    din_file.read(reinterpret_cast<char*>(&size), 1); // read first string of the file and save it in size

    droot = new huffmand_node;
    for (int i = 0; i < size; i++)
    {
        char a_code;
        unsigned char h_code_c[16];																//16 unsigned char to obtain the binary code
        din_file.read(&a_code, 1);

        din_file.read(reinterpret_cast<char*>(h_code_c), 16);
        string h_code_s = "";
        for (int i = 0; i < 16; i++)
        {
            //obtain the original 128-bit binary string
            h_code_s += decimal_to_binary(h_code_c[i]);
        }
        int j = 0;
        while (h_code_s[j] == '0')
        {
            //delete the added '000¡­¡­1' to get the real huffman code
            j++;
        }
        h_code_s = h_code_s.substr(j + 1);
        build_tree(h_code_s, a_code);
    }
    din_file.close();
}

void huffmandec::decoding_save()
{
    int n = in_file_name2.length();
    char in_array[n + 1];
    strcpy(in_array, in_file_name2.c_str());
    din_file.open(in_array,  ios::in | ios::binary);
    unsigned char size;																		//get the size of huffman tree
    din_file.read(reinterpret_cast<char*>(&size), 1);
    vector<char> countd;
    int cd=0;
    int ch=0;
    vector<int> file;
    int y=-3;
    file.push_back(y);
    din_file.seekg(1 + 17 * size, ios::beg);													//jump to the position where text starts

    vector<unsigned char> text;
    unsigned char textseg;
    din_file.read(reinterpret_cast<char*>(&textseg), 1);
    while (!din_file.eof())
    {
        //get the text byte by byte using unsigned char
        text.push_back(textseg);
        din_file.read(reinterpret_cast<char*>(&textseg), 1);
    }
 for(int j=0; j<text.size(); j++)
    {

        if(text[j]=='e'&&text[j+1]=='o'&&text[j+2]=='f')
        {

            countd.push_back(text[j-1]);
            cd++;
            file.push_back(ch);

        }
ch++;
    }
    din_file.close();

   for(int x=0; x<cd; x++)
    {

        std::string Str;
      for(unsigned int i = 0; i < 5; ++i)
        {
            Str += alphanum[rand() % stringLength];
        }
        string out;
        out=Str + ".txt";
        int n2 = out.length();
        char out_array[n2 + 1];
        strcpy(out_array, out.c_str());
        dout_file.open(out_array, ios::out);

        dnode_ptr current = droot;
        string path;
        for (int i = (0+file[x]+3); i < file[x+1]-1; i++)
        {


//            translate the huffman code

            path = decimal_to_binary(text[i]);
            if (i == file[x+1]-2)
                path = path.substr(0, 8 - countd[x]);
            for (int j = 0; j < path.size(); j++)
            {
                if (path[j] == '0')
                    current = current->left;
                else
                    current = current->right;
                if (current->left == NULL && current->right == NULL)
                {
                    dout_file.put(current->id);
                    current = droot;
                }
            }
      }

//
        dout_file.close();
   }
}
