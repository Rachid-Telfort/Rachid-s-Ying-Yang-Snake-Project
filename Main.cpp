#include<algorithm>
#include<chrono>
#include<cmath>
#include<cstdlib>
#include<limits>
#include<new>
#include<random>
#include<ratio>
#include<stdexcept>
#include<SFML/Graphics/CircleShape.hpp>
#include<SFML/Graphics/RectangleShape.hpp>
#include<SFML/Graphics/RenderWindow.hpp>
#include<SFML/System/Vector2.hpp>
#include<SFML/Window/Event.hpp>
#include<SFML/Window/Keyboard.hpp>
#include<SFML/Window/VideoMode.hpp>

struct Display
{
    bool paused;
    bool yingWon;
    bool yangWon;
    sf::Event event;
    sf::Image image;
    sf::RenderWindow renderWindow;

    Display()=default;
    Display(const Display&)=delete;
    Display(Display&&)=delete;
    ~Display()=default;
    Display& operator=(const Display&)& = delete;
    Display& operator=(Display&&)& = delete;
};

struct YingSnake
{
    bool moved;
    float yingAngle;
    std::vector<sf::CircleShape> yingBody;

    YingSnake()=default;
    YingSnake(const YingSnake&)=delete;
    YingSnake(YingSnake&&)=delete;
    ~YingSnake()=default;
    YingSnake& operator=(const YingSnake&)& = delete;
    YingSnake& operator=(YingSnake&&)& = delete;
};

struct YangSnake
{
    float yangAngle;
    std::chrono::time_point<std::chrono::steady_clock> idleStart;
    std::chrono::time_point<std::chrono::steady_clock> idleEnd;
    std::vector<sf::CircleShape> yangBody;

    YangSnake()=default;
    YangSnake(const YangSnake&)=delete;
    YangSnake(YangSnake&&)=delete;
    ~YangSnake()=default;
    YangSnake& operator=(const YangSnake&)& = delete;
    YangSnake& operator=(YangSnake&&)& = delete;
};

struct YingYangPickup
{
    bool separated;
    bool yingTaken;
    bool yangTaken;
    sf::RectangleShape yingPiece;
    sf::RectangleShape yangPiece;

    YingYangPickup()=default;
    YingYangPickup(const YingYangPickup&)=delete;
    YingYangPickup(YingYangPickup&&)=delete;
    ~YingYangPickup()=default;
    YingYangPickup& operator=(const YingYangPickup&&)& = delete;
    YingYangPickup& operator=(YingYangPickup&&)& = delete;
};

struct YYPLocationMutator
{
    std::random_device randomDevice;
    std::mt19937 mersenneTwister;
    std::bernoulli_distribution bernoulliDistribution;
    std::uniform_real_distribution<float> yingYangURDX;
    std::uniform_real_distribution<float> yingYangURDY;
    std::uniform_real_distribution<float> yingURDX;
    std::uniform_real_distribution<float> yangURDX;
    std::uniform_real_distribution<float> yingURDY;
    std::uniform_real_distribution<float> yangURDY;

    YYPLocationMutator()=default;
    YYPLocationMutator(const YYPLocationMutator&)=delete;
    YYPLocationMutator(YYPLocationMutator&&)=delete;
    ~YYPLocationMutator()=default;
    YYPLocationMutator& operator=(const YYPLocationMutator&&)& = delete;
    YYPLocationMutator& operator=(YYPLocationMutator&&)& = delete;
};

void setDisplay(Display& display)
{
    display.paused=false;
    display.yingWon=false;
    display.yangWon=false;
    display.renderWindow.create(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height, sf::VideoMode::getDesktopMode().bitsPerPixel), "Ying Yang Snake");
    display.renderWindow.setFramerateLimit(0u);
    display.renderWindow.setVerticalSyncEnabled(true);

    if(display.image.loadFromFile("Media/YYSIcon.bmp"))
    {
        display.renderWindow.setIcon(256u, 256u, display.image.getPixelsPtr());
    }
}

void setYingSnake(YingSnake& yingSnake)
{
    yingSnake.moved=false;
    yingSnake.yingAngle=0.0f;

    try
    {
        yingSnake.yingBody.resize(3u);
    }

    catch(const std::length_error& exception)
    {
        throw std::length_error("Error: Cannot form initial length of Ying Snake.");
    }

    for(std::vector<sf::CircleShape>::size_type index(0u); index<yingSnake.yingBody.size(); ++index)
    {
        yingSnake.yingBody[index].setFillColor(sf::Color::Yellow);
        yingSnake.yingBody[index].setRadius(sf::VideoMode::getDesktopMode().width/100.0f);
        yingSnake.yingBody[index].setOrigin(yingSnake.yingBody[index].getLocalBounds().left+yingSnake.yingBody[index].getLocalBounds().width/2.0f, yingSnake.yingBody[index].getLocalBounds().top+yingSnake.yingBody[index].getLocalBounds().height/2.0f);

        if(!index)
        {
            yingSnake.yingBody[index].setPointCount(6u);
            yingSnake.yingBody[index].setPosition(sf::VideoMode::getDesktopMode().width/2.0f, sf::VideoMode::getDesktopMode().height/2.0f);
        }

        else if(index==yingSnake.yingBody.size()-1u)
        {
            yingSnake.yingBody[index].setPointCount(4u);
            yingSnake.yingBody[index].setPosition(yingSnake.yingBody[index-1u].getPosition().x, yingSnake.yingBody[index-1u].getPosition().y+yingSnake.yingBody[index-1u].getRadius());
        }

        else
        {
            yingSnake.yingBody[index].setPointCount(4u);
            yingSnake.yingBody[index].setRotation(45.0f);
            yingSnake.yingBody[index].setPosition(yingSnake.yingBody[index-1u].getPosition().x, yingSnake.yingBody[index-1u].getPosition().y+yingSnake.yingBody[index-1u].getRadius());
        }
    }
}

void setYangSnake(YangSnake& yangSnake)
{
    yangSnake.yangAngle=0.0f;
    yangSnake.idleStart=std::chrono::steady_clock::now();
    yangSnake.idleEnd=std::chrono::steady_clock::now();

    try
    {
        yangSnake.yangBody.resize(3u);
    }

    catch(const std::length_error& exception)
    {
        throw std::length_error("Error: Cannot form initial length of Yang Snake.");
    }

    for(std::vector<sf::CircleShape>::size_type index(0u); index<yangSnake.yangBody.size(); ++index)
    {
        yangSnake.yangBody[index].setFillColor(sf::Color::Blue);
        yangSnake.yangBody[index].setRadius(sf::VideoMode::getDesktopMode().width/100.0f);
        yangSnake.yangBody[index].setOrigin(yangSnake.yangBody[index].getLocalBounds().left+yangSnake.yangBody[index].getLocalBounds().width/2.0f, yangSnake.yangBody[index].getLocalBounds().top+yangSnake.yangBody[index].getLocalBounds().height/2.0f);

        if(!index)
        {
            yangSnake.yangBody[index].setPointCount(6u);
            yangSnake.yangBody[index].setPosition(sf::VideoMode::getDesktopMode().width/2.0f, sf::VideoMode::getDesktopMode().height/2.0f);
        }

        else if(index==yangSnake.yangBody.size()-1u)
        {
            yangSnake.yangBody[index].setPointCount(4u);
            yangSnake.yangBody[index].setPosition(yangSnake.yangBody[index-1u].getPosition().x, yangSnake.yangBody[index-1u].getPosition().y+yangSnake.yangBody[index-1u].getRadius());
        }

        else
        {
            yangSnake.yangBody[index].setPointCount(4u);
            yangSnake.yangBody[index].setRotation(45.0f);
            yangSnake.yangBody[index].setPosition(yangSnake.yangBody[index-1u].getPosition().x, yangSnake.yangBody[index-1u].getPosition().y+yangSnake.yangBody[index-1u].getRadius());
        }
    }
}

void setYingYangPickup(YingYangPickup& yingYangPickup)
{
    yingYangPickup.separated=false;
    yingYangPickup.yingTaken=false;
    yingYangPickup.yangTaken=false;

    yingYangPickup.yingPiece.setSize(sf::Vector2f(sf::VideoMode::getDesktopMode().width/50.0f, sf::VideoMode::getDesktopMode().width/50.0f));
    yingYangPickup.yangPiece.setSize(sf::Vector2f(sf::VideoMode::getDesktopMode().width/50.0f, sf::VideoMode::getDesktopMode().width/50.0f));

    yingYangPickup.yingPiece.setFillColor(sf::Color::Yellow);
    yingYangPickup.yangPiece.setFillColor(sf::Color::Blue);

    yingYangPickup.yingPiece.setOrigin(yingYangPickup.yingPiece.getLocalBounds().left+yingYangPickup.yingPiece.getLocalBounds().width/2.0f, yingYangPickup.yingPiece.getLocalBounds().top+yingYangPickup.yingPiece.getLocalBounds().height/2.0f);
    yingYangPickup.yangPiece.setOrigin(yingYangPickup.yangPiece.getLocalBounds().left+yingYangPickup.yangPiece.getLocalBounds().width/2.0f, yingYangPickup.yangPiece.getLocalBounds().top+yingYangPickup.yangPiece.getLocalBounds().height/2.0f);
}

void setYYPLocationMutator(const YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    yYPLocationMutator.mersenneTwister.seed(yYPLocationMutator.randomDevice());

    yYPLocationMutator.yingYangURDX=std::uniform_real_distribution<float>(yingYangPickup.yingPiece.getSize().x/2.0f, std::nextafter(sf::VideoMode::getDesktopMode().width-yingYangPickup.yangPiece.getSize().x/2.0f, std::numeric_limits<float>::max()));
    yYPLocationMutator.yingYangURDY=std::uniform_real_distribution<float>(yingYangPickup.yingPiece.getSize().y/2.0f, std::nextafter(sf::VideoMode::getDesktopMode().height-yingYangPickup.yangPiece.getSize().y/2.0f, std::numeric_limits<float>::max()));

    yYPLocationMutator.yingURDX=std::uniform_real_distribution<float>(yingYangPickup.yingPiece.getSize().x/2.0f, std::nextafter(sf::VideoMode::getDesktopMode().width-yingYangPickup.yingPiece.getSize().x/2.0f, std::numeric_limits<float>::max()));
    yYPLocationMutator.yangURDX=std::uniform_real_distribution<float>(yingYangPickup.yangPiece.getSize().x/2.0f, std::nextafter(sf::VideoMode::getDesktopMode().width-yingYangPickup.yangPiece.getSize().x/2.0f, std::numeric_limits<float>::max()));
    yYPLocationMutator.yingURDY=std::uniform_real_distribution<float>(yingYangPickup.yingPiece.getSize().y/2.0f, std::nextafter(sf::VideoMode::getDesktopMode().height-yingYangPickup.yingPiece.getSize().y/2.0f, std::numeric_limits<float>::max()));
    yYPLocationMutator.yangURDY=std::uniform_real_distribution<float>(yingYangPickup.yangPiece.getSize().y/2.0f, std::nextafter(sf::VideoMode::getDesktopMode().height-yingYangPickup.yangPiece.getSize().y/2.0f, std::numeric_limits<float>::max()));
}

void drawYingSnake(Display& display, const YingSnake& yingSnake)
{
    for(const sf::CircleShape& circleShape:yingSnake.yingBody)
    {
        display.renderWindow.draw(circleShape);
    }
}

void drawYangSnake(Display& display, const YangSnake& yangSnake)
{
    for(const sf::CircleShape& circleShape:yangSnake.yangBody)
    {
        display.renderWindow.draw(circleShape);
    }
}

void drawYingYangPickup(Display& display, const YingYangPickup& yingYangPickup)
{
    display.renderWindow.draw(yingYangPickup.yingPiece);
    display.renderWindow.draw(yingYangPickup.yangPiece);
}

void mutateYYPLocation(YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    if(yingYangPickup.yingPiece.getFillColor()==sf::Color::Transparent)
    {
        yingYangPickup.yingPiece.setFillColor(sf::Color::Yellow);
    }

    if(yingYangPickup.yangPiece.getFillColor()==sf::Color::Transparent)
    {
        yingYangPickup.yangPiece.setFillColor(sf::Color::Blue);
    }

    yingYangPickup.separated=yYPLocationMutator.bernoulliDistribution(yYPLocationMutator.mersenneTwister);

    if(yingYangPickup.separated)
    {
        if(yingYangPickup.yingTaken)
        {
            yingYangPickup.yingTaken=false;
        }

        if(yingYangPickup.yangTaken)
        {
            yingYangPickup.yangTaken=false;
        }

        yingYangPickup.yingPiece.setPosition(yYPLocationMutator.yingURDX(yYPLocationMutator.mersenneTwister), yYPLocationMutator.yingURDY(yYPLocationMutator.mersenneTwister));
        yingYangPickup.yangPiece.setPosition(yYPLocationMutator.yangURDX(yYPLocationMutator.mersenneTwister), yYPLocationMutator.yangURDY(yYPLocationMutator.mersenneTwister));
    }

    else
    {
        yingYangPickup.yingPiece.setPosition(yYPLocationMutator.yingYangURDX(yYPLocationMutator.mersenneTwister), yYPLocationMutator.yingYangURDY(yYPLocationMutator.mersenneTwister));
        yingYangPickup.yangPiece.setPosition(yingYangPickup.yingPiece.getPosition().x+yingYangPickup.yingPiece.getSize().x/2.0f, yingYangPickup.yingPiece.getPosition().y+yingYangPickup.yingPiece.getSize().y/2.0f);
    }
}

void reset(Display& display, YingSnake& yingSnake, YangSnake& yangSnake, YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    if(display.paused)
    {
        display.paused=false;
    }

    if(display.yingWon)
    {
        display.yingWon=false;
    }

    else if(display.yangWon)
    {
        display.yangWon=false;
    }

    yingSnake.yingBody.clear();
    yangSnake.yangBody.clear();

    try
    {
        setYingSnake(yingSnake);
    }

    catch(const std::length_error& exception)
    {
        throw std::length_error("Error: Cannot reform initial length of Ying Snake.");
    }

    try
    {
        setYangSnake(yangSnake);
    }

    catch(const std::length_error& exception)
    {
        throw std::length_error("Error: Cannot reform initial length of Yang Snake.");
    }

    mutateYYPLocation(yingYangPickup, yYPLocationMutator);
}

void processEvent(Display& display, YingSnake& yingSnake, YangSnake& yangSnake, YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    switch(display.event.type)
    {
        case sf::Event::Closed:
        {
            display.renderWindow.close();

            break;
        }

        case sf::Event::KeyPressed:
        {
            switch(display.event.key.code)
            {
                case sf::Keyboard::Escape:
                {
                    display.renderWindow.close();

                    break;
                }

                case sf::Keyboard::Space:
                {
                    if(display.event.key.control&&!display.yingWon&&!display.yangWon)
                    {
                        try
                        {
                            reset(display, yingSnake, yangSnake, yingYangPickup, yYPLocationMutator);
                        }

                        catch(const std::length_error& exception)
                        {
                            throw std::length_error("Error: Cannot reform initial length of either Ying or Yang Snake.");
                        }
                    }

                    else if(!display.yingWon&&!display.yangWon)
                    {
                        display.paused=!display.paused;
                    }

                    else if(!display.event.key.control&&(display.yingWon||display.yangWon))
                    {
                        try
                        {
                            reset(display, yingSnake, yangSnake, yingYangPickup, yYPLocationMutator);
                        }

                        catch(const std::length_error& exception)
                        {
                            throw std::length_error("Error: Cannot reform initial length of either Ying or Yang Snake.");
                        }
                    }

                    break;
                }

                default:
                {
                    break;
                }
            }

            break;
        }

        case sf::Event::LostFocus:
        {
            if(!display.paused&&!display.yingWon&&!display.yangWon)
            {
                display.paused=true;
            }

            break;
        }

        default:
        {
            break;
        }
    }
}

void moveYingSnake(YingSnake& yingSnake)
{
    if(!yingSnake.yingBody.empty())
    {
        sf::Vector2f currentPosition(yingSnake.yingBody.front().getPosition());
        sf::Vector2f nextPosition;

        yingSnake.yingBody.front().move(yingSnake.yingBody.front().getRadius()*std::cos(yingSnake.yingAngle*3.14159265359f/180.0f), -yingSnake.yingBody.front().getRadius()*std::sin(yingSnake.yingAngle*3.14159265359f/180.0f));

        for(std::vector<sf::CircleShape>::iterator vectorIterator(yingSnake.yingBody.begin()+1u); vectorIterator!=yingSnake.yingBody.end(); ++vectorIterator)
        {
            nextPosition=vectorIterator->getPosition();
            vectorIterator->setPosition(currentPosition);
            currentPosition=nextPosition;
        }
    }
}

void moveYangSnake(YangSnake& yangSnake)
{
    yangSnake.idleEnd=std::chrono::steady_clock::now();

    std::chrono::duration<float, std::milli> idleDuration(yangSnake.idleEnd-yangSnake.idleStart);

    if(idleDuration.count()>=17.5f)
    {
        yangSnake.idleStart=std::chrono::steady_clock::now();

        if(!yangSnake.yangBody.empty())
        {
            sf::Vector2f currentPosition(yangSnake.yangBody.front().getPosition());
            sf::Vector2f nextPosition;

            yangSnake.yangBody.front().move(yangSnake.yangBody.front().getRadius()*std::cos(yangSnake.yangAngle*3.14159265359f/180.0f), -yangSnake.yangBody.front().getRadius()*std::sin(yangSnake.yangAngle*3.14159265359f/180.0f));

            for(std::vector<sf::CircleShape>::iterator vectorIterator(yangSnake.yangBody.begin()+1u); vectorIterator!=yangSnake.yangBody.end(); ++vectorIterator)
            {
                nextPosition=vectorIterator->getPosition();
                vectorIterator->setPosition(currentPosition);
                currentPosition=nextPosition;
            }
        }
    }
}

void computeYangYingX(const sf::Vector2f& yingOffset, const sf::Vector2f& yingEpsilon, YangSnake& yangSnake)
{
    if(yingOffset.x<0.0f&&std::abs(yingOffset.x)>yingEpsilon.x)
    {
        yangSnake.yangAngle=360.0f;
    }

    else if(yingOffset.x>0.0f&&yingOffset.x>yingEpsilon.x)
    {
        yangSnake.yangAngle=180.0f;
    }

    else if(yingOffset.y<0.0f&&std::abs(yingOffset.y)>yingEpsilon.y)
    {
        yangSnake.yangAngle=270.0f;
    }

    else if(yingOffset.y>0.0f&&yingOffset.y>yingEpsilon.y)
    {
        yangSnake.yangAngle=90.0f;
    }
}

void computeYangYangX(const sf::Vector2f& yangOffset, const sf::Vector2f& yangEpsilon, YangSnake& yangSnake)
{
    if(yangOffset.x<0.0f&&std::abs(yangOffset.x)>yangEpsilon.x)
    {
        yangSnake.yangAngle=360.0f;
    }

    else if(yangOffset.x>0.0f&&yangOffset.x>yangEpsilon.x)
    {
        yangSnake.yangAngle=180.0f;
    }

    else if(yangOffset.y<0.0f&&std::abs(yangOffset.y)>yangEpsilon.y)
    {
        yangSnake.yangAngle=270.0f;
    }

    else if(yangOffset.y>0.0f&&yangOffset.y>yangEpsilon.y)
    {
        yangSnake.yangAngle=90.0f;
    }
}

void computeYangYingY(const sf::Vector2f& yingOffset, const sf::Vector2f& yingEpsilon, YangSnake& yangSnake)
{
    if(yingOffset.y<0.0f&&std::abs(yingOffset.y)>yingEpsilon.y)
    {
        yangSnake.yangAngle=270.0f;
    }

    else if(yingOffset.y>0.0f&&yingOffset.y>yingEpsilon.y)
    {
        yangSnake.yangAngle=90.0f;
    }

    else if(yingOffset.x<0.0f&&std::abs(yingOffset.x)>yingEpsilon.x)
    {
        yangSnake.yangAngle=360.0f;
    }

    else if(yingOffset.x>0.0f&&yingOffset.x>yingEpsilon.x)
    {
        yangSnake.yangAngle=180.0f;
    }
}

void computeYangYangY(const sf::Vector2f& yangOffset, const sf::Vector2f& yangEpsilon, YangSnake& yangSnake)
{
    if(yangOffset.y<0.0f&&std::abs(yangOffset.y)>yangEpsilon.y)
    {
        yangSnake.yangAngle=270.0f;
    }

    else if(yangOffset.y>0.0f&&yangOffset.y>yangEpsilon.y)
    {
        yangSnake.yangAngle=90.0f;
    }

    else if(yangOffset.x<0.0f&&std::abs(yangOffset.x)>yangEpsilon.x)
    {
        yangSnake.yangAngle=360.0f;
    }

    else if(yangOffset.x>0.0f&&yangOffset.x>yangEpsilon.x)
    {
        yangSnake.yangAngle=180.0f;
    }
}

void controlYangSeparated(YangSnake& yangSnake, const YingYangPickup& yingYangPickup)
{
    sf::Vector2f yingEpsilon(yingYangPickup.yingPiece.getSize().x/2.0f, yingYangPickup.yingPiece.getSize().y/2.0f);
    sf::Vector2f yangEpsilon(yingYangPickup.yangPiece.getSize().x/2.0f, yingYangPickup.yangPiece.getSize().y/2.0f);

    sf::Vector2f yingOffset(yangSnake.yangBody.front().getPosition()-yingYangPickup.yingPiece.getPosition());
    sf::Vector2f yangOffset(yangSnake.yangBody.front().getPosition()-yingYangPickup.yangPiece.getPosition());

    float minOffset(std::min({std::abs(yingOffset.x), std::abs(yangOffset.x), std::abs(yingOffset.y), std::abs(yangOffset.y)}));

    if(minOffset==std::abs(yingOffset.x))
    {
        if(!yingYangPickup.yingTaken)
        {
            computeYangYingX(yingOffset, yingEpsilon, yangSnake);
        }

        else
        {
            minOffset=std::min(std::abs(yangOffset.x), std::abs(yangOffset.y));

            if(minOffset==std::abs(yangOffset.x))
            {
                computeYangYangX(yangOffset, yangEpsilon, yangSnake);
            }

            else
            {
                computeYangYangY(yangOffset, yangEpsilon, yangSnake);
            }
        }
    }

    else if(minOffset==std::abs(yangOffset.x))
    {
        if(!yingYangPickup.yangTaken)
        {
            computeYangYangX(yangOffset, yangEpsilon, yangSnake);
        }

        else
        {
            minOffset=std::min(std::abs(yingOffset.x), std::abs(yingOffset.y));

            if(minOffset==std::abs(yingOffset.x))
            {
                computeYangYingX(yingOffset, yingEpsilon, yangSnake);
            }

            else
            {
                computeYangYingY(yingOffset, yingEpsilon, yangSnake);
            }
        }
    }

    else if(minOffset==std::abs(yingOffset.y))
    {
        if(!yingYangPickup.yingTaken)
        {
            computeYangYingY(yingOffset, yingEpsilon, yangSnake);
        }

        else
        {
            minOffset=std::min(std::abs(yangOffset.x), std::abs(yangOffset.y));

            if(minOffset==std::abs(yangOffset.x))
            {
                computeYangYangX(yangOffset, yangEpsilon, yangSnake);
            }

            else
            {
                computeYangYangY(yangOffset, yangEpsilon, yangSnake);
            }
        }
    }

    else if(minOffset==std::abs(yangOffset.y))
    {
        if(!yingYangPickup.yangTaken)
        {
            computeYangYangY(yangOffset, yangEpsilon, yangSnake);
        }

        else
        {
            minOffset=std::min(std::abs(yingOffset.x), std::abs(yingOffset.y));

            if(minOffset==std::abs(yingOffset.x))
            {
                computeYangYingX(yingOffset, yingEpsilon, yangSnake);
            }

            else
            {
                computeYangYingY(yingOffset, yingEpsilon, yangSnake);
            }
        }
    }

    if((yangSnake.yangAngle==180.0f&&yangSnake.yangBody.front().getPosition().x>yangSnake.yangBody.front().getRadius()*2.0f)||(yangSnake.yangAngle==360.0f&&yangSnake.yangBody.front().getPosition().x<sf::VideoMode::getDesktopMode().width-yangSnake.yangBody.front().getRadius()*2.0f)||(yangSnake.yangAngle==90.0f&&yangSnake.yangBody.front().getPosition().y>yangSnake.yangBody.front().getRadius()*2.0f)||(yangSnake.yangAngle==270.0f&&yangSnake.yangBody.front().getPosition().y<sf::VideoMode::getDesktopMode().height-yangSnake.yangBody.front().getRadius()*2.0f))
    {
        moveYangSnake(yangSnake);
    }

    else
    {
        if(yangSnake.yangAngle==180.0f)
        {
            yangSnake.yangAngle=360.0f;
        }

        else if(yangSnake.yangAngle==360.0f)
        {
            yangSnake.yangAngle=180.0f;
        }

        else if(yangSnake.yangAngle==90.0f)
        {
            yangSnake.yangAngle=270.0f;
        }

        else if(yangSnake.yangAngle==270.0f)
        {
            yangSnake.yangAngle=90.0f;
        }

        moveYangSnake(yangSnake);
    }
}

void controlYangUnseparated(YangSnake& yangSnake, const YingYangPickup& yingYangPickup)
{
    sf::Vector2f epsilon(yingYangPickup.yangPiece.getPosition()-yingYangPickup.yingPiece.getPosition());

    float offsetX(yangSnake.yangBody.front().getPosition().x-yingYangPickup.yingPiece.getPosition().x);
    float offsetY(yangSnake.yangBody.front().getPosition().y-yingYangPickup.yangPiece.getPosition().y);

    float minOffset(std::min(std::abs(offsetX), std::abs(offsetY)));

    if(minOffset==std::abs(offsetX))
    {
        if(offsetX<0.0f&&std::abs(offsetX)>epsilon.x)
        {
            yangSnake.yangAngle=360.0f;
        }

        else if(offsetX>0.0f&&offsetX>epsilon.x)
        {
            yangSnake.yangAngle=180.0f;
        }

        else if(offsetY<0.0f&&std::abs(offsetY)>epsilon.y)
        {
            yangSnake.yangAngle=270.0f;
        }

        else if(offsetY>0.0f&&offsetY>epsilon.y)
        {
            yangSnake.yangAngle=90.0f;
        }
    }

    else
    {
        if(offsetY<0.0f&&std::abs(offsetY)>epsilon.y)
        {
            yangSnake.yangAngle=270.0f;
        }

        else if(offsetY>0.0f&&offsetY>epsilon.y)
        {
            yangSnake.yangAngle=90.0f;
        }

        else if(offsetX<0.0f&&std::abs(offsetX)>epsilon.x)
        {
            yangSnake.yangAngle=360.0f;
        }

        else if(offsetX>0.0f&&offsetX>epsilon.x)
        {
            yangSnake.yangAngle=180.0f;
        }
    }

    if((yangSnake.yangAngle==180.0f&&yangSnake.yangBody.front().getPosition().x>yangSnake.yangBody.front().getRadius()*2.0f)||(yangSnake.yangAngle==360.0f&&yangSnake.yangBody.front().getPosition().x<sf::VideoMode::getDesktopMode().width-yangSnake.yangBody.front().getRadius()*2.0f)||(yangSnake.yangAngle==90.0f&&yangSnake.yangBody.front().getPosition().y>yangSnake.yangBody.front().getRadius()*2.0f)||(yangSnake.yangAngle==270.0f&&yangSnake.yangBody.front().getPosition().y<sf::VideoMode::getDesktopMode().height-yangSnake.yangBody.front().getRadius()*2.0f))
    {
        moveYangSnake(yangSnake);
    }

    else
    {
        if(yangSnake.yangAngle==180.0f)
        {
            yangSnake.yangAngle=360.0f;
        }

        else if(yangSnake.yangAngle==360.0f)
        {
            yangSnake.yangAngle=180.0f;
        }

        else if(yangSnake.yangAngle==90.0f)
        {
            yangSnake.yangAngle=270.0f;
        }

        else if(yangSnake.yangAngle==270.0f)
        {
            yangSnake.yangAngle=90.0f;
        }

        moveYangSnake(yangSnake);
    }
}

void controlYingSnake(YingSnake& yingSnake)
{
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)||sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        yingSnake.yingAngle=180.0f;
    }

    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)||sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        yingSnake.yingAngle=360.0f;
    }

    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)||sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        yingSnake.yingAngle=90.0f;
    }

    else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)||sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        yingSnake.yingAngle=270.0f;
    }

    if(!yingSnake.moved&&yingSnake.yingAngle>0.0f)
    {
        yingSnake.moved=true;
    }

    if(yingSnake.moved&&((yingSnake.yingAngle==180.0f&&yingSnake.yingBody.front().getPosition().x>yingSnake.yingBody.front().getRadius()*2.0f)||(yingSnake.yingAngle==360.0f&&yingSnake.yingBody.front().getPosition().x<sf::VideoMode::getDesktopMode().width-yingSnake.yingBody.front().getRadius()*2.0f)||(yingSnake.yingAngle==90.0f&&yingSnake.yingBody.front().getPosition().y>yingSnake.yingBody.front().getRadius()*2.0f)||(yingSnake.yingAngle==270.0f&&yingSnake.yingBody.front().getPosition().y<sf::VideoMode::getDesktopMode().height-yingSnake.yingBody.front().getRadius()*2.0f)))
    {
        moveYingSnake(yingSnake);
    }
}

void controlYangSnake(const YingSnake& yingSnake, YangSnake& yangSnake, const YingYangPickup& yingYangPickup)
{
    if(yingSnake.moved)
    {
        if(yingYangPickup.separated)
        {
            controlYangSeparated(yangSnake, yingYangPickup);
        }

        else
        {
            controlYangUnseparated(yangSnake, yingYangPickup);
        }
    }
}

void increaseYingSnake(YingSnake& yingSnake)
{
    if(!yingSnake.yingBody.empty()&&yingSnake.yingBody.size()<103u)
    {
        if(yingSnake.yingBody.size()>=2u)
        {
            yingSnake.yingBody.back().setRotation(45.0f);
        }

        try
        {
            yingSnake.yingBody.emplace_back(sf::VideoMode::getDesktopMode().width/100.0f, 4u);
        }

        catch(const std::bad_alloc& exception)
        {
            throw std::bad_alloc();
        }

        yingSnake.yingBody.back().setFillColor(sf::Color::Yellow);
        yingSnake.yingBody.back().setOrigin(yingSnake.yingBody.back().getLocalBounds().left+yingSnake.yingBody.back().getLocalBounds().width/2.0f, yingSnake.yingBody.back().getLocalBounds().top+yingSnake.yingBody.back().getLocalBounds().height/2.0f);

        std::vector<sf::CircleShape>::size_type previous(yingSnake.yingBody.size()-2u);

        if(yingSnake.yingAngle==180.0f||yingSnake.yingAngle==360.0f)
        {
            yingSnake.yingBody.back().setPosition(yingSnake.yingBody[previous].getPosition().x+yingSnake.yingBody[previous].getRadius(), yingSnake.yingBody[previous].getPosition().y);
        }

        else if(yingSnake.yingAngle==90.0f||yingSnake.yingAngle==270.0f)
        {
            yingSnake.yingBody.back().setPosition(yingSnake.yingBody[previous].getPosition().x, yingSnake.yingBody[previous].getPosition().y+yingSnake.yingBody[previous].getRadius());
        }
    }
}

void increaseYangSnake(YangSnake& yangSnake)
{
    if(!yangSnake.yangBody.empty()&&yangSnake.yangBody.size()<103u)
    {
        if(yangSnake.yangBody.size()>=2u)
        {
            yangSnake.yangBody.back().setRotation(45.0f);
        }

        try
        {
            yangSnake.yangBody.emplace_back(sf::VideoMode::getDesktopMode().width/100.0f, 4u);
        }

        catch(const std::bad_alloc& exception)
        {
            throw std::bad_alloc();
        }

        yangSnake.yangBody.back().setFillColor(sf::Color::Blue);
        yangSnake.yangBody.back().setOrigin(yangSnake.yangBody.back().getLocalBounds().left+yangSnake.yangBody.back().getLocalBounds().width/2.0f, yangSnake.yangBody.back().getLocalBounds().top+yangSnake.yangBody.back().getLocalBounds().height/2.0f);

        std::vector<sf::CircleShape>::size_type previous(yangSnake.yangBody.size()-2u);

        if(yangSnake.yangAngle==180.0f||yangSnake.yangAngle==360.0f)
        {
            yangSnake.yangBody.back().setPosition(yangSnake.yangBody[previous].getPosition().x+yangSnake.yangBody[previous].getRadius(), yangSnake.yangBody[previous].getPosition().y);
        }

        else if(yangSnake.yangAngle==90.0f||yangSnake.yangAngle==270.0f)
        {
            yangSnake.yangBody.back().setPosition(yangSnake.yangBody[previous].getPosition().x, yangSnake.yangBody[previous].getPosition().y+yangSnake.yangBody[previous].getRadius());
        }
    }
}

void decreaseYingSnake(YingSnake& yingSnake)
{
    if(!yingSnake.yingBody.empty())
    {
        yingSnake.yingBody.pop_back();

        if(!yingSnake.yingBody.empty()&&yingSnake.yingBody.back().getRotation()!=0.0f)
        {
            yingSnake.yingBody.back().setRotation(0.0f);
        }
    }
}

void decreaseYangSnake(YangSnake& yangSnake)
{
    if(!yangSnake.yangBody.empty())
    {
        yangSnake.yangBody.pop_back();

        if(!yangSnake.yangBody.empty()&&yangSnake.yangBody.back().getRotation()!=0.0f)
        {
            yangSnake.yangBody.back().setRotation(0.0f);
        }
    }
}

void inspectSeparated(YingSnake& yingSnake, YangSnake& yangSnake, YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    if(!yingYangPickup.yingTaken)
    {
        if(yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())&&!yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds()))
        {
            yingYangPickup.yingTaken=true;
            yingYangPickup.yingPiece.setFillColor(sf::Color::Transparent);

            try
            {
                increaseYingSnake(yingSnake);
            }

            catch(const std::bad_alloc& exception)
            {
                throw std::bad_alloc();
            }
        }

        else if(!yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())&&yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds()))
        {
            yingYangPickup.yingTaken=true;
            yingYangPickup.yingPiece.setFillColor(sf::Color::Transparent);
            decreaseYingSnake(yingSnake);
        }

        else if(yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())&&yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds()))
        {
            yingYangPickup.yingTaken=true;
            yingYangPickup.yingPiece.setFillColor(sf::Color::Transparent);
        }
    }

    if(!yingYangPickup.yangTaken)
    {
        if(yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds())&&!yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds()))
        {
            yingYangPickup.yangTaken=true;
            yingYangPickup.yangPiece.setFillColor(sf::Color::Transparent);
            decreaseYangSnake(yangSnake);
        }

        else if(!yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds())&&yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds()))
        {
            yingYangPickup.yangTaken=true;
            yingYangPickup.yangPiece.setFillColor(sf::Color::Transparent);

            try
            {
                increaseYangSnake(yangSnake);
            }

            catch(const std::bad_alloc& exception)
            {
                throw std::bad_alloc();
            }
        }

        else if(yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds())&&yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds()))
        {
            yingYangPickup.yangTaken=true;
            yingYangPickup.yangPiece.setFillColor(sf::Color::Transparent);
        }
    }

    if(yingYangPickup.yingTaken&&yingYangPickup.yangTaken)
    {
        mutateYYPLocation(yingYangPickup, yYPLocationMutator);
    }
}

void inspectUnseparated(YingSnake& yingSnake, YangSnake& yangSnake, YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    if((yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())||yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds()))&&!(yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())||yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds())))
    {
        try
        {
            increaseYingSnake(yingSnake);
        }

        catch(const std::bad_alloc& exception)
        {
            throw std::bad_alloc();
        }

        decreaseYangSnake(yangSnake);
        mutateYYPLocation(yingYangPickup, yYPLocationMutator);
    }

    else if(!(yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())||yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds()))&&(yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())||yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds())))
    {
        decreaseYingSnake(yingSnake);

        try
        {
            increaseYangSnake(yangSnake);
        }

        catch(const std::bad_alloc& exception)
        {
            throw std::bad_alloc();
        }

        mutateYYPLocation(yingYangPickup, yYPLocationMutator);
    }

    else if((yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())||yingSnake.yingBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds()))&&(yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yingPiece.getGlobalBounds())||yangSnake.yangBody.front().getGlobalBounds().intersects(yingYangPickup.yangPiece.getGlobalBounds())))
    {
        mutateYYPLocation(yingYangPickup, yYPLocationMutator);
    }
}

void inspectIntersections(YingSnake& yingSnake, YangSnake& yangSnake, YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    if(yingYangPickup.separated)
    {
        inspectSeparated(yingSnake, yangSnake, yingYangPickup, yYPLocationMutator);
    }

    else
    {
        inspectUnseparated(yingSnake, yangSnake, yingYangPickup, yYPLocationMutator);
    }
}

void inspectState(Display& display, const YingSnake& yingSnake, const YangSnake& yangSnake)
{
    if(yingSnake.yingBody.empty()&&(!yangSnake.yangBody.empty()||yangSnake.yangBody.size()==103u))
    {
        display.yangWon=true;
    }

    else if(yingSnake.yingBody.size()==103u&&(yangSnake.yangBody.empty()||yangSnake.yangBody.size()!=103u))
    {
        display.yingWon=true;
    }

    else if(yangSnake.yangBody.empty()&&(!yingSnake.yingBody.empty()||yingSnake.yingBody.size()==103u))
    {
        display.yingWon=true;
    }

    else if(yangSnake.yangBody.size()==103u&&(yingSnake.yingBody.empty()||yingSnake.yingBody.size()!=103u))
    {
        display.yangWon=true;
    }
}

void start(Display& display, YingSnake& yingSnake, YangSnake& yangSnake, YingYangPickup& yingYangPickup, YYPLocationMutator& yYPLocationMutator)
{
    while(display.renderWindow.isOpen())
    {
        while(display.renderWindow.pollEvent(display.event))
        {
            try
            {
                processEvent(display, yingSnake, yangSnake, yingYangPickup, yYPLocationMutator);
            }

            catch(const std::length_error& exception)
            {
                throw std::length_error("Error: Cannot reform initial length of either Ying or Yang Snake.");
            }
        }

        if(display.yingWon)
        {
            display.renderWindow.clear(sf::Color::Yellow);
        }

        else if(display.yangWon)
        {
            display.renderWindow.clear(sf::Color::Blue);
        }

        else
        {
            if(display.paused)
            {
                display.renderWindow.clear(sf::Color::Red);
            }

            else
            {
                controlYingSnake(yingSnake);
                controlYangSnake(yingSnake, yangSnake, yingYangPickup);

                try
                {
                    inspectIntersections(yingSnake, yangSnake, yingYangPickup, yYPLocationMutator);
                }

                catch(const std::bad_alloc& exception)
                {
                    throw std::bad_alloc();
                }

                inspectState(display, yingSnake, yangSnake);

                display.renderWindow.clear(sf::Color::Magenta);
            }

            drawYingSnake(display, yingSnake);
            drawYangSnake(display, yangSnake);
            drawYingYangPickup(display, yingYangPickup);
        }

        display.renderWindow.display();
    }
}

int main()
{
    Display display;
    YingSnake yingSnake;
    YangSnake yangSnake;
    YingYangPickup yingYangPickup;
    YYPLocationMutator yYPLocationMutator;

    setDisplay(display);

    try
    {
        setYingSnake(yingSnake);
    }

    catch(const std::length_error& exception)
    {
        return EXIT_FAILURE;
    }

    try
    {
        setYangSnake(yangSnake);
    }

    catch(const std::length_error& exception)
    {
        return EXIT_FAILURE;
    }

    setYingYangPickup(yingYangPickup);
    setYYPLocationMutator(yingYangPickup, yYPLocationMutator);

    mutateYYPLocation(yingYangPickup, yYPLocationMutator);

    try
    {
        start(display, yingSnake, yangSnake, yingYangPickup, yYPLocationMutator);
    }

    catch(const std::bad_alloc& exception)
    {
        return EXIT_FAILURE;
    }

    catch(const std::length_error& exception)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
