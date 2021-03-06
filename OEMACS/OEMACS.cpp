// OEMACS.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <cmath>
using namespace std;

#define MaxIter 50				//最大迭代次数
#define m 5						//蚂蚁数目
#define ServerNumber 20			//服务器数目(需要修改的参数)
#define VMNumber 20				//虚拟机数目N(需要修改的参数)
#define N VMNumber
#define t0  1.0/VMNumber
//相关系数
#define Beta 2.0
#define Epsilon 0.1
#define q0 0.7
#define prho 0.1

int VMRank[VMNumber+5];	
int VMRankSg[VMNumber+5];
int VMRankSb[VMNumber+5];

//CPU,RAM required for VM
int vc[VMNumber+5];
int vm[VMNumber+5];

//capacities of server
int PC[ServerNumber+5];
int PM[ServerNumber+5];

//normalized remaining CPU and memory
int LC[ServerNumber+5];
int LM[ServerNumber+5];

//the remaining resource of Solution_best
int Sb_UC[ServerNumber+5];
int Sb_UM[ServerNumber+5];

//Solution
int Solution[ServerNumber+5][VMNumber+5];			//a solution
int Solution_Global[ServerNumber+5][VMNumber+5];	//globally best solution
int Solution_best[ServerNumber+5][VMNumber+5];		//best solution of eachh iter
double ansOfFit_2 = DBL_MAX;

int x[ServerNumber+5][VMNumber+5];			//x(ij) VMj is assigned to server i
int s[ServerNumber+5][VMNumber+5];			//s(i)is the existing VM set on server i

bool I[ServerNumber+5][VMNumber+5];			//a set of available servers for placing VM(j)

double t[VMNumber+5][VMNumber+5];			//pheromone value indicate the preference of two VMs
double T[ServerNumber+5][VMNumber+5];		//the preference value of VM(j) to be assigned to server i

double n[ServerNumber+5][VMNumber+5];		//the heuristic information is associated to each VM assignment for
											//measuring the utilization improvement that VM(j) can bring to server i 

double p[ServerNumber+5][VMNumber+5];		//probability for assigning an unassigned VMj to server i
double r[ServerNumber+5][VMNumber+5];		//for overloade

//the usage of CPU and memory of server i before joining VM(j)
int UC[ServerNumber+5];
int UM[ServerNumber+5];

double over[ServerNumber+5];				//overload rate (unuse)

int Mmin;				//最小服务器数量
int Mt;					//当前服务器数量

int k;					//ant k
int iter;

//常规测试
void TestData1() {
	vc[1] = 1;
	vm[1] = 1;

	vc[2] = 1;
	vm[2] = 1;

	vc[3] = 5;
	vm[3] = 5;

	vc[4] = 5;
	vm[4] = 5;

	vc[5] = 10;
	vm[5] = 10;

	vc[6] = 10;
	vm[6] = 10;

	vc[7] = 15;
	vm[7] = 15;

	vc[8] = 15;
	vm[8] = 15;

	vc[9] = 20;
	vm[9] = 20;

	vc[10] = 20;
	vm[10] = 20;


	for (int i = 1; i <= ServerNumber; i++) {
		PC[i] = PM[i] = 25;
	}
}
//常规测试
void TestData2() {
	vc[1] = 1;
	vm[1] = 2;

	vc[2] = 3;
	vm[2] = 4;

	vc[3] = 5;
	vm[3] = 6;

	vc[4] = 7;
	vm[4] = 8;

	vc[5] = 9;
	vm[5] = 10;

	vc[6] = 11;
	vm[6] = 12;

	vc[7] = 13;
	vm[7] = 14;

	vc[8] = 15;
	vm[8] = 16;

	vc[9] = 17;
	vm[9] = 18;

	vc[10] = 19;
	vm[10] = 20;

	for (int i = 1; i <= ServerNumber; i++) {
		PC[i] = PM[i] = 25;
	}

}
//瓶颈情况
void TestData3() {
	vc[1] = 1;
	vm[1] = 11;

	vc[2] = 2;
	vm[2] = 12;

	vc[3] = 3;
	vm[3] = 13;

	vc[4] = 4;
	vm[4] = 14;

	vc[5] = 10;
	vm[5] = 15;

	vc[6] = 6;
	vm[6] = 8;

	vc[7] = 8;
	vm[7] = 10;

	vc[8] = 10;
	vm[8] = 12;

	vc[9] = 12;
	vm[9] = 14;

	vc[10] = 14;
	vm[10] = 16;

	for (int i = 1; i <= ServerNumber; i++) {
		PC[i] = PM[i] = 25;
	}
}
//异质
void TestData4() {
	vc[1] = 1;
	vm[1] = 1;

	vc[2] = 1;
	vm[2] = 1;

	vc[3] = 5;
	vm[3] = 5;

	vc[4] = 5;
	vm[4] = 5;

	vc[5] = 10;
	vm[5] = 10;

	vc[6] = 10;
	vm[6] = 10;

	vc[7] = 15;
	vm[7] = 15;
	
	vc[8] = 15;
	vm[8] = 15;

	vc[9] = 20;
	vm[9] = 20;

	vc[10] = 20;
	vm[10] = 20;

	for (int i = 1; i <= 5; i++) {
		PC[i] = PM[i] = 35;
	}
	for (int i = 6; i <= 10; i++) {
		PC[i] = PM[i] = 20;
	}
}
//两两互补
void TestData5() {
	vc[1] = 1;
	vm[1] = 9;

	vc[2] = 9;
	vm[2] = 1;

	vc[3] = 2;
	vm[3] = 8;

	vc[4] = 8;
	vm[4] = 2;

	vc[5] = 3;
	vm[5] = 7;

	vc[6] = 7;
	vm[6] = 3;

	vc[7] = 4;
	vm[7] = 6;

	vc[8] = 6;
	vm[8] = 4;

	vc[9] = 10;
	vm[9] = 10;

	vc[10] = 10;
	vm[10] = 10;

	for (int i = 1; i <= ServerNumber; i++) {
		PC[i] = PM[i] = 10;
	}
}
//数据量变大
//修改参数为20
void TestData6() {
	for (int i = 1; i <= 10; i++) {
		vc[i] = 5;
		vm[i] = 10;
		PC[i] = PM[i] = 35;
	}
	for (int i = 11; i <= 20; i++) {
		vc[i] = 20;
		vm[i] = 30;
		PC[i] = PM[i] = 40;
	}

}


void ShowSolution() {
	for (int i = 1; i <= VMNumber; i++) {
		cout << VMRankSg[i] << " ";
	} 
	cout << endl << "---------------------------------" << endl;

	for (int i = 1; i <= ServerNumber; i++) {
		for (int j = 1; j <= VMNumber; j++) {
			cout << Solution_Global[i][j] << " ";
		}
		cout << endl;
	}
	cout << "当前server数量： " << Mmin << endl;
}

void showsb() {

	for (int i = 1; i <= VMNumber; i++) {
		cout << VMRankSb[i] << " ";
	}
	cout << endl << "---------------------------------" << endl;

	for (int i = 1; i <= ServerNumber; i++) {
		for (int j = 1; j <= VMNumber; j++) {
			cout << Solution_best[i][j] << " ";
		}
		cout << endl;
	}
	cout << "当前Mt： " << Mt << endl;
}

void init() {

	//init pheromone
	for (int i = 1; i <= VMNumber; i++)
		for (int j = 1; j <= VMNumber; j++)
				t[i][j] = t0;

	//set S(gb)
	//place each VM on one server
	for (int i = 1; i <= VMNumber; i++) {
		Solution_Global[i][i] = 1;
	}
	
	//set Mmin t
	Mmin = VMNumber;
	iter = 1;
	
	for (int i = 1; i <= VMNumber; i++)
		VMRank[i] = i;
	
}

//为每一个VM选择合适的server
//【修改】
void SelectSet4VM(int j) {
	int allvc = 0;
	int allvm = 0;
	
	for (int i = 1; i <= ServerNumber; i++)
		I[i][j] = 0;
	
	//修改Mt为ServerNumber
	for (int i = 1; i <= Mt; i++) {
		if ((UC[i] + vc[j] <= PC[i]) && (UM[i] + vm[j] <= PM[i]))	I[i][j] = 1;
	}
}

double Calcu_T(int i, int j) {
	int num = 0;
	double allt = 0;

	for (int k = 1; k <= VMNumber; k++) {
		if (Solution[i][k] != 0) {
			num++;
			allt += t[k][j];
		}
	}
	if (num == 0)	T[i][j] = t0;
	else T[i][j] = (1.0 / num) * 1.0 * allt;
	
	return T[i][j];
}
double Calcu_n(int i, int j) {

	double molecular = 1.0 - abs(1.0*(PC[i] - UC[i] - vc[j]) / PC[i] - 1.0*(PM[i] - UM[i] - vm[j]) / PM[i]);
	double denominator = abs(1.0*(PC[i] - UC[i] - vc[j]) / PC[i]) + abs(1.0*(PM[i] - UM[i] - vm[j]) / PM[i]) + 1.0;
	
	n[i][j] = molecular / denominator;
	return n[i][j];

}
double Calcu_Probability(int i, int j) {
	
	double molecular = Calcu_T(i,j) * pow(Calcu_n(i, j), Beta);
	double denominator = 0.0;

	SelectSet4VM(j);
	for (int k = 1; k <= ServerNumber; k++) {
		if (I[k][j]) {
			denominator += Calcu_T(k, j) * pow(Calcu_n(k, j), Beta);
		}
	}

	p[i][j] = molecular / denominator;
	return p[i][j];
}
double Calcu_over(int i,int j) {
	double temp1 = 0.0;
	temp1 = abs(PC[i] - UC[i] - vc[j]) / PC[i];
	double temp2 = 0.0;
	temp2 = abs(PM[i] - UM[i] - vm[j]) / PM[i];
	over[i] = temp1 + temp2;

	return over[i];

}
double Calcu_r(int i, int j) {
	double molecular = 0.0;
	molecular = over[i];
	double denominator = 0.0;
	for (int k = 1; k <= Mt; k++) {
		denominator += over[k];
	}
	r[i][j] = 1.0 - molecular / denominator;
	return r[i][j];
}

int CPUUseOfServeri(int S[ServerNumber+5][VMNumber+5], int i) {
	int total = 0;
	for (int j = 1; j <= VMNumber; j++) {
		if (S[i][j])	total += vc[j];
	}
	return total;
}
int MemUseOfServeri(int S[ServerNumber+5][VMNumber+5], int i) {
	int total = 0;
	for (int j = 1; j <= VMNumber; j++) {
		if (S[i][j])	total += vm[j];
	}
	return total;
}

//wheel selection for general case
int WheelSelection1(int j) {
	srand(time(0));
	double q = ((double)rand()) / RAND_MAX;
	for (int k = 1; k <= VMNumber; k++) {
		if (I[k][j]) {
			q -= p[k][j];
			if (q <= 0)	return k;
		}
	}
	return -1;
}
//wheel selection for special case
int WheelSelection2(int j) {
	srand(time(0));
	double q = ((double)rand()) / RAND_MAX;
	for (int k = 1; k <= Mt; k++) {
			q -= r[k][j];
			if (q <= 0)	return k;
	}
	return -1;

}
//state transition rule
int StateTransRule(int j) {
	int index = 0;
	srand(time(0));
	double q = ((double)rand()) / RAND_MAX;
	if (q <= q0) {

		double maxx = 0.0;
		double temp = 0.0;

		for (int k = 1; k <= ServerNumber; k++) {
			if (I[k][j]) {
				temp = T[k][j] * pow(n[k][j], Beta);
				if (temp > maxx) {
					maxx = temp;
					index = k;
				}
			}
		}
	}
	else
	{
		index = WheelSelection1(j);
	}
	return index;
}
//complementary rule
int ComplementaryRule(int j) {
	int index = 0;
	srand(time(0));
	double q = ((double)rand()) / RAND_MAX;
	
	if (q <= q0) {
		double minn = DBL_MAX;
		for (int k = 1; k <= Mt; k++) {
			if (Calcu_over(k,j) < minn) {
				index = k;
				minn = over[k];
			}
		}
	}
	else {
		for (int k = 1; k <= Mt; k++) {
			Calcu_r(k, j);
		}
		index = WheelSelection2(j);
	}

	return index;
}

void Construct() {
	
	//shuffle VMs randomly
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(vc + 1, vc + 1 + VMNumber, std::default_random_engine(seed));
	shuffle(vm + 1, vm + 1 + VMNumber, std::default_random_engine(seed));
	shuffle(VMRank + 1, VMRank + 1 + VMNumber, std::default_random_engine(seed));
	
	Mt = Mmin - 1;
	int L = 1;
	while (L <= VMNumber) {
		//是否要加??
		//SelectSet4VM(L);
		int i = 1;

		while (i <= Mt) {
			Calcu_Probability(i, L);
			i++;
		}
		//all servers are overloaded after joining VMj
		bool allOverloaded = true;
		for (int server = 1; server <= ServerNumber; server++) {
			if (I[server][L]) {
				allOverloaded = false;
				break;
			}
		}
		int selectSever;
		if (!allOverloaded) {
			//not all the server are overloaded afer joining VMj
			//seclect server i for VMj according to the construction rule
			selectSever = StateTransRule(L);
		}
		else {
			selectSever = ComplementaryRule(L);
		}
		
		
		Solution[selectSever][L] = 1;
		x[selectSever][L] = 1;
		
		LC[selectSever] -= vc[L];
		LM[selectSever] -= vm[L];
		
		UC[selectSever] += vc[L];
		UM[selectSever] += vm[L];

		L++;
	}

}

//f1(s)
int EvaluateFitness_f1(int S[ServerNumber+5][VMNumber+5]) {	
	//if Solution satisfies the capacity constraint
	int y = 0;

	for (int i = 1; i <= Mt; i++) {

		if ((LC[i] == PC[i]) && (LM[i] == PM[i]))	continue;		//未曾使用过
		
		if((LC[i] >= 0) && (LM[i] >= 0)) y++;
		else
		{
			y = Mt + 1;
			return y;
		}
	}
	return y;
}
//f2(s)
double EvaluateFitness_f2() {
	double ans = 0.0;
	bool isUse = false;

	for (int i = 1; i <= Mt; i++) {
		if ((LC[i] != PC[i]) && (LM[i] != PM[i])) {
			//被使用过
			ans += (1.0*abs(PC[i] - UC[i]) / PC[i]) + (1.0*abs(PM[i] - UM[i]) / PM[i]);
		}
	}
	return ans;

}

//update best solution
void UpdateBestSolution() {
	cout << "UpdateBestSolution" << endl;

	for (int j = 1; j <= VMNumber; j++) {
		VMRankSb[j] = VMRank[j];
	}

	for (int i = 1; i <= ServerNumber; i++) {
		for (int j = 1; j <= VMNumber; j++) {
			Solution_best[i][j] = Solution[i][j];
		}
	}
	
	showsb();
	
}
//update global solution
void UpdateGlobalSolution() {
	cout << "UpdateGlobalSolution" << endl;
	//SetToFront(Solution_best);
	for (int j = 1; j <= VMNumber; j++) {
		VMRankSg[j] = VMRankSb[j];
	}

	for (int i = 1; i <= ServerNumber; i++)
		for (int j = 1; j <= VMNumber; j++)
			Solution_Global[i][j] = Solution_best[i][j];

	Mmin = EvaluateFitness_f1(Solution_best);
	ShowSolution();
}

//local pheromone updating after the construction of each ant
void LocalUpdate() {
	
	int VMList[VMNumber+5];
	int cnt = 0;

	//Mt修改
	for (int i = 1; i <= Mt; i++) {
		//对于每一个server
		//记录下该服务器上的VM
		cnt = 0;
		for (int j = 1; j <= VMNumber; j++) {
			if (Solution[i][j])	VMList[++cnt] = j;
		}

		for (int k = 1; k <= cnt; k++) {
			for (int j = 1; j <= cnt; j++) {
				t[VMList[k]][VMList[j]] = (1 - prho) * t[VMList[k]][VMList[j]] + prho * t0;
			}
		}
	}
	
}

//the best solution is feasible
bool isFeasible() {
	
	bool isUse = false;
	int UseCPU = 0;
	int UseMem = 0;

	for (int i = 1; i <= ServerNumber; i++) {			//这里的ServerNumber 需要进一步推敲
		isUse = false;
		UseCPU = 0;
		UseMem = 0;

		for (int j = 1; j <= VMNumber; j++) {
			if (Solution_best[i][j]) {
				isUse = true;
				UseCPU += vc[j];
				UseMem += vm[j];
			}
		}
		
		if ((PC[i] < UseCPU) && (PM[i] < UseMem)) return false;
		
	}
	return true;
}

//升序,用于non-overloaded
bool Ascending(int a, int b) {
	int d1 = abs(vc[a] - vm[a]);
	int d2 = abs(vc[b] - vm[b]);
	return d1 < d2;
}
//降序,用于overloaded
bool Descending(int a, int b) {
	int d1 = abs(vc[a] - vm[a]);
	int d2 = abs(vc[b] - vm[b]);
	return d1 > d2;
}

bool Search4Swap(int S[ServerNumber + 5][VMNumber + 5], int overloade, int VMonOver[], int VMovercnt, int nonoverloade, int VMonNon[], int VMnoncnt) {

	int CPUUse1 = CPUUseOfServeri(S, overloade);
	int MemUse1 = MemUseOfServeri(S, overloade);

	int CPUUse2 = CPUUseOfServeri(S, nonoverloade);
	int MemUse2 = MemUseOfServeri(S, nonoverloade);

	
	for (int i = 1; i <= VMovercnt; i++) {

		for (int j = 1; j <= VMnoncnt; j++) {
			int CPUSwap2over = CPUUse1 - vc[VMonOver[i]] + vc[VMonNon[j]];
			int MemSwap2over = MemUse1 - vm[VMonOver[i]] + vm[VMonNon[j]];

			int CPUSwap2non = CPUUse2 - vc[VMonNon[j]] + vc[VMonOver[i]];
			int MemSwap2non = MemUse2 - vm[VMonNon[j]] + vm[VMonOver[i]];
			
		
			//两者都没有超过容量
			if (((CPUSwap2over <= PC[overloade]) && (MemSwap2over <= PM[overloade])) && ((CPUSwap2non <= PC[nonoverloade]) && (MemSwap2non <= PM[nonoverloade]))) {
				
				S[overloade][VMonOver[i]] = 0;
				S[overloade][VMonNon[j]] = 1;

				S[nonoverloade][VMonOver[i]] = 1;
				S[nonoverloade][VMonNon[j]] = 0;


				return true;
			}
			
			
		}
		
	}


	return false;
}

//对于当前Solution_Best,更新剩余资源
void UseOfSolutionBest() {
	int totCPU = 0;
	int totMem = 0;

	for (int i = 1; i <= ServerNumber; i++) {
		totCPU = 0;
		totMem = 0;
		for (int j = 1; j <= VMNumber; j++) {
			if (Solution_best[i][j]) {
				totCPU += vc[j];
				totMem += vm[j];
			}
		}
		Sb_UC[i] = totCPU;
		Sb_UM[i] = totMem;
	}
}

void OrderingExchange() {
	int overloaded[ServerNumber+5];			//记录overloaded的server
	int overcnt = 0;						//记录数目
	int nonoverloader[ServerNumber+5];		//记录non-overloade的server
	int nonovercnt = 0;						//记录数目

	UseOfSolutionBest();					//更新目前的使用情况
	
	//记录超载和未超载的server
	for (int i = 1; i <= ServerNumber; i++) {
		if (Sb_UC[i] > PC[i] || Sb_UM[i] > PM[i])	overloaded[++overcnt] = i;
		else nonoverloader[++nonovercnt] = i;
	}

	//分别记录在server上面的VM
	int VMonOver[VMNumber+5];
	int VMonOvercnt = 0;
	int VMonNonover[VMNumber+5];
	int VMonNoncnt = 0;

	for (int i = 1; i <= overcnt; i++) {
		//对于每一个超载的server

		VMonOvercnt = 0;
		for (int j = 1; j <= VMNumber; j++) {
			if (Solution_best[overloaded[i]][j])	VMonOver[++VMonOvercnt] = j;
		}
		//排序
		sort(VMonOver + 1, VMonOver + 1 + VMonOvercnt, Descending);
		
		//针对每一个nonoverloaded
		//寻找合适的不超载的server交换VM
		for (int k = 1; k <= nonovercnt; k++) {
			
			VMonNoncnt = 0;
			for (int j = 1; j <= VMNumber; j++) {
				if (Solution_best[nonoverloader[k]][j])	VMonNonover[++VMonNoncnt] = j;
			}

			//排序
			sort(VMonNonover + 1, VMonNonover + 1 + VMonNoncnt, Ascending);
			
			//寻找交换
			//有寻找到合适的便跳出
			if (Search4Swap(Solution_best,overloaded[i], VMonOver, VMonOvercnt, nonoverloader[k] ,VMonNonover, VMonNoncnt)) break;		
		}
		
	}


}

void Migration() {
	int overloaded[ServerNumber + 5];			//记录overloaded的server
	int overcnt = 0;							//记录数目
	int nonoverloader[ServerNumber + 5];		//记录non-overloade的server
	int nonovercnt = 0;							//记录数目

	UseOfSolutionBest();						//更新目前的使用情况

	//记录超载和未超载的server
	for (int i = 1; i <= ServerNumber; i++) {
		if (Sb_UC[i] > PC[i] || Sb_UM[i] > PM[i])	overloaded[++overcnt] = i;
		else nonoverloader[++nonovercnt] = i;
	}

	int VMonOver[VMNumber+5];
	int VMonOvercnt = 0;

	for (int i = 1; i <= overcnt; i++) {
		//对于每一个超载的server
		bool succ = false;				//是否转换成功
		VMonOvercnt = 0;
		
		for (int j = 1; j <= VMNumber; j++) {
			if (Solution_best[overloaded[i]][j])	VMonOver[++VMonOvercnt] = j;
		}

		//对于该overloaded server上面的VM,进行逐一寻找
		for (int j = 1; j <= VMonOvercnt; j++) {
			
			for (int k = 1; k <= nonovercnt; k++) {
				int CPUleft = PC[nonoverloader[k]] - Sb_UC[nonoverloader[k]];
				int Memleft = PM[nonoverloader[k]] - Sb_UM[nonoverloader[k]];

				//有足够容量容纳
				if ((CPUleft >= vc[VMonOver[j]]) && (Memleft >= vm[VMonOver[j]])) {
					Solution_best[overloaded[i]][VMonOver[j]] = 0;
					Solution_best[nonoverloader[k]][VMonOver[j]] = 1;
					
					Sb_UC[overloaded[i]] -= vc[VMonOver[j]];
					Sb_UM[overloaded[i]] -= vm[VMonOver[j]];
					
					Sb_UC[nonoverloader[k]] += vc[VMonOver[j]];
					Sb_UM[nonoverloader[k]] += vm[VMonOver[j]];

					//使得原本overload变为non-overload
					if ((Sb_UC[overloaded[i]] <= PC[overloaded[i]]) && (Sb_UM[overloaded[i]] <= PM[overloaded[i]])) {
						succ = true;
						break;
					}

				}
				
			}
			if (succ)	break;		//转换成功,不再需要比较转换
		}

	}


}
//perform the OEM local search on Sbest
void OEM_LocalSearch() {
	
	OrderingExchange();
	Migration();
}

//perform global pheromone updating on Sbest
void GlobalUpdate() {
	
	UseOfSolutionBest();
	double temp1 = 1.0 / EvaluateFitness_f1(Solution_best);

	int VMList[VMNumber + 5];
	int cnt = 0;

	for (int i = 1; i <= ServerNumber; i++) {
		//对于每一个server
		//记录下该服务器上的VM
		double temp2 = temp1 + 1.0 / ((PC[i] - Sb_UC[i]) + (PM[i] - Sb_UM[i]) + 1);

		cnt = 0;
		for (int j = 1; j <= VMNumber; j++) {
			if (Solution[i][j])	VMList[++cnt] = j;
		}

		for (int k = 1; k <= cnt; k++) {
			for (int j = 1; j <= cnt; j++) {
				t[VMList[k]][VMList[j]] = (1 - Epsilon)*t[VMList[k]][VMList[j]] + Epsilon * temp2;
			}
		}
	}
	

	

}

int main()
{
	//设置数据
	//TestData1();			
	//TestData2();
	//TestData3();
	//TestData4();
	//TestData5();
	TestData6();

	init();				//初始化
	
	while (iter <= MaxIter) {
		k = 1;
		Mt = Mmin - 1;
		
		//clean the best solution of each iteration
		for (int i = 1; i <= ServerNumber; i++)
			for (int j = 1; j <= VMNumber; j++)
				Solution_best[i][j] = Solution_Global[i][j];
		
		ansOfFit_2 = DBL_MAX;

		while(k <= m) {

			//clean the solution (and the other things)of ant k 
			for (int i = 1; i <= ServerNumber; i++) {
				//剩余部分清空
				LC[i] = PC[i];
				LM[i] = PM[i];
				UC[i] = 0;
				UM[i] = 0;
				over[i] = 0.0;

				for (int j = 1; j <= VMNumber; j++)
					Solution[i][j] = x[i][j] = I[i][j] = 
					n[i][j] = p[i][j] = r[i][j] = 0;
			}
			
			Construct();

			int y = EvaluateFitness_f1(Solution);
			if (y < Mt) {
				UpdateBestSolution();
				ansOfFit_2 = EvaluateFitness_f2();			//最新的bestSolution的f2的值
			}
			else if (y == Mt) {
				//当f1相同时,采用f2去判别
				double tempf2 = EvaluateFitness_f2();
				if (tempf2 < ansOfFit_2) {
					UpdateBestSolution();
					ansOfFit_2 = tempf2;
				}
			}
			LocalUpdate();
			k++;
		}
		
		if (isFeasible()) {
			UpdateGlobalSolution();
			
		}
		else 
		{
			OEM_LocalSearch();
		}

		GlobalUpdate();
		iter++;
	}
	
	ShowSolution();			//显示结果
	

    return 0;
}