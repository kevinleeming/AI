#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <stack>

using namespace std;

struct Word{
	string s;
	int length;
};

struct Node{
	vector<string> domain;
	int len;
};

struct Constraint{
	int coordX;
	int coordY;
	int nodeId1;
	int nodeId2;
};

struct NodeAnswer{
	string ans;
	int nodeIndex;
};

struct SearchNode{
	NodeAnswer nodeAns;
	vector<int> unvisitedId;
	vector<NodeAnswer> confirmAns;
	vector<Node> nodeInfo;
};

int CheckPosition(vector<int> c, char dir, int x, int y){
	if(dir == 'A'){
		return (x - c[0]);
	}
	else{
		return (y - c[1]);
	}
}

bool AC3(struct SearchNode* n, queue<struct Constraint> constraint, vector<vector<int> > info, vector<char> dir){
	vector<struct Constraint> modifiedConstraint;
	while(!constraint.empty()){
			struct Constraint con;
			con = constraint.front();
			int nId1 = con.nodeId1;
			int nId2 = con.nodeId2;
			int n1ConPos = CheckPosition(info[nId1], dir[nId1], con.coordX, con.coordY);
			int n2ConPos = CheckPosition(info[nId2], dir[nId2], con.coordX, con.coordY);

			vector<string> domain1, domain2;
			int c;
			bool tag;
			for(int i = 0; i < n->nodeInfo[nId1].domain.size(); i++){
				tag = false;
				int domain1Size = domain1.size();
				for(int k = 0; k < domain1Size; k++){
					if(n->nodeInfo[nId1].domain[i][n1ConPos] == domain1[k][n1ConPos]){
						tag = true;
						domain1.push_back(n->nodeInfo[nId1].domain[i]);
						break;
					}
				}
				if(tag == true) {continue;}
				c = 0;
				for(int j = 0; j < n->nodeInfo[nId2].domain.size(); j++){
					if(n->nodeInfo[nId1].domain[i][n1ConPos] == n->nodeInfo[nId2].domain[j][n2ConPos]){
						domain2.push_back(n->nodeInfo[nId2].domain[j]);
						c++;
					}
				}
				if(c > 0){
					domain1.push_back(n->nodeInfo[nId1].domain[i]);
				}
			}

			bool m1, m2; // modified tag
			m1 = (domain1.size() < n->nodeInfo[nId1].domain.size()) ? true : false;
			m2 = (domain2.size() < n->nodeInfo[nId2].domain.size()) ? true : false;

			n->nodeInfo[nId1].domain.clear();
			n->nodeInfo[nId2].domain.clear();
			n->nodeInfo[nId1].domain = domain1;
			n->nodeInfo[nId2].domain = domain2;

			// Check whether the domain of two nodes had been modified
			if(m1 == true){
				while(1){
					bool full = (modifiedConstraint.size() > 0) ? false : true;
					for(int i = 0; i < modifiedConstraint.size(); i++){
						if(modifiedConstraint[i].nodeId1 == nId1 || modifiedConstraint[i].nodeId2 == nId1){
							constraint.push(modifiedConstraint[i]);
							modifiedConstraint.erase(modifiedConstraint.begin()+i);
							break;
						}
						if(i == modifiedConstraint.size()-1) full = true;
					}
					if(full) break;
				}
			}
			if(m2 == true){
				while(1){
					bool full = (modifiedConstraint.size() > 0) ? false : true;
					for(int i = 0; i < modifiedConstraint.size(); i++){
						if(modifiedConstraint[i].nodeId1 == nId2 || modifiedConstraint[i].nodeId2 == nId2){
							constraint.push(modifiedConstraint[i]);
							modifiedConstraint.erase(modifiedConstraint.begin()+i);
							break;
						}
						if(i == modifiedConstraint.size()-1) full = true;
					}
					if(full) break;
				}
			}

			modifiedConstraint.push_back(constraint.front());
			constraint.pop();
		}

		for(int i = 0; i < n->nodeInfo.size(); i++){
			if(n->nodeInfo[i].domain.size() == 0){
				return false;
			}
		}
		return true;
}


int main(){
	ifstream fp("puzzle.txt");
	ifstream fp2("word.txt");
	string ts, temps;
	vector<struct Word> word; // Word dictionary
	
	while(fp2 >> ts){
		struct Word tw;
		tw.s = ts;
		tw.length = ts.size();
		word.push_back(tw);
	}

	while(getline(fp, temps)){
		vector<char> dir; // input direction
		vector<vector<int> > info; // input coordinate and length
		vector<int> t;
		t.resize(3);
		stringstream ss;
		ss << temps;
		int cnt = 0, tempInt;
		char tempChar;

		while(1){
			if(cnt < 3){
				if(ss >> tempInt){
					t[cnt] = tempInt;
					cnt++;
				}
				else break;
			}
			else if(cnt == 3){
				info.push_back(t);
				ss >> tempChar;
				dir.push_back(tempChar);
				cnt = 0;
			}
		}

		vector<Node> node;
		node.resize(info.size());

		// trim domain by unary constraint (length of the word)
		for(int i = 0; i < node.size(); i++){
			node[i].len = info[i][2];
			for(int j = 0; j < word.size(); j++){
				if(word[j].length == node[i].len){
					node[i].domain.push_back(word[j].s);
				}
			}
		}

		// Find the constraint by intersection of nodes
		queue<struct Constraint> constraint, remainConstraint;
		for(int i = 0; i < node.size(); i++){
			for(int j = i+1; j < node.size(); j++){				
				if(dir[i] == 'A'){
					if(dir[j] == 'D'){
						if(info[j][0] >= info[i][0] && info[j][0] <= info[i][0]+info[i][2]-1){
							if(info[i][1] >= info[j][1] && info[i][1] <= info[j][1]+info[j][2]-1){
								struct Constraint tempConstraint;
								tempConstraint.nodeId1 = i;
								tempConstraint.nodeId2 = j;
								tempConstraint.coordX = info[j][0];
								tempConstraint.coordY = info[i][1];
								constraint.push(tempConstraint);
							}
						}
					}
				}
				else{
					if(dir[j] == 'A'){
						if(info[i][0] >= info[j][0] && info[i][0] <= info[j][0]+info[j][2]-1){
							if(info[j][1] >= info[i][1] && info[j][1] <= info[i][1]+info[i][2]-1){
								struct Constraint tempConstraint;
								tempConstraint.nodeId1 = i;
								tempConstraint.nodeId2 = j;
								tempConstraint.coordX = info[i][0];
								tempConstraint.coordY = info[j][1];
								constraint.push(tempConstraint);
							}
						}
					}
				}				
			}
		}
		remainConstraint = constraint;


		vector<struct Constraint> modifiedConstraint;
		// Use these constraints to further trim the domain of each node (AC-3 part)
		while(!constraint.empty()){
			struct Constraint con;
			con = constraint.front();
			int nId1 = con.nodeId1; // the first node id of this constraint
			int nId2 = con.nodeId2; // the second node id of this constraint
			// check the constraint position of the string in the node
			int n1ConPos = CheckPosition(info[nId1], dir[nId1], con.coordX, con.coordY);
			int n2ConPos = CheckPosition(info[nId2], dir[nId2], con.coordX, con.coordY);

			vector<string> domain1, domain2;
			int c;
			bool tag;
			for(int i = 0; i < node[nId1].domain.size(); i++){
				tag = false;
				int domain1Size = domain1.size();
				for(int k = 0; k < domain1Size; k++){
					if(node[nId1].domain[i][n1ConPos] == domain1[k][n1ConPos]){
						tag = true;
						domain1.push_back(node[nId1].domain[i]);
						break;
					}
				}
				if(tag == true) {continue;}
				c = 0;
				for(int j = 0; j < node[nId2].domain.size(); j++){
					if(node[nId1].domain[i][n1ConPos] == node[nId2].domain[j][n2ConPos]){
						domain2.push_back(node[nId2].domain[j]);
						c++;
					}
				}
				if(c > 0){
					domain1.push_back(node[nId1].domain[i]);
				}
			}

			bool m1, m2; // modified tag
			m1 = (domain1.size() < node[nId1].domain.size()) ? true : false;
			m2 = (domain2.size() < node[nId2].domain.size()) ? true : false;

			node[nId1].domain.clear();
			node[nId2].domain.clear();
			node[nId1].domain = domain1;
			node[nId2].domain = domain2;

			// Check whether the domain of two nodes had been modified
			// if the domain of constraint had been modified , then push the relevant constraint to queue
			if(m1 == true){
				while(1){
					bool full = (modifiedConstraint.size() > 0) ? false : true;
					for(int i = 0; i < modifiedConstraint.size(); i++){
						if(modifiedConstraint[i].nodeId1 == nId1 || modifiedConstraint[i].nodeId2 == nId1){
							constraint.push(modifiedConstraint[i]);
							modifiedConstraint.erase(modifiedConstraint.begin()+i);
							break;
						}
						if(i == modifiedConstraint.size()-1) full = true;
					}
					if(full) break;
				}
			}
			if(m2 == true){
				while(1){
					bool full = (modifiedConstraint.size() > 0) ? false : true;
					for(int i = 0; i < modifiedConstraint.size(); i++){
						if(modifiedConstraint[i].nodeId1 == nId2 || modifiedConstraint[i].nodeId2 == nId2){
							constraint.push(modifiedConstraint[i]);
							modifiedConstraint.erase(modifiedConstraint.begin()+i);
							break;
						}
						if(i == modifiedConstraint.size()-1) full = true;
					}
					if(full) break;
				}
			}

			modifiedConstraint.push_back(constraint.front());
			constraint.pop();
		}


		// Start to search the answer
		int visitedNodeNum = 0;
		stack<struct SearchNode> searchStack;
		// Declare the root node
		SearchNode root;
		root.confirmAns.clear();
		root.nodeInfo = node;
		for(int i = 0; i < node.size(); i++){
			root.unvisitedId.push_back(i);
		}

		while(1){
			// create unvisited node to push into the stack 
			for(int i = 0; i < root.unvisitedId.size(); i++){
				struct SearchNode tempNode;
				tempNode.nodeAns.nodeIndex = root.unvisitedId[i];
				tempNode.unvisitedId = root.unvisitedId;
				tempNode.confirmAns = root.confirmAns;
				tempNode.nodeInfo = root.nodeInfo;
				searchStack.push(tempNode);
			}

			// prune the domain by AC-3
			struct SearchNode topNode;
			string key;
			bool ac3;
			// node expansion : find a node with single word of its domain , and fit all the constraint
			while(1){
				topNode = searchStack.top();
				searchStack.pop();

				vector<string> tempDm;
				tempDm = topNode.nodeInfo[topNode.nodeAns.nodeIndex].domain;
				for(int i = 0; i < topNode.nodeInfo[topNode.nodeAns.nodeIndex].domain.size(); i++){
					topNode.nodeInfo[topNode.nodeAns.nodeIndex].domain.clear();
					topNode.nodeInfo[topNode.nodeAns.nodeIndex].domain.push_back(tempDm[i]);
					key = tempDm[i];
					ac3 = AC3(&topNode, remainConstraint, info, dir);
					if(ac3) break;
				}
				if(ac3) break;

			}
			
			if(ac3){
				topNode.nodeAns.ans = key;
				topNode.confirmAns.push_back(topNode.nodeAns);
				for(int i = 0; i < topNode.unvisitedId.size(); i++){
					if(topNode.unvisitedId[i] == topNode.nodeAns.nodeIndex){
						topNode.unvisitedId.erase(topNode.unvisitedId.begin() + i);
					}
				}
				visitedNodeNum++;
			}
			root = topNode;
			if(topNode.confirmAns.size() == node.size()) break;
		}

		cout << "visited node number : " << visitedNodeNum << endl;
		for(int i = 0; i < root.confirmAns.size(); i++){
			cout << root.confirmAns[i].nodeIndex << " " << root.confirmAns[i].ans << endl;;
		}
		cout << endl;

		dir.clear();
		info.clear();
		t.clear();
	}
	
	return 0;
}

