#include <stdio.h>
#include <stdlib.h>

typedef struct triangle_
{
    int leftVertex;
    int middleVertex;
    int rightVertex;
} triangle;

triangle inMF = {1,2,3};

int main(int argc, char* argv[])
{
    inMF.leftVertex = 3;
    inMF.middleVertex = 3;
    inMF.rightVertex = 3;

    printf("%d %d %d",inMF.leftVertex, inMF.middleVertex, inMF.rightVertex);

    return 0;
}
