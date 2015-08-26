#include <dht11.h>
#include <delay.h>

uint8_t U8FLAG, U8temp;
uint8_t U8T_data_H, U8T_data_L, U8RH_data_H, U8RH_data_L, U8checkdata;
uint8_t U8T_data_H_temp, U8T_data_L_temp, U8RH_data_H_temp, U8RH_data_L_temp, U8checkdata_temp;
uint8_t U8comdata;
float   temp,humi;
static float sfDht11Queue[DHT11_QUEUE_LENGTH][2];
static int snDht11QueueIndex = 0;


static __INLINE void InsertDht11Queue(float temperature, float humidity)
{
	sfDht11Queue[snDht11QueueIndex][0] = temperature;
	sfDht11Queue[snDht11QueueIndex][1] = humidity;
	if (DHT11_QUEUE_LENGTH == ++snDht11QueueIndex) {
		snDht11QueueIndex = 0;
	}
}

static __INLINE void FullDHT11Queue()
{
	for (int i = 0; i < DHT11_QUEUE_LENGTH; ++i) {
		GetDht11(&temp,&humi);
	}
}

static __INLINE void Dht11DelayOneUs(unsigned int us)
{   
	DelayUs(us); 
}

static __INLINE void Dht11DelayOneMs(unsigned int ms)
{
	while(ms--)
	{
		DelayUs(1000);
	}
}

static __INLINE void Dht11ReadValue(void)
{     
	uint8_t i;  
	for(i=0;i<8;i++)    
	{
		U8FLAG=2; 
		GpioConfig(DHT11_PIN,OUTPUT);
		GpioWrite(DHT11_PIN, OFF);
		GpioWrite(DHT11_PIN, ON);
		GpioConfig(DHT11_PIN,INPUT);
		while((!GpioRead(DHT11_PIN))&&U8FLAG++);
		Dht11DelayOneUs(30);
		U8temp=0;
		if(GpioRead(DHT11_PIN)) U8temp=1;
		U8FLAG=2;
		while(GpioRead(DHT11_PIN)&&U8FLAG++);   
		if(U8FLAG==1) break;    
		U8comdata<<=1;
		U8comdata|=U8temp; 
	}    
}

void GetDht11(float *temperature, float *humidity)
{
	GpioConfig(DHT11_PIN,OUTPUT);
	GpioWrite(DHT11_PIN, ON);
	Dht11DelayOneMs(100);
	GpioWrite(DHT11_PIN, OFF);
	Dht11DelayOneMs(20);
	GpioWrite(DHT11_PIN, ON);
	GpioConfig(DHT11_PIN,INPUT);
	Dht11DelayOneUs(40);
	if(!GpioRead(DHT11_PIN)) {
		U8FLAG=2;
		while((!GpioRead(DHT11_PIN))&&U8FLAG++);
		U8FLAG=2;
		while((GpioRead(DHT11_PIN))&&U8FLAG++); 
		Dht11ReadValue();
		U8RH_data_H_temp=U8comdata;
		Dht11ReadValue();
		U8RH_data_L_temp=U8comdata;
		Dht11ReadValue();
		U8T_data_H_temp=U8comdata;
		Dht11ReadValue();
		U8T_data_L_temp=U8comdata;
		Dht11ReadValue();
		U8checkdata_temp=U8comdata;
		GpioConfig(DHT11_PIN,OUTPUT);
		GpioWrite(DHT11_PIN,1);
		U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
		if(U8temp==U8checkdata_temp) {
			U8RH_data_H=U8RH_data_H_temp;
			U8RH_data_L=U8RH_data_L_temp;
			U8T_data_H=U8T_data_H_temp;
			U8T_data_L=U8T_data_L_temp;
			U8checkdata=U8checkdata_temp;
		}
		*temperature=(float)U8T_data_H;
		*humidity=(float)U8RH_data_H;
	} else {  
      *temperature=temp;            //Read error ,use the data of last time
      *humidity=humi;
	} 
	InsertDht11Queue(*temperature,*humidity);
}

void InitDht11(void)                                            //Initializing the DHT11
{
		DelayMs(DHT11_INIT_TIME);
		FullDHT11Queue();
}


void GetAverageDht11(float *temperature, float *humidity)				//Get the average of DHT11
{
		GetDht11(&temp,&humi);
		float fRet[2] = {0};
		for (int i = 0; i < DHT11_QUEUE_LENGTH; ++i) 
		{
			fRet[0] += sfDht11Queue[i][0];
			fRet[1] += sfDht11Queue[i][1];
		}	
		*temperature = fRet[0] / DHT11_QUEUE_LENGTH;
		*humidity = fRet[1] / DHT11_QUEUE_LENGTH;
}




