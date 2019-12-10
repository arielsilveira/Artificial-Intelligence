#include <bits/stdc++.h>
#include <omp.h>
#include <unistd.h>

using namespace std;

double temp_inicial;
double temp_final;
double num_iteracoes;
double alpha;
int num_clausulas;
int num_atomos;
vector<double> z(250000);
long long int soma = 0;
double desvio = 0;
double media = 0;


random_device g_rd;
mt19937 g_e(g_rd());
uniform_real_distribution<> g_dist(0, 1);

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(0,1);

class Clausulas
{
public:
	vector<int> atomos_clausula;

	Clausulas(int a, int b, int c){
		atomos_clausula.push_back(a);
		atomos_clausula.push_back(b);
		atomos_clausula.push_back(c);
	}

	bool satisfazibilidade(vector<bool> v_atomos){
		bool r = 0;
		for (int i = 0; i < atomos_clausula.size(); ++i){

			if(atomos_clausula[i] < 0){
				r = r || (not v_atomos[abs(atomos_clausula[i]) - 1]);
			} else {
				r = r || v_atomos[abs(atomos_clausula[i]) - 1];
			}

		}

		return r;
	}

};
vector<Clausulas *> clausula;

int countClausulas(vector<bool> &v, int t_it){
	int count = 0;

	#pragma omp parallel for
	for (int i = 0; i < t_it; ++i){
		if(clausula[i] -> satisfazibilidade(v)){
			#pragma omp critical
			count++;
		}
	}

	return count;
}


void geraAleatorio(vector<bool> &v, int t_it){

	#pragma omp parallel for
	for (int i = 0; i < t_it; ++i){
		v[i] = rand() % 2;
	}

}

void perturbacao(vector<bool> &v, int tam, double porc){
	int p = tam * porc;

	#pragma omp parallel for
	for (int i = 0; i < p; ++i){
		int aux = rand() % tam;
		v[aux] = not v[aux];
	}
}


double func(double t0, double tf, int n, int i){
	double numerador = (double) t0 - tf;
	double denominador = 1.0 + exp(3*(i-n/2.0));
	double result = (double)numerador/denominador;
	return result + tf;
}

void simulatedAnnealing(int it, double t0, double tf, double alpha, double pert){
	vector<bool> auxiliar(num_atomos);
	vector<bool> atomos_aux(num_atomos);
	vector<bool> vizinho(num_atomos);
	vector<int> valores(it);

	int v_vizinho, v_atomos, v_atomos_aux;

	geraAleatorio(auxiliar, num_atomos);
	atomos_aux = auxiliar;

	v_atomos = countClausulas(auxiliar, clausula.size()); //O(n)
	v_atomos_aux = countClausulas(atomos_aux, clausula.size());
	double t = t0;
	int SAmax = 1;
	double i = 0;

	while (i < it) {
		int iterT = 0;
		while (iterT < SAmax) {
			iterT++;
			vizinho = auxiliar;
			perturbacao(vizinho, vizinho.size(), pert);

			v_vizinho = countClausulas(vizinho, clausula.size()); //O(n)

			if(v_vizinho > v_atomos){
				v_atomos = v_vizinho;
				auxiliar = vizinho;

				if(v_vizinho > v_atomos_aux){
					v_atomos_aux = v_vizinho;
					atomos_aux = vizinho;
				}

			}else{
				double x = g_dist(g_e);
				int delta = abs(v_vizinho - v_atomos);

				if(x < exp(- delta / t)){
					v_atomos = v_vizinho;
					auxiliar = vizinho;
				}
			}

			z[i] = z[i] + v_atomos;
			soma += v_atomos;
		}
		i++;
		t = func(t0, tf, it, i);
	 }

}

void randomSearch(int it){
	vector<bool> vet_aux(num_atomos, false);
	vector<int> valores(it);
	int num_c = 0;

	soma = 0;
	desvio = 0;

	#pragma omp parallel for
	for (int i = 0; i < it; ++i){ //O(n)
		geraAleatorio(vet_aux, num_atomos); //O(a)
		int num_c_aux = countClausulas(vet_aux, clausula.size()); //O(c)

		#pragma omp critical
		if(num_c < num_c_aux){
			num_c = num_c_aux;
		}

		valores[i] = num_c_aux;
		soma += num_c_aux;
	}

	cout << "Total de clausulas verdadeiras: " << num_c << endl;
	media = soma/(double)it;

	for (int i = 0; i < it; ++i){
		desvio += pow((valores[i] - media), 2)/(double)it;
	}

	desvio = sqrt(desvio);

	cout << "Média: " << media << endl;
	cout << "Desvio Padrão: " << desvio << endl;
}


int main(int argc, char const *argv[]){

	if(argc != 7){
		cout << "./SATSolver temp_inicial temp_final num_iteracoes alpha nome_arquivo perturbacao" << endl;
		exit(0);
	}

	srand(time(NULL));

	temp_inicial = stod(argv[1]);
	temp_final = stod(argv[2]);
	num_iteracoes = stod(argv[3]);
	alpha = stod(argv[4]);
	string nome = argv[5];
	double pert = stod(argv[6]);

	nome = "Entrada/" + nome;

	string linha;
	string lixo;

	ifstream arq;
	arq.open(nome);

	if(!arq.is_open()){
		cout << "Falha ao abrir o arquivo" << endl;
		exit(1);
	}

	for (int i = 1; i <= 7; ++i){
		getline(arq, linha);
	}

	string qnt_atomos, qnt_clausulas;

	arq >> lixo >> lixo >> qnt_atomos >> qnt_clausulas;

	//Converte as string para int na base 10
	num_atomos = stoi(qnt_atomos, 0, 10);
	num_clausulas = stoi(qnt_clausulas, 0, 10);
	string l1, l2, l3;

	//Cada clausula tem seu conjunto de literais
	while(arq >> l1 >> l2 >> l3 >> lixo){
		clausula.push_back(new Clausulas(stoi(l1, 0, 10), stoi(l2, 0, 10), stoi(l3, 0, 10)));
	}

	// 	randomSearch(num_iteracoes);

	for (int i = 0; i < num_iteracoes; ++i){
		z[i] = 0;
	}

    for (int i = 0; i < 10; ++i){
		cout << i << endl;
		simulatedAnnealing(num_iteracoes, temp_inicial, temp_final, alpha, pert);
	}

	ofstream saida;
	saida.open("SA-250.csv");

	media = soma/(num_iteracoes*10.0);

	for (int i = 0; i < num_iteracoes; ++i){
		z[i] = z[i]/10.0;
		saida << i << "," << z[i] << endl;
		desvio += pow((z[i] - media), 2)/(double)num_iteracoes;
	}

	desvio = sqrt(desvio);

	cout << "Média: " << media << endl;
	cout << "Desvio Padrão: " << desvio << endl;

	return 0;
}
