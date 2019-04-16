#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>


using namespace std;

void Read2DUCharImg(char *filename, unsigned char **img, int dim_x, int dim_y) {
    FILE *fp;
    int i;

    fp = fopen(filename, "rb");

    for (i = 0; i < dim_y; i++){
        fread(img[i], sizeof(unsigned char), dim_x, fp);
    }

    fclose(fp);
}

void Write2DUCharImg(char *filename, unsigned char **img, int dim_x, int dim_y)
{
    int i;
    FILE *fp;
    fp = fopen(filename, "wb");

    for (i = 0; i < dim_y; i++)
        fwrite(img[i], sizeof(unsigned char), dim_x, fp);
    fclose(fp);
}

unsigned char **UCalloc2d(int i_size, int j_size)
{
    unsigned char **array;
    int i;

    array = (unsigned char **)calloc(i_size, sizeof(unsigned char *));

    for (i = 0; i < i_size; i++)
        array[i] = (unsigned char *)calloc(j_size, sizeof(unsigned char));

    return(array);
}

void UCfree2d(unsigned char **array, int i_size)
{
    int i;

    for (i = 0; i < i_size; i++)
        free(array[i]);

    free(array);
}

unsigned char** subimage(unsigned char **orig, int dim_x, int dim_y, int x, int y){
    unsigned char **sub;
    sub = UCalloc2d(dim_y, dim_x);
    int ytemp = y;
    int xtemp = x;
    for(int i = 0; i < dim_x; i++, x++){
        for(int j = 0; j < dim_y; j++, y++){
            sub[j][i] = orig[ytemp][xtemp];
        }
    }
    return sub;

}

void correlation(unsigned char **orig, unsigned char **templ, int dim_x, int dim_y, int dim_x_templ, int dim_y_templ, string filename){
    unsigned char **out, **sub;
    out = UCalloc2d(dim_y, dim_x);
    char output[100];
    int k = dim_x*dim_y;
    double Rsum, Isum, Rb;
    double ans;

    for(int x = 0; x < dim_x_templ; x++){
        for(int y = 0; y < dim_y_templ; y++){
            Rsum += templ[y][x];
        }
    }
    //R bar for reference image
    Rb = Rsum / k;
    for(int x = 21; x < dim_x - 21; x++){
        for(int y = 23; y < dim_y - 23; y++){
            Isum = 0.0;
            long t = 0.0, b1 = 0.0, b2 = 0.0;
            sub = subimage(orig, dim_x_templ, dim_y_templ, x - 21, y - 23);

            for (int i = 0; i < dim_x_templ; ++i) {
                for (int j = 0; j < dim_y_templ; ++j) {
                    Isum += sub[j][i];
                }
            }
            double Ib = Isum/k;
            for (int i = 0; i < dim_x_templ; ++i) {
                for (int j = 0; j < dim_y_templ; ++j) {
                    t += (sub[j][i] - Ib) * (templ[j][i] - Rb);
                    b1 += (sub[j][i] - Ib) * (sub[j][i] - Ib);
                    b2 += (templ[j][i] - Rb) * (templ[j][i] - Rb);
                }
            }
            ans = ((t)/(sqrt(b1) * sqrt(b2)));
            out[y][x] = (int)((t)/(sqrt(b1) * sqrt(b2))) * 255;
        }
    }

    string filepath = "/home/quincy/CLionProjects/untitled/" + filename + "_correlation_coefficient.raw";
    strcpy(output, filepath.c_str());
    Write2DUCharImg(output, out, dim_x, dim_y);
    UCfree2d(out, dim_y);

}

unsigned char** downsample2x(unsigned char **orig, int dim_x, int dim_y){
    unsigned char **small;
    small = UCalloc2d(dim_y/2, dim_x/2);

    for(int x = 1, a = 0; x < dim_x; x += 2, a++){
        for(int y = 1, b = 0; y < dim_y; y += 2, b++){
            small[b][a] = orig[y][x];
        }
    }

    return small;
}

void downupsample(unsigned char **orig, int dim_x, int dim_y, string filename){
    unsigned char **down, **out;
    out = UCalloc2d(dim_x, dim_y);
    down = downsample2x(orig, dim_x, dim_y);
    char output[100];



    string filepath = "/home/quincy/CLionProjects/untitled/" + filename + "_resampling.raw";
    strcpy(output, filepath.c_str());
    Write2DUCharImg(output, out, dim_x, dim_y);
    UCfree2d(out, dim_y);
}

int main(int argc, char *argv[]){

    unsigned char **flowers, **flowersTemplate, **lena;
    char flowersfilename[100], flowerstemplatefilename[100], lenafilename[100];
    FILE *fp;
    int dim_x_flowers, dim_y_flowers, dim_x_flowerst, dim_y_flowerst, dim_x_lena, dim_y_lena;

    string flowersname = "flowers";
    string lenaname = "lena";

    //flowers           - 400 x 300
    //flowerstemplate   - 42 x 45
    //lena              - 512 x 512

    dim_x_flowers = 400; dim_y_flowers = 300;
    dim_x_flowerst = 42; dim_y_flowerst = 45;
    dim_x_lena = 512; dim_y_lena = 512;

    flowers = UCalloc2d(dim_y_flowers, dim_x_flowers);
    flowersTemplate = UCalloc2d(dim_y_flowerst, dim_x_flowerst);
    lena = UCalloc2d(dim_y_lena, dim_x_lena);

    strcpy(flowersfilename, "/home/quincy/CLionProjects/untitled/flowers.raw");
    strcpy(flowerstemplatefilename, "/home/quincy/CLionProjects/untitled/flowers-template.raw");
    strcpy(lenafilename, "/home/quincy/CLionProjects/untitled/lena.raw");

    Read2DUCharImg(flowersfilename, flowers, dim_x_flowers, dim_y_flowers);
    Read2DUCharImg(flowerstemplatefilename, flowersTemplate, dim_x_flowerst, dim_y_flowerst);
    Read2DUCharImg(lenafilename, lena, dim_x_lena, dim_y_lena);



    correlation(flowers, flowersTemplate, dim_x_flowers, dim_y_flowers, dim_x_flowerst, dim_y_flowerst, flowersname);
    //downupsample(lena, dim_x_lena, dim_y_lena, lenafilename);

    UCfree2d(flowers, dim_y_flowers);
    UCfree2d(flowersTemplate, dim_y_flowerst);
    UCfree2d(lena, dim_y_lena);

    return EXIT_SUCCESS;
}