#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#define preOfPI 106
#define readyNumSize 500
struct num{
	char *shuZi;
	int xsd;    /*xsd即小数点，记录小数点的下标 (radix point)*/
	int length;   /*length即数字的数组长度*/
	int intLength; /*intLength即整数部分的长度*/
	int fractionLength; /*fractionLength即小数部分的长度*/
	//int validIntStart; /*非零数起始下标*/
	int xb; /*xb即下标 (position)*/
	bool is_positive;   /*判断是否为正数*/
	bool intIsZero;     /*判断整数部分是否为零*/
	bool xsIsZero;     /*判断小数部分是否为零*/
	bool moveOneStep;   /*仅移动一步，值为真时，下标应减一*/
};
typedef struct num    snum;

/**定义进制，范围应在char的大小以内**/
int jinZhi = 10;  /*此处定义为十进制*/

const bool is_DeBugMode=false;
int times=0;

/*mode=1:plus,mode=2:minus,mode=3:multiply,mode=4:divide;
*模式1-4分别为加减乘除
*/
char *bigNumCompute(char shu1[],char shu2[],bool headspace,int mode,int precision,char **remainder);
/*加法、减法和乘法，本质上都是加法，
*此函数为方便除法运算而设计，同时给bigNumComptue瘦身
*/
void plusUnit(snum *s1,snum *s2,char *result,int *i,int mode);

/*limitSize允许外部传入数据以控制函数分析范围，如果不是正数，则默认读完整个数组，默认最后一位是结束符（'\0'）*/
void analyzeNum(snum *num,int limitSize);
/*顾名思义，covertInt2Char即整型值转换成字符型值*/
void covertInt2Char(char *result,snum *aim,bool headspace);
void justCopyResult(char *result,char *num1,char *num2,int size,int mode);

int serialZeroCount(char *shuZi,int s1end);
/*除去字符型数组中的无效字符*/
void jumpUselessChar(char **ShuZi);

/*charSwap顾名思义，即字符型变量交换*/
void charSwap(char *a,char *b);
/*charPtrSwap即charPointerSwap
*顾名思义，即字符型指针变量交换
*/
void charPtrSwap(char **a,char **b);
void strcpy2(char *a,char *b,int s1end);
/*getMaxInt顾名思义，即获取a,b两者中最大的值*/
int getMaxInt(int a,int b);
int justGetNum(void);
int limitInputNum(int min,int max);

bool judgeSmallerNum(char num1[],char num2[],int s1end,int s2end);
bool judgeSmallerInt(char num1[],char num2[],int s1end,int s2end);
bool judgeSmallerXS(char num1[],char num2[],int s1end,int s2end);
bool strcmp2(char str1[],char str2[],int end);
void testSystem(char *a,char *b);
void memeryIsNotEnough(void);
/*Have not finished yet...*/
char *getPI(void);

int main(void)
{
	int i,precision=0;
	char *shu1,*shu2,*result;
	shu1=shu2=result=NULL;
	/*puts(result=getZeroStr(6));
	printf("strSize=%d\n",_msize(result));
	testSystem(shu1,shu2);
	i=4;precision=500;
	printf("Please define the length of Num:");
	i = limitInputNum(1,2100000000);
	shu1 = (char *)malloc(sizeof(char)*(i+1));
	shu2 = (char *)malloc(sizeof(char)*(i+1));
	if(shu1 == NULL || shu2 == NULL)
		exit(444);
	printf("Please input Num1:");
	scanf("%s",shu1);
	printf("Please input Num2:");
	scanf("%s",shu2);
	printf("\nNumber init is completed!!!\n\n");
	printf("Please choose a mode:");
	scanf("%d",&i);
	if(i == 4)
	{
		printf("Please input precision:");
		scanf("%d",&precision);
	}
	if(precision<0 || i<0 || i>4)
		exit(250);
	printf("\n\n");
	*/
	result = getPI();
	//result=bigNumCompute(shu1,shu2,false,i,precision,NULL);
	while(result[++i]!='\0');
	printf("Result=%s\n",result);
	printf("\nstrlen=%d\n",i);
	getchar();
	/*printf("strSize=%d\n",_msize(result));*/
	return 0;
}
char *bigNumCompute(char shu1[],char shu2[],bool headspace,int mode,int precision,char **remainder)
{
	int i,i2;
	int tmpInt,minSize=0,moveStep=0;
	bool flag1,flag2;
	bool is_syn=false,is_positive=true,need_roundUp=false;
	char *tmpCharPoint,*shu1src,*shu2src;
	/*startPoint记录“结果”里的小数点的下标，
	*但不同算法，由于i的移动方向不一样
	*startPoint的值并不是统一的startPoint = i*/
	int startPoint=-1;
	char *result,*yu2,*buff;
	char oneCharStr[2]={'0','\0'},zeroCopy[2]={'0','\0'};
	i=i2=tmpInt=0;
	flag1=flag2=false;
	result = yu2 = buff = tmpCharPoint = shu1src = shu2src = NULL;
	if(is_DeBugMode)
	{
		printf("***这是第%d次运行计算***************************\n",times++);
		printf("执行任务为：");
		switch (mode)
		{
			case 1:printf("加法\n");break;
			case 2:printf("减法\n");break;
			case 3:printf("乘法\n");break;
			case 4:printf("除法\n");break;
			default:
				break;
		}
		printf("初始数据shu1:%s\n",shu1);
		printf("初始数据shu2:%s\n",shu2);
	}
	if(shu1 == NULL || shu2 == NULL)
		exit(250);
	if(shu1[0]=='-')
	{
		shu1++;/*截掉负号*/
		flag1=true;
	}
	if(shu2[0]=='-')
	{
		shu2++;
		flag2=true;
	}
	jumpUselessChar(&shu1);
	jumpUselessChar(&shu2);
	if(mode == 4)
	{
		snum ss1,ss2;
		ss1.shuZi = shu1;
		ss2.shuZi = shu2;
		analyzeNum(&ss2,-1);
		analyzeNum(&ss1,-1);
		shu1src = (char *)malloc(sizeof(char)*(ss1.length +3) );
		if( shu1src == NULL )
			memeryIsNotEnough();
		strcpy(shu1src,ss1.shuZi);
		shu2src = (char *)malloc(sizeof(char)*(ss2.length +3) );
		if( shu2src == NULL )
			memeryIsNotEnough();
		strcpy(shu2src,ss2.shuZi);
		if( ss2.xsd != -1 )
		{
			if(ss2.xsIsZero)
			{
				shu2src = (char *)realloc(shu2src,sizeof(char)*(ss2.length - ss2.fractionLength +3));
				if(shu2src == NULL)
					memeryIsNotEnough();
				strcpy2(shu2src,ss2.shuZi,ss2.xsd);
				shu2src[ss2.xsd]='\0';
			}
			else if( ss2.fractionLength > ss1.fractionLength )
			{
				shu1src = (char *)realloc(shu1src, sizeof(char)*(ss1.length + ss2.fractionLength - ss1.fractionLength +3) );
				if( shu1src == NULL )
					memeryIsNotEnough();
				ss1.xb = ss1.length;
				if(ss1.xsd == -1)
				{
					ss1.xsd = ss1.length ++;
					shu1src[ss1.xsd]='.';
					ss1.xb++;
				}
				for(  ; ss1.xb < ss1.length + ss2.fractionLength - ss1.fractionLength ; ss1.xb++ )
					shu1src[ss1.xb]='0';
				shu1src[ss1.xb]='\0';
			}
			ss1.xb = ss1.xsd;
			ss2.xb = ss2.xsd;
			while( ss2.xb < ss2.length )
			{
				charSwap(&shu1src[ss1.xb] , &shu1src[ss1.xb+1]);
				charSwap(&shu2src[ss2.xb] , &shu2src[ss2.xb+1]);
				ss1.xb++;
				ss2.xb++;
				moveStep++; 
			}
			shu1src[ss1.xb +1]='\0';
		}
		shu1 = shu1src;
		shu2 = shu2src;
		jumpUselessChar(&shu1);
		jumpUselessChar(&shu2);
	}
	else
	{
		shu1src = shu1;
		shu2src = shu2;
	}
	if(is_DeBugMode)
	{
		printf("备份数据shu1src:%s\n",shu1src);
		printf("备份数据shu2src:%s\n",shu2src);
		printf("操作数据shu1:%s\n",shu1);
		printf("操作数据shu2:%s\n",shu2);
	}
	if(  (!flag1 && flag2) || (flag1 && !flag2)  )
	{/**一个正数和一个负数的情况下*/
		switch (mode)
		{
			case 1:
				/**正数加负数运算 和 负数加正数运算 可以化作减法运算**/
				if(!flag1)
					tmpCharPoint=bigNumCompute(shu1,shu2,false,2,0,NULL);
				else
					tmpCharPoint=bigNumCompute(shu2,shu1,false,2,0,NULL);
				return tmpCharPoint;/***大数字减法已经解决好符号问题，直接返回答案***/
				break;
			case 2:
				/**正数加负数运算 和 负数加正数运算 可以化作减法运算**/
				if(flag1 && !flag2)
				{
					tmpCharPoint=bigNumCompute(shu1,shu2,true,1,0,NULL);
					tmpCharPoint[0]='-';/***若是负减正运算，则结果的符号还是负号***/
				}
				else
				{
					tmpCharPoint=bigNumCompute(shu1,shu2,false,1,0,NULL);
				}
				return tmpCharPoint;
			case 3:case 4:
				is_positive=false;
				break;
			default:
				break;
		}
	}
	else if(flag1 && flag2)
	{/**减法，负负相减，转成正减正;乘法和除法，负负得正，无需改变初始数据**/
		if(mode==2)
			return bigNumCompute(shu2,shu1,false,2,0,NULL);
	}
	snum s1,s2,tmpSnum,*yu=NULL;
	s1.is_positive = s2.is_positive = true;
	if(flag1)
		s1.is_positive = false;
	if(flag2)
		s2.is_positive = false;
	while(shu1[++i]!='\0');
	while(shu2[++i2]!='\0');
	/**************交换空间，方便设计****************/
	if(mode == 2)
	{
		if(judgeSmallerNum(shu1,shu2,i-1,i2-1))/*默认shu1数字最大*/
		{/*如果shu2数字更大，则交换名称*/
			charPtrSwap(&shu1,&shu2);
			charPtrSwap(&shu1src,&shu2src);
			is_positive = false;/*结果取负*/
		}
	}
	else if(mode == 3)
	{
		if(i < i2)
		{
			charPtrSwap(&shu1,&shu2);
			charPtrSwap(&shu1src,&shu2src);
		}
	}
	s1.shuZi = shu1;
	s2.shuZi = shu2;
	analyzeNum(&s1,-1);
	analyzeNum(&s2,-1);
	/**检查是否有数字为零**/
	if(mode == 3 || mode == 4)
	{
		if( (s1.intIsZero && s1.xsIsZero) || (s2.intIsZero && s2.xsIsZero) )
		{
			is_positive = true;/**总不能出现“负零”的情况吧**/
			/**若除数为零**/
			if(mode == 4)
			{
				puts(shu1src);
				puts(shu2src);
				free(shu2src);
				/**检查除数是否为零**/
				if(s2.intIsZero && s2.xsIsZero)
				{
					tmpCharPoint = (char *)malloc(sizeof(char)*8);
					if(tmpCharPoint == NULL)
						memeryIsNotEnough();
					strcpy(tmpCharPoint,"error!");/**那就直接报错，不用算了**/
					free(shu1src);
					return tmpCharPoint;
				}
				return shu1;/**已经做了dropPort处理，直接返回被除数**/
			}
		}
	}
	/*****************运算结果临时存储空间初始化************************/
	minSize = getMaxInt(s1.intLength, s2.intLength) + \
		getMaxInt(s1.fractionLength, s2.fractionLength) + \
		precision + 2 ;/*预留2位空间做进位处理*/
	result=(char *)malloc(sizeof(char)*(minSize +8));
	if(result == NULL)
		memeryIsNotEnough();
	memset(result,0,minSize);
	result[minSize]='\0';
	/**初始化余数空间**/
	if(mode == 4)
	{
		minSize--;  /**只留多一位做四舍五入判断**/
		if(s1.xsd == -1 )
			minSize--;
		result[minSize]='\0';
		yu = (snum *)malloc(sizeof(snum));
		if(yu == NULL)
			memeryIsNotEnough();
		yu->shuZi = (char *)malloc(sizeof(char)*(s2.length +8));
		buff = (char *)malloc(sizeof(char)*(s2.length +8));
		yu2 = yu->shuZi;
		if(yu2 == NULL || buff == NULL)
			memeryIsNotEnough();
		memset(yu2,'\0',s2.length +4);
		memset(buff,'\0',s2.length +4);
		yu2[0]='1';
		analyzeNum(yu,-1);
		yu2[0]='\0';
		yu->length = 0;
		//yu2[yu->length++]=shu1[s1.xb++];
		yu2[yu->length]='\0';
	}
	/********************运算框架***********************/
	s1.moveOneStep = s2.moveOneStep = is_syn = flag1 = flag2 = false;
	s1.xb = s1.length -1;
	s2.xb = s2.length -1;
	i=minSize-1;
	if(mode == 4)
		s1.xb = i = 0;
	while(  (!flag1 || !flag2) && i>=0  )
	{/*****乘法和除法都需要需要异步处理*****/
		if(mode == 3)
		{
			if(s2.xb < 0)
				break;
			i = minSize -1 - (s2.length -1 - s2.xb);
			if(s2.shuZi[s2.xb]=='.')
				s2.xb--;
		}
		else if(mode == 4)
		{	/*****处理小数点*****/
			if( s1.shuZi[s1.xb]=='.' )
			{
				startPoint = i;
				s1.xb++;
			}
			if( i == minSize || (precision==0 && flag1 && yu2[0]=='0') )
			{
				//printf("\n最终余数：%s\n",yu2);
				if(yu2[0]!='0' && result[i-1] >= 5)
					need_roundUp = true;
				minSize--;/**丢弃用于判断四舍五入的最后一位**/
				break;
			}
			if(yu2[0]=='0')
				yu->length = 0;
			yu2[yu->length++]=s1.shuZi[s1.xb];
			yu2[yu->length]='\0';
			/**判断原始被除数是否读完**/
			if(!flag1)
			{	/**如果读完了**/
				if(s1.xb +1 == s1.length)
				{
					flag1 = true;
					s1.shuZi = zeroCopy;/**抄零处理**/
					s1.xb = 0;
					if(startPoint == -1)
						startPoint = i+1;
				}
				else
				{
					s1.xb++;
				}
			}
		}
		else
		{	/*****同步处理*****/
			if(!is_syn)
			{
				if( !flag1 && !flag2)
				{
					if(s1.fractionLength > s2.fractionLength)
					{
						if( s1.fractionLength - (s1.length -1 - s1.xb) == s2.fractionLength )
						{/*当两个数字的下标与各自的小数点距离相等的时候，结束抄零处理，同步进行运算*/
							s2.shuZi = shu2src;
							s2.xb = s2.length -1;
							is_syn=true;
						}
						else
						{
							s2.shuZi = zeroCopy;
							s2.xb = 0;
							s1.moveOneStep = true;
						}
					}
					else if( s1.fractionLength < s2.fractionLength )
					{
						if( s2.fractionLength - (s2.length -1 - s2.xb) == s1.fractionLength )
						{
							s1.shuZi = shu1src;
							s1.xb = s1.length -1;
							is_syn=true;
						}
						else
						{
							s1.shuZi = zeroCopy;
							s1.xb = 0;
							s2.moveOneStep = true;
						}
					}
					else
					{/*整数运算或小数位数相同，直接同步*/
						is_syn=true;
					}
				}
				else
				{
					if(flag1 && s2.xb >= 0)
						s2.moveOneStep = true;
					else if(flag2 && s1.xb >= 0)
						s1.moveOneStep = true;
				}
			}
			/*******处理小数点******/
			if( shu1[s1.xb]=='.' || shu2[s2.xb]=='.' )
			{
				startPoint = i +1;
				if(s1.xsd != -1)
					s1.xb--;
				if(s2.xsd != -1)
					s2.xb--;
			}
		}
		
		/*****运算核心*****/
		if(mode != 4)
		{
			plusUnit(&s1,&s2,result,&i,mode);
		}
		else
		{	/**如果被除数位数比除数位数少**/
			if(yu->length < s2.length)
			{
				result[i]=0;/**被除数小于除数，商为零**/
			}	 /**如果被除数位数和除数位数相等**/
			else if( yu->length == s2.length && \
			judgeSmallerInt(yu2,s2.shuZi,yu->length -1,s2.length -1) )
			{	/**那就看看被除数是否更小**/
				result[i]=0;
			}
			else
			{
				if(yu->length == s2.length)
				{
					result[i] = (yu2[0] - '0') / (s2.shuZi[0] - '0');
				}
				else
				{	/**如果被除数位数比除数位数多（最多也就多一位）**/
					result[i] = (yu2[0] - '0')*jinZhi + yu2[1] - '0';
					result[i] = result[i] / (s2.shuZi[0] - '0');
				}
				oneCharStr[0] = result[i] + '0';
				strcpy(buff,s2.shuZi);
				justCopyResult(buff,buff,oneCharStr,s2.length+3,3);
				justCopyResult(buff,yu2,buff,s2.length+3,2);
				while(buff[0]=='-')
				{
					result[i]--;
					if(result[i]!=0)
					{
						oneCharStr[0] = result[i] + '0';
						strcpy(buff,s2.shuZi);
						justCopyResult(buff,buff,oneCharStr,s2.length+3,3);
						justCopyResult(buff,yu2,buff,s2.length+3,2);
					}
					else
					{
						break;
					}
				}
				strcpy(yu2,buff);
				analyzeNum(yu,-1);
			}
			i++;
		}
		/*****后期同步处理*****/
		if(mode == 1 || mode == 2)
		{
			if(s1.moveOneStep)
			{
				s1.xb--;
				if(s1.xb == -1)
					flag1=true;
				s1.moveOneStep = false;
			}
			if(s2.moveOneStep)
			{
				s2.xb--;
				if(s2.xb == -1)
					flag2=true;
				s2.moveOneStep = false;
			}
			if(is_syn)
			{
				if(s1.xb > 0)
				{
					s1.xb--;
				}
				else
				{
					s1.shuZi = zeroCopy;
					flag1=true;
					is_syn=false;
				}
				if(s2.xb > 0)
				{
					s2.xb--;
				}
				else
				{
					s2.shuZi = zeroCopy;
					flag2=true;
					is_syn=false;
				}
			}
			i--;/**移动“结果”的下标**/
		}
	}
	/********************整理最终数据**********************/
	if(is_DeBugMode)
	{
		printf("结果初始数据result:");
		for(i2=0;i2<minSize;i2++)
			printf("%c",result[i2]+'0');
		printf("\n");
	}
	if(mode == 3)
	{
		if(s1.xsd != -1 || s2.xsd != -1)
		{
			tmpInt=0;
			if(s1.xsd != -1)
				tmpInt = tmpInt + s1.length -1 - s1.xsd;
			if(s2.xsd != -1)
				tmpInt = tmpInt + s2.length -1 - s2.xsd;
			startPoint = minSize -1 - tmpInt;
		}
	}
	else if(mode == 4)
	{
		minSize = i;
		i=0;
		free(buff);
		free(shu1src);
		free(shu2src);
	}
	if(i<0)
		i=0;
	while(result[i]==0 && i+1 != startPoint && i < minSize -1 )
		i++;/***除去整数部分的高位零***/
	tmpSnum.length = minSize;
	tmpSnum.xb = i;
	tmpSnum.shuZi = NULL;
	tmpSnum.xsd = startPoint;
	/*******************转化答案存储空间***********************/
	if(headspace || !is_positive)
		flag1 = true;
	else
		flag1 = false;
	covertInt2Char(result,&tmpSnum,flag1);

	if(mode == 4)
	{
		if(need_roundUp)
		{
			i = 0;
			while(tmpSnum.shuZi[++i]!='\0');
			buff = (char *)malloc(sizeof(char)*(i+5));
			if(buff == NULL)
				memeryIsNotEnough();
			memset(buff,'0',i+4);
			buff[i-1]='1';
			buff[i]='\0';
			if(startPoint != -1)
				buff[1]='.';
			tmpCharPoint = bigNumCompute(tmpSnum.shuZi,buff,flag1,1,0,NULL);
			free(buff);
			free(tmpSnum.shuZi);
			tmpSnum.shuZi = tmpCharPoint;
		}
		if(remainder != NULL)
		{
			(*remainder) = yu->shuZi;
		}
		else
		{
			free(yu->shuZi);
			free(yu);
		}
	}
	if(!is_positive)
		tmpSnum.shuZi[0]='-';
	if(is_DeBugMode)
	{
		printf("结果最终数据result:%s\n",tmpSnum.shuZi);
		printf("***汇报完毕***************************\n");
	}
	return tmpSnum.shuZi;
}
void plusUnit(snum *s1,snum *s2,char *result,int *i,int mode)
{
	char *shu1 = s1->shuZi;
	char *shu2 = s2->shuZi;
	switch(mode)
	{
		case 1:
			result[*i]=result[*i]+shu1[s1->xb]-'0'+shu2[s2->xb]-'0';
			if(result[*i] > jinZhi -1)
			{
				result[(*i)-1] += result[*i] / jinZhi;
				result[*i] = result[*i] % jinZhi;
			}
			break;
		case 2:
			result[*i]=result[*i]+shu1[s1->xb]-shu2[s2->xb];
			if(result[*i] < 0)
			{
				result[(*i)-1]--;
				result[*i] = result[*i] + jinZhi;
			}
			break;
		case 3:
			for(s1->xb = s1->length -1; s1->xb >= 0; s1->xb--,(*i)--)
			{
				if(shu1[s1->xb]=='.')
					s1->xb--;
				result[*i] += (shu2[s2->xb]-'0') * (shu1[s1->xb]-'0');
				if(result[*i] > jinZhi -1)
				{
					result[*i-1] += result[*i] / jinZhi;
					result[*i] = result[*i] % jinZhi;
				}
			}
			s2->xb--;
			break;
		default:break;
	}
	return;
}
void analyzeNum(snum *num,int limitSize)
{
	int i;
	char *shuZi=NULL;
	if(num == NULL)
		exit(250);
	shuZi = num->shuZi;
	if(shuZi[0]=='\0')
		exit(685);
	num->fractionLength = 0;
	num->xsd = -1;
	num->intIsZero = false;
	num->xsIsZero = true;
	if(shuZi[0]=='0')
		num->intIsZero = true;
	for(i=0; shuZi[i] != '\0' ;i++)
	{
		if(limitSize > 0)
			if(i > limitSize)
				break;
		if( num->intIsZero && num->xsd == -1 )
			if(shuZi[i] != '0' && shuZi[i] != '.')
				num->intIsZero = false;
		if(num->xsIsZero && num->xsd != -1 && shuZi[i]!='0')
			num->xsIsZero=false;
		if(shuZi[i] == '.')
			num->xsd = i;
	}
	num->intLength = num->length = i;
	if(num->xsd != -1)
	{
		num->fractionLength = i -1 - num->xsd;
		num->intLength = num->xsd;
	}
	return;
}
void covertInt2Char(char *result,snum *aim,bool headspace)
{
	int i,i2;
	char *charResult=NULL;
	if(headspace)
		i = aim->length - aim->xb + 3;
	else
		i = aim->length - aim->xb + 2;
	charResult=(char *)malloc(sizeof(char)*i );
	if(charResult == NULL)
		memeryIsNotEnough();
	for( i = aim->xb, i2 = 0 ; i < aim->length ; i++ )
	{
		if(i2==0 && headspace)
			charResult[i2++]=' ';//预留符号位
		if(i == aim->xsd)
			if(aim->xsd != -1)
				charResult[i2++]='.';
		charResult[i2++]=(char)(result[i]+'0');
	}
	//free(result);
	charResult[i2]='\0';
	aim->shuZi = charResult;
	return;
}
void justCopyResult(char *result,char *num1,char *num2,int size,int mode)
{
	if(result == NULL || size <=0 || \
	num1 == NULL || num2 == NULL || \
	mode <0 || mode >3 )
		exit(250);
	int i=0;
	char *buff = bigNumCompute(num1,num2,false,mode,0,NULL);
	while(buff[++i]!='\0');
	if(i>size)
	{
		printf("\n\nOut of memery!\n\n");
		exit(251);
	}
	strcpy(result,buff);
	free(buff);
	return;
}
int serialZeroCount(char *shuZi,int s1end)
{
	int i = 0 , count = 0;
	while(1)
	{
		if(s1end > 0)
		{
			if( i > s1end)
				break;
		}
		if(shuZi[i]=='\0')
			break;
		if(shuZi[i]=='0')
			count++;
		else
			break;
		i++;
	}
	return count;
}
void jumpUselessChar(char **shuZi)
{
	int i=0;
	while((*shuZi)[i]==' ')
		(*shuZi)++;/***除去空格***/
	i=serialZeroCount(*shuZi,-1);
	/***除去整数部分的高位零***/
	if((*shuZi)[i]=='.' || (*shuZi)[i]=='\0')
		(*shuZi)=(*shuZi) + i -1; /**至少保留整数部分的个位数**/
	else
		(*shuZi)=(*shuZi) + i ;
	return;
}
void charSwap(char *a,char *b)
{
	char tempChar;
	tempChar= *a;
	*a = *b;
	*b = tempChar;
	return;
}
void charPtrSwap(char **a,char **b)
{
	char *tmpChar;
	tmpChar= *a;
	*a = *b;
	*b = tmpChar;
	return;
}
void strcpy2(char *a,char *b,int s1end)
{
	int i=0;
	while(1)
	{
		if(s1end >= 0)
			if(i > s1end)
				break;
		if(b[i]=='\0')
			break;
		a[i]=b[i];
		i++;
	}
	return;
}
int getMaxInt(int a,int b)
{
	return a>b?a:b;
}
int justGetNum(void)
{
	char b[50];
	int i,result;
	while(1)
	{
		scanf("%s",b);
		for(i=0,result=0;b[i]!='\0';i++)
		{
			if( !(b[i]>='0'&&b[i]<='9'))
				break;
			result=b[i]-'0'+result*10;
		}
		if(b[i]=='\0' && i!=0)
			break;
		printf("\n请输入一个数字！\n");
		printf("请输入：");
	}
	return result;
}
int limitInputNum(int min,int max)
{
	int result;
	while(1)
	{
		result=justGetNum();
		if(result>=min && result<=max)
			break;
		printf("\n超限！请输入合理的正常值！\n");
	}
	return result;
}
bool judgeSmallerNum(char num1[],char num2[],int s1end,int s2end)
{/**返回false意味着num1>=num2**/
	snum s1,s2;
	jumpUselessChar(&num1);
	jumpUselessChar(&num2);
	s1.shuZi = num1;
	s2.shuZi = num2;
	analyzeNum(&s1 , s1end);
	analyzeNum(&s2 , s2end);
	if(s1.intLength < s2.intLength)
		return true;
	if(s1.intLength == s2.intLength)
	{/**大家整数部分位数相同**/
		if(judgeSmallerInt(num1,num2,s1.intLength -1,s2.intLength -1))
			return true;/**如果整数小，小数就不需要比较了**/
		if(strcmp2(num1,num2,s1.intLength-1))
		{
			if(!s2.xsIsZero)
			{
				if(s1.xsIsZero)
					return true;
				return judgeSmallerXS(num1 + s1.xsd +1 , num2 + s2.xsd +1 ,s1.fractionLength , s2.fractionLength );/**都有小数，一决高下**/
			}
		}
	}
	return false;
}
bool judgeSmallerInt(char num1[],char num2[],int s1end,int s2end)
{
	if(s1end > s2end)
		return false;
	if(s1end < s2end)
		return true;
	int i;/**执行到这，说明位数一致**/
	for(i=0; i <= s1end ; i++)
	{
		if(num1[i] < num2[i])
			return true;
		if(num1[i] > num2[i])
			return false;
	}
	return false;/**执行到这，说明两数相等，不过num1确实不比num2大**/
}
bool judgeSmallerXS(char num1[],char num2[],int s1end,int s2end)
{
	int i=0,i2=0;
	while(i<=s1end || i2<=s2end)
	{
		if(i > s1end)
			return true;
		if(num1[i] < num2[i2])
			return true;
		if(num1[i] > num2[i2])
			return false;
		if(i<=s1end)
			i++;
		if(i2<=s2end)
			i2++;
	}
	if( s1end == s2end)
		return false;
	exit(250);
}
bool strcmp2(char str1[],char str2[],int end)
{
	int i;
	for(i=0;i<=end;i++)
		if(str1[i]!=str2[i])
			return false;
	return true;
}
void testSystem(char *a,char *b)
{
	int i,i2;
	char *tes[3]={"999999","0.000001"};
	char *item[23][3]={ {"12","12"} , \
		{"12","11"} , {"12","1.2"} , \
		{"1.2","1.2"} , {"0","0"} , \
		{"0","-1.1"} , {"-1.1","0"} , \
		{"-1.1","11"} , {"-11","-1.1"} , \
		{"0.001","10000"} , {"10000","0.001"} , \
		{"999999","0.000001"} , {"0.02","0.003"} , \
		{"00.03","000.005"} , {"-00.03","-000.005"} , \
		{"0.0000","6"} , {"7","0.0000"} , \
		{"355","113"} , {"22","7"} , \
		{"52163","16604"}  \
	};
	printf("+----+--------+--------+\n");
	printf("|%-4s|%-8s|%-8s|\n","No.","Num1","Num2");
	printf("+----+--------+--------+\n");
	for(i=0;i<20;i++)
		printf("|%-4d|%-8s|%-8s|\n",i+1,item[i][0],item[i][1]);
	printf("+----+--------+--------+\n");
	/*计划设计成可以自动循环测试全部数据的testSystem*/
	printf("Please enter a choice:");
	scanf("%d",&i);
	i--;
	if(i>19 || i<0)
		exit(250);
	strcpy(a,item[i][0]);
	strcpy(b,item[i][1]);
}
char *getPI(void)
{
	char *shu1,*shu2,*num1,*num2;
	char *buff,*buff2;
	int i,i2;
	buff = buff2 = NULL;
	shu1 = (char *)malloc(sizeof(char)*(readyNumSize +1));
	shu2 = (char *)malloc(sizeof(char)*(readyNumSize +1));
	num1 = (char *)malloc(sizeof(char)*(readyNumSize +1));
	num2 = (char *)malloc(sizeof(char)*(readyNumSize +1));
	if( !(shu1 && shu2 && num1 && num2) )
		memeryIsNotEnough();
	memset(shu1,0,readyNumSize);
	memset(shu2,0,readyNumSize);
	memset(num1,0,readyNumSize);
	memset(num2,0,readyNumSize);
	num1[0]='2';
	num2[0]='1';
	shu1[0]='2';
	shu2[0]='1';
	for(i=1;i< preOfPI;i++)
	{
		if(is_DeBugMode)
			printf("\n\n---这是第%d次循环-------\n",i);
		if( i % 2 == 0 )
		{
			justCopyResult(num1,num1,"2",readyNumSize,1);
		}
		else
		{
			justCopyResult(num2,num2,"2",readyNumSize,1);
		}
		justCopyResult(shu1,shu1,num1,readyNumSize,3);
		justCopyResult(shu2,shu2,num2,readyNumSize,3);
	}
	if(is_DeBugMode)
		printf("\n\n---循环结束-------\n");
	justCopyResult(shu1,shu1,"2",readyNumSize,3);
	if(is_DeBugMode)
		printf("被除数：%s\n除数：%s\n\n",shu1,shu2);
	return bigNumCompute(shu1,shu2,false,4,1000,NULL);
}
void memeryIsNotEnough(void)
{
	puts("Sorry!Yours mechine did not have enough memery!");
	puts("Program terminated!");
	exit(444);
}
