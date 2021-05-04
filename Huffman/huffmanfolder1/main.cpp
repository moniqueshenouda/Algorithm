#include <iostream>
#include <stdlib.h>
#include "huffman.h"
#include "huffmandec.h"
#include <unistd.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <iomanip>

using namespace std;

vector<string> v;
int ct=-2;


void read_directory(const std::string& name)
{
    DIR* dirp = opendir(name.c_str());
    struct dirent * dp;
    while ((dp = readdir(dirp)) != NULL)
    {
        v.push_back(dp->d_name);
        ct++;
    }
    closedir(dirp);
}
int main()
{

    int ch;
    cout<< "For compression choose 1, for decompression choose 2 :";
    cin>>ch;
    if(ch==1)
    {
        std::string name="C:\\Users\\User\\Desktop\\hfold\\text";
        vector<string> in;

        read_directory(name);
        for(int m=2; m<=ct+1; m++)
        {
            in.push_back(name + "/" + v[m]);
        }
        string out;
        out="out.txt";
        int file_size=0;
        const clock_t begin_time = clock();
        huffman h(in,out,ct);
        cout<<"here";
        h.create_pq();
        cout<<"here";
        h.create_huffman_tree();
        cout<<"here";
        h.calculate_huffman_codes();
        cout<<"here";
        h.coding_save();
        cout<<"here";
        for(int i=0; i<in.size(); i++)
        {
            int n = in[i].length();
            char in_array[n + 1];
            strcpy(in_array, in[i].c_str());
            std::ifstream in_file(in_array, std::ios::binary | std::ios::ate);
            file_size+= in_file.tellg();

        }
        std::ifstream in_file2("out.txt", std::ios::binary | std::ios::ate);
        int file_size2 = in_file2.tellg();
        cout << "\nCompression ratio : ";
        cout << std::setprecision(5) << (float)file_size/file_size2 << endl;
        cout << ":1";
        cout <<"\nExecution time:";
        cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
        cout << endl;
    }
    else if(ch==2)
    {
        string indec;
        cout << "\nName your file> ";
        cin>>indec;
        ifstream file( indec.c_str() );
        if (!file)
        {
            cout << "I could not open the file.\n";
            return EXIT_FAILURE;
        }
        else
        {
            const clock_t begin_time = clock();
            huffmandec h2(indec);
            h2.recreate_huffman_tree();
            h2.decoding_save();
            cout <<"\nExecution time:";
            cout << float( clock () - begin_time ) /  CLOCKS_PER_SEC;
            cout << endl;
        }
    }
    else
    {
        cout<<"Wrong Entry";
    }


    return 0;
}
