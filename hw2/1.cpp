#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <time.h>
#define RAND_SIZE 100000
#define FOREST_NUM 5

using namespace std;

class Flower{
public:
	float sepal_len;
	float sepal_wid;
	float petal_len;
	float petal_wid;
	string type;
	int randNum;
};

class GINI{
public:
	float gini;
	float threshold;
};

class Classification{
public:
	string feature;
	float Threshold;
	float Ig;
	bool same;
};

struct DTree{
	Classification ClassifyFeature;
	bool leave;
	string flowerType;
	struct DTree *left;
	struct DTree *right;
};
typedef struct DTree treenode;
typedef treenode *dtree;

void Process_input();
void Data_cross_validation(vector<Flower> &flower);
Classification ComputeIG(vector<Flower> &info);
float ComputeGini(vector<Flower> flower, int thre, int type);
void BuildTree(dtree root, vector<Flower> &f);
float TestData(vector<dtree> root, vector<Flower> &f);
void TrimTrainData(vector<Flower> &f);

bool compare(const Flower &f1, const Flower &f2){
	return (f1.randNum < f2.randNum);
}

bool compare1(const Flower &f1, const Flower &f2){
	return (f1.sepal_len < f2.sepal_len);
}

bool compare2(const Flower &f1, const Flower &f2){
	return (f1.sepal_wid < f2.sepal_wid);
}

bool compare3(const Flower &f1, const Flower &f2){
	return (f1.petal_len < f2.petal_len);
}

bool compare4(const Flower &f1, const Flower &f2){
	return (f1.petal_wid < f2.petal_wid);
}

bool compareGini(const GINI &g1, const GINI &g2){
	return (g1.gini < g2.gini);
}

int main(){
	Process_input();
	return 0;
}

void Process_input(){
	int inputType;
	cout << "Choose datasets you want to train :" << endl << "1.Iris" << endl;
	cin >> inputType;

	if(inputType == 1){
		fstream infile;
		infile.open("iris.txt", ios::in);
		string s1;
		vector<string> Vs1;
		
		if(infile.is_open()){
			while(!infile.eof()){
				getline(infile, s1);
				Vs1.push_back(s1);
			}
		}

		int flowerInfoIndex = 0;
		int flowerNum = Vs1.size();
		vector<Flower> flower; // all flower data in this
		string ts;
		ts.clear();
		flower.resize(flowerNum);

		for(int i = 0; i < flowerNum; i++){
			for(int j = 0; j < Vs1[i].size(); j++){
				if(Vs1[i][j] != ','){
					ts = ts + Vs1[i][j];
					if(j == Vs1[i].size()-1){
						flower[i].type = ts;
						ts.clear();
					}
				}
				else{
					if(flowerInfoIndex == 0){
						flower[i].sepal_len = strtof(ts.c_str(), 0);
						ts.clear();
						flowerInfoIndex++;
					}
					else if(flowerInfoIndex == 1){
						flower[i].sepal_wid = strtof(ts.c_str(), 0);
						ts.clear();
						flowerInfoIndex++;
					}
					else if(flowerInfoIndex == 2){
						flower[i].petal_len = strtof(ts.c_str(), 0);
						ts.clear();
						flowerInfoIndex++;
					}
					else if(flowerInfoIndex == 3){
						flower[i].petal_wid = strtof(ts.c_str(), 0);
						ts.clear();
						flowerInfoIndex = 0;
					}
				}
			}
		}
		Data_cross_validation(flower);
	}

	
	
}

void Data_cross_validation(vector<Flower> &flower){
	// random shuffle
	srand(time(NULL));
	for(int i = 0; i < flower.size(); i ++){ 
		flower[i].randNum = (rand() % RAND_SIZE);
	}
	sort(flower.begin(), flower.end(), compare);

	// K-fold cross validation (K = 5)
	int trainNum = (flower.size()) * 4 / 5;
	int testNum = (flower.size()) / 5;
	vector<Flower> TrainFlower;
	vector<Flower> TestFlower;
	TrainFlower.resize(trainNum);
	TestFlower.resize(testNum);
	float final[5];

	for(int i = 0; i < 5; i++){
		if(i == 0){
			for(int j = 0; j < TrainFlower.size(); j++){
				TrainFlower[j] = flower[j];
			}
		}
		else if(i == 1){
			for(int j = 0; j < 90; j++){
				TrainFlower[j] = flower[j];
			}
			for(int j = 120; j < 150; j++){
				TrainFlower[j-30] = flower[j];
			}
		}
		else if(i == 2){
			for(int j = 0; j < 60; j++){
				TrainFlower[j] = flower[j];
			}
			for(int j = 90; j < 150; j++){
				TrainFlower[j-30] = flower[j];
			}
		}
		else if(i == 3){
			for(int j = 0; j < 30; j++){
				TrainFlower[j] = flower[j];
			}
			for(int j = 60; j < 150; j++){
				TrainFlower[j-30] = flower[j];
			}
		}
		else if(i == 4){
			for(int j = 30; j < 150; j++){
				TrainFlower[j-30] = flower[j];
			}
		}

		// Build random forest
		vector<dtree> Root;
		Root.resize(FOREST_NUM);
		for(int j = 0; j < FOREST_NUM; j++){
			Root[j] = NULL;
			dtree tnode = new treenode;
			vector<Flower> train = TrainFlower;
			TrimTrainData(train);
			tnode->ClassifyFeature = ComputeIG(train);
			srand(time(NULL));
			for(int i = 0; i < train.size(); i ++){ 
				train[i].randNum = (rand() % RAND_SIZE);
			}
			sort(train.begin(), train.end(), compare);
			tnode->leave = false;
			Root[j] = tnode;
			BuildTree(Root[j], train);
		}

		if(i == 0){
			for(int j = 0; j < TestFlower.size(); j++){ // test data
				TestFlower[j] = flower[j+trainNum];
			}
		}
		else if(i == 1){
			for(int j = 90; j < 120; j++){
				TestFlower[j-90] = flower[j];
			}
		}
		else if(i == 2){
			for(int j = 60; j < 90; j++){
				TestFlower[j-60] = flower[j];
			}
		}
		else if(i == 3){
			for(int j = 30; j < 60; j++){
				TestFlower[j-30] = flower[j];
			}
		}
		else if(i == 4){
			for(int j = 0; j < 30; j++){
				TestFlower[j] = flower[j];
			}
		}

		final[i] = TestData(Root, TestFlower);
	}

	cout << "5-fold cross validation result :" << endl;
	for(int i = 0; i < 5; i++){
		cout << final[i] << " ";
	}cout << endl;

}

Classification ComputeIG(vector<Flower> &info){
	if(info.size() == 0){
		Classification cla;
		cla.feature = "serious_error";
		return cla;
	}

	Classification Ac;
	//Compute sepal_len Gini's impurity
	vector<GINI> G; // gini value of all threshold
	G.resize(info.size()-1);
	sort(info.begin(), info.end(), compare1);
	for(int i = 0; i < info.size()-1; i++){
		float thre = (info[i].sepal_len + info[i+1].sepal_len) / 2;
		G[i].threshold = thre;
		G[i].gini = ComputeGini(info, thre, 1);
	}
	sort(G.begin(), G.end(), compareGini);
	Ac.feature = "sepal_len";
	Ac.Threshold = G[0].threshold;
	Ac.Ig = G[0].gini;

	//Compute sepal_wid Gini's impurity
	sort(info.begin(), info.end(), compare2);
	for(int i = 0; i < info.size()-1; i++){
		float thre = (info[i].sepal_wid + info[i+1].sepal_wid) / 2;
		G[i].threshold = thre;
		G[i].gini = ComputeGini(info, thre, 2);
	}
	sort(G.begin(), G.end(), compareGini);
	if(G[0].gini < Ac.Threshold){
		Ac.feature = "sepal_wid";
		Ac.Threshold = G[0].gini;
		Ac.Ig = G[0].gini;
	}

	//Compute petal_len Gini's impurity
	sort(info.begin(), info.end(), compare3);
	for(int i = 0; i < info.size()-1; i++){
		float thre = (info[i].petal_len + info[i+1].petal_len) / 2;
		G[i].threshold = thre;
		G[i].gini = ComputeGini(info, thre, 3);
	}
	sort(G.begin(), G.end(), compareGini);
	if(G[0].gini < Ac.Threshold){
		Ac.feature = "petal_len";
		Ac.Threshold = G[0].gini;
		Ac.Ig = G[0].gini;
	}

	//Compute petal_wid Gini's impurity
	sort(info.begin(), info.end(), compare4);
	for(int i = 0; i < info.size()-1; i++){
		float thre = (info[i].petal_wid + info[i+1].petal_wid) / 2;
		G[i].threshold = thre;
		G[i].gini = ComputeGini(info, thre, 4);
	}
	sort(G.begin(), G.end(), compareGini);
	if(G[0].gini < Ac.Threshold){
		Ac.feature = "petal_wid";
		Ac.Threshold = G[0].gini;
		Ac.Ig = G[0].gini;
	}
	if(Ac.Ig == 0){
		Ac.same = true;
	}
	else Ac.same = false;
	return Ac;
}

float ComputeGini(vector<Flower> flower, int thre, int type){
	float f1Pro, f2Pro, setosaPro, versicolorPro, virginicaPro, f1Gini, f2Gini, gini;
	vector<Flower> f1, f2;
	for(int i = 0; i < flower.size(); i++){
		if(type == 1){
			if(flower[i].sepal_len <= thre) f1.push_back(flower[i]);
			else f2.push_back(flower[i]);
		}
		else if(type == 2){
			if(flower[i].sepal_wid <= thre) f1.push_back(flower[i]);
			else f2.push_back(flower[i]);
		}
		else if(type == 3){
			if(flower[i].petal_len <= thre) f1.push_back(flower[i]);
			else f2.push_back(flower[i]);
		}
		else if(type == 4){
			if(flower[i].petal_wid <= thre) f1.push_back(flower[i]);
			else f2.push_back(flower[i]);
		}	
	}
	f1Pro = (flower.size() == 0) ? 0 : ((float)f1.size() / (float)flower.size());
	f2Pro = (flower.size() == 0) ? 0 : ((float)f2.size() / (float)flower.size());
	
	int setosaCnt = 0, versicolorCnt = 0, virginicaCnt = 0;
	for(int i = 0; i < f1.size(); i++){
		if(f1[i].type == "Iris-setosa") setosaCnt++;
		else if(f1[i].type == "Iris-versicolor") versicolorCnt++;
		else if(f1[i].type == "Iris-virginica") virginicaCnt++;
	}
	setosaPro = (f1.size() == 0) ? 0 : ((float)setosaCnt / (float)f1.size());
	versicolorPro = (f1.size() == 0) ? 0 : ((float)versicolorCnt / (float)f1.size());
	virginicaPro = (f1.size() == 0) ? 0 : ((float)virginicaCnt / (float)f1.size());
	f1Gini = 1.0 - (setosaPro*setosaPro + versicolorPro*versicolorPro + virginicaPro*virginicaPro);

	setosaCnt = 0;
	versicolorCnt = 0;
	virginicaCnt = 0;
	for(int i = 0; i < f2.size(); i++){
		if(f2[i].type == "Iris-setosa") setosaCnt++;
		else if(f2[i].type == "Iris-versicolor") versicolorCnt++;
		else if(f2[i].type == "Iris-virginica") virginicaCnt++;
	}
	setosaPro = (f2.size() == 0) ? 0 : ((float)setosaCnt / (float)f2.size());
	versicolorPro = (f2.size() == 0) ? 0 : ((float)versicolorCnt / (float)f2.size());
	virginicaPro = (f2.size() == 0) ? 0 : ((float)virginicaCnt / (float)f2.size());
	f2Gini = 1.0 - (setosaPro*setosaPro + versicolorPro*versicolorPro + virginicaPro*virginicaPro);

	gini = f1Pro * f1Gini + f2Pro * f2Gini;
	return gini;
}

void BuildTree(dtree root, vector<Flower> &f){
	dtree leftChild;
	dtree rightChild;
	
	if(root->ClassifyFeature.same){ // leave node
		root->left = NULL;
		root->right = NULL;
		root->leave = true;
		//cout << "leave" << endl;
		root->flowerType = f[0].type;
		return;
	}

	float thre = root->ClassifyFeature.Threshold;
	vector<Flower> f1,f2;

	if(root->ClassifyFeature.feature == "sepal_len"){
		for(int i = 0; i < f.size(); i++){
			if(f[i].sepal_len < thre) f1.push_back(f[i]);
			else f2.push_back(f[i]);
		}
	}
	else if(root->ClassifyFeature.feature == "sepal_wid"){
		for(int i = 0; i < f.size(); i++){
			if(f[i].sepal_wid < thre) f1.push_back(f[i]);
			else f2.push_back(f[i]);
		}
	}
	else if(root->ClassifyFeature.feature == "petal_len"){
		for(int i = 0; i < f.size(); i++){
			if(f[i].petal_len < thre) f1.push_back(f[i]);
			else f2.push_back(f[i]);
		}
	}
	else if(root->ClassifyFeature.feature == "petal_wid"){
		for(int i = 0; i < f.size(); i++){
			if(f[i].petal_wid < thre) f1.push_back(f[i]); 
			else f2.push_back(f[i]);
		}
	}

	leftChild = new treenode; // build left subtree
	leftChild->ClassifyFeature = ComputeIG(f1);
	if(leftChild->ClassifyFeature.feature == "serious_error"){
		root->left = NULL;
		root->right = NULL;
		root->leave = true;
		root->flowerType = f[0].type;
		//cout << "sleave" << endl;
		return;
	}
	leftChild->leave = false;
	root->left = leftChild;
	BuildTree(root->left, f1);

	rightChild = new treenode; // build right subtree
	rightChild->ClassifyFeature = ComputeIG(f2);
	if(rightChild->ClassifyFeature.feature == "serious_error"){
		root->left = NULL;
		root->right = NULL;
		root->leave = true;
		root->flowerType = f[0].type;
		//cout << "sleave" << endl;
		return;
	}
	rightChild->leave = false;
	root->right = rightChild;
	BuildTree(root->right, f2);
}

float TestData(vector<dtree> root, vector<Flower> &f){
	int rightNum = 0;
	float acc;

	for(int i = 0; i < f.size(); i++){
		int in[3], voteIndex = 0, vote = 0;
		for(int j = 0; j < 3; j++) {in[j] = 0;}
		for(int j = 0; j < root.size(); j++){
			dtree cur = root[j];
			while(!cur->leave){
				float thre = cur->ClassifyFeature.Threshold;

				if(cur->ClassifyFeature.feature == "sepal_len"){
					if(f[i].sepal_len < thre){
						cur = cur->left;
					}
					else{
						cur = cur->right;
					}
				}
				else if(cur->ClassifyFeature.feature == "sepal_wid"){
					if(f[i].sepal_wid < thre){
						cur = cur->left;
					}
					else{
						cur = cur->right;
					}
				}
				else if(cur->ClassifyFeature.feature == "petal_len"){
					if(f[i].petal_len < thre){
						cur = cur->left;
					}
					else{
						cur = cur->right;
					}
				}
				else if(cur->ClassifyFeature.feature == "petal_wid"){
					if(f[i].petal_wid < thre){
						cur = cur->left;
					}
					else{
						cur = cur->right;
					}
				}
			}
			if(cur->flowerType == "Iris-setosa"){
				in[0] += 1;
			}
			else if(cur->flowerType == "Iris-versicolor"){
				in[1] += 1;
			}
			else if(cur->flowerType == "Iris-virginica"){
				in[2] += 1;
			}
		}
		// Voting process
		for(int j = 0; j < 3; j++){
			if(in[j] > vote){
				vote = in[j];
				voteIndex = j;
			}
		}
		string result;
		if(voteIndex == 0) result = "Iris-setosa";
		else if(voteIndex == 1) result = "Iris-versicolor";
		else result = "Iris-virginica";
		if(f[i].type == result){
			rightNum++;
		}
	}
	acc = (float)rightNum / (float)f.size();
	return acc;
}

void TrimTrainData(vector<Flower> &f){
	srand(time(NULL));
	for(int i = 0; i < 20; i++){
		int ran = (rand() % f.size());
		f.erase(f.begin()+ran);
	}
}
