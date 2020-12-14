#include<pthread.h>
#include<iostream>
#include<string>
#include<stdlib.h>
#include<vector>
#include<fstream>
#include <stdio.h> 
#include <string.h> 
#include<bits/stdc++.h>
using namespace std;

vector<float> split(char* str){
    vector<float> data;
    char *token = strtok(str, ",");
    while (token != NULL) 
    { 
        data.push_back(stof(token));
        token = strtok(NULL, ","); 
    } 
    return data;
}

float calc_weight(vector<float> weight, vector<float> data){
    float sum = 0;
    for(int i = 0; i < weight.size() - 1; i++){
        sum += weight[i]*data[i];
    }
    sum += weight[weight.size()-1];
    return sum;
}

int find_price_range(vector<vector<float>> weights, vector<float> data){
    int price_range = 0;
    float max_weigh = - 10e10;
    for(int i=0; i<weights.size(); i++){
       float weight = calc_weight(weights[i], data);
       if(weight >= max_weigh){ // tavajooh
            max_weigh = weight;
            price_range = i;
       }
    }
    return price_range;
}

float calc_accuracy(vector<vector<float>> weights, vector<vector<float>> data){
    int correct = 0;
    for(int i = 0; i< data.size(); i++){
        int range = find_price_range(weights, data[i]);
        if(range == data[i][data[i].size() - 1])
            correct++;
    }
    return (float)correct/(float) data.size();
}

void normalize(vector<vector<float>> & data){
    for(int i = 0; i < data[0].size() - 1; i++){
        float max = -10e10, min=  10e10;
        for(int j = 0; j< data.size(); j++){
            if (data[j][i] < min)
                min = data[j][i];
            if (data[j][i] > max)
                max = data[j][i];
        }
        for(int j = 0; j< data.size(); j++){
            data[j][i] = (data[j][i] - min)/(max - min);
        }
     }
}

int main(int argc, char *argv[] ){
    vector<vector<float>> weights, data;
    string dir = argv[1];
    string weights_file = dir + "weights.csv";
    string train_file = dir + "train.csv";
    fstream weights_input, train_input;
    weights_input.open(weights_file);
    train_input.open(train_file);
    string input;
    getline(weights_input, input);
    string new_data;
    while(getline(weights_input, input))
        weights.push_back(split((char*)input.c_str()));
    getline(train_input, input);
    while(getline(train_input, input))
        data.push_back(split((char*)input.c_str()));
    normalize(data);
    printf("Accuracy: %.2f", calc_accuracy(weights, data)*100);
    cout<<"%"<<endl;
}