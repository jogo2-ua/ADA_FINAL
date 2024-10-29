//JAVIER ORTS GARCÍA 49370397T

#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <queue>

using namespace std;

const int VALOR_MAX_INTEGER = 2147483647;
int visitados = 1, explorados = 0, nodosHoja = 0, nodoDescartadosFactibles = 0, nodoDescartadosnoPrometedores = 0, 
	prometedoresDescartados = 0, actualizaciones = 0, mejorActualizaciones = 0;

//Estructura del Nodo
struct Node {
	int x, y, valor;
	vector<int> direcciones;
	
	bool operator<(const Node& nodo) const{
		return valor > nodo.valor;
	}
};

//Posibles cambios de valor segun la dirección a tomar
vector<pair<int, int> > parejasDir = {
    {-1, 0},   // Norte
    {-1, 1},   // Noreste
    {0, 1},    // Este
    {1, 1},    // Sureste
    {1, 0},    // Sur
    {1, -1},   // Suroeste
    {0, -1},   // Oeste
    {-1, -1}   // Noroeste
};

bool checkArgs(int argc, char *argv[], bool &existP,bool &existP2D,string &fichero){
    bool validArgs = false;
    string argumento,ficheroAux;
    ifstream ficheroExterno;
  
    if(argc>1){
        validArgs = true;
        for(int i = 1; i<argc && validArgs==true;i++){
            argumento = argv[i];
            if(argumento == "-p"){
                existP = true;
            }
            else if(argumento == "--p2D"){
                existP2D = true;
            }
            else if(argumento == "-f"){
                ficheroAux=argv[i+1];
                if(argc  > i+1){
					fichero = argv[i+1];
					if(fichero.length()==0){
						validArgs=false;
					}
					ficheroExterno.open(fichero.c_str());
					if(ficheroExterno.is_open()==false){
						validArgs = false;
						ficheroExterno.close();
						cerr << "ERROR: can’t open file: " << fichero << "." <<endl;
					}
                }
                else{
                    validArgs = false;
                    cerr << "ERROR: missing filename."<<endl;
                } 
            }
            else if(argumento==ficheroAux){
                //ignora este valor
            }
            else{
                validArgs = false;
                cerr << "ERROR: unknown option " << argumento <<endl;
                break;
            }

        }
    }

    return validArgs;
}

bool abreFichero(string fichero,int &filas,int &columnas,vector<vector<int> > &laberinto){
    bool opened = false;
    ifstream ficheroExterno;
	
	ficheroExterno.open(fichero.c_str());
	if(ficheroExterno.is_open() != false){
		ficheroExterno >> filas >> columnas;
		laberinto = vector< vector<int> >(filas, vector<int>(columnas));				
		for(int i = 0; i < filas; i++){
			for(int j = 0; j < columnas; j++){
				ficheroExterno >> laberinto[i][j];
			}	
		}
		opened = true;
		ficheroExterno.close();
	}
    return opened;
}

//Vector igual que el laberinto, pero dando un valor diferente, segun el vector de direcciones, que luego cambiamos por *
void copiaVector(vector<vector<int> > &camino,const vector<int> &solucionDirecciones,const int pasos){
	int auxFila=0, auxCol=0;
	camino[auxFila][auxCol] = -1; // valores =-1 es el camino seguido en el mejor de los casos
	for(int i = 0; i < pasos; i++){
		switch(solucionDirecciones[i]){
			case 1:
				auxFila--;
				break;
			case 2:
				auxCol++;
				auxFila--; 
				break;
			case 3:
				auxCol++;
				break;
			case 4:
				auxCol++;
				auxFila++;
				break;
			case 5:
				auxFila++;
				break;
			case 6:
				auxCol--;
				auxFila++;
				break;
			case 7:
				auxCol--;
				break;
			case 8:
				auxCol--;
				auxFila--;
				break;
			default:
				//nunca va a llegar al caso default
				break;
		}
		camino[auxFila][auxCol] = -1;
	}
}

void muestraP2D(const vector<vector<int> > &laberinto,const int &pasos, vector<int> &solucionDirecciones){
	vector<vector<int> > camino(laberinto);
	
	copiaVector(camino,solucionDirecciones,pasos);
	for(unsigned int i=0;i<camino.size();i++){
		for(unsigned int j=0;j<camino[i].size();j++){
			if(camino[i][j] != -1){ 
				cout << camino[i][j];					
			}
			else{ 
				cout << "*"; 
			}	
		}
		cout << endl;
	}
	
}

int distanciaMayor(int x, int y, int filas, int columnas){
	int disx = abs(x - filas);
	int disy = abs(y - columnas);
	if(disx > disy){
		return disx;
	}
	else{
		return disy;
	}
};

int maze_bb(const vector<vector<int> >& laberinto, const int &filas, const int &columnas, vector<vector<int> >& matrizDist, vector<int> &solucionDirecciones) {
	priority_queue<Node> pq;
	int distancia = 0;

	if (laberinto[0][0] == 0) {
		return VALOR_MAX_INTEGER;
	}
	pq.push({0, 0, 1, {}});
	matrizDist[0][0] = 1;

	while (pq.empty()==false) {
		Node actual = pq.top();
		pq.pop();

		int distAnterior = matrizDist[actual.x][actual.y];

		if (actual.x == filas - 1 && actual.y == columnas - 1) {
			distancia = distAnterior;
			solucionDirecciones = actual.direcciones;  // Guardar el camino tomado
			return distancia;
		}

		for (unsigned int i = 0; i < parejasDir.size(); i++) {
			int x = actual.x + parejasDir[i].first;
			int y = actual.y + parejasDir[i].second;

			if (x < 0 || x >= filas || y < 0 || y >= columnas) {
				//Las nuevas direcciones nos llevan fuera del laberinto
				continue;
			}

			if (laberinto[x][y] == 0) {
				nodoDescartadosFactibles++;
                continue;
			}

			int siguiente = distAnterior + 1;
			if (siguiente >= matrizDist[x][y]) {
				if(x<1 || y<1){
					nodoDescartadosnoPrometedores++; 
				}
				else{
					prometedoresDescartados++;
				}
				continue;
			}

			matrizDist[x][y] = siguiente;

			vector<int> nuevoCamino = actual.direcciones;  // Crear una copia del camino actual
			nuevoCamino.push_back(i + 1);
			int valor =  distanciaMayor(x, y, filas - 1, columnas - 1) + siguiente;
			pq.push({x, y, valor, nuevoCamino});
		}
		explorados++;
	}
	return distancia;
}


void muestraP(const int &pasos,const vector<int> &solucionDirecciones){
	cout << "<";
	if(pasos==VALOR_MAX_INTEGER){
		cout << "NO EXIT";
	} 
	else{
		for(int i = 0; i < pasos-1; i++){
			cout << solucionDirecciones[i];
		}
	}
	cout << ">" << endl;
}


void muestraDatos(const clock_t &inicio,const clock_t &final,const int &pasos){
	
	if(pasos==VALOR_MAX_INTEGER){
		cout << 0 << endl
			 << visitados << " " <<  explorados << " " << nodosHoja << " " << nodoDescartadosFactibles << " " << nodoDescartadosnoPrometedores/8 << " "
			 << prometedoresDescartados/8 << " " << actualizaciones << " " << mejorActualizaciones << endl
			 << (final - inicio) * 1000.0 / CLOCKS_PER_SEC  << endl;
	}
	else{
		visitados = explorados + nodoDescartadosFactibles + nodoDescartadosnoPrometedores/8;
		cout << pasos << endl
			 << visitados << " " <<  explorados << " " << nodosHoja << " " << nodoDescartadosFactibles << " " << nodoDescartadosnoPrometedores/8 << " "
			 << prometedoresDescartados/8 << " " << actualizaciones << " " << mejorActualizaciones << endl
			 << (final - inicio) * 1000.0 / CLOCKS_PER_SEC  << endl;	
	}
}

int main(int argc, char *argv[]){
    bool comprueba,existP=false,existP2D=false;
	string fichero;
	vector<int> solucionDirecciones;
    vector<vector<int> > laberinto;
	int filas=0, columnas=0,pasos = 0;
    
    comprueba= checkArgs(argc,argv,existP,existP2D,fichero);
    if(comprueba == true){
        comprueba = abreFichero(fichero,filas,columnas,laberinto);
        if(comprueba==true){ 
			vector<vector<int> > matrizDist(filas, vector<int>(columnas, VALOR_MAX_INTEGER));
			clock_t inicio = clock();	
			pasos = maze_bb(laberinto, filas, columnas,matrizDist,solucionDirecciones);
			clock_t final = clock();
			
			muestraDatos(inicio,final,pasos);		
			if(existP2D){
				if(pasos ==  VALOR_MAX_INTEGER){
					cout << "NO EXIT" << endl;
				}
				else{
					muestraP2D(laberinto,pasos-1, solucionDirecciones);
				}
			}
            if(existP){
                muestraP(pasos,solucionDirecciones);
            }
        }
        else{
            cerr << "ERROR: can't open file: " << fichero << endl
				 << "Usage:" << endl 
				 << "maze bt [-p] [--p2D] -f file" << endl;
        }
    }
    else{
        cerr << "Usage:" << endl 
        	 << "maze bt [-p] [--p2D] -f file" << endl;
    }

    return 0;
}