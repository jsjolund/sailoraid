import processing.serial.*;

Serial myPort = new Serial(this, "/dev/ttyACM0", 115200);

float mxMax = MIN_FLOAT, mxMin = MAX_FLOAT, myMax = MIN_FLOAT, myMin = MAX_FLOAT, mzMax = MIN_FLOAT, mzMin = MAX_FLOAT;

void setup() {
}

void draw()
{
    serialEvent();
    
    float mxBias = (mxMax+mxMin)/2f;
    float myBias = (myMax+myMin)/2f;
    float mzBias = (mzMax+mzMin)/2f;
    
    float mxS = (mxMax-mxMin)/2f;
    float myS = (myMax-myMin)/2f;
    float mzS = (mzMax-mzMin)/2f;
    float avgScl = (mxS+myS+mzS)/3f;
    
    float mxScl = avgScl/mxS;
    float myScl = avgScl/myS;
    float mzScl = avgScl/mzS;
    
    println("bias: x="+mxBias+" y="+myBias+" z="+mzBias+" scl: x="+mxScl+" y="+myScl+" z="+mzScl); 
}
void serialEvent() {
    int newLine = 13;
    String message;
    do {
        message = myPort.readStringUntil(newLine);

        if (message != null) {
            String[] list = split(trim(message), " ");
            if (list.length >= 6) {
                float yaw = float(list[0]);
                float pitch = float(list[1]);
                float roll = float(list[2]);

                float mx = float(list[3]);
                float my = float(list[4]);
                float mz = float(list[5]);
                
                if (mx > mxMax) {
                  mxMax = mx;
                }
                if (mx < mxMin) {
                  mxMin = mx;
                }
                if (my > myMax) {
                  myMax = my;
                }
                if (my < myMin) {
                  myMin = my;
                }
                if (mz > mzMax) {
                  mzMax = mz;
                }
                if (mz < mzMin) {
                  mzMin = mz;
                }
                
            }
        }
    } while (message != null);
}