#include <iostream>
#include <stdlib.h>
#include "huffman.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <iomanip>

int main()
{
    string in;
    cout << "Name your file> ";
    getline( cin, in );
    fstream file( in.c_str() );
    if (!file)
    {
        cout << "I could not open the file.\n";
        return EXIT_FAILURE;
    }
    else
    {
        int ch;
        cout<< "\nFor compression choose 1, for decompression choose 2 :";
        cin>>ch;
        if(ch==1)
        {
            int n = in.length();
            char in_array[n + 1];
            strcpy(in_array, in.c_str());
            string out="out.txt";
            const clock_t begin_time = clock();
            huffman h(in, out);
            h.create_pq();
            h.create_huffman_tree();
            h.calculate_huffman_codes();
            h.coding_save();
            std::ifstream in_file(in_array, std::ios::binary | std::ios::ate);
            int file_size = in_file.tellg();
            std::ifstream in_file2("out.txt", std::ios::binary | std::ios::ate);
            int file_size2 = in_file2.tellg();
            cout << "\n\nCompression ratio : ";
            cout << std::setprecision(5) << (float)file_size/file_size2;
			cout << ":1\n";
            cout <<"\nExecution time:";
            cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
            cout << endl;

        }
        else if(ch==2)
        {
            string out="huff2.txt";
            const clock_t begin_time = clock();
            huffman h(in, out);
            h.recreate_huffman_tree();
            h.decoding_save();
            cout <<"\nExecution time:";
            cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
            cout << endl;
        }
        else
        {
            cout<<"Wrong Entry";
        }
    }
    return 0;
}

