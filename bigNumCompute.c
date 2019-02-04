#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
struct num{
	char *shuZi;
	int xsd;    /*xsd��С���㣬��¼С������±� (radix point)*/
	int length;   /*length�����ֵ����鳤��*/
	int intLength; /*intLength���������ֵĳ���*/
	int fractionLength; /*fractionLength��С�����ֵĳ���*/
	//int validIntStart; /*��������ʼ�±�*/
	int xb; /*xb���±� (position)*/
	bool is_positive;   /*�ж��Ƿ�Ϊ����*/
	bool intIsZero;     /*�ж����������Ƿ�Ϊ��*/
	bool xsIsZero;     /*�ж�С�������Ƿ�Ϊ��*/
	bool moveOneStep;   /*���ƶ�һ����ֵΪ��ʱ���±�Ӧ��һ*/
};
typedef struct num    snum;

/**������ƣ���ΧӦ��char�Ĵ�С����**/
int jinZhi = 10;  /*�˴�����Ϊʮ����*/

/*mode=1:plus,mode=2:minus,mode=3:multiply,mode=4:divide;
*ģʽ1-4�ֱ�Ϊ�Ӽ��˳�
*/
char *bigNumCompute(char shu1[],char shu2[],bool headspace,int mode,int precision,char **remainder);
/*�ӷ��������ͳ˷��������϶��Ǽӷ���
*�˺���Ϊ��������������ƣ�ͬʱ��bigNumComptue����
*/
void plusUnit(snum *s1,snum *s2,char *result,int *i,int mode);

/*limitSize�����ⲿ���������Կ��ƺ���������Χ�����������������Ĭ�϶����������飬Ĭ�����һλ�ǽ�������'\0'��*/
void analyzeNum(snum *num,int limitSize);
/*����˼�壬covertInt2Char������ֵת�����ַ���ֵ*/
void covertInt2Char(char *result,snum *aim,bool headspace);
/*���·��������ַ�����Ŀռ䣬������Զ��忽��*/
void reallocStr(char **aim,int size,int startP,int endP);
/*dropPort����˼�壬������С���㣬
*�˺������רΪ�������񣬻������b����ĸ������������������
*����a������b���Ե�С���㽫�ƶ�ͬ�����λ����
*���շ����ƶ�λ��
*/
int dropPort(char **a,char **b);
int serialZeroCount(char *shuZi,int s1end);
/*��ȥ�ַ��������е���Ч�ַ�*/
void jumpUselessChar(char **ShuZi);

/*charSwap����˼�壬���ַ��ͱ�������*/
void charSwap(char *a,char *b);
/*charPtrSwap��charPointerSwap
*����˼�壬���ַ���ָ���������
*/
void charPtrSwap(char **a,char **b);
void strcpy2(char *a,char *b,int s1end);
/*getMaxInt����˼�壬����ȡa,b����������ֵ*/
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
		shu1++;/*�ص�����*/
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
	{/**һ��������һ�������������*/
		switch (mode)
		{
			case 1:
				/**�����Ӹ������� �� �������������� ���Ի�����������**/
				if(!flag1)
					tmpCharPoint=bigNumCompute(shu1,shu2,false,2,0,NULL);
				else
					tmpCharPoint=bigNumCompute(shu2,shu1,false,2,0,NULL);
				return tmpCharPoint;/***�����ּ����Ѿ�����÷������⣬ֱ�ӷ��ش�***/
				break;
			case 2:
				/**�����Ӹ������� �� �������������� ���Ի�����������**/
				if(flag1 && !flag2)
				{
					tmpCharPoint=bigNumCompute(shu1,shu2,true,1,0,NULL);
					tmpCharPoint[0]='-';/***���Ǹ��������㣬�����ķ��Ż��Ǹ���***/
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
	{/**���������������ת��������;�˷��ͳ�������������������ı��ʼ����**/
		if(mode==2)
			return bigNumCompute(shu2,shu1,false,2,0,NULL);
	}
	int minSize=0,startPoint;
	/*startPoint��¼����������С������±꣬Ĭ���Ǵ��±�ĺ�һλ*/
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
	/**************�����ռ䣬�������****************/
	if(mode == 2)
	{
		if(judgeSmallerNum(shu1,shu2,i-1,i2-1))/*Ĭ��shu1�������*/
		{/*���shu2���ָ����򽻻�����*/
			charPtrSwap(&shu1,&shu2);
			is_positive = false;/*���ȡ��*/
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
	/**����Ƿ�������Ϊ��**/
	if(mode == 3 || mode == 4)
	{
		if( (s1.intIsZero && s1.xsIsZero) || (s2.intIsZero && s2.xsIsZero) )
		{
			is_positive = true;/**�ܲ��ܳ��֡����㡱�������**/
			/**������Ϊ��**/
			if(mode == 4)
			{
				free(shu2);
				/**�������Ƿ�Ϊ��**/
				if(s2.intIsZero && s2.xsIsZero)
				{
					tmpCharPoint = (char *)malloc(sizeof(char)*8);
					if(tmpCharPoint == NULL)
						exit(444);
					strcpy(tmpCharPoint,"error!");/**�Ǿ�ֱ�ӱ�����������**/
					free(shu1);
					return tmpCharPoint;
				}
				return shu1;/**�Ѿ�����dropPort����ֱ�ӷ��ر�����**/
			}
		}
	}
	/*****************��������ʱ�洢�ռ��ʼ��************************/
	minSize = getMaxInt(s1.intLength, s2.intLength) + \
		getMaxInt(s1.fractionLength, s2.fractionLength) + \
		precision + 2 ;/*Ԥ��2λ�ռ�����λ����*/
	result=(char *)malloc(sizeof(char)*(minSize +1));
	if(result == NULL)
		exit(444);
	for(i=0;i<minSize;i++)
		result[i]=0;
	result[i]='\0';
	/**��ǰ�㶨�����ռ��С�����⣬ͬʱ���ñ�Ҫ�ĳ�ʼ������**/
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
	/********************������***********************/
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
		{/*****�˷���Ҫ�첽����*****/
			if(s2.xb < 0)
				break;
			i = minSize -1 - (s2.length -1 - s2.xb);
			if(shu2[s2.xb]=='.')
				s2.xb--;
		}
		else if(mode == 4)
		{	/**�ж������Ƿ�Ϊ�㣬���������**/
			/*
			if(readDNum(yu,0)==0)
			{
				buff = outputDNum(yu);
				if(serialZeroCount(buff,yu->length) == yu->length)
					setZeroDNum(yu);
			}
			*/
			/*******����С����******/
			if( shu1[s1.xb]=='.' )
			{
				startPoint=i;
				s1.xb++;
			}
			yu2 = yu->shuZi;
			/**����ȷ���������ȳ�����**/
			/**���ԭʼ������û�ж���**/
			if(!flag1)
			{	/**������һ��ѭ��**/
				if(i > 0)
				{	/**����������ͳ���λ����ͬ**/
					if( yu->length -1 == s2.intLength -1)
					{
						/**�����������λû�г�����**/
						if( yu2[0] < shu2[0] )
						{
							yu2[yu->length++]=shu1[s1.xb];/**�Ǿʹ�ԭʼ�����������һλ**/
							yu2[yu->length]='\0';
							/* yu->intLength++; ������������������length���intLength */
							s1.moveOneStep = true;
						}    /**�����������λ�ͳ���һ����**/
						else if (  yu2[0] == shu2[0]  )
						{
							/**�Ǿ�����ȶԣ����������С�ڳ���**/
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
				/**��������Ҫ���ʽ�λ�ռ�**/
				if( i >= minSize-3 || serialZeroCount(yu2,yu->length -1)== yu->length)
				{
					printf("\n����������%s\n",yu2);
					free(shu1);
					free(shu2);
					break;
				}
				yu2[yu->length++]='0';
				yu2[yu->length]='\0';/**ԭʼ�����������ˣ�ֻ�ܲ�����**/
			}
			
		}
		else
		{/*****ͬ������******/
			if(!is_syn)
			{
				if( !flag1 && !flag2)
				{
					if(s1.fractionLength > s2.fractionLength)
					{
						if( s1.fractionLength - (s1.length -1 - s1.xb) == s2.fractionLength )
						{/*���������ֵ��±�����Ե�С���������ȵ�ʱ�򣬽������㴦��ͬ����������*/
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
					{/*���������С��λ����ͬ��ֱ��ͬ��*/
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
			/*******����С����******/
			if( shu1[s1.xb]=='.' || shu2[s2.xb]=='.' )
			{
				startPoint=i;
				if(s1.xsd != -1)
					s1.xb--;
				if(s2.xsd != -1)
					s2.xb--;
			}
		}
		
		/********************�������***********************/
		if(mode != 4)
		{
			plusUnit(&s1,&s2,result,&i,mode);
		}
		else
		{
			/**������������ȱȳ������ȴ�**/
			if(yu->length > s2.length)
			{
			}
			i++;
		}
		
				
		/**********************����ͬ������*********************/
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
			i--;/**�ƶ�����������±�**/
		}
	}
	/********************������������**********************/
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
		i++;/***��ȥ�������ֵĸ�λ��***/
	tmpSnum.length = minSize;
	tmpSnum.xb = i  ;
	tmpSnum.shuZi = NULL;
	tmpSnum.xsd = startPoint;
	/*******************ת���𰸴洢�ռ�***********************/
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
			charResult[i2++]=' ';//Ԥ������λ
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
		(*shuZi)++;/***��ȥ�ո�***/
	i=serialZeroCount(*shuZi,-1);
	/***��ȥ�������ֵĸ�λ��***/
	if(i == 1)
		return;
	if((*shuZi)[i]=='.')
		(*shuZi)=(*shuZi) + i -1; /**���ٱ����������ֵĸ�λ��**/
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
{/**����false��ζ��num1>=num2**/
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
	{/**�����������λ����ͬ**/
		if(judgeSmallerInt(num1,num2,s1.intLength -1,s2.intLength -1))
			return true;/**�������С��С���Ͳ���Ҫ�Ƚ���**/
		if(strcmp2(num1,num2,s1.intLength-1))
		{
			if(!s2.xsIsZero)
			{
				if(s1.xsIsZero)
					return true;
				return judgeSmallerXS(num1 + s1.xsd +1 , num2 + s2.xsd +1 ,s1.fractionLength , s2.fractionLength );/**����С����һ������**/
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
	/*�ƻ���Ƴɿ����Զ�ѭ������ȫ�����ݵ�testSystem*/
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
