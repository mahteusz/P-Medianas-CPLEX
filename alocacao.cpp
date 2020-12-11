#include <ilcplex/ilocplex.h>
#include <fstream>
ILOSTLBEGIN

int main(int argc, char** argv) {
	IloEnv env;
	
	//Lendo dados
	string filename = "instancia.dat";
	IloInt P;
	IloArray<IloNumArray> c(env);
	ifstream file(filename.c_str());
	if (!file) {
		cerr << "Erro na leitura do arquivo!!!" << endl;
	}
	file >> c; //Matriz de custos ou distâncias
	file >> P; //Número de medianas
	IloInt N = c.getSize(); //Tamanho da matriz
	
	try {
		IloModel myModel(env);
		
		//Declarando a matriz de alocação
		IloArray<IloNumVarArray> x(env, N);
		for (int i = 0;i < N;i++) {
			x[i] = IloNumVarArray(env, N, 0, 1, ILOINT);
		}

		//Declarando a Função Objetivo
		IloExpr funcaoObjetivo(env);
		for (int i = 0;i < N;i++) {
			for (int j = 0;j < N;j++) {
				funcaoObjetivo += c[i][j] * x[i][j];
			}
		}
		//adicionando a FO ao modelo
		myModel.add(IloMinimize(env, funcaoObjetivo));

		//Restrição 1
		IloExpr r1(env);
		for (int i = 0;i < N;i++) {
			r1 += x[i][i];
		}
		myModel.add(r1 == P);
		r1.end();

		//Restrição 2
		for (int i = 0;i < N;i++) {
			for (int j = 0;j < N;j++) {
				myModel.add(x[i][j] <= x[j][j]);
			}
		}

		//Restrição 3
		for (int i = 0;i < N;i++) {
			IloExpr r3(env);
			for (int j = 0;j < N;j++) {
				r3 += x[i][j];
			}
			myModel.add(r3 == 1);
		}
		
		
		//criando meu solver baseado no modelo
		IloCplex myCplex(myModel);
		if (myCplex.solve()) {
			env.out() << myCplex.getStatus() << endl;
			env.out() << myCplex.getObjValue() << endl;
		}
		else {
			cout << "Erro ao resolver" << endl;
		}
	}
	catch (IloException& e) {
		cerr << e << endl;
	}
	env.end();
	return 0;
}