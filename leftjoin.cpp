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
#include "IntervalTree.h"
using namespace std;
typedef IntervalTree<int, string> ITree;

vector<int> split_to_vectorint(string s) {
    vector<int> v;
    istringstream ss(s);
    string token;

    while (getline(ss,token,',')) v.push_back(atoi(token.c_str()));

    return v;
}

void interval_data(string s, vector<int> intervals, ITree::interval_vector &dest, int &sizes) {
    
    istringstream ss(s);
    string token, value;
    int i;

    vector<int> coordinates;

    for (i=0; getline(ss,token,'\t');i++) { //iterating each column...

        if ( find(intervals.begin(), intervals.end(), i+1) != intervals.end()) { // if (i+1) is an index
            coordinates.push_back(atoi(token.c_str()));
        } else { // if data..
            if (value.size()>0)
                value.append("\t");
            value.append(token);
        }
    }

    switch (coordinates.size()) {
        case 1:
            coordinates.push_back(coordinates.front());
            break;
        case 2:
            break;
        default:
            cerr << "Error: incorrect coordinate size: "<< coordinates.size() << "..." << endl;
            exit(1);
    }

    dest.push_back(ITree::interval(coordinates.front(), coordinates.back(),
                value));
    sizes=i+1;
}

// This will write the data in "string", using the key indices in "keys" to 
// "dict", and write the data size to "sizes"
void key_data(string s, vector<int> keys, auto &dict, int &sizes) {

    istringstream ss(s);
    string token, key, value;
    int i;

    for (i=0; getline(ss,token,'\t');i++) {
        if ( find(keys.begin(), keys.end(), i+1) != keys.end()) { // if (i+1) is a key
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
    vector<vector<int>> intervals;
    vector<int> tmpvector;
    int c;   
    string line;
      while ((c = getopt (argc, argv, "i:k:")) != -1)
        switch (c)
          {
          case 'i':
                intervals.push_back(split_to_vectorint(optarg));
                break;
          case 'k':
                keys.push_back(split_to_vectorint(optarg));
                break;
            break;
          case '?':
            cerr<<"Unknown option `-"<<optopt<<"'.\n";
            return 1;
          default:
            abort ();
          }

      if (argc-optind < 2) {
        cerr << "Error: Insufficient arguments specified" << endl;
        exit(1);
      }

      if (!keys.empty() && keys.size()!= 1 && keys.size()!=(uint)(argc-optind)) {
          cerr << "Error: Incorrect number of keys specified"<< endl;
        return 1;
      }

    if (!keys.empty() && !intervals.empty()) {

        cerr << "Error: Both -i and -k are set. These are mutually exclusive" << endl;
        cerr << keys.size() << endl; 
        exit(1);
    }

    int sizes[argc-optind];

    if (!intervals.empty())  {
       
        if (argc-optind != 2) {
            cerr << "Error: -i only supports exactly two files" << endl;
            exit(1);
        }
 
        ITree::interval_vector leftmost;
        ITree::interval_vector rightmost;
        ITree rightmost_tree;
        
        for (int i=0; i<2; i++) {
              ifstream infile(argv[optind+i]);

              if (!infile) {
                cerr << " Error: could not open file " << endl;
              }

              while (getline(infile,line)) {
                if (intervals.size()>1) {
                    if (i==0)
                        interval_data(line, intervals[i], leftmost, sizes[i]);
                    else
                        interval_data(line, intervals[i], rightmost, sizes[i]);
                } else {
                    if (i==0)
                        interval_data(line, intervals[0], leftmost, sizes[i]);
                    else
                        interval_data(line, intervals[0], rightmost, sizes[i]);
                }
              }
        }

    rightmost_tree = ITree(std::move(rightmost));
    ITree::interval_vector results;

     for (auto it: leftmost) {
        results = rightmost_tree.findOverlapping(it.start,it.stop);
        for (auto r: results) {
            cout << it.start << "\t" << it.stop << "\t" << it.value 
                 << "\t" << r.value << endl;
        }
        if (results.empty()) {
            cout << it.start << "\t" << it.stop << "\t" << it.value;
            for (int i=0;i<sizes[0]; i++)
                cout << "\tNULL";
            cout << endl;
        }
     }
    } else { 
        map<string,string> leftmost; 
        unordered_map<string,string> others[argc-optind-1];

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
    }

    return 0;

}
