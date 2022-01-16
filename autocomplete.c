#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "autocomplete.h"

int lexi_comp (const void *a, const void *b){
    return(strcmp(((struct term *)a)->term, ((struct term *)b)->term));
}
int num_sort (const void *a, const void *b){
    return ((((struct term *)b)->weight)- (((struct term *)a)->weight));
}
int find_ref_loc(struct term *terms, int nterms, char *substr){

    int a  = nterms;
    int b = 0;
    int half;
    int len =strlen(substr);
    char *temp_str = (char*)malloc((sizeof(char)+1)*len);
    strcpy(temp_str, substr);
    char *comp_term = (char*)malloc((sizeof(char)+1)*len);

    while (b<a){
        half = (a+b)/2;
        int i = 0;
        for (i; i < len ; i++){
            comp_term[i] = (terms[half].term)[i];
        }
        int int_res = strcmp(comp_term, temp_str);
        if (int_res > 0){
            a = half;
        }
        else if (int_res < 0) {
            b = half +1;
        }
        else{
            break;
        }
    }
    if (a == b){
        half = a;
    }
    return half;
}

void read_in_terms(struct term **terms, int *pnterms, char *filename){
    char line[200];
    FILE *fp = fopen(filename, "r");
    *pnterms = atoi(fgets(line, sizeof(line), fp));
    *terms = (struct term*)malloc(sizeof(struct term) * (*pnterms));

    for (int i = 0; i < *pnterms; i++){
        fgets(line, sizeof(line), fp);
        (*terms)[i].weight = (double)(atof(line));
        int in_index;
        int end_index;
        for (int i = 0; i < 200; i++){
            if (line[i] == '\t'){
                in_index = i + 1;
            }
            else if (line[i] == '\n'){
                end_index = i - 1;
                break;
            }
        }
        int num_char = end_index - in_index + 1;
        int x = 0;
        char *word = (char *)malloc(num_char * (sizeof(char)+1));
        for (int a = 0; a < num_char; a++){
            word[a] = line[in_index + x];
            x++;
        }
        word[num_char] = '\0';
        strcpy((*terms)[i].term, word);
    }
    qsort(*terms, *pnterms, sizeof(struct term), lexi_comp);
    fclose(fp);
}
int lowest_match(struct term *terms, int nterms, char *substr){

    int len =strlen(substr);
    char *temp_str = (char*)malloc((sizeof(char)+1)*len);
    strcpy(temp_str, substr);
    int max = find_ref_loc(terms, nterms, temp_str);
    char *compare = (char*)malloc((sizeof(char)+1)*len);
    while (max){

        for (int i = 0; i < len ; i++){
            compare[i] = (terms[max].term)[i];
        }
        compare[len] = '\0';
        int res = strcmp(compare, temp_str);
        if (res != 0){
            return max+1;
        }
        max--;
    }
    return -1;
}

int highest_match(struct term *terms, int nterms, char *substr){
    int len =strlen(substr);
    char *temp_str = (char*)malloc((sizeof(char)+1)*len);
    strcpy(temp_str, substr);
    int min = find_ref_loc(terms, nterms, temp_str);
    char *compare =(char*) malloc((sizeof(char)+1)*len);
    while (min){
       
        for (int i = 0; i < len ; i++){
            compare[i] = (terms[min].term)[i];
        }
        compare[len] = '\0';
        int res = strcmp(compare, temp_str);
        if (res != 0){
            return min-1;
        }
        min++;
    }
    return -1;
}
void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr){
    int begin_ind = lowest_match(terms, nterms, substr);
    int end_ind = highest_match(terms, nterms, substr);
    if (begin_ind == -1){
        *n_answer = 0;
        return;
    }
    if (begin_ind>end_ind){
        *n_answer = 0;
        return;
    }
    int num_terms = end_ind - begin_ind +1;
    *n_answer = num_terms;
    *answer = (struct term *)malloc(sizeof(struct term) * num_terms);
    for (int i = 0; i < num_terms; i++){
        strcpy((*answer)[i].term , terms[begin_ind + i].term);
        (*answer)[i].weight = terms[begin_ind + i].weight;
    }
    qsort(*answer, num_terms, sizeof(struct term), num_sort);
}
