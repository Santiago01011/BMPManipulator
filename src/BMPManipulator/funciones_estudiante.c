#include "constantes.h"

int _min(int a, int b);
int _max(int a, int b);

int solucion(int argc, char* argv[]) {
    bool amarillo=false, gris=false, achicar=false ,rotarD=false, rotarI=false, tonAzu=false, contrasteSube=false, contrasteBaja=false, tonRoja=false, tonVerde=false, recorte=false, negativo=false, monocromo=false;
    char* nombreArch = NULL;
    for(int i=1; i<argc; i++){
        char* currArgv = argv[i];
        if(!strcmp(currArgv,"--gris")) gris = true;
        if(!strcmp(currArgv,"--rotar-derecha")) rotarD = true;
        if(!strcmp(currArgv,"--rotar-izquierda")) rotarI = true;
        if(!strcmp(currArgv, "--tonalidad-azul")) tonAzu = true;
        if(!strcmp(currArgv, "--aumentar-contraste")) contrasteSube = true;
        if(!strcmp(currArgv, "--disminuir-contraste")) contrasteBaja = true;
        if(!strcmp(currArgv, "--tonalidad-roja")) tonRoja = true;
        if(!strcmp(currArgv, "--tonalidad-verde")) tonVerde = true;
        if(!strcmp(currArgv, "--recortar")) recorte = true;
        if(!strcmp(currArgv, "--negativo")) negativo = true;
        if(!strcmp(currArgv, "--monocromo")) monocromo = true;
        if(!strcmp(currArgv, "--achicar")) achicar = true;
        if(!strcmp(currArgv, "--tonalidad-amarilla")) amarillo = true;
        if(*(currArgv)!='-') nombreArch=currArgv;
    }
    if(nombreArch!=NULL){
        t_metadata metadata;
        t_pixel* imagenOriginal = NULL;
        if (leerImagen(nombreArch,&metadata, &imagenOriginal) != TODO_OK) {
            return ERROR_ARCHIVO;
        }
        size_t tam=metadata.alto*metadata.ancho;
        if(gris){
            t_pixel* imagenGris = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            if(!imagenGris) return MEMORIA_INSUFICIENTE;
            escalaGrises(imagenOriginal, &metadata, imagenGris);
            guardarArch("gris.bmp", &metadata, imagenGris);
            free(imagenGris);
        }
        if(contrasteSube||contrasteBaja){
            t_pixel* imagenContraste = malloc (metadata.ancho * metadata.alto * sizeof(t_pixel));
            if (!imagenContraste) return MEMORIA_INSUFICIENTE;
            if(contrasteSube){
                modificarContraste(imagenOriginal, &metadata, imagenContraste, 1.5);
                guardarArch("contraste_alto.bmp", &metadata, imagenContraste);
            }
            if(contrasteBaja){
                modificarContraste(imagenOriginal, &metadata, imagenContraste, 0.5);
                guardarArch("contraste_bajo.bmp", &metadata, imagenContraste);
            }
            free(imagenContraste);
        }
        if(rotarD){
            t_metadata metadataRotadaDer = metadata;
            t_pixel* imagenRotadaDer = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            if(!imagenRotadaDer) return MEMORIA_INSUFICIENTE;
            rotarDerImagen(imagenOriginal, &metadataRotadaDer, imagenRotadaDer);
            guardarArch("rotar_derecha.bmp", &metadataRotadaDer, imagenRotadaDer);
            free(imagenRotadaDer);
        }
        if(rotarI){
            t_metadata metadataRotada = metadata;
            t_pixel* imagenRotada = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            t_pixel* imagenRotadaV2 = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            if(!imagenRotada) return MEMORIA_INSUFICIENTE;
            rotarIzqImagen(imagenOriginal, &metadataRotada, imagenRotada);
            guardarArch("rotar_izquierda.bmp", &metadataRotada, imagenRotada);
            rotarIzqImagen(imagenRotada, &metadataRotada, imagenRotadaV2);
            guardarArch("rotar_izquierda_2.bmp", &metadataRotada, imagenRotadaV2);
            free(imagenRotada);
        }
        if(tonAzu){
            t_pixel* imagenazul = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            if(!imagenazul) return MEMORIA_INSUFICIENTE;
            cambioTonalidad(imagenOriginal, &metadata, imagenazul, 1, 1, 1.5);
            if(guardarArch("tonalidad_azul.bmp", &metadata, imagenazul)){
                printf("Error al guardar la imagen\n");
            }
            free(imagenazul);
        }
        if(tonRoja){
            t_pixel* imagenRoja = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            if(!imagenRoja) return MEMORIA_INSUFICIENTE;
            cambioTonalidad(imagenOriginal, &metadata, imagenRoja, 1.5, 1, 1);
            guardarArch("tonalidad_roja.bmp", &metadata, imagenRoja);
            free(imagenRoja);
        }
        if(tonVerde){
            t_pixel* imagenVerde = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            if(!imagenVerde) return MEMORIA_INSUFICIENTE;
            cambioTonalidad(imagenOriginal, &metadata, imagenVerde, 1, 1.5, 1);
            guardarArch("tonalidad_verde.bmp", &metadata, imagenVerde);
            free(imagenVerde);
        }
        if(recorte){
            t_metadata metadataRecorte = metadata;
            metadataRecorte.ancho = metadata.ancho / 2;
            metadataRecorte.alto = metadata.alto / 2;
            t_pixel* imagenRecorte = malloc(metadataRecorte.ancho * metadataRecorte.alto * sizeof(t_pixel));
            if(!imagenRecorte) return MEMORIA_INSUFICIENTE;
            metadataRecorte.tamarchivo = metadataRecorte.alto*metadataRecorte.ancho+54;
            for (int i = 0; i < metadataRecorte.alto; i++) {
                for (int j = 0; j < metadataRecorte.ancho; j++) {
                    imagenRecorte[i * metadataRecorte.ancho + j] = imagenOriginal[i * metadata.ancho + j];
                }
            }
            guardarArch("recorte.bmp", &metadataRecorte, imagenRecorte);
            free(imagenRecorte);
        }
        if(negativo){
            t_pixel* imagenNegativa = malloc(metadata.ancho * metadata.alto * sizeof(t_pixel));
            if(!imagenNegativa) return MEMORIA_INSUFICIENTE;
            negativos(imagenOriginal, &metadata, imagenNegativa);
            guardarArch("negativo.bmp", &metadata, imagenNegativa);
            free(imagenNegativa);
        }
        if(monocromo){
            t_metadata metadataMonocromo = metadata;
            int anchoEnBytes = (metadata.ancho + 7) / 8; // Redondear al byte más cercano
            int relleno = (4 - (anchoEnBytes % 4)) % 4; // Calcular el relleno necesario para alinear a 4 bytes
            unsigned char* imagenMonocromoBits = calloc(metadata.alto * (anchoEnBytes + relleno), sizeof(unsigned char));
            if (imagenMonocromoBits == NULL) return MEMORIA_INSUFICIENTE;
            monocromos(imagenOriginal, &metadataMonocromo, imagenMonocromoBits);
            metadataMonocromo.tamarchivo = 54 + 8 + metadata.alto * (anchoEnBytes + relleno);
            metadataMonocromo.profundidad = 1;
            guardarArch("monocromo.bmp", &metadataMonocromo, imagenMonocromoBits);
            free(imagenMonocromoBits);
        }
        if(achicar){
            t_metadata metadataAchicada = metadata;
            metadataAchicada.alto/=2;
            metadataAchicada.ancho/=2;
            t_pixel* imagenAchicada = malloc(metadataAchicada.alto* metadataAchicada.ancho * sizeof(t_pixel));
            if(!imagenAchicada) return MEMORIA_INSUFICIENTE;
            achicarImagen(imagenOriginal, &metadata, 2, imagenAchicada, metadataAchicada.alto, metadataAchicada.ancho);
            guardarArch("achicada.bmp", &metadataAchicada, imagenAchicada);
            free(imagenAchicada);
        }
        if(amarillo){
            t_pixel * imagenAmarilla = malloc(sizeof(t_pixel)* tam);
            if(!imagenAmarilla) return MEMORIA_INSUFICIENTE;
            //cambioTonalidad(imagenOriginal, &metadata, imagenAmarilla, 1.5, 1.4, 0.7); //168, 158, 50
            cambioTonalidad(imagenOriginal, &metadata, imagenAmarilla, 1.5, 1.5, 1.5);
            guardarArch("tonAmarilla.bmp", &metadata, imagenAmarilla);
            free(imagenAmarilla);
        }
        free(imagenOriginal);
    }
    printf("Proceso finalizado :)\n");
    return 1;
}

int leerImagen(char* nombreArch,t_metadata* metadata, t_pixel** imagenOriginal)
{
    char header[54]; // Tamaño del encabezado de un archivo BMP
    FILE *file;
    file = fopen(nombreArch, "rb");
    if (file == NULL)
    {
        printf("Error al abrir la imagen original.\n");
        return ARCHIVO_NO_ENCONTRADO;
    }
    fread(header, sizeof(header), 1, file);
    if (header[0] != 'B' || header[1] != 'M') {
        printf("El archivo no es un BMP.\n");
        return ERROR_ARCHIVO;
    }
    memcpy(metadata, header + 2, sizeof(metadata->tamarchivo));
    memcpy(&metadata->tamencabezado, header + 14, sizeof(metadata->tamencabezado));
    memcpy(&metadata->comienzoimagen, header + 10, sizeof(metadata->comienzoimagen));
    memcpy(&metadata->ancho, header + 18, sizeof(metadata->ancho));
    memcpy(&metadata->alto, header + 22, sizeof(metadata->alto));
    memcpy(&metadata->profundidad, header + 28, sizeof(metadata->profundidad));


    fseek(file, metadata->comienzoimagen, SEEK_SET); // Mover al comienzo de los datos de la imagen
    int filaSize = metadata->ancho * 3; // tamaño de cada fila en bytes
    int paddingSize = (4 - (filaSize % 4)) % 4; // tamaño de padding necesario
    int imageSize = metadata->alto * (filaSize + paddingSize); // tamaño total de la imagen
    char* imageData = malloc(imageSize);
    if (!imageData) {
        printf("Error al reservar memoria para la imagen.\n");
        fclose(file);
        return MEMORIA_INSUFICIENTE;
    }
    if (fread(imageData, 1, imageSize, file) != imageSize) {
        printf("Error al leer la imagen.\n");
        free(imageData);
        fclose(file);
        return ERROR_ARCHIVO;
    }
    *imagenOriginal = malloc(metadata->alto * metadata->ancho * sizeof(t_pixel));
    if (!*imagenOriginal) {
        printf("Error al reservar memoria para la imagen.\n");
        return MEMORIA_INSUFICIENTE;
    }
    t_pixel* pixelActual = *imagenOriginal;
    for (int i = 0; i < metadata->alto; i++) {
        for (int j = 0; j < metadata->ancho; j++) {
            pixelActual->pixel[0] = imageData[i * (filaSize + paddingSize) + j * 3];
            pixelActual->pixel[1] = imageData[i * (filaSize + paddingSize) + j * 3 + 1];
            pixelActual->pixel[2] = imageData[i * (filaSize + paddingSize) + j * 3 + 2];
            pixelActual->profundidad = metadata->profundidad;
            pixelActual++;
        }
    }
    printf("Imagen leida correctamente.\n");
    free(imageData);
    fclose(file);
    return TODO_OK;
}


void rotarIzqImagen(t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenRotada){
    for (int i = 0; i < metadata->alto; i++) {
        for (int j = 0; j < metadata->ancho; j++) {
            imagenRotada[j * metadata->alto + i] = imagenOriginal[(metadata->alto - i - 1) * metadata->ancho + j];
        }
    }

    int temp = metadata->ancho;
    metadata->ancho = metadata->alto;
    metadata->alto = temp;
}

void rotarDerImagen (t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenRotada){
    for (int i = 0; i < metadata->alto; i++) {
        for (int j = 0; j < metadata->ancho; j++) {
           imagenRotada[j * metadata->alto + i] = imagenOriginal[i * metadata->ancho + metadata->ancho - j - 1];
        }
    }
    int temp = metadata->ancho;
    metadata->ancho = metadata->alto;
    metadata->alto = temp;
}

int guardarArch(char* nombreArch,t_metadata* metadata, void *imagenOriginal){
    char ruta[256];
    char header[54] = {0}; // Tamaño del encabezado de un archivo BMP
    snprintf(ruta, sizeof(ruta), "Resultados/%s", nombreArch);
    FILE* fp = fopen(ruta, "wb"); //abro el archivo en modo escritura binaria
    int error = 0;
     if (!fp){
        printf("Error al abrir el archivo %s.\n", nombreArch);
        return 2;
    }
    //volcar metadata en el vector header
    memcpy(header, "BM", 2);
    int tam = BMP_HEADER_SIZE + metadata->ancho * metadata->alto * 3;
    memcpy(header + 2, &tam, sizeof(tam));
    memcpy(header + 10, &metadata->comienzoimagen, sizeof(metadata->comienzoimagen));
    memcpy(header + 14, &metadata->tamencabezado, sizeof(metadata->tamencabezado));
    memcpy(header + 18, &metadata->ancho, sizeof(metadata->ancho));
    memcpy(header + 22, &metadata->alto, sizeof(metadata->alto));
    short planos = 1;
    memcpy(header + 26, &planos, sizeof(planos));
    memcpy(header + 28, &metadata->profundidad, sizeof(metadata->profundidad));
    int tamImagen = metadata->ancho * metadata->alto * 3;
    memcpy(header + 34, &tamImagen, sizeof(tamImagen));
    if (!error && fwrite(header, sizeof(header), 1, fp) != 1) error = 1;

    if (!error && metadata->profundidad == 24) {
        fseek(fp, metadata->comienzoimagen, SEEK_SET);
        int rowSize = ((metadata->ancho * 3 + 3) / 4) * 4; // Tamaño de fila con padding
        int imageSize = rowSize * metadata->alto;
        unsigned char* imageData = malloc(imageSize);
        if (!imageData) {
            printf("Error al reservar memoria para la imagen.\n");
            return MEMORIA_INSUFICIENTE;
        }

        for (int i = 0; i < metadata->alto; i++) {
            for (int j = 0; j < metadata->ancho; j++) {
                memcpy(imageData + i * rowSize + j * 3, &((t_pixel*)imagenOriginal)[i * metadata->ancho + j].pixel, 3);
            }
        }

        if (fwrite(imageData, imageSize, 1, fp)!= 1) {
            error = 1;
            printf("Error al escribir el archivo.\n");
            return ERROR_ARCHIVO;
        }

        free(imageData);
    }else if (!error && metadata->profundidad == 1) {
        unsigned char tablaColores[8] = {0, 0, 0, 0, 255, 255, 255, 0};
        int anchoEnBytes = (metadata->ancho + 7) / 8;
        int relleno = (4 - (anchoEnBytes % 4)) % 4;
        tamImagen = (anchoEnBytes + relleno) * metadata->alto;
        tam = BMP_HEADER_SIZE + sizeof(tablaColores) + tamImagen;
        metadata->comienzoimagen = BMP_HEADER_SIZE + sizeof(tablaColores);

        // Escribir la tabla de colores
        fwrite(tablaColores, sizeof(tablaColores), 1, fp);

        // Escribir la imagen, incluyendo el relleno necesario
        for (int i = 0; i < metadata->alto; i++) {
            fwrite((unsigned char*)imagenOriginal + (i * anchoEnBytes), anchoEnBytes, 1, fp);
            if (relleno > 0) {
                unsigned char pad[3] = {0};
                fwrite(pad, relleno, 1, fp);
            }
        }

    }

    fclose(fp);

    if (error) {
        printf("Error al escribir el archivo.\n");
        return 3;
    }

    return TODO_OK;
}

int escalaGrises(t_pixel *imagenOriginal, t_metadata* metadata, t_pixel* imagenGris){
    for (int i=0; i<metadata->alto*metadata->ancho; i++)
    {
        unsigned char promedio = (imagenOriginal[i].pixel[0] + imagenOriginal[i].pixel[1] + imagenOriginal[i].pixel[2]) / 3;
        imagenGris[i].pixel[0] = promedio;
        imagenGris[i].pixel[1] = promedio;
        imagenGris[i].pixel[2] = promedio;
    }
    return TODO_OK;
}

int cambioTonalidad(t_pixel *imagenOriginal, t_metadata* metadata, t_pixel* imagenNueva, float rojo, float verde, float azul){
    for (int i = 0; i < metadata->alto * metadata->ancho; i++) {
        imagenNueva[i].pixel[2] = _min(imagenOriginal[i].pixel[2] * rojo, 255);
        imagenNueva[i].pixel[1] = _min(imagenOriginal[i].pixel[1] * verde, 255);
        imagenNueva[i].pixel[0] = _min(imagenOriginal[i].pixel[0] * azul, 255);
    }
    return TODO_OK;
}

int _min(int a, int b){
    return (a < b) ? a : b;
}
int _max(int a, int b){
    return (a > b) ? a : b;
}

int modificarContraste (t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenContraste, float factor)
{
    // for (int i = 0; i < metadata->ancho * metadata->alto; i++) {
    //     imagenContraste[i].pixel[2] = _min(_max(factor * (imagenOriginal[i].pixel[2] - 128) + 128, 0), 255);
    //     imagenContraste[i].pixel[1] = _min(_max(factor * (imagenOriginal[i].pixel[1] - 128) + 128, 0), 255);
    //     imagenContraste[i].pixel[0] = _min(_max(factor * (imagenOriginal[i].pixel[0] - 128) + 128, 0), 255);
    // }
    for (int i = 0; i < metadata->ancho * metadata->alto; i++) {
        imagenContraste[i].pixel[2] = _max(_min(factor * (imagenOriginal[i].pixel[2] - 128) + 128, 255), 0);
        imagenContraste[i].pixel[1] = _max(_min(factor * (imagenOriginal[i].pixel[1] - 128) + 128, 255), 0);
        imagenContraste[i].pixel[0] = _max(_min(factor * (imagenOriginal[i].pixel[0] - 128) + 128, 255), 0);
    }
    return TODO_OK;
}


int negativos (t_pixel* imagenOriginal, t_metadata* metadata, t_pixel* imagenNegativa)
{
    for (int i = 0; i < metadata->ancho * metadata->alto; i++) {
        imagenNegativa[i].pixel[2] = 255 - imagenOriginal[i].pixel[2];
        imagenNegativa[i].pixel[1] = 255 - imagenOriginal[i].pixel[1];
        imagenNegativa[i].pixel[0] = 255 - imagenOriginal[i].pixel[0];
    }
    return TODO_OK;
}

int monocromos(t_pixel* imagenOriginal, t_metadata* metadata, unsigned char *imagenMonocromoBits){
    for (int i = 0; i < metadata->alto * metadata->ancho; i++) {
        int promedio = (imagenOriginal[i].pixel[0] + imagenOriginal[i].pixel[1] + imagenOriginal[i].pixel[2]) / 3;
        int indiceByte = i / 8;
        int posicionBit = i % 8;
        if (promedio >= 128) {
            imagenMonocromoBits[indiceByte] |= (1 << (7 - posicionBit));
        }
    }
    return TODO_OK;
}

//      [][][][]  <--fin
//      [][][][]    =[][]   [b][g][r]
//      [][][][]     [][]
//com-->[][][][]

int achicarImagen(t_pixel* imagenOriginal, t_metadata* metadata, int factor, t_pixel* imagenAchicada, int alto, int ancho){
    for (int i = 0; i < alto; i++) {
        for (int j = 0; j < ancho; j++) {
            int promedioR = 0;
            int promedioG = 0;
            int promedioB = 0;
            for (int k = 0; k < factor; k++) {
                for (int l = 0; l < factor; l++) {
                    promedioR += imagenOriginal[(i * factor + k) * metadata->ancho + j * factor + l].pixel[2];
                    promedioG += imagenOriginal[(i * factor + k) * metadata->ancho + j * factor + l].pixel[1];
                    promedioB += imagenOriginal[(i * factor + k) * metadata->ancho + j * factor + l].pixel[0];
                }
            }
            promedioR /= factor * factor;
            promedioG /= factor * factor;
            promedioB /= factor * factor;
            imagenAchicada[i * ancho + j].pixel[2] = promedioR;
            imagenAchicada[i * ancho + j].pixel[1] = promedioG;
            imagenAchicada[i * ancho + j].pixel[0] = promedioB;
        }
    }
    return TODO_OK;
}
