#include<pthread.h>
#include<iostream>
#include<string>
#include<stdlib.h>
#include<vector>
#include<fstream>
#include <stdio.h> 
#include <string.h> 
#include<bits/stdc++.h>
#define THREAD_NUMBER 4
using namespace std;
pthread_mutex_t mut, mut_fin;
struct multiThread
{
    string dir;
    string index;
    vector<vector<float>> weights;
};
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
    }    sum += weight[weight.size()-1];
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

int calc_accuracy(vector<vector<float>> weights, vector<vector<float>> data){
    int correct = 0;
    for(int i = 0; i< data.size(); i++){
        int range = find_price_range(weights, data[i]);

        if(range == data[i][data[i].size() - 1])
            correct++;
    }
    return correct;
}

void normalize(vector<vector<float>> & data){
    float max = -10e10, min=  10e10;
    for(int i = 0; i <  data[0].size()-1; i++){
        max = data[0][i], min=  data[0][i];        
        for(int j = 0; j<data.size() ; j++){
            if (data[j][i] < min)
                min = data[j][i];
            if (data[j][i] > max)
                max = data[j][i];
        }
        for(int j = 0; j< data.size(); j++){
//            cout<<min<<endl;
            data[j][i] = (data[j][i] - min)/(max - min);
        }
     }

}
void * handel(void* args){
    struct multiThread *my = (struct multiThread*) args;
    vector<vector<float>> data;
    fstream train_input;
    train_input.open(my->dir);
    string input;
    string new_data;
    getline(train_input, input);
    int f = 0;
    pthread_mutex_lock (&mut);

    while(getline(train_input, input)){
        data.push_back(split((char*)input.c_str()));
    }

    pthread_mutex_unlock (&mut);
    normalize(data);
    int result[2];
    result[0] = data.size();
    vector<vector<float>> weights (my->weights); 
    result[1] = calc_accuracy(weights, data);
    pthread_exit((void*)result);

}
int main(int argc, char *argv[] ){
    pthread_t threads[THREAD_NUMBER];
    struct multiThread mt[THREAD_NUMBER];

    string data[2];
    data[0] = argv[1];
    string weights_file = data[0] + "weights.csv";
    vector<vector<float>> weights;
    fstream weights_input;

    weights_input.open(weights_file);
    string input;
    getline(weights_input, input);
    string new_data;
    while(getline(weights_input, input)){
        weights.push_back(split((char*)input.c_str()));
    }
    pthread_mutex_init(&mut, NULL);
    pthread_mutex_init(&mut_fin, NULL);

    for (int i = 0; i < THREAD_NUMBER; i++){
        string s=data[0] + "train_"+to_string(i)+".csv";
        mt[i].dir = s;
        mt[i].index = to_string(i);
        mt[i].weights = weights;
       pthread_create(&threads[i],NULL, handel, (void*)(&mt[i]));

    }

    void* status;
    int all = 0;
    int correct = 0;
    for(int i = 0; i < THREAD_NUMBER; i++){
        pthread_join(threads[i], &status);
        int* res = (int*)status;
        all += res[0];
        correct += res[1];
    }
    printf("Accuracy: %.2f", (correct/(float)all)*100);
    cout<<"%";
    pthread_exit(NULL);

}
