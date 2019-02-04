#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
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
/*重新分配数字字符数组的空间，并完成自定义拷贝*/
void reallocStr(char **aim,int size,int startP,int endP);
/*dropPort顾名思义，即丢掉小数点，
*此函数设计专为除法服务，会把数字b（分母，除数）化成整数，
*数字a和数字b各自的小数点将移动同样多的位数，
*最终返回移动位数
*/
int dropPort(char **a,char **b);
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
char *getZeroStr(int fractionLength);

bool judgeSmallerNum(char num1[],char num2[],int s1end,int s2end);
bool judgeSmallerInt(char num1[],char num2[],int s1end,int s2end);
bool judgeSmallerXS(char num1[],char num2[],int s1end,int s2end);
bool strcmp2(char str1[],char str2[],int end);
void testSystem(char *a,char *b);

int main(void)
{
	int i=0;
	char shu1[200],shu2[200],*result=NULL;
	/*puts(result=getZeroStr(6));
	printf("strSize=%d\n",_msize(result));*/
	//gets(shu1);
	//gets(shu2);
	testSystem(shu1,shu2);
	printf("\n\n");
	result=bigNumCompute(shu1,shu2,false,3,0,NULL);
	while(result[++i]!='\0');
	puts(result);
	printf("\nstrlen=%d\n",i);
	/*printf("strSize=%d\n",_msize(result));*/
	/*
	for(i=0;result[i]!='\0';i++)
		printf("%c",result[i]);
	*/
	return 0;
}
char *bigNumCompute(char shu1[],char shu2[],bool headspace,int mode,int precision,char **remainder)
{
	int i,i2;
	bool flag1,flag2;
	bool is_syn=false,is_positive=true;
	char *tmpCharPoint=NULL,*backup1,*backup2;
	int tmpInt;
	i=i2=tmpInt=0;
	flag1=flag2=false;
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
		tmpInt = dropPort(&shu1,&shu2);
		backup1 = shu1;
		backup2 = shu2;
		jumpUselessChar(&shu1);
		jumpUselessChar(&shu2);
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
	int minSize=0,startPoint;
	/*startPoint记录“结果”里的小数点的下标，默认是此下标的后一位*/
	char *result,*buff,zeroCopy[2]={'0','\0'},*yu2;
	bool tmpBool,is_divided;
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
			is_positive = false;/*结果取负*/
		}
	}
	else if(mode == 3)
	{
		if(i < i2)
			charPtrSwap(&shu1,&shu2);
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
				free(shu2);
				/**检查除数是否为零**/
				if(s2.intIsZero && s2.xsIsZero)
				{
					tmpCharPoint = (char *)malloc(sizeof(char)*8);
					if(tmpCharPoint == NULL)
						exit(444);
					strcpy(tmpCharPoint,"error!");/**那就直接报错，不用算了**/
					free(shu1);
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
	result=(char *)malloc(sizeof(char)*(minSize +1));
	if(result == NULL)
		exit(444);
	for(i=0;i<minSize;i++)
		result[i]=0;
	result[i]='\0';
	/**提前搞定余数空间大小的问题，同时做好必要的初始化工作**/
	if(mode == 4)
	{
		is_divided = false;
		startPoint = -1;
		tmpInt = s1.intLength -1;
		if(s1.intIsZero && s1.xsd != -1)
		{
			tmpInt = 0;
			startPoint = 1;
			s1.xb = 2;
		}
		else if( tmpInt > s2.length -1 )
		{
			tmpInt = s2.length -1;
			s1.xb = s2.length ;
			if(s2.length > s1.length)
				s1.xb = s1.length;
		}
		yu = (snum *)malloc(sizeof(snum));
		if(yu == NULL)
			exit(444);
		yu->shuZi = (char *)malloc(sizeof(char)*(s2.length +3));
		strcpy2(yu->shuZi,s1.shuZi,tmpInt);
	}
	/********************运算框架***********************/
	s1.moveOneStep = s2.moveOneStep = is_syn = flag1 = flag2 = false;
	s1.xb = s1.length -1;
	s2.xb = s2.length -1;
	i=minSize-1;
	if(mode == 4)
		i = 0;
	else
		startPoint = -1 ;
	while(  (!flag1 || !flag2) && i>=0  )
	{
		if(mode == 3)
		{/*****乘法需要异步处理*****/
			if(s2.xb < 0)
				break;
			i = minSize -1 - (s2.length -1 - s2.xb);
			if(shu2[s2.xb]=='.')
				s2.xb--;
		}
		else if(mode == 4)
		{	/**判断余数是否为零，并完成置零**/
			/*
			if(readDNum(yu,0)==0)
			{
				buff = outputDNum(yu);
				if(serialZeroCount(buff,yu->length) == yu->length)
					setZeroDNum(yu);
			}
			*/
			/*******处理小数点******/
			if( shu1[s1.xb]=='.' )
			{
				startPoint=i;
				s1.xb++;
			}
			yu2 = yu->shuZi;
			/**优先确保被除数比除数大**/
			/**如果原始被除数没有读完**/
			if(!flag1)
			{	/**跳过第一次循环**/
				if(i > 0)
				{	/**如果被除数和除数位数相同**/
					if( yu->length -1 == s2.intLength -1)
					{
						/**如果被除数首位没有除数大**/
						if( yu2[0] < shu2[0] )
						{
							yu2[yu->length++]=shu1[s1.xb];/**那就从原始被除数里读入一位**/
							yu2[yu->length]='\0';
							/* yu->intLength++; 余数本来就是整数，length恒等intLength */
							s1.moveOneStep = true;
						}    /**如果被除数首位和除数一样大**/
						else if (  yu2[0] == shu2[0]  )
						{
							/**那就深入比对，如果被除数小于除数**/
							if(judgeSmallerInt(yu2,shu2,yu->length -1,s2.length -1))
							{
								yu2[yu->length++]=shu1[s1.xb];
								yu2[yu->length]='\0';
								s1.moveOneStep = true;
							}
						}
					}
				}
			}
			else
			{
				/**除法不需要访问进位空间**/
				if( i >= minSize-3 || serialZeroCount(yu2,yu->length -1)== yu->length)
				{
					printf("\n最终余数：%s\n",yu2);
					free(shu1);
					free(shu2);
					break;
				}
				yu2[yu->length++]='0';
				yu2[yu->length]='\0';/**原始被除数读完了，只能补零了**/
			}
			
		}
		else
		{/*****同步处理******/
			if(!is_syn)
			{
				if( !flag1 && !flag2)
				{
					if(s1.fractionLength > s2.fractionLength)
					{
						if( s1.fractionLength - (s1.length -1 - s1.xb) == s2.fractionLength )
						{/*当两个数字的下标与各自的小数点距离相等的时候，结束抄零处理，同步进行运算*/
							shu2 = s2.shuZi;
							s2.xb = s2.length -1;
							is_syn=true;
						}
						else
						{
							shu2 = zeroCopy;
							s2.xb = 0;
							s1.moveOneStep = true;
						}
					}
					else if( s1.fractionLength < s2.fractionLength )
					{
						if( s2.fractionLength - (s2.length -1 - s2.xb) == s1.fractionLength )
						{
							shu1 = s1.shuZi;
							s1.xb = s1.length -1;
							is_syn=true;
						}
						else
						{
							shu1 = zeroCopy;
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
				startPoint=i;
				if(s1.xsd != -1)
					s1.xb--;
				if(s2.xsd != -1)
					s2.xb--;
			}
		}
		
		/********************运算核心***********************/
		if(mode != 4)
		{
			plusUnit(&s1,&s2,result,&i,mode);
		}
		else
		{
			/**如果被除数长度比除数长度大**/
			if(yu->length > s2.length)
			{
			}
			i++;
		}
		
				
		/**********************后期同步处理*********************/
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
		if(mode == 1 || mode == 2)
		{
			if(is_syn)
			{
				if(s1.xb > 0)
				{
					s1.xb--;
				}
				else
				{
					shu1=zeroCopy;
					flag1=true;
					is_syn=false;
				}
				if(s2.xb > 0)
				{
					s2.xb--;
				}
				else
				{
					shu2=zeroCopy;
					flag2=true;
					is_syn=false;
				}
			}
			i--;/**移动“结果”的下标**/
		}
	}
	/********************整理最终数据**********************/
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
	if(i<0)
		i=0;
	while(result[i]==0 && startPoint != i && i < minSize -1 )
		i++;/***除去整数部分的高位零***/
	tmpSnum.length = minSize;
	tmpSnum.xb = i  ;
	tmpSnum.shuZi = NULL;
	tmpSnum.xsd = startPoint;
	/*******************转化答案存储空间***********************/
	if(headspace || !is_positive)
		covertInt2Char(result,&tmpSnum,true);
	else
		covertInt2Char(result,&tmpSnum,false);
	if(!is_positive)
		tmpSnum.shuZi[0]='-';
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
				result[*i-1] += result[*i] / jinZhi;
				result[*i] = result[*i] % jinZhi;
			}
			break;
		case 2:
			result[*i]=result[*i]+shu1[s1->xb]-shu2[s2->xb];
			if(result[*i] < 0)
			{
				result[*i-1]--;
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
	shuZi = num->shuZi;num->fractionLength = 0;
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
		charResult=(char *)malloc(sizeof(char)*(aim->length - aim->xb + 3) );
	else
		charResult=(char *)malloc(sizeof(char)*(aim->length - aim->xb + 2) );
	if(charResult == NULL)
		exit(444);
	for( i = aim->xb, i2 = 0 ; i < aim->length ; i++ )
	{
		if(i2==0 && headspace)
			charResult[i2++]=' ';//预留符号位
		if(i == aim->xsd + 1)
			if(aim->xsd != -1)
				charResult[i2++]='.';
		charResult[i2++]=(char)(result[i]+'0');
	}
	free(result);
	charResult[i2]='\0';
	aim->shuZi = charResult;
	return;
}
void reallocStr(char **aim,int size,int startP,int endP)
{
	int i;
	char *c=(char *)malloc( sizeof(char) * size );
	if(c == NULL)
		exit(444);
	if( endP - startP < 0)
		exit(250);
	for(i=0 ; i < size ; i++)
		c[i] = (*aim)[startP + i];
	c[i]='\0';
	free(*aim);
	*aim = c;
	return;
}
int dropPort(char **a,char **b)
{
	int moveStep=0;
	char *c=NULL;
	snum s1,s2;
	s1.shuZi = *a;
	s2.shuZi = *b;
	analyzeNum(&s2,-1);
	analyzeNum(&s1,-1);
	c = (char *)malloc(sizeof(char)*s1.length );
	if( c == NULL )
		exit(444);
	strcpy(c,s1.shuZi);
	(*a)=c;
	c = (char *)malloc(sizeof(char)*s2.length );
	if( c == NULL )
		exit(444);
	strcpy(c,s2.shuZi);
	(*b)=c;
	if( s2.xsd != -1 )
	{
		if(s2.xsIsZero)
		{
			c = (char *)realloc( (*b) , sizeof(char)*(s2.length - s2.fractionLength +1 ) );
			if( c == NULL )
				exit(444);
			(*b) = c;
			strcpy2(*b,s2.shuZi,s2.xsd);
			(*b)[s2.xsd]='\0';
			return 0;
		}
		if( s2.fractionLength > s1.fractionLength )
		{

			c = (char *)realloc((*a), sizeof(char)*(s1.length + s2.fractionLength - s1.fractionLength ) );
			if( c == NULL )
				exit(444);
			(*a) = c;
			s1.xb = s1.length;
			if(s1.xsd == -1)
			{
				s1.xsd = s1.length ;
				(*a)[s1.xsd]='.';
				s1.xb++;
			}
			for(  ; s1.xb < s1.length + s2.fractionLength - s1.fractionLength ; s1.xb++ )
				c[s1.xb]='0';
			c[s1.xb]='\0';
			(*a) = c;
		}
		s1.xb = s1.xsd;
		s2.xb = s2.xsd;
		while( s2.xb < s2.length )
		{
			charSwap(&((*a)[s1.xb]),&((*a)[s1.xb+1]));
			charSwap(&((*b)[s2.xb]),&((*b)[s2.xb+1]));
			s1.xb++;
			s2.xb++;
			moveStep++; 
		}
		(*a)[s1.xb + 1]='\0';
	}
	return moveStep;
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
	if(i == 1)
		return;
	if((*shuZi)[i]=='.')
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
char *getZeroStr(int fractionLength)
{
	char *tmpCharPoint = NULL;
	int i = 2;
	if(fractionLength > 0)
		i = 3 + fractionLength;
	tmpCharPoint = (char *)malloc(sizeof(char)*i);
	if(tmpCharPoint == NULL)
		exit(444);
	tmpCharPoint[0]='0';
	i=1;
	if(fractionLength > 0)
	{
		tmpCharPoint[1]='.';
		for(i=2; i-2 < fractionLength ; i++)
			tmpCharPoint[i]='0';
	}
	tmpCharPoint[i]='\0';
	return tmpCharPoint;
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
	int i;
	for(i=0; i <= s1end ; i++)
		if(num1[i] < num2[i])
			return true;
	return false;
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
	char *item[19][3]={ \
		{"12","12"} , {"12","11"} , \
		{"12","1.2"} , {"1.2","1.2"} , \
		{"0","0"} , {"0","-1.1"} , \
		{"-1.1","0"} , {"-1.1","11"} , \
		{"-11","-1.1"} , {"0.001","10000"} , \
		{"999999","0.000001"} , {"0.02","0.003"} , \
		{"00.03","000.005"} , {"-00.03","-000.005"} , \
		{"0.0000","6"} , {"7","0.0000"}  \
	};
	printf("+----+--------+--------+\n");
	printf("|%-4s|%-8s|%-8s|\n","No.","Num1","Num2");
	printf("+----+--------+--------+\n");
	for(i=0;i<16;i++)
		printf("|%-4d|%-8s|%-8s|\n",i+1,item[i][0],item[i][1]);
	printf("+----+--------+--------+\n");
	/*计划设计成可以自动循环测试全部数据的testSystem*/
	printf("Please enter a choice:");
	scanf("%d",&i);
	i--;
	if(i>15 || i<0)
		exit(250);
	strcpy(a,item[i][0]);
	strcpy(b,item[i][1]);
}
/*
TEST 1:
000000000000000000000000000000.123456789876543212345678987654321
0000000000000000000.1111111111111111111111111111111111111111
TEST 2:
0
0
TEST 3:
1
0
TEST 4:
12345
12
TEST 5:
12
12345.67
TEST 6:
0.33
12345.67
TEST 7:
0.00001
9999.99999
TEST 8:
0.001
999.99999
TEST 9:
0.00001
99.9999
TEST 10:
0.10001000
0.00100
****/
/**/
