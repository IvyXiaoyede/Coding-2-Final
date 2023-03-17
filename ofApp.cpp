#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
    
    //Initialize the drawing variables
    for (int i = 0; i < ofGetWidth(); ++i) {
        waveform[i] = 0;
    }
    waveIndex = 0;
    
    // Maximilian audio stuff
    int sampleRate = 44100; /* Sampling Rate */
    int bufferSize= 512; /* Buffer Size. you have to fill this buffer with sound using the for loop in the audioOut method */
    ofxMaxiSettings::setup(sampleRate, 2, bufferSize);
    
    myClock.setTempo(120);
    myClock.setTicksPerBeat(4);
    
    mySample.load(ofToDataPath("4.WAV"));
    
    // Setup ofSound
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.sampleRate = sampleRate;
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    //3D
//    ofSetVerticalSync(true);
//    mesh.load("lofi-bunny.ply");
    
    //mesh 图像
    image.load("15.jpeg");
    image.resize(200, 200);
    
    mesh.setMode(OF_PRIMITIVE_POINTS);
    mesh.enableColors();
    ofSetFrameRate(60);
    float intensityThreshold = 150.0;
    int w = image.getWidth();
    int h = image.getHeight();
    for (int x=0; x<w; ++x) {
        for (int y=0; y<h; ++y) {
            ofColor c = image.getColor(x, y);
            float intensity = c.getLightness();
            if (intensity >= intensityThreshold) {
                //像素处颜色的饱和度来改变其对应顶点的 z 坐标
                float saturation = c.getSaturation();
                float z = ofMap(saturation, 0, 255, -100, 100);
                ofVec3f pos(x*4, y*4, z);
                mesh.addVertex(pos);
                // When addColor(...), the mesh will automatically convert
                // the ofColor to an ofFloatColor
                mesh.addColor(c-100);


//                   offsets.push_back(ofVec3f(ofRandom(0,100000), ofRandom(0,100000), ofRandom(0,100000)));
                offsets.push_back(ofVec3f(ofRandom(0,100000),ofRandom(0,100000) , ofRandom(0,100000)));
            }
        }

     }
    mesh.setMode(OF_PRIMITIVE_LINES);

    // We are going to be using indices this time
    mesh.enableIndices();

    // ...
    // Omitting the code for creating vertices for clarity
    // but don't erase it from your file!

    // Let's add some lines!
    float connectionDistance = 60;
    int numVerts = mesh.getNumVertices();
    for (int a=0; a<numVerts; ++a) {
        ofVec3f verta = mesh.getVertex(a);
        for (int b=a+1; b<numVerts; ++b) {
            ofVec3f vertb = mesh.getVertex(b);
            float distance = verta.distance(vertb);
            if (distance <= connectionDistance) {
                // In OF_PRIMITIVE_LINES, every pair of vertices or indices will be
                // connected to form a line
                mesh.addIndex(a);
                mesh.addIndex(b);
            }
        }
    }
    
    //Arduino
    ofSetVerticalSync(true);

    ofSetLogLevel(OF_LOG_VERBOSE);
        
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
        
    int baud =9600;
        
    serial.setup("/dev/cu.usbmodem14201", baud);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    float time =ofGetElapsedTimef();
        for (int i=0; i<900; i+=5)
        {
            ofSetColor(4+127*sin(i *0.01 +time), 127+127*cos(i*0.11 +time), 150+127*sin(i*0.012 +time));
        }
    
    int numVerts = mesh.getNumVertices();
    for (int i=0; i<numVerts; ++i) {
        ofVec3f vert = mesh.getVertex(i);

        //float time = ofGetElapsedTimef();
        float timeScale = 5.0;
        float displacementScale = 0.75+mouseY/100;
        ofVec3f timeOffsets = offsets[i];


        vert.x += (ofSignedNoise(time*timeScale+timeOffsets.x)) * displacementScale;
        vert.y += (ofSignedNoise(time*timeScale+timeOffsets.y)) * displacementScale;
        vert.z += (ofSignedNoise(time*timeScale+timeOffsets.z)) * displacementScale;
        mesh.setVertex(i, vert);
    }
    cout<<time<<endl;
    
    //Arduino
    if(serial.available()<0)
        {
            SensorValue ="Arduino Error";
        }
        else
        {
            while(serial.available()>0)
            {
                byteData = serial.readByte();
          //      printf("w:",byteData);
                SensorValue = "value:" + ofToString(byteData);
            }
        }
        cout << SensorValue <<endl;
}

//--------------------------------------------------------------
void ofApp::draw(){
    timeValue =byteData;
    /////////////// waveform
    ofTranslate(0, ofGetHeight()/2);
    ofFill();
    //Background
    ofColor centerColor = ofColor(85, 78, 68);
       ofColor edgeColor(0, 0, 0);
       ofBackgroundGradient(centerColor, edgeColor, OF_GRADIENT_CIRCULAR);
    
    //  ofDrawLine(0, 0, 10, waveform[1] * ofGetHeight()/2.); //first line
    for(int i = 1; i < (ofGetWidth() - 1); ++i)
    {
        ofDrawCircle(ofRandom(0,800), ofRandom(-800,800), timeValue+waveform[i] * ofGetHeight()/5.-mouseX/2000);
    }

    
    //3D
    
    
    ofSetColor(255);
    
    cam.begin();
    ofPushMatrix();
        ofTranslate(-ofGetWidth()/2, -ofGetHeight()/2);
    mesh.draw();
    ofPopMatrix();
    
    
    cam.end();


}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer& input){
    std::size_t nChannels = input.getNumChannels();
    for (size_t i = 0; i < input.getNumFrames(); i++)
    {
        // handle input here
    }
}
//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer& output){
    std::size_t outChannels = output.getNumChannels();
    for (int i = 0; i < output.getNumFrames(); ++i){
        
        myClock.ticker();
        float myOut = mySample.play(15);
        
        output[i * outChannels] = myOsc1.sinewave(10)/100+myOut;
        //output[i * outChannels] =myOut;
     //   output[i * outChannels + 1] = output[i * outChannels];
        
        //Hold the values so the draw method can draw them
        waveform[waveIndex] =  output[i * outChannels];
        if (waveIndex < (ofGetWidth() - 1)) {
            ++waveIndex;
        } else {
            waveIndex = 0;
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
