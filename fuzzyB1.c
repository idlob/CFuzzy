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

//ruleOperator[numOfRules]
//ruleInOperands[numOfInVars][numOfRules]
//ruleOutOperands[numOfOutVars][numOfRules]
char ruleOperator[9] = {AND, AND, AND, AND, AND, AND, AND, AND, AND};

int ruleInOperands[2][9] = { {1, 1, 1, 2, 2, 2, 3, 3, 3},
                             {1, 2, 3, 1, 2, 3, 1, 2, 3}};
int ruleOutOperands[2][9] = {{1, 1, 1, 2, 2, 2, 3, 3, 3},
                             {2, 2, 3, 1, 2, 3, 1, 2, 3}};
char isError;
int numOfInVars;
int numOfOutVars;
int numOfRules;
int sumNumOfInMFs;
float dom[2][3];
float input[2];
float output[2];
float mu[9];

//inference: Sugeno, aggregation method: max
void defuzzification()
{
    int ruleNum, inVNum;
    double sum;
    for(inVNum = 0; inVNum < numOfInVars; inVNum++)
    {
        sum = 0;
        for(ruleNum = 0; ruleNum < numOfRules; ruleNum++)
        {
            if(ruleInOperands[inVNum][ruleNum])
            {
                output[inVNum] += mu[ruleNum] * outputMF[inVNum].inMFs[ruleOutOperands[inVNum][ruleNum]-1].middleVertex;
                sum += mu[ruleNum];
            }
        }
        output[inVNum] /= sum;
    }
}

//implication method: min
void evalRules()
{
    int ruleNum, inVNum, ruleOp;
    double prevDom;
    for(ruleNum = 0; ruleNum < numOfRules; ruleNum++)
    {
        prevDom = dom[0][ruleInOperands[0][ruleNum]-1]; //oh fuck :S
        ruleOp = ruleOperator[ruleNum];
        for(inVNum = 0; inVNum < numOfInVars; inVNum++)
            if(ruleInOperands[inVNum][ruleNum])
            {
                if(ruleOp == AND)
                    prevDom = FZYAND(prevDom, dom[inVNum][ruleInOperands[inVNum][ruleNum]-1]);
                else
                    prevDom = FZYOR(prevDom, dom[inVNum][ruleInOperands[inVNum][ruleNum]-1]);
            }
        mu[ruleNum] = prevDom;
    }
}

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
    int i,j,numOfMFs;
    for(i = 0; i < numOfInVars; i++)
    {
        numOfMFs = inputMF[i].numOfMFs;
        for(j = 0; j < numOfMFs; j++)
            dom[i][j] = fuzzTriangle(input[i], inputMF[i].inMFs[j]);
    }
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
    numOfRules = sizeof(ruleOperator)/sizeof(char);
    for(i = 0; i < numOfInVars; i++) sumNumOfInMFs += inputMF[i].numOfMFs;
}

int main(int argc, char* argv[])
{
    int i,j;

    init();

    input[0] = 2.5;
    input[1] = 8.11;

    fuzzification();
    evalRules();
    defuzzification();

    for(j=0;j<2;j++) for(i=0;i<3;i++) printf("%f ",dom[j][i]);
    printf("\n\n");
    for(i=0;i<9;i++) printf("%f\n",mu[i]);
    printf("\n");
    for(i=0;i<2;i++) printf("%f\n",output[i]);

    return 0;
}
