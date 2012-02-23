/*
At the moment outMFs have to be isosceles with identical slope, this highly simplifies defuzzification.
This constraint doesn't apply for inMFs.
*/

#include <stdio.h>
#include <stdlib.h>

#define ABS(x)     ((x) > (0) ? (x) : (-(x)))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define FZYAND(a, b)  MIN(a,b)
#define FZYOR(a, b)   MAX(a,b)
#define AND 1
#define OR 2

typedef struct triangle_
{
    int leftVertex;
    int middleVertex;
    int rightVertex;
} triangle;

typedef struct mfs_
{
    char numOfMFs;
    triangle *inMFs;
} mfs;

triangle inMF[] = { {0,0,5}, {2,6,10}, {5,10,10} };
triangle outMF[] = { {-3,-3,1}, {-3,1,5}, {1,5,5} };
mfs inputMF[] =  { {3, inMF }, {3, inMF } };
mfs outputMF[] = { {3, outMF}, {3, outMF} };
int rules[] = {
                1, 1, 1, 1, 2,
                1, 1, 2, 1, 2,
                1, 1, 3, 1, 3,
                1, 2, 1, 2, 1,
                1, 2, 2, 2, 2,
                1, 2, 3, 2, 3,
                1, 3, 1, 3, 1,
                1, 3, 2, 3, 2,
                1, 3, 3, 3, 3
              };

//ruleOperator[numOfRules]
//ruleInOperands[numOfInVars][numOfRules]
//ruleOutOperands[numOfOutVars][numOfRules]
char ruleOperator[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

char ruleInOperands[][9] = {{1, 1, 1, 2, 2, 2, 3, 3, 3},
                            {1, 2, 3, 1, 2, 3, 1, 2, 3}};
char ruleOutOperands[][9] = {{1, 1, 1, 2, 2, 2, 3, 3, 3},
                             {2, 2, 3, 1, 2, 3, 1, 2, 3}};
char isError;
char numOfInVars;
char numOfOutVars;
char numOfRules;
int sumNumOfInMFs;
float *dom;
float *input;
float *output;
float *mu;

float fuzzTriangle(float input, triangle t)
{
    if(input < t.leftVertex || input > t.rightVertex) return 0.0;
    if(input < t.middleVertex)
        return ABS((1.0/(t.middleVertex - t.leftVertex))*(input - t.leftVertex));
    else
        return ABS((1.0/(t.middleVertex - t.rightVertex))*(t.rightVertex-input));
}

void fuzzification()
{
    int i,j,k,l=0;
    for(i = 0; i < numOfInVars; i++)
    {
        j = inputMF[i].numOfMFs;
        for(k = 0; k < j; k++)
            dom[l++] = fuzzTriangle(input[i], inputMF[i].inMFs[k]);
    }
}

void evalRules2()
{


}

void evalRules()
{
    int i, row, cell, pos, ruleLenght = numOfInVars + numOfOutVars + 1;
    double prev;
    for(row = 0; row < numOfRules; row++)
    {
        cell = row * ruleLenght + 1;
        prev = dom[rules[cell]-1];
        if(rules[cell-1] == AND)
            for(i = 1; i < numOfInVars; i++)
            {
                pos = rules[cell+i];
                if(pos)
                    prev = FZYAND(prev, dom[i*3+(pos-1)]); //TODO: clean up, subst const 3 (sum of prev mfs lenght)
            }

        else
            for(i = 1; i < numOfInVars; i++)
            {
                pos = rules[cell+i];
                if(pos)
                    prev = FZYOR(prev, dom[i*3+(pos-1)]); //TODO: clean up, subst const 3 (sum of prev mfs lenght)
            }
        mu[row] = prev;
    }
}

void defuzzification()
{
    int i, row, cell, ruleLenght = numOfInVars + numOfOutVars + 1;
    double sumMu = 0;
    for(i = 0; i < numOfOutVars; i++)
        output[i] = 0;
    for(i = 0; i < numOfRules; i++)
        sumMu += mu[i];
    for(row = 0; row < numOfRules; row++)
    {
        cell = row * ruleLenght + numOfInVars + 1;
        for(i = 0; i < numOfOutVars; i++)
        {
            output[i] +=  mu[row] * outputMF[i].inMFs[rules[cell+i]-1].middleVertex;
        }
    }
    for(i = 0; i < numOfOutVars; i++)
        output[i] /= sumMu;
}

void error(char *message)
{
    isError++;
    printf("\n%s\n", message);
}

void init()
{
    int i;
    isError = 0;
    numOfInVars = sizeof(inputMF)/sizeof(mfs);
    numOfOutVars = sizeof(outputMF)/sizeof(mfs);
    numOfRules = sizeof(rules) / (sizeof(int)*(numOfInVars+numOfOutVars+1));
    input = malloc(sizeof(float)*numOfInVars);
    output = malloc(sizeof(float)*numOfOutVars);
    for(i = 0; i < numOfInVars; i++) sumNumOfInMFs += inputMF[i].numOfMFs;
    dom = malloc(sizeof(float)*sumNumOfInMFs);
    mu = malloc((sizeof(float)*numOfRules));
    if(input == NULL || output == NULL || dom == NULL || mu == NULL) error("We are doomed.");
}

int main(int argc, char* argv[])
{
    int i;

    init();

    input[0] = 2.5;
    input[1] = 8.106;

    fuzzification();
    evalRules();
    defuzzification();

    for(i=0;i<sumNumOfInMFs;i++) printf("%f ",dom[i]);
    printf("\n\n");
    for(i=0;i<9;i++) printf("%f\n",mu[i]);
    printf("\n");
    for(i=0;i<2;i++) printf("%f\n",output[i]);

    free(input);
    free(output);
    free(dom);
    free(mu);
    return 0;
}
