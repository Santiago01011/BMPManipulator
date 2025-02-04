#ifndef ESTRUCTURAS_H_INCLUDED
#define ESTRUCTURAS_H_INCLUDED

typedef struct
{
    unsigned char pixel[3];
    unsigned char profundidad;
}t_pixel;

typedef struct
{
    unsigned int tamarchivo;
    unsigned int tamencabezado;
    unsigned int comienzoimagen;
    unsigned int ancho;
    unsigned int alto;
    unsigned short profundidad;
}t_metadata;
#endif // estructuras_h_included
