#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <algorithm>
#include <map>
#include <unordered_map>
using namespace std;

vector<int> split_to_vectorint(string s) {
    vector<int> v;
    istringstream ss(s);
    string token;

    while (getline(ss,token,',')) v.push_back(atoi(token.c_str()));

    return v;
}

void key_data(string s, vector<int> keys, auto &dict, int &sizes) {

     vector<int> v;
    istringstream ss(s);
    string token, key, value;
    int i;

    for (i=0; getline(ss,token,'\t');i++) {
        if ( find(keys.begin(), keys.end(), i+1) != keys.end()) { // if a key
            if (key.size()>0)
                key.append("\t");
            //cout << "appending '" << token << "'"<< endl;
            key.append(token);
        } else { // if data..
            if (value.size()>0)
                value.append("\t");
            value.append(token);
        }
    }

    auto it = dict.find(key);
    if (it != dict.end()) {
        cerr << "Error... key '"<<key<<"' already exists"<<endl;
        exit(1);
    }

    dict[key]=value;
    sizes=i+1;
}
int main (int argc, char *argv[]) {

    vector<vector<int>> keys;
    vector<int> tmpvector;
    int c;   
    string line;
      while ((c = getopt (argc, argv, "k:")) != -1)
        switch (c)
          {
          case 'k':
                keys.push_back(split_to_vectorint(optarg));
            break;
          case '?':
            cerr<<"Unknown option `-"<<optopt<<"'.\n";
            return 1;
          default:
            abort ();
          }

      if (keys.size()!= 1 && keys.size()!=(uint)(argc-optind)) {
          cerr << "Error... incorrect number of keys specified"<< endl;
        return 1;
      }

    map<string,string> leftmost; 
    unordered_map<string,string> others[argc-optind-1];
    int sizes[argc-optind];
      for (int i=0; i<argc-optind; i++) {
          ifstream infile(argv[optind+i]);

          if (!infile) {
            cerr << " Error: could not open file " << endl;
          }

          while (getline(infile,line)) {
            if (keys.size()>1) {
                if (i==0)
                    key_data(line, keys[i],leftmost,sizes[i]);
                else
                    key_data(line, keys[i],others[i-1],sizes[i]);
            } else  {
                if (i==0)
                    key_data(line, keys[0],leftmost,sizes[i]);
                else
                    key_data(line, keys[0],others[i-1],sizes[i]);
            }
          }
      }

      for (auto x: leftmost) {
           cout<<x.first<<"\t"<<x.second;
           for (int i=0;i<argc-optind-1;i++) {
               auto it = others[i].find(x.first);
               if (it != others[i].end())
                cout << "\t" << others[i][x.first];
               else
                   for (int j=0;j<sizes[i+1];j++) {
                       if (j>0) cout << "\t";
                    cout << "NULL";
                   }
           }
           cout << endl;
      }
    return 0;
}
