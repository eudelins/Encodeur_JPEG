#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


/*
    Type opaque représentant le flux d'octets à écrire dans le fichier JPEG de
    sortie (appelé bitstream dans le sujet).
*/
struct bitstream{
    FILE *fichier_sortie;
    uint8_t buffer;
    uint8_t compteur_bits;
};


/* Retourne un nouveau bitstream prêt à écrire dans le fichier filename. */
extern struct bitstream *bitstream_create(const char *filename)
{
    struct bitstream *stream = malloc(sizeof(struct bitstream));
    stream->fichier_sortie = fopen(filename, "wb");
    stream->buffer = 0;
    stream->compteur_bits = 0;
    return stream;
}


/*
    Ecrit nb_bits bits dans le bitstream. La valeur portée par cet ensemble de
    bits est value. Le paramètre is_marker permet d'indiquer qu'on est en train
    d'écrire un marqueur de section dans l'entête JPEG ou non (voir section
    2.10.4 du sujet).
*/
extern void bitstream_write_bits(struct bitstream *stream,
                                 uint32_t value,
                                 uint8_t nb_bits,
                                 bool is_marker)
{
    assert(nb_bits <= 32);
    
    uint8_t bits_poids_faible, bits_poids_fort, octet;
    while (stream->compteur_bits + nb_bits >= 8){
        bits_poids_fort = stream->buffer << (8 - stream->compteur_bits);
        bits_poids_faible = value >> (nb_bits + stream->compteur_bits - 8);
        octet = bits_poids_faible + bits_poids_fort;
        fwrite(&octet, 1, 1, stream->fichier_sortie);

        if (!is_marker && (octet == 0xff)){
            uint8_t zero = 0;
            fwrite(&zero, 1, 1, stream->fichier_sortie);
        }
        
        // On indique le nb de bits qu'il nous reste à écrire
        value -= (value >> (nb_bits + stream->compteur_bits - 8)) << (nb_bits + stream->compteur_bits - 8);
        nb_bits -= (8 - stream->compteur_bits);
        stream->compteur_bits = 0;
        stream->buffer = 0;
    }
    
    stream->buffer = stream->buffer << nb_bits;
    stream->buffer += value;
    stream->compteur_bits += nb_bits;
}


/*
    Force l'exécution des écritures en attente sur le bitstream, s'il en
    existe.
*/
extern void bitstream_flush(struct bitstream *stream)
{
    for (uint8_t i = 0; i < ((8 - stream->compteur_bits) % 8); i++){
        stream->buffer = stream ->buffer << 1;
    }
    fwrite(&(stream->buffer), 1, 1, stream->fichier_sortie);
    stream->buffer = 0;
    stream->compteur_bits = 0;
}


/*
    Détruit le bitstream passé en paramètre, en libérant la mémoire qui lui est
    associée.
*/
extern void bitstream_destroy(struct bitstream *stream)
{
    fclose(stream->fichier_sortie);
    free(stream);
}



// int main(void)
// {
//     struct bitstream *donnees = bitstream_create("test.jpg");

//     // Start of Image
//     // uint32_t test = 0xff;
//     bitstream_write_bits(donnees, 0xffd, 12, false);
//     bitstream_flush(donnees);

//     bitstream_write_bits(donnees, 0xffe0, 16, true);  // En-tête section
//     bitstream_write_bits(donnees, 0x0010, 16, true);  // Longueur section
//     bitstream_write_bits(donnees, 'J', 8, true);
//     bitstream_write_bits(donnees, 'F', 8, true);
//     bitstream_write_bits(donnees, 'I', 8, true);
//     bitstream_write_bits(donnees, 'F', 8, true);
//     bitstream_write_bits(donnees, '\0', 8, true);
//     bitstream_write_bits(donnees, 1, 8, true);
//     bitstream_write_bits(donnees, 1, 8, true);
//     bitstream_write_bits(donnees, 0, 32, true);
//     bitstream_write_bits(donnees, 0, 24, true);

//     bitstream_destroy(donnees);

//     return 0;
// }
