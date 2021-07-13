#include <bits/stdc++.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include <ctime>
#include <omp.h>

using namespace std;

//Formigas vivas = Red
//Formigas mortas = Blue
// Branco = 0;
// Vermelho = 1;
// Azul = -1;

int tam_matriz;
int rect_size;
vector<vector<int> > mapa;


random_device g_rd;
mt19937 g_e(g_rd());
uniform_real_distribution<> g_dist(0, 1);

class Formigas{
private:
	int r,c;
	int posX, posY;
	bool carregando;
	int raio_visao;
	double dist;

public:
	Formigas(int raio){
    	carregando = false;
    	raio_visao = raio;
		r = rand() % tam_matriz;
		c = rand() % tam_matriz;
		dist = pow((1+2*raio), 2) - 1;

    	while(mapa[r][c] != 0){
    		r = rand() % tam_matriz;
    		c = rand() % tam_matriz;
    	}

    	mapa[r][c] = 1;
	}

	int posicao(){
		int escolha = rand() % 3;
		switch(escolha){
			case 0: return -1;
			case 1: return 0;
			case 2: return 1;
		}
	}

	void movimento(){
		int posX = r;
		int posY = c;

		while((mapa[posX][posY] == 1 || mapa[posX][posY] == 2) || (mapa[posX][posY] == -1 && carregando) || (r == posX && c == posY )){
			posX = posX + posicao();
			posY = posY + posicao();

			posX %= (tam_matriz - 1);
			posY %= (tam_matriz - 1);

			if(posX < 0){
				posX = tam_matriz - 1;
			}


			if(posY < 0){
				posY = tam_matriz - 1;
			}

		}

		r = posX;
		c = posY;
	}

	int getR(){
		return r;
	}

	int getC(){
		return c;
	}

	void acao(){

		int aux_x = r;
		int aux_y = c;
		int aux = 0;

		movimento();

		if(mapa[r][c] == 0 && !carregando){
			mapa[r][c] = 1;

		}else if(mapa[r][c] == 0 && carregando){
			double soltar = g_dist(g_e);

			if(soltar < (double)(qntVisao()/dist)){
				carregando = false;
			}

			if(!carregando){
				mapa[r][c] = -1;
			}else{
				mapa[r][c] = 2;
			}

		}else if(mapa[r][c] == -1 && !carregando){
			double pegar = g_dist(g_e);
			int qntForm = qntVisao();

			if(pegar <= (double)(1.0 - qntForm/dist)){
				carregando = true;
			}

			if(carregando){
				mapa[r][c] = 2;
			}
		}

		if (mapa[aux_x][aux_y] == 1 || mapa[aux_x][aux_y] == 2){
				mapa[aux_x][aux_y] = 0;
		}

	}

	int qntVisao(){
		int viz = 0;
		int x, y;
		

		for (int i = -raio_visao; i <= raio_visao; ++i){
			for (int j = -raio_visao; j <= raio_visao; ++j){
				x = r + i;
				y = c + j;

				if(x < 0){
					x =  tam_matriz + x;
				}else if(x > tam_matriz - 1){
					x = x - tam_matriz;
				}

				if(y < 0){
					y = tam_matriz + y;

				}else if(y > tam_matriz - 1){
					y = y - tam_matriz;
				}

				if(mapa[x][y] == -1){
					viz++;

				}

			}
		}

		return viz;
	}

};

vector<Formigas *> formigas_vivas;

void draw(sf::RenderWindow &window){

		rect_size = 500/tam_matriz;


	    sf::RectangleShape rect(sf::Vector2f(rect_size, rect_size));
	    rect.setOutlineThickness(0.75f);
	    rect.setOutlineColor(sf::Color(122,122,122));
	    rect.setOrigin(sf::Vector2f(0,0));

	    for(int i = 0; i < tam_matriz; i++){
	        for(int j = 0; j < tam_matriz; j++){
	            rect.setPosition(rect_size * j, rect_size * i);

	            if(mapa[i][j] == 0){
		            rect.setFillColor(sf::Color::White);
	            }else if(mapa[i][j] == 1){
		            rect.setFillColor(sf::Color::Red);

	            }else if(mapa[i][j] == -1){
		            rect.setFillColor(sf::Color::Black);

	            }else{
	            	rect.setFillColor(sf::Color(0,255,0));
	            }

	            window.draw(rect);
	        }
	    }
	}

//---------------------------- MAIN ---------------------------------------------------------

int main(int argc, char const *argv[]){
	if(argc != 5){
		cout << "./FORMIGA tam_matriz ant_live ant_dead raio" << endl;
		exit(0);
	}else{
		cout << "Aperte enter para pular uma iteração" << endl;
	}	

	srand(time(NULL));

	tam_matriz = atoi(argv[1]);
	int vivas = atoi(argv[2]);
	int itens = atoi(argv[3]);
	int raio = atoi(argv[4]);
	std::vector<int> v(tam_matriz);
	for (int i = 0; i < tam_matriz; ++i){
		mapa.push_back(v);
	}
	sf::RenderWindow window(sf::VideoMode(500, 500), "Formigueiro");

	for (int i = 0; i < itens; ++i){
		int x = rand() % tam_matriz;
		int y = rand() % tam_matriz;

		while(mapa[x][y] != 0){
			x = rand() % tam_matriz;
			y = rand() % tam_matriz;
		}

		mapa[x][y] = -1;
	}

	for (int i = 0; i < vivas; ++i){
		formigas_vivas.push_back(new Formigas(raio));
	}

	bool open = true;
	int aux = 0;


	window.clear(sf::Color::White);
	draw(window);
	window.display();
	cin.get();

	for (int x = 0; x < 1*pow(10,6); ++x){
		sf::Event event;

		while(window.pollEvent(event)){
			if(event.type == sf::Event::Closed){
				open = false;
			}
		}

	    for (int i = 0; i < vivas; ++i){
	    	formigas_vivas[i] -> acao();
	    }

		++aux;
		if(aux == (3 * pow(10,5))){
			window.clear(sf::Color::White);
			draw(window);
			window.display();
			cin.get();
			aux = 0;
		}
	}

	window.clear(sf::Color::White);
	draw(window);
	window.display();
	cin.get();
	
	for (int i = 0; i < formigas_vivas.size(); ++i){
		while(mapa[formigas_vivas[i] -> getR()][formigas_vivas[i] -> getC()] != 1){
			formigas_vivas[i] -> acao();
		}
	}

	window.clear(sf::Color::White);
	draw(window);
	window.display();
	cin.get();

	return 0;

}
