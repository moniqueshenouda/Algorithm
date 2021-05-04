#include "huffman.h"
#include <fstream>
#include<sstream>
#include <string.h>
#include <bitset>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

void huffman::create_node_array()
{
    for (int i = 0; i < 256; i++)
    {
        node_array[i] = new huffman_node;
        node_array[i]->id =(char)i;
        node_array[i]->freq = 0;
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

int huffman::binary_to_decimal(string in)
{
    int result = 0;
    for (int i = 0; i < in.size(); i++)
        result = result * 2 + in[i] - '0';
    return result;
}

string huffman::decimal_to_binary(int in)
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

inline void huffman::build_tree(string& path, uint8_t a_code)
{
    //build a new branch according to the inpue code and ignore the already existed branches
    node_ptr current = root;
    for (int i = 0; i < path.size(); i++)
    {
        if (path[i] == '0')
        {
            if (current->left == NULL)
                current->left = new huffman_node;
            current = current->left;
        }
        else if (path[i] == '1')
        {
            if (current->right == NULL)
                current->right = new huffman_node;
            current = current->right;
        }
    }
    current->id = a_code;
    //attach id to the leaf
}

huffman::huffman(string in, string out)
{
    in_file_name = in;
    out_file_name = out;
    create_node_array();
}
void huffman::create_pq()
{
    int n = in_file_name.length();
    char in_array[n + 1];
    strcpy(in_array, in_file_name.c_str());
    ifstream f(in_array, std::ios::binary);
    std::vector<uint8_t> v(std::istreambuf_iterator<char> {f}, {});
    for(int i=0; i<v.size(); i++)
    {
        id = v[i];
        node_array[id]->freq++;

    }

    for (int i = 0; i < 256; i++)
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
    int n = in_file_name.length();
    char in_array[n + 1];
    strcpy(in_array, in_file_name.c_str());
    ifstream f(in_array, std::ios::binary);
    std::vector<uint8_t> v(std::istreambuf_iterator<char> {f}, {});
    int n2 = out_file_name.length();
    char out_array[n2 + 1];
    strcpy(out_array, out_file_name.c_str());
    out_file.open(out_array, ios::out|ios::binary);
    string in = "", s = "";
    int i=0;
    in += (char)pq.size();
    priority_queue<node_ptr, vector<node_ptr>, compare> temp(pq);
    while (!temp.empty())
    {
        //get all characters and their huffman codes for output
        node_ptr current = temp.top();
        in += current->id;
        s.assign(15 - current->code.size(), '0');
        s += '1';
        s.append(current->code);
        in += (char)binary_to_decimal(s.substr(0, 8));
        for (int i = 0; i < 1; i++)
        {
            s = s.substr(8);
            in += (char)binary_to_decimal(s.substr(0, 8));


        }
        temp.pop();
        i++;
    }
    s.clear();

    for(int i=0; i<v.size(); i++)
    {
        id = v[i];
        s += node_array[id]->code;

        while (s.size() > 8)
        {
            //cut into 8-bit binary codes that can convert into saving char needed for binary file
            in += (char)binary_to_decimal(s.substr(0, 8));
            s = s.substr(8);
        }

    }

    int count = 8 - s.size();
    if (s.size() < 8)
    {
        //append number of 'count' '0' to the last few codes to create the last byte of text
        s.append(count, '0');
    }
    in += (char)binary_to_decimal(s);															//save number of 'count' at last
    in += (char)count;
    out_file.write(in.c_str(), in.size());
    in_file.close();
    out_file.close();

}

void huffman::recreate_huffman_tree()
{
    int n = in_file_name.length();
    char in_array[n + 1];
    strcpy(in_array, in_file_name.c_str());
    ifstream f(in_array, std::ios::binary);
    root = new huffman_node;
    std::vector<uint8_t> v(std::istreambuf_iterator<char> {f}, {});
    int x=v[0];
    if(x<=0)
        x+=256;

    int c=1;
    for (int i = 1; i < x+1; i++)
    {
        uint8_t ac;
        ac=v[c];
        c++;
        string h_code_s = "";
        for (int i = 0; i < 2; i++)
        {
            h_code_s += decimal_to_binary(v[c]);
            c++;
        }
        int j = 0;
        while (h_code_s[j] == '0')
        {
            j++;
        }
        h_code_s = h_code_s.substr(j + 1);

        build_tree(h_code_s, ac);
    }
    in_file.close();
}

void huffman::decoding_save()
{
    int n = in_file_name.length();
    char in_array[n + 1];
    strcpy(in_array, in_file_name.c_str());
    ifstream f(in_array, std::ios::binary);
    std::vector<uint8_t> v(std::istreambuf_iterator<char> {f}, {});
    int n2 = out_file_name.length();
    char out_array[n2 + 1];
    strcpy(out_array, out_file_name.c_str());
    ofstream fout(out_array, ios::out | ios::binary);

    string data="";
    int x=v[0];
    if(x<=0)
        x+=256;

    char count0=v[v.size()-1];

    node_ptr current = root;
    string path;


    for (int i = (1+3*x); i < v.size() - 1; i++)
    {

        path = decimal_to_binary(v[i]);

        if (i == v.size() - 2)
            path = path.substr(0, 8 - count0);


        for (int j = 0; j < path.size(); j++)
        {
            if (path[j] == '0')
            {
                current = current->left;
            }
            else
                current = current->right;
            if (current->left == NULL && current->right == NULL)
            {


                data+=current->id;

                current = root;
            }
        }
    }



    fout<<data;
    in_file.close();
    fout.close();

}



