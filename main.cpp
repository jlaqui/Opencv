/*
GRUPO
- Laqui Pacaya, Jordan Uriel
- Lorenzo Quilla, Fanny Lorena

******* DESCRIPTORES ********
demo:
/proyecto$ g++ main.cpp -o test1
/proyecto$ ./test1
*/
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <dirent.h>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "string.h"
#include "Image.h"
// VARIABLES GLOBALES Mean, Variance, Skewness y  kurtosis
using namespace std;
// dirección archivos, entrada desde ejecución
char dir_imagenes[60];
char dir_csv[60];

// METODOS
string cabeceraHistograma(int size);
void rellenar(int arr[], int size);
double mean(int histo[], int size);
double variance(int histo[], int size, double mean);
double skewness(int histo[], int size, double mean, double variance);
double kurtosis(int histo[], int size, double mean, double variance);
string txt_histograma(int array[], int size);
string txt_momentosHistograma(int histograma[], int size);
string txt_imagenes(const std::string &a_carpeta);
vector<string> split(string str, char pattern);
void imprimirVectorImagenes(vector<string> results);
void escribirCSV(string cadena, int nro);
string generaContenidoCSV(vector<string> imagenes, int clase);


// genera la cabcecera del archivo CSV con histogramas de longitud = size o de LBP
string cabeceraHistograma(int size){
    string cabecera="clase, imagen";

    int i;
    for (i = 0; i < size; i++) {
        cabecera.append(","+to_string(i));
    }
    cabecera.append("\n");
    return cabecera;

}
void rellenar(int *arr, int *LBP, string path){
     Image<uchar> image;
     image.Read(path);
     image.GetHistogram(arr);
     image.LocalBinaryPattern(LBP, 3);
}

double mean(int histo[], int size){
    //  mean (X) = SUM_i X(i) / N
    double mean =0;
    double suma =0;
    double N = size;
    for (int i = 0; i < N; i++) {
        suma+=histo[i];

    }
    // obteniendo la media
    mean = suma/N;

    return mean;
}
double variance(int histo[], int size, double mean){
    //  var (X) = 1/(N-1) SUM_i (X(i) - mean(X))^2
    double variance =0;
    double suma =0;
    double N = size;
    for (int i = 0; i < N; i++) {
        suma+=pow(histo[i] - mean,2);
    }
    // obteniendo la varianza
    variance = 1/(N-1) * suma;
    return variance;
}
double skewness(int histo[], int size, double mean, double variance){
    //                     mean ((X - mean (X)).^3)
    //      skewness (X) = ------------------------.
    //                            std (X).^3

    //   std (X) = sqrt ( 1/(N-1) SUM_i (X(i) - mean(X))^2 )
    double N = size;
    double skewness =0;
    double std = sqrt(variance*(N-1)/N);
    double suma =0;

    for (int i = 0; i < N; i++) {
        suma+=pow((histo[i] - mean),3);
    }
    // obteniendo skewness
    skewness = (suma/((N)*pow(std,3)));
    return skewness;

}
double kurtosis(int histo[], int size, double mean, double variance){
    //           mean ((X - mean (X)).^4)
    //      k1 = ------------------------
    //                std (X).^4

    double N = size;
    double kurtosis =0;
    double std = sqrt(variance*(N-1)/N);
    double suma =0;

    for (int i = 0; i < N; i++) {
        suma+=pow((histo[i] - mean),4);
    }
    // obteniendo kurtosis
    kurtosis = (suma/((N)*pow(std,4)));
    return kurtosis;

}
string txt_histograma(int array[], int size){
    string impresion ="";
    for (int i = 0; i < size; i++) {
        impresion.append(to_string(array[i]));

        if(i+1<size){
            impresion.append(",");
        }else{
            impresion.append("\n");
        }
    }
    return impresion;

}
string txt_momentosHistograma(int histograma[], int size){
    string momentos="";
    double mean_ = mean(histograma, size);
    double variance_ = variance(histograma, size,mean_);
    double skewness_ = skewness(histograma, size,mean_,variance_);
    double kurtosis_ = kurtosis(histograma, size,mean_,variance_);
    momentos.append(to_string(mean_)+", ");
    momentos.append(to_string(variance_)+", ");
    momentos.append(to_string(skewness_)+", ");
    momentos.append(to_string(kurtosis_)+"\n");
    return momentos;
}
//recupera los nombres de las imagenes dentro de una Carpeta en un string
string txt_imagenes(const std::string &a_carpeta)
{
    string imagenes, filtro;

    if (DIR *dpdf = opendir(a_carpeta.c_str()))
    {
        dirent *epdf = nullptr;
        while (epdf = readdir(dpdf))
        {
            if(filtro.append(epdf->d_name).size()>4){
                imagenes.append(",");
                imagenes.append(epdf->d_name);
                filtro="";
            }
            filtro="";

        }
    }
    return imagenes;

}
//genera vector de strings con los nombres de las imagenes almacenadas en un string
vector<string> split(string str, char pattern) {

    int posInit = 1; // para que no asuma el 1er miembro, el cual no es valido por ser vacio
    int posFound = 0;
    string splitted;
    vector<string> results;

    while(posFound >= 0){
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;
        results.push_back(splitted);
    }

    return results;
}
// imprime en consola un vector de strings
void imprimirVectorImagenes(vector<string> results){
    for(int i = 0; i < results.size(); i++){
        cout << results[i] << ", ";
    }
}

void escribirCSV(string cadena, int nro){
    int contadorFrames= 0;
    ofstream file;

    string dirFinal = dir_csv;
    dirFinal.append("/CSV_").append(to_string(nro)).append(".csv");
    file.open(dirFinal);

    file << cadena ;
    file.close();
}

// genera el contenido de cada registro del CSV - ejm HISTOGRAMAS
void generaContenidoCSV(vector<string> imagenes, int clase, string &contenidoCSV1, string &contenidoCSV2, string &contenidoCSV3 ){

    // int histograma[]={15, 3, 0, 0, 10, 11, 10, 9, 9, 9, 1, 16, 15, 4, 2, 7, 17, 2, 6, 7};
    //int size = 20; // prueba temporal - cambiar a 256
    //int size = 256;
    //int histograma[256] = {0};
    int size = 256;
    int LBP_size = 6400;
    for(int i = 0; i < imagenes.size(); i++){

        int histograma[256] = {0};
        int LBP[6400];
        // bubbly , zigzagged, waffled ./images/bubbly // images/zigzagged/zigzagged_0014.jpg
        string path = "";
        switch (clase) {
            case 1: path.append("images/bubbly/"); break;
            case 2: path.append("images/zigzagged/"); break;
            case 3: path.append("images/waffled/"); break;
            default:
                break;
        }
        path.append(imagenes[i]);

        if(path == "images/waffled/.directory"){
            std::cout << "Not found" << std::endl;
        }else {
            rellenar(histograma, LBP, path);
            contenidoCSV1.append(to_string(clase)).append(",").append(imagenes[i]).append(",").append(
                    txt_histograma(histograma, size));
            contenidoCSV2.append(to_string(clase)).append(",").append(imagenes[i]).append(",").append(
                    txt_momentosHistograma(histograma, size));
            contenidoCSV3.append(to_string(clase)).append(",").append(imagenes[i]).append(",").append(
                    txt_histograma(LBP, LBP_size)); // aqui cambiar
        }
    }
}
int main(){
    strcpy(dir_imagenes, "./images");
    strcpy(dir_csv, "./csvs");
    cout << "Dir IMAGENES: " << dir_imagenes << "  Dir CSVs: " << dir_csv << endl; //Imprimir en la consola


    int size =256;
    int sizeLBP = 6400;

    string cabeceraCSV1 = cabeceraHistograma(size);
    string cabeceraCSV2 = "clase, imagen, media, varianza, skewness, kurtosis\n";
    string cabeceraCSV3 = cabeceraHistograma(sizeLBP);

// imagenes de
// bubbly , zigzagged, waffled

    /*
     *
     * /carpeta1/imag2
     * ...
     * /carpeta2/imag3
     *
     * */

    string imagenesC1= txt_imagenes("./images/bubbly"); // dirección de la carpeta
    string imagenesC2= txt_imagenes("./images/zigzagged"); // dirección de la carpeta
    string imagenesC3= txt_imagenes("./images/waffled"); // dirección de la carpeta

    char pattern=',';
    // VECTOR CON LOS NOMBRES DE LAS IMAGENES
    vector<string> CARPETA1 = split(imagenesC1, pattern);
    vector<string> CARPETA2 = split(imagenesC2, pattern);
    vector<string> CARPETA3 = split(imagenesC3, pattern);

    string contenidoCSV1 = ""; contenidoCSV1.append(cabeceraCSV1);
    string contenidoCSV2 = ""; contenidoCSV2.append(cabeceraCSV2);
    string contenidoCSV3 = ""; contenidoCSV3.append(cabeceraCSV3);


    // llenando contenido en CSV1 - histogramas
    generaContenidoCSV(CARPETA1,1, contenidoCSV1, contenidoCSV2,contenidoCSV3);
    generaContenidoCSV(CARPETA2,2, contenidoCSV1, contenidoCSV2,contenidoCSV3);
    generaContenidoCSV(CARPETA3,3, contenidoCSV1, contenidoCSV2,contenidoCSV3);

    escribirCSV(contenidoCSV1,1);
    escribirCSV(contenidoCSV2,2);
    escribirCSV(contenidoCSV3,3);

    return 1;
}