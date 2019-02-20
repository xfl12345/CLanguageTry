#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
//#include<string.h>
#include<time.h>
#include<pthread.h>
struct num
{
    long int start;
    long int end;
    long long int result;
};
typedef struct num   snum;
void sumUp(snum *src);
void snumInit(snum *aim);
void calculateTime(clock_t start,clock_t end);
int main(void)
{
    int i,i2;
    long long int result=0;
    clock_t start;
    printf("Please Enter a num:");
    scanf("%d",&i);
    printf("Please switch a mode:");
    scanf("%d",&i2);
    start = clock();
    if(i2 == 1)
    {
        snum direct;
        snumInit(&direct);
        direct.end = i;
        sumUp(&direct);
        result = direct.result;
    }
    else
    {
        pthread_t thread_id[5];
        pthread_attr_t attr[5];
        bool flag;
        snum sss[5];
        long int part = i/4;
        long int yu = i%4;
        int err;
        for(i2=0;i2<4;i2++)
        {
            snumInit(&sss[i2]);
            sss[i2].start = i2 * part +1;
            sss[i2].end = sss[i2].start + part -1;
            pthread_attr_init(&attr[i2]);
            pthread_attr_setdetachstate(&attr[i2] , PTHREAD_CREATE_DETACHED);
            if(err = pthread_create(&thread_id[i2],&attr[i2],sumUp,&sss[i2])==0)
            {
                printf("线程%d创建成功！\n",i2+1);
            }
            else
            {
                printf("线程%d创建失败！\n",i2+1);
                exit(err);
            }
        }
        if(yu != 0)
        {
        	snumInit(&sss[i2]);
            sss[i2].start = part * 4 +1;
            sss[i2].end = i;
            pthread_attr_init(&attr[i2]);
            pthread_attr_setdetachstate(&attr[i2] , PTHREAD_CREATE_DETACHED);
            if(err = pthread_create(&thread_id[i2],NULL,sumUp,&sss[i2])==0)
            {
                printf("线程%d创建成功！\n",i2+1);
            }
            else
            {
                printf("线程%d创建失败！\n",i2+1);
                exit(err);
            }
            pthread_join(thread_id[i2],NULL);
            result = result + sss[i2].result;
        }
        flag = true;
        while(flag)
        {
            printf("\r90c\r各线程完成情况：",' ');
            for(flag = false,i2=0 ; i2<4 ; i2++)
            {
                printf("线程%d：",i2+1);
                if(err = pthread_kill(thread_id,0) == ESRCH)
                {
                    printf("Done;");
                }
                else if(err == 0)
                {
                    printf("Runing;");
                    flag = true;
                }
                else
                {
                    printf("线程出错！程序终止！");
                    exit(err);
                }
                /*
                if(kill_rc == ESRCH)
                    printf("the specified thread did not exists or already quit/n");
                else if(kill_rc == EINVAL)
                    printf("signal is invalid/n");
                else
                    printf("the specified thread is alive/n");
                */
            }
        }
        for(i2=0 ; i2<4 ; i2++)
            result = result + sss[i2].result;
    }
    printf("\nresult=%lld\n",result);
    printf("累计耗时：");
    calculateTime(start,clock());

    return 0;
}
void sumUp(snum *src)
{
    long long int i=0 ;
    long long int *res=&(src->result);
    for(i = src->start; i <= src->end ; i++)
    {
        *res = *res + i;
    }
    return;
}
void snumInit(snum *aim)
{
    aim->end = aim ->start = aim->result = 0;
    return;
}
void calculateTime(clock_t start,clock_t end)
{
	long int msSrc = end - start; /*ms Src Int*/
	double src=msSrc*0.001 ,sec;   /*sec Src double*/
	long int intTmp = src ;/*sec Src Int*/
	short min,hour,day;
	sec = (double)(intTmp % 60) + (src - intTmp); /*sec double*/
	if(src < 60)
	{
		printf("%lf秒",sec);
	}
	else if(src < 3600)
	{
		min = intTmp / 60;
		printf("%d分钟%lf秒",min,sec);
	}
	else if(src < 86400)
	{
		hour = intTmp / 3600;
		min = (intTmp % 3600)/60;
		printf("%d小时%d分钟%lf秒",hour,min,sec);
	}
	else
	{
		day = intTmp / 86400;
		hour = (intTmp % 86400)/3600;
		min = (intTmp % 3600)/60;
		printf("%d天%d小时%d分钟%lf秒",day,hour,min,sec);
	}
	printf("%-10c",' ');
	return;
}
