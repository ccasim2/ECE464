#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <map>
#include <cstdbool>

using namespace std;

// Sort method for non Alphabetic gate names
void Numsort(vector<string> &In) {
  string word, wordNum;
  string word2, wordNum2;
  int k = In.size();
  int i = 0;
  vector<string> temp = In;

  while (i != k) {
    word = temp[i];
    wordNum = word.substr(0,2);
    int temp1,temp2;
    temp1 = stoi(wordNum);
    for (int j = 0; j < k; j++) {
      word2 = temp[j];
      wordNum2 = word2.substr(0,2);
      temp2 = stoi(wordNum2);
      if (temp2 > temp1) {
        temp[j] = word2;
        temp1 = temp2;
      }
    }
    i++;
  }
  //cout << temp.size() << endl;
}

// Separates the gate data
void gateData(string &line, string &gateName, string &gate, string &input) {
  size_t pos = line.find('=');
  gateName = line.substr(0,pos-1);
  size_t pos2 = line.find('(');
  size_t posend = line.find(')');
  int dif = pos2 - pos - 2;
  gate = line.substr(pos + 1);
  gate.erase(gate.begin());
  int end = posend - pos2 - 1;
  input = line.substr(pos2+1, end);
  gate = gate.substr(0, dif);
}

// Deciphers the total amount of inputs for the gate
void numInputs(int &inputSize, string gateInput) {
  size_t pos3 = gateInput.find(',');
  if (pos3 != string::npos) {
    pos3 = gateInput.find(',', pos3+1);
    if (pos3 == string::npos) {
      inputSize = 2;
    } else {
      inputSize = 3;
    }
  } else {
    inputSize = 1;
  }
}

// Checks the type of gate and prints it
void gatesList(list<string> &gates, vector<string> &gatelist) {
  int i = gates.size() - 1;
  int n = 0;
  bool junk = true;
  while (i != -1) {
    string line = gates.front();
    while (junk == true) {
      if (line == "" || line[0] == '#') {
        gates.pop_front();
        line = gates.front();
        i--;
      } else {
        junk = false;
      }
    }

    string gateName,gateType,gateInput;
    gateData(line, gateName, gateType, gateInput);
    gatelist.push_back(line);
    numInputs(n,gateInput);

    // Checks for total amount of inputs into each gate
    cout << n << " Input " << gateType << " gate with input " << gateInput << endl;
    gates.pop_front();
    i--;
  }
}

// Puts each gate into a level based on the inputs
void levelizer(string line, map<int,vector<string>> &levels, int maxLevel, int currentLevel, vector<string> &remaiderData) {
  vector<string> level0, nextLevel, totGateInputs;
  string gateName, gate, input, gateInputs, restInputs;
  int size = 0;
  size_t pos;
  numInputs(size,line);
  gateData(line, gateName, gate, input);
  level0 = levels[0];

  // Separates the inputs into a vector for easier parsing
  for (int i = 0; i < size; i++) {
    pos = input.find(',');
    gateInputs = input.substr(0,pos);
    if (pos != string::npos) {
      restInputs = input.substr(pos);
      restInputs.erase(0,2);
      input = restInputs;
    }
    totGateInputs.push_back(gateInputs);
  }
  
  // checks for current level
  int comp = 0;
  for (int i = 0; i < totGateInputs.size(); i++) {
    for (int j = 0; j < level0.size(); j++) {
      if (totGateInputs[i] == level0[j]) {
        comp++;
        break;
      }
    }
  }

  if (maxLevel > 1) {
    for (int k = 1; k < maxLevel; k++) {
      level0 = levels[k];
      for (int i = 0; i < totGateInputs.size(); i++) {
        for (int j = 0; j < level0.size(); j++) {
          if (totGateInputs[i] == level0[j]) {
            comp++;
          }
        }
      }
    }
  }

  if (comp == size) {
    levels[maxLevel].push_back(gateName);
  } else {
    remaiderData.push_back(line);
  }
}

// Prints the map containing the levels
void printMap(map<int, vector<string>> temp) {
  for (auto a = temp.begin(); a != temp.end(); a++) {
    cout << endl << "Level " << (*a).first << ": ";
    vector<string> inputLevel = (*a).second;
    for (int j = 0; j < inputLevel.size(); j++) {
      cout << inputLevel[j] << " ";
    }
    cout << endl;
  }
}

void printfaults(map<int, vector<string>> temp) {
  bool first=true;
  string placeholder;
  cout<<"====================="<<endl;
  for (auto a = temp.begin(); a != temp.end(); a++) {
    if((*a).first==0){//first col
      cout << endl << "Faults at inputs: " <<endl;
      placeholder="Input";
    }
    else if((*a).first==(*temp.end()).first){//last col
      cout << endl << "Faults at outputs: " <<endl;
      placeholder="Output";
    }
    else{
      if(first){
      cout<<endl<< "faults at internal nodes:"<<endl;       
      first=false;
      placeholder="Internal node";
      }
    }
    vector<string> inputLevel = (*a).second;
    for (int j = 0; j < inputLevel.size(); j++) {
      cout << endl<<placeholder<<" "<<inputLevel[j]<< " can be stuck at 0/1."<<endl;
    }
    cout << endl;
  }
  
}

// Gets the value of a specific gate
void gateOutcome (string gateName, map<string,string> &temp, vector<string> gateInputData) {
  int code;
  int neg;
  string gateType = gateInputData[0];
  char negCheck = gateType[0];
  vector<string> inputs = gateInputData;
  
  // Checks if gate is a NAND, NOR, XNOR
  if ((gateType[0] == 'N' || gateType[2] == 'O') && (gateType != "NOT")) {
    neg = 1;
    size_t pos = gateType.find('N');
    if (pos == 1) {
      gateType = "XOR";
    } else {
      gateType = gateType.substr(pos+1);
    }
  }

  if (gateType == "AND") {
    code = 1;
  } else if (gateType == "OR") {
    code = 2;
  } else if (gateType == "NOT") {
    code = 3;
  } else if (gateType == "XOR") {
    code = 4;
  }

  switch (code) {
    case 1:
      //cout << "AND" << endl;
      for (int i = 1; i < inputs.size(); i++) {
        if (temp[inputs[i]] != "1") {
          temp[gateName] = "0";
          if (neg == 1) {
            temp[gateName] = "1";
          }
          break;
        }
        temp[gateName] = "1";
        if (neg == 1) {
          temp[gateName] = "0";
        }
      }
      break;
    
    case 2:
      //cout << "OR" << endl;
      for (int i = 1; i < inputs.size(); i++) {
        if (temp[inputs[i]] == "1") {
          temp[gateName] = "1";
          if (neg == 1) {
            temp[gateName] = "0";
          }
          break;
        }
        temp[gateName] = "0";
        if (neg == 1) {
          temp[gateName] = "1";
        }
      }
      break;

    case 3:
      //cout << "NOT" << endl;
      if (temp[inputs[1]] == "1") {
        temp[gateName] = "0";
      } else {
        temp[gateName] = "1";
      }
      break;

    case 4:
      int totalOnes = 0;
      //cout << "XOR" << endl;
      for (int i = 1; i < inputs.size(); i++) {
        if (temp[inputs[i]] == "1") {
          totalOnes++;
        }
      }
      if (totalOnes%2 == 0) {
        temp[gateName] = "0";
        if (neg == 1) {
          temp[gateName] = "1";
        }
      } else {
        temp[gateName] = "1";
        if (neg == 1) {
          temp[gateName] = "0";
        }
      }
      break;
  }

}

void gateoutfull(map<int, vector<string>> temp,map<string,string> &temps, map<string,vector<string>> &gateInputData){
  bool first=true;
  for (auto a = temp.begin(); a != temp.end(); a++) {
    if (first){
      first=false;
      continue;
    }
    cout<<endl<<"level "<<(*a).first;
    vector<string> inputLevel = (*a).second;
    for (int j = 0; j < inputLevel.size(); j++) {
      cout<<"inputlevel[j]: "<<inputLevel[j]<<endl;
      gateOutcome(inputLevel[j],temps,gateInputData[inputLevel[j]]);
    }
    cout << endl;
  }
}

// Puts the gate inputs, type and name into one data structure
void gateMapValues(map<string,vector<string>> &gateMapData, vector<string> gatesList) {
  string gateName, gate, input, gateInputs, restInputs;
  int size = 0;
  size_t pos;
  vector<string> data;
  for (int i = 0; i < gatesList.size(); i++) {
    gateData(gatesList[i], gateName, gate, input);
    data.push_back(gate);
    numInputs(size,gatesList[i]);
    for (int j = 0; j < size; j++) {
      pos = input.find(',');
      gateInputs = input.substr(0,pos);
      if (pos != string::npos) {
        restInputs = input.substr(pos);
        restInputs.erase(0,2);
        input = restInputs;
      }
      data.push_back(gateInputs);
    }
    gateMapData[gateName] = data;
    data.clear();
  }
}

int main() {
  // Only used for the file reading
  ifstream Myfile;
  Myfile.open("hw1.bench");
  vector<string> inputs;
  vector<string> outputs;
  
  // Only used in file reading and will be empty at the end
  list<string> gates;

  // Will contain all relevant data for future uses
  vector<string> gatelist;
  map<string,string> gatevalue;
  map<int,vector<string>> levelization;
  map<string,vector<string>> gateMap;

  // Copies the data from the file into a vectors
  if (Myfile.is_open()) {
    string word;
    while (getline(Myfile, word)) {
      if (word[0] == 'I') {
        size_t pos = word.find('(');
        word = word.substr(pos+1);
        pos = word.find(")");
        word.erase(pos);
        inputs.push_back(word);
      } else if (word[0] == 'O') {
        size_t pos = word.find('(');
        word = word.substr(pos+1);
        pos = word.find(")");
        word.erase(pos);
        outputs.push_back(word);
      } else {
        gates.push_back(word);
      }
    }
    Myfile.close();
  }

  // Prints the vectors
  cout << "Inputs: ";
  for (int i = 0; i < inputs.size(); i++) {
    cout << inputs[i] << " ";
    levelization[0].push_back(inputs[i]);
  }

  cout << endl << "Outputs: ";
  for (int i = 0; i < outputs.size(); i++) {
    cout << outputs[i] << " ";
  }
  cout << endl << "Gates: " << endl;
  
  gatesList(gates,gatelist);

  // Sort
  string temp = gatelist[0];
  if (isdigit(temp[0])) {
    Numsort(gatelist);
  } else {
    sort(gatelist.begin(), gatelist.end());
  }

  // Levelizer
  int currLevel = 0;
  int totLev = 1;
  int k = gatelist.size();
  vector<string> temp4 = gatelist;
  vector<string> temp5;
  while (k != -1) {
    levelizer(temp4[0],levelization,totLev,currLevel, temp5);
    temp4.erase(temp4.begin());
    if (temp4.size() == 0) {
      temp4 = temp5;
      k = temp5.size();
      totLev++;
      currLevel++;
      temp5.erase(temp5.begin(),temp5.end());
    }
    k--;
  }

  // Wire values map setting
  for (auto a = levelization.begin(); a != levelization.end(); a++) {
    vector<string> inputLevel = (*a).second;
    for (int j = 0; j < inputLevel.size(); j++) {
      gatevalue[inputLevel[j]] = "X";
    }
  }

  printMap(levelization);
  printfaults(levelization);
  gateMapValues(gateMap,gatelist);

  // Testing set the initial conditions for the inputs
  gatevalue["a"] = "1";
  gatevalue["b"] = "0";
  gatevalue["b'"] = "1";
  gatevalue["c"] = "1";
  //gatevalue["6"] = "0";
  //gatevalue["7"] = "1";
  //gateOutcome("d", gatevalue, gateMap["d"]); 
  // gateOutcome("c'", gatevalue, gateMap["c'"]);
  // gateoutfull(levelization,gatevalue,gateMap);

  // just being used to print the entire map
  for (auto a = gateMap.begin(); a != gateMap.end(); a++) {
    string gateN = (*a).first;
    gateOutcome(gateN, gatevalue, gateMap[gateN]);
  }

  for (auto a = gatevalue.begin(); a != gatevalue.end(); a++) {
    cout << (*a).first << " " << (*a).second << endl;
  }
  
  return 0;
}