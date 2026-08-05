class BongoCatAnimation {
private:
    enum PawState { IDLE, LEFT_PAW, RIGHT_PAW, BOTH_PAWS };
    PawState currentState = IDLE;
    
public:
    void triggerLeftPaw() {
        currentState = LEFT_PAW;
        animateFrame(0, 100);
    }
    
    void triggerRightPaw() {
        currentState = RIGHT_PAW;
        animateFrame(100, 200);
    }
    
    void triggerBothPaws() {
        currentState = BOTH_PAWS;
        animateFrame(200, 300);
    }
    
private:
    void animateFrame(int startFrame, int endFrame) {
        // Sprite animation logic
    }
};
