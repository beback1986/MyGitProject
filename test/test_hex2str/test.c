#include <stdio.h>

unsigned int hexchar2int(const char hexchar){
	if(hexchar > 47 && hexchar < 58)
		return (hexchar - 48);
	else if(hexchar >64 && hexchar < 71)
		return (hexchar - 55);
	else if(hexchar >96 && hexchar < 103)
		return (hexchar - 87);
	return -1;
}
long hexstr2int(const char* hexstr, const int len){
	long intstr, i, base;
	int intchar;
	intstr = 0;
	base = 1;
	for(i=1;i<=len;i++){
		intchar = hexchar2int(hexstr[len-i]);
		if(intchar > -1){
			intstr += intchar * base;
			base *= 16;
		}
		else
			return -1;
	}
	return intstr;
}
void main(void)
{
	char str[9] = "80000000";
	str[8] = '\0';
	printf("%x\n",hexstr2int(str,8));
}
