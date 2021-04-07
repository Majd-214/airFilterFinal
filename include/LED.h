//initialize led pins
int ledRed = 0;
int ledGreen = 0;
int ledBlue = 0;
bool type = false;

class LED
{   
    public:
    void Setup(int R, int G, int B, bool commonPin)
    {
        ledRed = R;
        ledGreen = G;
        ledBlue = B;
        type = commonPin;
    }

    void Color(char color)
    {   
        pinMode(ledRed, OUTPUT);
        pinMode(ledGreen, OUTPUT);
        pinMode(ledBlue, OUTPUT);

        if(type == false)
        {
            switch (color)
            {
            case 'R':
                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledBlue, LOW);
                break;
            
            case 'G':
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, HIGH);
                digitalWrite(ledBlue, LOW);
                break;
            
            case 'B':
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledBlue, HIGH);
                break;
            
            case 'Y':
                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, HIGH);
                digitalWrite(ledBlue, LOW);
                break;
            
            case 'M':
                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledBlue, HIGH);
                break;
            
            case 'C':
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, HIGH);
                digitalWrite(ledBlue, HIGH);
                break;

            case 'W':
                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, HIGH);
                digitalWrite(ledBlue, HIGH);
                break;
            }
        }
        else if(type == true)
        {
            switch (color)
            {
            case 'R':
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, HIGH);
                digitalWrite(ledBlue, HIGH);
                break;
            
            case 'G':
                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledBlue, HIGH);
                break;
            
            case 'B':
                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, HIGH);
                digitalWrite(ledBlue, LOW);
                break;
            
            case 'Y':
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledBlue, HIGH);
                break;
            
            case 'M':
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, HIGH);
                digitalWrite(ledBlue, LOW);
                break;
            
            case 'C':
                digitalWrite(ledRed, HIGH);
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledBlue, LOW);
                break;

            case 'W':
                digitalWrite(ledRed, LOW);
                digitalWrite(ledGreen, LOW);
                digitalWrite(ledBlue, LOW);
                break;
            }
        }
        
    }

    void Flash(char color, int time)
    {
        Color(color);
        delay(time);
        Clear();
        delay(time);
    }

    void Clear()
    {   
        digitalWrite(ledRed, LOW);
        digitalWrite(ledGreen, LOW);
        digitalWrite(ledBlue, LOW);
        if(type == true)
        {
            digitalWrite(ledRed, HIGH);
            digitalWrite(ledGreen, HIGH);
            digitalWrite(ledBlue, HIGH);
        }
    }
};
