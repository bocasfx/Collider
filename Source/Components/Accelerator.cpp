
#include "../../JuceLibraryCode/JuceHeader.h"
#include "Accelerator.h"

Accelerator::Accelerator(QAudioProcessor& p)
    : processor (p),
      spring(1.0)
{
    background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    setSize(WIDTH, HEIGHT);
    startTimer(20);
}

Accelerator::~Accelerator()
{
    for(auto it = particles.begin(); it != particles.end(); ++it) {
        delete *it;
    }
}

void Accelerator::paint (Graphics& g)
{
    g.fillAll(Colour(30, 30, 30));
    g.drawImageAt(background, 0, 0);
    detectCollissions();
}

void Accelerator::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void Accelerator::mouseDown(const MouseEvent &event)
{
    createParticle(event.x, event.y);
}

void Accelerator::createParticle(int x, int y)
{
    int note = rand() % 127;
    uint8 velocity = (uint8)(rand() % 127);
    Particle* p = new Particle(processor, note, velocity, x, y);
    particles.add(p);
    addAndMakeVisible(p);
}

void Accelerator::detectCollissions()
{
    for(int i = 0; i < particles.size(); ++i) {
        
        Particle* particleI = particles[i];
        Point<float>* posI = particleI->getPosition();
        Point<float>* deltaI = particleI->getDelta();
        
        for (int j = i + 1; j < particles.size(); ++j) {
            
            Particle* particleJ = particles[j];
            
            Point<float>* posJ = particleJ->getPosition();
            Point<float>* deltaJ = particleJ->getDelta();
            
            float dx = posJ->x - posI->x;
            float dy = posJ->y - posI->y;
            
            float distance = sqrt(pow(dx, 2) + pow(dy, 2));
            
            if (distance <= PARTICLE_SIZE)
            {
                float angle = atan2(dy, dx);
                
                float targetX = posI->x + cos(angle) * PARTICLE_SIZE;
                float targetY = posI->y + sin(angle) * PARTICLE_SIZE;
                
                float ax = (targetX - posJ->x) * spring;
                float ay = (targetY - posJ->y) * spring;
                
                float deltaIx = deltaI->x - ax;
                float deltaIy = deltaI->y - ay;
                
                float deltaJx = deltaJ->x + ax;
                float deltaJy = deltaJ->y + ay;
                
                particleI->setDelta(deltaIx, deltaIy);
                particleJ->setDelta(deltaJx, deltaJy);
                
                particles[i]->collided();
                particles[j]->collided();
            }
        }
    }
}

void Accelerator::setGravity(float g)
{
    for(auto it = particles.begin(); it != particles.end(); ++it)
    {
        Particle* p = *it;
        p->setGravity(g);
    }
}

void Accelerator::setFriction(float f)
{
    for(auto it = particles.begin(); it != particles.end(); ++it)
    {
        Particle* p = *it;
        p->setFriction(f);
    }
}

void Accelerator::setSpring(float s)
{
    spring = s;
}

void Accelerator::setDiameter(int d)
{
    for(auto it = particles.begin(); it != particles.end(); ++it)
    {
        Particle* p = *it;
        p->setDiameter(d);
    }
}

void Accelerator::timerCallback()
{
    for(auto it = particles.begin(); it != particles.end(); ++it)
    {
        Particle* p = *it;
        p->updatePosition();
    }
}
