typedef enum eState
{
	tyd,
	direksie,
	hardloop
};

typedef enum eTyd
{
	t10,
	t20,
	t30,
	t40
};

typedef enum eDir
{
	dr,
	dl
};

eState STATE;
eTyd TYD;
eDir DIR;

int buttonstate = 0;
int buttonup = 0;
int changestate = 0;
int lights = 0;
int notify = 0;
int motorTeller = 0;
int motordelay = 0;
unsigned long starttime = 0;
unsigned long currtime = 0;
unsigned long last_motor = 0;
unsigned long last_flikker = 0;
int flikkerState = 1;

uint8_t pin_btn = 4;//12
uint8_t pin_mtr_clk = 3;//7
uint8_t pin_lgt_clk = 2;//6
uint8_t pin_enable = 1;//5
uint8_t pin_data = 0;//4


void setup() 
{
	pinMode(pin_btn,INPUT);//Button
	pinMode(pin_lgt_clk,OUTPUT);//light clock
	pinMode(pin_enable,OUTPUT);//enable
	pinMode(pin_data,OUTPUT);//data

        pinMode(pin_mtr_clk,OUTPUT);//motor clock

	STATE = tyd;
	TYD = t10;
	DIR = dr;
	printSettings();

        updatemotorshift(0);
       
}

void printSettings()
{
	lights = 0;

	

	if (STATE == hardloop)
        {
          unsigned long timenow = millis();
          if(timenow > last_flikker + 1000)
          {
            last_flikker = millis();
            if(flikkerState == 0)
            {
              flikkerState = 1;
            }else
            {
              flikkerState = 0;
            }
          }
          
          if(flikkerState == 0)
          {
//            if (TYD == t10)
//		  bitSet(lights,3);
//    	      else if (TYD == t20)
//    		  bitSet(lights,4);
//    	      else if (TYD == t30)
//    		  bitSet(lights,5);
//    	      else if (TYD == t40)
//    		  bitSet(lights,6);
//    
//            if (DIR == dr)
//    		  bitSet(lights,1);
//            else if (DIR == dl)
//    	  	  bitSet(lights,2);
    
            bitSet(lights,0);
	      bitSet(lights,7);
      
          }else
          {
            lights = 0;
            
          }
		


        }else
        {
          if (TYD == t10)
		bitSet(lights,3);
      	  else if (TYD == t20)
      		bitSet(lights,4);
      	  else if (TYD == t30)
      		bitSet(lights,5);
      	  else if (TYD == t40)
      		bitSet(lights,6);
      
          if (DIR == dr)
      		bitSet(lights,1);
          else if (DIR == dl)
      	  	bitSet(lights,2);
        }

	updatelightshift();
}

void updatelightshift()
{
	digitalWrite(pin_enable,LOW);
	shiftOut2(pin_data,pin_lgt_clk,LSBFIRST,lights);
	digitalWrite(pin_enable,HIGH);
}

void updatemotorshift(uint8_t counter)
{
	digitalWrite(pin_enable,LOW);
	shiftOut2(pin_data,pin_mtr_clk,LSBFIRST,counter);
	digitalWrite(pin_enable,HIGH);
        //delay(10);
        //digitalWrite(5,LOW);
        //shiftOut2(4,7,LSBFIRST,0);
        //digitalWrite(5,HIGH);
}

void shiftOut2(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val)
{
	uint8_t i;
	
	for (i = 0; i < 8; i++)  
	{
		if (bitOrder == LSBFIRST)
		digitalWrite(dataPin, !!(val & (1 << i)));
		else    
		digitalWrite(dataPin, !!(val & (1 << (7 - i))));

		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);            
	}
}

void walk()
{
        uint8_t stepper = 0;
        
        if(motorTeller == 0)
          bitSet(stepper,0);
        else if(motorTeller == 1)
          bitSet(stepper,1);
        else if(motorTeller == 2)
          bitSet(stepper,2);     
        else if(motorTeller == 3)
          bitSet(stepper,3); 
          
        updatemotorshift(stepper);
        
        if(DIR == dr) 
        {
          if(motorTeller == 3)
            motorTeller = 0;
          else 
            motorTeller ++;        
        }
        else
       {
           if(motorTeller == 0)
             motorTeller = 3;
           else
             motorTeller --;
       }
      
           
}

void loop() 
{
	buttonstate = digitalRead(pin_btn);
	currtime = millis();

	if ((starttime != 0) && (currtime - starttime > 2000))//Knoppie was 2 sec in
	{ 
		if (notify == 0)
		{
			changestate = 1;
			notify = 1;
			lights = 0;
			updatelightshift();
			delay(200);

			if (STATE == tyd)
				STATE = direksie;    
			else if (STATE == direksie)
                        {        		
		                STATE = hardloop;
                                if (TYD == t10)
					motordelay = 63;
				else if (TYD == t20)
					motordelay = 125;
				else if (TYD == t30)
					motordelay = 188; 
				else if (TYD == t40)
					motordelay = 250;
                        }
			else if (STATE == hardloop)
                        {
                          flikkerState = 1;
                          STATE = tyd;
                        }
				
		}
	}

	if ((buttonup == 1 )&& (buttonstate == LOW ))//Knoppie Los
	{
		if (changestate == 1)
		{
			changestate = 0;
			notify = 0;      
		}
		else
		{
			if (STATE == tyd)
			{
				if (TYD == t10)
					TYD = t20;
				else if (TYD == t20)
					TYD = t30;
				else if (TYD == t30)
					TYD = t40; 
				else if (TYD == t40)
					TYD = t10;
			}
			else if (STATE == direksie)
			{
				if (DIR == dr)
					DIR = dl;
				else if (DIR == dl)
					DIR = dr;
			}      
		}

		buttonup = 0;
		starttime = 0;
	}

	if ((buttonup == 0 )&&( buttonstate == HIGH))//Knoppie Gedruk
	{
		buttonup = 1;
		starttime = millis();
	}
	printSettings();

        if(STATE == hardloop)
        {
          unsigned long timenow = millis();
          if(timenow > last_motor + motordelay)
          {
            last_motor = millis();
            walk();
          }
        }
}
