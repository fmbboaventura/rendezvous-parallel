#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

//-------------------Functions-------------------
double vX(double w, double t, int gama, double A, double B, double E, double F);
double vY(double w, double t, int gama, double A, double B, double C);
double vZ(double w, double t, int gama, double H, double I, double J);

double rT(double X, double Y, double Z);
double vT(double dx, double dy, double dz);

double dX(double w, double t, int gama, double A, double E, double F, double G);
double dY(double w, double t, double A, double B, double C, double D);
double dZ(double w, double t, int gama, double G, double H, double I, double J);

double brute_A (double y0, double xl0, double gama, double X, double w, double vex, double vey);
double brute_B (double yl0, double gama, double X, double w, double vex, double vey);
double brute_C (int n, double gama, double X, double w, double vex, double vey);
double brute_D (double y0, double xl0, double Y, double X, double w, double vex);
double brute_E (double y0, double xl0, double X, double w, double vex);
double brute_F(double Y, double X, double w, int n, double vex, double vey);
double brute_G (double x0, double yl0, double X, double w, double vex, double vey);
double brute_H (double z0, double Y, double w, double vex);
double brute_I(double zl0, double Y, double X, double w, double vez);
double brute_J(double Y, double X, double w, double vez, int n);

double x=0, y=0, z=0, xl0=0, yl0=0, zl0=0, w=0, n=0, gama=0;

static int const N = 20;

/* @author Gledson
 * main
 */
void main(int argc, char *argv[]) {

	int Alt= 220;
	int deltaT = 1;
	int Tmax = 86400;
	w = 398600.4418/sqrt((6378.0 + Alt)*(6378.0 + Alt)*(6378.0 + Alt));
	int gama = 0;
	int NPI = atoi(argv[1]); // numero de posicoes iniciais
	FILE *arq;
	char url[] = "in.dat";
	arq = fopen(url, "r");
	double var1;

	printf("Numero de posicoes iniciais: %d\n", NPI);

	for(int np = 0; np <= NPI; np++) {
		if(arq == NULL) {
			printf("Erro, nao foi possivel abrir o arquivo\n");
		} else {
			fscanf(arq,"%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", &var1, &var1, &var1, &x, &y, &z, &var1, &xl0, &yl0, &zl0, &var1, &var1, &var1, &var1, &var1, &var1, &var1, &var1, &var1);
			printf("%lf %lf %lf %lf %lf %lf\n", x, y, z, xl0, yl0, zl0);
		}

		for(int VeAux = 1; VeAux<=10; VeAux++) {
            double Ve = VeAux/2; //------------------------- errado, arrumar uma solucao pra isso
            int vex, vey, vez;
            vex = vey = vez =Ve*Ve/3;
            for(int aux = -14; aux<=2; aux++){
				double Y = pow(10, aux);
				//int tid = omp_get_thread_num();
        		//printf("Hello world from omp thread %d\n", tid);
                #pragma omp parallel for
				for(int X=1; X<=100; X++) {

					double A = brute_A (y, xl0, gama, X, w, vex, vey);
					double B = brute_B (yl0, gama, X, w, vex, vey);
					double C = brute_C (n, gama, X, w, vex, vey);
					double D = brute_D (y, xl0, Y, X, w, vex);
					double E = brute_E (y, xl0, X, w, vex);
					double F = brute_F (Y, X, w, n, vex, vey);
					double G = brute_G (x, yl0, X, w, vex, vey);
					double H = brute_H (z, Y, w, vex);
					double I = brute_I (zl0, Y, X, w, vez);
					double J = brute_J (Y, X, w, vez, n);

                    #pragma omp parallel for 
					for(int t = 0; t <= Tmax; t++) {
						double fx = dX(w, t, gama, A, E, F, G);
                        double fy = dY(w, t, A, B, C, D);
                        double fz = dZ(w, t, gama, G, H, I, J);

						double r = rT(fx, fy, fz);
                        printf("%lf\n", r);
						if(r == 0) {
							double fdx =  vX( w, t, gama, A, B, E, F);
                            double fdy =  vY( w, t, gama, A, B, C);
                            double fdz =  vZ( w, t, gama, H, I, J);

							double v = vT( fdx, fdy, fdz);
						}
					}
				}
			}
		}
	}
}

/**
* Calcular coeficiente A do Rendezvous
* @author Weverson, Jhone, Gledson
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param x0 valor no eixo X da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param y0 valor no eixo Y da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param z0 valor no eixo z da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param xl0 valor no eixo x da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param yl0 valor no eixo y da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param zl0 valor no eixo z da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de A
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de A
* @param w
* @param a - 0 - PotÃªncia utilizada dentro do somÃ¡torio para casos em que o indice do somÃ¡torio Ã© utilizado elevado a potÃªncias diferentes
*   a = 1  -> n^1
*   a = 2  -> n^2
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de A
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de A
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de A
* @returns O coeficiÃªnte A dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_A (double y0, double xl0, double gama, double X, double w, double vex, double vey) {
    double result = 0;
    int n;
    double aux;
    double sum = 0;

    result += (2*xl0)/w - 3*y0 +((2*vex)/w)*log((X+1)/X);

    //Calculo do somatorio
    for (n = 1; n <= N; n++) {
//		aux = (1/(n*pow(X, n)))*(1/(1+pow(((n*Y)/w),2)))*(((2*vex)/w)+((n*Y*vey)/(w*w)));
		aux = (1/(n*pow(X, n)))*(1/(1+((n*gama)/w)*((n*gama)/w)))*(((2*vex)/w)+((n*gama*vey)/(w*w)));
        if (n%2 == 0) {//iteraÃ§Ã£o Par
            aux = -aux;
        }
        sum += aux;
    }

    result+= sum;

    return result;
}

/**
* Calcular coeficiente B do Rendezvous
* @author Weverson, Jhone, Gledson
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param x0 valor no eixo X da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param y0 valor no eixo Y da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param z0 valor no eixo z da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param xl0 valor no eixo x da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param yl0 valor no eixo y da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param zl0 valor no eixo z da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de B
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de B
* @param w
* @param a - 0 - PotÃªncia utilizada dentro do somÃ¡torio para casos em que o indice do somÃ¡torio Ã© utilizado elevado a potÃªncias diferentes
*   a = 1  -> n^1
*   a = 2  -> n^2
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de B
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de B
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de B
* @returns O coeficiÃªnte B dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_B (double yl0, double gama, double X, double w, double vex, double vey) {
    double result = 0;
    double sum = 0;
    int n;
    double aux;

    result += yl0/w + (vey/w)*log((X+1)/X);

    //Calculo do somatorio
    for (n = 1; n <= N; n++) {
//      aux = (1/(n*pow(X,n)))*(1/(1+pow(((n*Y)/w),2)))*(vey/w + (2*n*Y*vex)/(w*w));
        aux = (1/(n*pow(X,n)))*(1/(1+pow(((n*gama)/w),2)))*(vey/w + (n*gama*vex)/(w*w));
        if (n%2 == 0) {//iteraÃ§Ã£o Par
            aux = -aux;
        }
        sum += aux;
    }

    result+= sum;

    return result;
}

/**
* Calcular o somatÃ³rio dos coeficientes Cn do Rendezvous
* @author Weverson, Jhone, Gledson
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param x0 valor no eixo X da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param y0 valor no eixo Y da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param z0 valor no eixo z da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param xl0 valor no eixo x da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param yl0 valor no eixo y da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param zl0 valor no eixo z da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de C
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de C
* @param w
* @param a - 0 - PotÃªncia utilizada dentro do somÃ¡torio para casos em que o indice do somÃ¡torio Ã© utilizado elevado a potÃªncias diferentes
*   a = 1  -> n^1
*   a = 2  -> n^2
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de C
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de C
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de C
* @returns O somatÃ³rio dos coeficiÃªntes Cn dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_C (int n, double gama, double X, double w, double vex, double vey) {
    double result = 0;
    double aux;

    //Calculo do somatorio Cn
    aux = 1/(n*pow(X, n)) * (vex + (n * gama * vey)/(w*w)) * (1/(1 + (n*gama/w)*(n*gama/w) ));
    if (n%2 == 0) {
        aux = -aux;
    }

    result +=aux;

    return result;
}

/**
* Calcular coeficiente D do Rendezvous
* @author Weverson, Jhone, Gledson
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param x0 valor no eixo X da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param y0 valor no eixo Y da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param z0 valor no eixo z da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param xl0 valor no eixo x da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param yl0 valor no eixo y da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param zl0 valor no eixo z da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de D
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de D
* @param w
* @param a - 0 - PotÃªncia utilizada dentro do somÃ¡torio para casos em que o indice do somÃ¡torio Ã© utilizado elevado a potÃªncias diferentes
*   a = 1  -> n^1
*   a = 2  -> n^2
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de D
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de D
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de D
* @returns O coeficiÃªnte D dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_D (double y0, double xl0, double Y, double X, double w, double vex) {
    double result = 0;

    result -= (vex* log((X+1)/X))/w;
    result += 4*y0 - 2*xl0/w;

    return result;
}


/**
* Calcular coeficiente E do Rendezvous
* @author Weverson, Jhone, Gledson
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param x0 valor no eixo X da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param y0 valor no eixo Y da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param z0 valor no eixo z da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param xl0 valor no eixo x da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param yl0 valor no eixo y da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param zl0 valor no eixo z da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de E
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de E
* @param w
* @param a - 0 - PotÃªncia utilizada dentro do somÃ¡torio para casos em que o indice do somÃ¡torio Ã© utilizado elevado a potÃªncias diferentes
*   a = 1  -> n^1
*   a = 2  -> n^2
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de E
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de E
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de E
* @returns O coeficiÃªnte E dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_E (double y0, double xl0, double X, double w, double vex) {
    double result = 0;

    result -=  3*vex*log((X+1)/X);
    result +=  6*w*y0 - 3*xl0;

    return result;
}

/**
* Calcular o somatÃ³rio dos coeficientes Fn do Rendezvous
* @author Filipe e Jhone
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de Fn
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de Fn
* @param w
* @param n Indice do somatÃ³rio
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de Fn
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de Fn
* @returns O coeficiente F dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_F(double Y, double X, double w, int n, double vex, double vey) {
    double result = 0;

    result = (1/(n*pow(X,n)))*((2*vey)/w + (4*vex)/(n*Y))/((1+pow((n*Y)/w,2)));
    if (n%2 == 0) {
        result = - result;
    }
    result -= vex/(n*Y);
    // aux *= pow(n,a); ??????

    return result;
}

/**
* Calcular coeficiente G do Rendezvous
* @author Iago, Filipe e JoÃ£o
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param x0 valor no eixo X da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param yl0 valor no eixo y da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de G
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de G
* @param w
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de G
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de G
* @returns o coeficiÃªnte G dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_G (double x0, double yl0, double X, double w, double vex, double vey) {
    double result = 0;
    double sum = 0;
    int n;
    double aux;

    result= 2*yl0/w + x0 + (2*vey*(log((X+1)/X)))/w;

		for (n = 1; n <= N; n++) {
        aux = 3*vex/(pow(n,2)*pow(X,n)*w);

				if (n%2 == 0) {
            aux = -aux;
        }

        sum +=aux;
    }

    result-=sum;

    return result;
}

/**
* Authors: Gledson e Jhone
* Calcular coeficiente H do Rendezvous
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param x0 valor no eixo X da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param y0 valor no eixo Y da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param z0 valor no eixo z da posiÃ§Ã£o relativa inicial entre o satÃ©lite e o detrito
* @param xl0 valor no eixo x da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param yl0 valor no eixo y da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param zl0 valor no eixo z da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de H
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de H
* @param w
* @param a - 0 - PotÃªncia utilizada dentro do somÃ¡torio para casos em que o indice do somÃ¡torio Ã© utilizado elevado a potÃªncias diferentes
*   a = 1  -> n^1
*   a = 2  -> n^2
* @param vex VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo X a ser calculado o valor de H
* @param vey VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Y a ser calculado o valor de H
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de H
* @returns O coeficiÃªnte H dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_H (double z0, double Y, double w, double vex) {
    double result = 0;
    double sum = 0;
    int n;
    double aux;

    result = z0;
    //Calculo do somatorio
    for (n = 1; n <= N; n++) {
        aux = ((vex*Y)/(pow(Y,n)*(w*w)))/(1+((n*Y)/w)*((n*Y)/w));
        if (n%2 == 0) {
            aux = -aux;
        }
        sum += aux;
    }
    result += sum;

    return result;
}

/**
* Calcular coeficiente I do Rendezvous
* @author Iago, Filipe e JoÃ£o
* @param N NÃºmero de iteraÃ§Ãµes no somatÃ³rio interno
* @param zl0 valor no eixo z da velocidade relativa inicial entre o satÃ©lite e o detrito
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de I
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de I
* @param w - velocidade angular
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de I
* @returns o coeficiÃªnte I dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_I(double zl0, double Y, double X, double w, double vez) {
    double result = 0;
    double sum = 0;
    int n;
    double aux;

    result = zl0/w - (vez/w)*(log((X+1)/X));

    //Calculo do somatorio
    for (n = 1; n <= N; n++) {
        aux = ((vez)/(pow(n,2)*pow(X,n)*w))/(1+pow((n*Y)/w,2));
        if (n%2 == 0) {
            aux = -aux;
        }
        sum += aux;
    }

    result += sum;

    return result;
}

/**
* Calcular o somatÃ³rio dos coeficientes Jn do Rendezvous
* @author Iago, Filipe e JoÃ£o
* @param Y Gama - VariÃ¡vel fÃ­sica Gama a ser calculado o valor de Jn
* @param X Chi - VariÃ¡vel fÃ­sica Chi a ser calculado o valor de Jn
* @param w - velocidade angular
* @param n - indÃ­ce do somatÃ³rio
* @param vez VariÃ¡vel fÃ­sica da Velocidade de exaustÃ£o no eixo Z a ser calculado o valor de Jn
* @returns o somatÃ³rio coeficiÃªnte Jn dado os valores iniciais e as variÃ¡veis fÃ­sicas a serem testadas
*/
double brute_J(double Y, double X, double w, double vez, int n) {
    double result = 0;

    result = vez/(n*pow(X,n)*w)/(1+pow((n*Y)/w,2));

    if (n%2 == 0) {
        result = -result;
    }

    return result;
}

/* @author Weverson, Iago
 * vetor X da distancia
 */
double dX(double w, double t, int gama, double A, double E, double F, double G) {

	double result1 = 2 * (A * sin(w * t) - cos(w * t)) + E * t;
	double result2 = G;
	int n;
	for ( n = 1; n <= N; n++) {
		result2 += F * M_E * pow(M_E, -(n * gama * t));
	}
	return result1 + result2;
}

double dY (double w, double t, double A, double B, double C, double D) {

	double result1 = A*cos(w*t)+B*sin(w*t);
	double result2 = 0;
	int n;
	for (n = 1; n < N; ++n){
		result2 = C*pow(M_E, -(n*w*t)) + D;
	}
	return result1 + result2;
}

/* @author Weverson, Iago
 * vetor Z da distancia
 */
double dZ(double w, double t, int gama, double G, double H, double I, double J) {

	double result1 = H * cos(w * t) + I * sin(w * t);
	double result2 = G;
	int n;
	for (n = 1; n <= N; n++) {
		result2 += J * M_E * pow(M_E, -(n * gama * t));
	}
	return result1 - result2;
}

/* @author Weverson, Jhone, Gledson
 * vetor X da velocidade
 */
double vX(double w, double t, int gama, double A, double B, double E, double F) {

	double result1 = 2 * ( (A * w * cos(w * t)) + (B * w * sin(w * t)) ) + E;
	double result2 = 0;
	int n;
	for (n = 1; n <= N; n++) {
		result2 += F * ((-n) * gama * pow(M_E, -(n * gama * t)));
	}
	return result1 + result2;
}

/* @author Weverson, Jhone, Gledson
 * vetor Y da velocidade
 */
double vY(double w, double t, int gama, double A, double B, double C) {

	double result1 = (-A) * w * sin(w * t);
	double result2 = B * w * cos(w * t);
	double result3 = 0;
	int n;
	for (n = 1; n <= N; n++) {
		result3 += C * ((-n) * gama * pow(M_E, -(n * gama * t)));
	}
	return result1 + result2 + result3;
}

/* @author Weverson
 * vetor Z da velocidade
 */
double vZ(double w, double t, int gama, double H, double I, double J) {

	double result1 = (-H) * w * sin(w * t);
	double result2 = I * w * cos(w * t);
	double result3 = 0;
	int n;

	for (n = 1; n <= N; n++) {
		result3 += J * ((-n) * gama * pow(M_E, -(n * gama * t)));
	}

	return result1 + result2 + result3;
}

/* @author Weverson
 * Modificada por Gledson
 * Velocidade
 */
double rT(double X, double Y, double Z) {
	return sqrt(X*X + Y*Y + Z*Z);
}

/* @author Weverson
 * Modificada por Gledson
 * Posicao
 */
double vT(double dx, double dy, double dz) {
	return sqrt(dx*dx + dy*dy + dz*dz);
}