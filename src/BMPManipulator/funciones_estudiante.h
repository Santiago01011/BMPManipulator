#ifndef FUNCIONES_ESTUDIANTE_H_INCLUDED
#define FUNCIONES_ESTUDIANTE_H_INCLUDED


#include "constantes.h"

int solucion(int argc, char* argv[]);
int leerImagen(char* nombreArch, t_metadata* metadata, t_pixel **imagenOriginal);
int guardarArch(char* nombreArch,t_metadata* metadata, void *imagenOriginal);
void rotarIzqImagen(t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenRotada);
int escalaGrises(t_pixel *imagenOriginal, t_metadata* metadata, t_pixel* imagenGris);
void rotarDerImagen (t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenRotada);
int cambioTonalidad(t_pixel *imagenOriginal, t_metadata* metadata, t_pixel* imagenNueva, float rojo, float verde, float azul);
int modificarContraste (t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenContraste, float factor);
int negativos(t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenNegativos);
int monocromos(t_pixel* imagenOriginal, t_metadata* metadata, unsigned char *imagenMonocromo);
int achicarImagen(t_pixel* imagenOriginal, t_metadata* metadata, int factor, t_pixel* imagenAchicada, int alto, int ancho);



#endif // FUNCIONES_ESTUDIANTE_H_INCLUDED
