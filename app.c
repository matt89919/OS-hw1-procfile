#include <stdio.h>
#include <stdlib.h>

void getv(FILE *fp)
{
    char result[200];
    for(int i=0; i<2; i++)	fgets(result,200,fp);
    fgets(result,200,fp);
    printf("\n");
    printf("Version: ");
    printf(result);
    printf("\n");
    printf("--------------------------------------------\n\n");
}

void getcpu(FILE *fp)
{
    char result[200];
    for(int i=0; i<5; i++)    fgets(result,200,fp);
    printf("\n");
    printf("Cpu information: \n");
    while(1)
    {
        fgets(result,200,fp);
        if(result[5]=='=')	break;
        printf(result);
    }
    printf("\n");
    printf("--------------------------------------------\n");
}

void getm(FILE *fp)
{
    char result[200];
    //getcpu but dont print
    for(int i=0; i<5; i++)    fgets(result,200,fp);
    printf("\n");
    printf("Memory information: \n");
    while(1)
    {
        fgets(result,200,fp);
        if(result[5]=='=')      break;
        //        printf(result);
    }
    while(1)
    {
        fgets(result,200,fp);
        if(result[5]=='=')	break;
        printf(result);
    }
    printf("\n");
    printf("--------------------------------------------\n");
}


void gett(FILE *fp)
{
    char result[200];
    //getcpu but dont print
    for(int i=0; i<5; i++)    fgets(result,200,fp);
    printf("\n");
    printf("Time information: \n");
    while(1)
    {
        fgets(result,200,fp);
        if(result[5]=='=')      break;
        //        printf(result);
    }
    //getm but dont print
    while(1)
    {
        fgets(result,200,fp);
        if(result[5]=='=')      break;
        //       printf(result);
    }

    fgets(result,200,fp);
    printf(result);
    fgets(result,200,fp);
    printf(result);

    printf("\n");
    printf("--------------------------------------------\n");
}


void geta(FILE *fp)
{
    char result[200];
    printf("\n");
    fgets(result,200,fp);
    while(1)
    {
        fgets(result,200,fp);
        if(feof(fp)) break;
        printf(result);
    }

    printf("\n");
    printf("--------------------------------------------\n\n");
}


int main()
{
    char input,o;

    while(1)
    {
        FILE *fp=fopen("/proc/my_info","r");
        if(!fp)
        {
            printf("failed to open");
            exit(1);
        }
        printf("Which information do you want?\n");
        printf("Version(v),CPU(c),Memory(m),Time(t),All(a),Exit(e)?\n");
        scanf("%c%c", &input, &o);
        switch(input)
        {
        case 'v':
            getv(fp);
            break;
        case 'c':
            getcpu(fp);
            break;
        case 'm':
            getm(fp);
            break;
        case 't':
            gett(fp);
            break;
        case 'a':
            geta(fp);
            break;
        case 'e':
            fclose(fp);
            return 0;
            break;
            /*		default:
            			printf("please enter valid character\n");
            			break;
            */
        }

        fclose(fp);
    }
    return 0;
}
