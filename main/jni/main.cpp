#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include <list>
#include <memory>
#include <android/window.h>
//#if defined(USE_JNI)
// These headers are only needed for direct NDK/JDK interaction
#include <jni.h>
#include <android/native_activity.h>

// Since we want to get the native activity from SFML, we'll have to use an
// extra header here:
#include <SFML/System/NativeActivity.hpp>

// NDK/JNI sub example - call Java code from native code

int vibrate(sf::Time duration)
{
    // First we'll need the native activity handle
    ANativeActivity *activity = sf::getNativeActivity();
 //   activity->callbacks->onPause(sf::getNativeActivity());
	
    // Retrieve the JVM and JNI environment
    JavaVM* vm = activity->vm;
    JNIEnv* env = activity->env;

    // First, attach this thread to the main thread
    JavaVMAttachArgs attachargs;
    attachargs.version = JNI_VERSION_1_6;
    attachargs.name = "NativeThread";
    attachargs.group = NULL;
     jint res = vm->AttachCurrentThread(&env, &attachargs);

    if (res == JNI_ERR)
        return EXIT_FAILURE;

    // Retrieve class information
    jclass natact = env->FindClass("android/app/NativeActivity");
    jclass context = env->FindClass("android/content/Context");
    
    // Get the value of a constant
    jfieldID fid = env->GetStaticFieldID(context, "VIBRATOR_SERVICE", "Ljava/lang/String;");
    jobject svcstr = env->GetStaticObjectField(context, fid);
    
    // Get the method 'getSystemService' and call it
    jmethodID getss = env->GetMethodID(natact, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject vib_obj = env->CallObjectMethod(activity->clazz, getss, svcstr);
    
    // Get the object's class and retrieve the member name
    jclass vib_cls = env->GetObjectClass(vib_obj);
    jmethodID vibrate = env->GetMethodID(vib_cls, "vibrate", "(J)V"); 
    
    // Determine the timeframe
    jlong length = duration.asMilliseconds();
    
    // Bzzz!
    env->CallVoidMethod(vib_obj, vibrate, length);
    
    // Free references
    env->DeleteLocalRef(vib_obj);
    env->DeleteLocalRef(vib_cls);
    env->DeleteLocalRef(svcstr);
    env->DeleteLocalRef(context);
    env->DeleteLocalRef(natact);
    
    // Detach thread again
    vm->DetachCurrentThread();
	
	return 0;
}

//#endif

void AutoHideNavBar()
{     
    ANativeActivity* activity = sf::getNativeActivity();
    JavaVM* vm = activity->vm;
    JNIEnv* env = activity->env;

    JavaVMAttachArgs attachargs;
    attachargs.version = JNI_VERSION_1_6;
    attachargs.name = "NativeThread";
    attachargs.group = NULL;
    jint res = vm->AttachCurrentThread(&env, &attachargs);

    if (res == JNI_ERR)
         return;

    activity->vm->AttachCurrentThread(&env, NULL);

    jclass activityClass = env->FindClass("android/app/NativeActivity");
    jmethodID getWindow = env->GetMethodID(activityClass, "getWindow", "()Landroid/view/Window;");

    jclass windowClass = env->FindClass("android/view/Window");
    jmethodID getDecorView = env->GetMethodID(windowClass, "getDecorView", "()Landroid/view/View;");

    jclass viewClass = env->FindClass("android/view/View");
    jmethodID setSystemUiVisibility = env->GetMethodID(viewClass, "setSystemUiVisibility", "(I)V");

    jobject window = env->CallObjectMethod(activity->clazz, getWindow);

    jobject decorView = env->CallObjectMethod(window, getDecorView);

    jfieldID flagFullscreenID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_FULLSCREEN", "I");
    jfieldID flagHideNavigationID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_HIDE_NAVIGATION", "I");
    jfieldID flagImmersiveStickyID = env->GetStaticFieldID(viewClass, "SYSTEM_UI_FLAG_IMMERSIVE_STICKY", "I");

    const int flagFullscreen = env->GetStaticIntField(viewClass, flagFullscreenID);
    const int flagHideNavigation = env->GetStaticIntField(viewClass, flagHideNavigationID);
    const int flagImmersiveSticky = env->GetStaticIntField(viewClass, flagImmersiveStickyID);
    const int flag = flagFullscreen | flagHideNavigation | flagImmersiveSticky;

    env->CallVoidMethod(decorView, setSystemUiVisibility, flag);   
    activity->vm->DetachCurrentThread();
}




class Lattice :public sf::Drawable
{
    sf::Texture t_lattice;
    float x;
    float y;
    sf::Sprite s_lattice;
public:
    Lattice();
    virtual void draw(sf::RenderTarget&, sf::RenderStates)const;
};

Lattice::Lattice() :x(137.5), y(542.5)
{
    t_lattice.loadFromFile("field.png");
    s_lattice.setTexture(t_lattice);
    s_lattice.setPosition(x, y);
}

void Lattice::draw(sf::RenderTarget& target, sf::RenderStates)const
{
    target.draw(s_lattice);
}

class Blue_lines : public sf::Drawable
{
    sf::Texture t_horisontal_line;
    sf::Texture t_vertical_line;
    sf::Texture t_right_line;
    sf::Texture t_left_line;
public:
    Blue_lines();
    sf::Sprite s_horisontal_line;
    sf::Sprite s_vertical_line;
    sf::Sprite s_right_line;
    sf::Sprite s_left_line;
    bool horisontal;
    bool vertical;
    bool right;
    bool left;
    virtual void draw(sf::RenderTarget&, sf::RenderStates)const;
};




void Blue_lines::draw(sf::RenderTarget& target, sf::RenderStates)const
{
    if (horisontal) { target.draw(s_horisontal_line); }
    if (vertical) { target.draw(s_vertical_line); }
    if (right) { target.draw(s_right_line); }
    if (left) { target.draw(s_left_line); }
}



Blue_lines::Blue_lines() :horisontal(false), vertical(false), right(false), left(false)
{
    t_horisontal_line.loadFromFile("horisontal_line.png");
    s_horisontal_line.setTexture(t_horisontal_line);

    t_vertical_line.loadFromFile("vertical_line.png");
    s_vertical_line.setTexture(t_vertical_line);

    t_right_line.loadFromFile("line_right.png");
    s_right_line.setTexture(t_right_line);

    t_left_line.loadFromFile("line_left.png");
    s_left_line.setTexture(t_left_line);
}


class Crosses :public sf::Drawable
{
    sf::Texture t_cross;
    float x = 0.0;
    float y = 0.0;
public:
    bool check;
    Crosses(float, float);
    sf::Sprite s_cross;
    virtual void draw(sf::RenderTarget&, sf::RenderStates)const;
    void show() { check = true; }

};

Crosses::Crosses(float a, float b) :x(a), y(b), check(false)
{
    t_cross.loadFromFile("cross.png");
    s_cross.setTexture(t_cross);
    s_cross.setPosition(x, y);
}

void Crosses::draw(sf::RenderTarget& target, sf::RenderStates)const
{
    if (check) { target.draw(s_cross); }
}

class Zero :public sf::Drawable
{
    sf::Texture t_zero;
    float x = 0.0;
    float y = 0.0;
public:
    Zero(float, float);
    sf::Sprite s_zero;
    bool check;
    virtual void draw(sf::RenderTarget&, sf::RenderStates)const;
    void show() { check = true; };
};

Zero::Zero(float a, float b) :x(a), y(b), check(false)
{
    t_zero.loadFromFile("zero.png");
    s_zero.setTexture(t_zero);
    s_zero.setPosition(x, y);
}

void Zero::draw(sf::RenderTarget& state, sf::RenderStates)const
{
    if (check) { state.draw(s_zero); }
}


class Sound
{
    sf::SoundBuffer buffer;
public:
    sf::Sound sound;
    Sound()
    {
        buffer.loadFromFile("win.wav");
        sound.setBuffer(buffer);
    }
};


class Button :public sf::Drawable
{
    sf::Texture t_button;
    float x;
    float y;
public:
    Button();
    sf::Sprite s_button;
    bool check;
    virtual void draw(sf::RenderTarget&, sf::RenderStates)const;
};

Button::Button() :x(270), y(1050), check(false)
{
    t_button.loadFromFile("button.png");
    s_button.setTexture(t_button);
    s_button.setPosition(x, y);
}


void Button::draw(sf::RenderTarget& t, sf::RenderStates)const
{
    if (check) { t.draw(s_button); }
}

class Restart :public sf::Drawable
{
    sf::Texture t_restart;
    sf::Sprite s_restart;
    float x;
    float y;
public:
    Restart();
    virtual void draw(sf::RenderTarget&, sf::RenderStates)const;
    bool check;
};

Restart::Restart() :x(270), y(1280), check(false)
{
    t_restart.loadFromFile("restart.png");
    s_restart.setTexture(t_restart);
    s_restart.setPosition(x, y);
}


void Restart::draw(sf::RenderTarget& t, sf::RenderStates)const
{
    if (check) { t.draw(s_restart); }
}


void to_prevent_entry(std::vector<bool>& a)
{
    for (int i = 0; i < a.size(); ++i) { a[i] = false; }
}

class Score:public sf::Drawable
{
    sf::Font font;
    sf::Text colon;
public:
    Score();
    sf::Text green_text;
    sf::Text red_text;
    std::string green_string;
    std::string red_string;
    int crosses;
    int zero;
    virtual void draw(sf::RenderTarget&,sf::RenderStates)const;
};

void Score::draw(sf::RenderTarget& target, sf::RenderStates)const
{
    target.draw(green_text);
    target.draw(red_text);
    target.draw(colon);
}



Score::Score():crosses(0),zero(0)
{
    font.loadFromFile("MyriadProBold.ttf");
    green_text.setFont(font);
    red_text.setFont(font);
    colon.setFont(font);
    colon.setString(":");
    sf::Color green_color(1,204,0);
    sf::Color red_color(246,9,9);
    sf::Color grey_color(172,165,167);
    green_text.setFillColor(green_color);
    red_text.setFillColor(red_color);
    colon.setFillColor(grey_color);
    green_text.setCharacterSize(72);
    red_text.setCharacterSize(72);
    colon.setCharacterSize(72);
    green_text.setPosition(300,405);
    red_text.setPosition(380,405);
    colon.setPosition(350, 400);
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(720, 1520), "TIC TAC toe");
    AutoHideNavBar();
    int selector = 0;
    bool the_alternation_of_moves = true;
    std::vector<bool>cells{ true,true,true,true,true,true,true,true,true };
    sf::Time bzzz = sf::microseconds(200000);
    Lattice lattice;
    Button button;
    Restart restart;
    Sound sound_win;
    Score red_number;
    Score green_number;

    red_number.red_string = std::to_string(red_number.crosses);
    red_number.red_text.setString(red_number.red_string);

    green_number.green_string = std::to_string(green_number.zero);
    green_number.green_text.setString(green_number.green_string);
    
    Crosses s1_cross[9] = { Crosses(140,545),Crosses(290,545),Crosses(440,545),
                      Crosses(140,695),Crosses(290,695),Crosses(440,695),
                      Crosses(140,845),Crosses(290,845),Crosses(440,845),
    };
    Zero s2_zero[9] = { Zero(140, 545), Zero(290, 545), Zero(440, 545),
                   Zero(140, 695), Zero(290, 695), Zero(440, 695),
                   Zero(140, 845), Zero(290, 845), Zero(440, 845),
    };

    Blue_lines lines[8] = { Blue_lines(), Blue_lines(),Blue_lines(),
                       Blue_lines(),Blue_lines(),Blue_lines(),
                       Blue_lines(),Blue_lines()
    };


    
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            sf::Vector2i get_pixel = sf::Mouse::getPosition(window);
            sf::Vector2f position = window.mapPixelToCoords(get_pixel);
            
                        if (event.type == sf::Event::MouseButtonPressed)
                        {
                            if (event.key.code == sf::Mouse::Left)
                            {

                                for (int i = 0; i < 9; ++i)
                                {
                                   
                                    switch (selector)
                                    {
                                    case 0:
                                    {
                                        if (s1_cross[i].s_cross.getGlobalBounds().contains(position))
                                        {
                                            if (cells[i])
                                            {
                                                s1_cross[i].show();
                                           
                                                if (s1_cross[0].check && s1_cross[1].check && s1_cross[2].check) { lines[0].s_horisontal_line.setPosition(138, 600); lines[0].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   ++red_number.crosses;
                                                   red_number.red_string = std::to_string(red_number.crosses);
                                                   red_number.red_text.setString(red_number.red_string);
                                                   vibrate(bzzz);
                                              
                                                }
                                                if (s1_cross[3].check && s1_cross[4].check && s1_cross[5].check) { lines[1].s_horisontal_line.setPosition(138, 750);  lines[1].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true; 
                                                   ++red_number.crosses;
                                                   red_number.red_string = std::to_string(red_number.crosses);
                                                   red_number.red_text.setString(red_number.red_string);
                                                   vibrate(bzzz);
                                                   
                                                }
                                                if (s1_cross[6].check && s1_cross[7].check && s1_cross[8].check) { lines[2].s_horisontal_line.setPosition(138, 900);  lines[2].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                    ++red_number.crosses;
                                                    red_number.red_string = std::to_string(red_number.crosses);
                                                    red_number.red_text.setString(red_number.red_string);
                                                    vibrate(bzzz);
                                                   
                                                }

                                                if (s1_cross[0].check && s1_cross[3].check && s1_cross[6].check) { lines[3].s_vertical_line.setPosition(198, 540); lines[3].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   ++red_number.crosses;
                                                   red_number.red_string = std::to_string(red_number.crosses);
                                                   red_number.red_text.setString(red_number.red_string);
                                                   vibrate(bzzz);
                                                   
                                                }
                                                if (s1_cross[1].check && s1_cross[4].check && s1_cross[7].check) { lines[4].s_vertical_line.setPosition(350, 540); lines[4].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   ++red_number.crosses;
                                                   red_number.red_string = std::to_string(red_number.crosses);
                                                   red_number.red_text.setString(red_number.red_string);
                                                   vibrate(bzzz);
                                                
                                                }
                                                if (s1_cross[2].check && s1_cross[5].check && s1_cross[8].check) { lines[5].s_vertical_line.setPosition(500, 540); lines[5].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   ++red_number.crosses;
                                                   red_number.red_string = std::to_string(red_number.crosses);
                                                   red_number.red_text.setString(red_number.red_string);
                                                   vibrate(bzzz);
                                                  
                                                }

                                                if (s1_cross[0].check && s1_cross[4].check && s1_cross[8].check) { lines[6].s_left_line.setPosition(140, 540); lines[6].left = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   ++red_number.crosses;
                                                   red_number.red_string = std::to_string(red_number.crosses);
                                                   red_number.red_text.setString(red_number.red_string);
                                                   vibrate(bzzz);
                                                 
                                                }
                                                if (s1_cross[2].check && s1_cross[4].check && s1_cross[6].check) { lines[7].s_right_line.setPosition(140, 540); lines[7].right = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   ++red_number.crosses;
                                                   red_number.red_string = std::to_string(red_number.crosses);
                                                   red_number.red_text.setString(red_number.red_string);
                                                   vibrate(bzzz);
                                                                                                     
                                                }

                                                selector += 1;
                                                cells[i] = false;

                                                if (std::all_of(cells.cbegin(), cells.cend(), [](int i) { return i == 0; }))
                                                {
                                                    button.check = true;
                                                    restart.check = true;
                                                }

                                                

                                            }

                                            
                                        }
     
                                        break;
                                    }

                                    case 1:
                                    {
                                        if (s2_zero[i].s_zero.getGlobalBounds().contains(position))
                                         {
                                            if (cells[i])
                                            {
                                                s2_zero[i].show();
                                            
                                                if (s2_zero[0].check && s2_zero[1].check && s2_zero[2].check) { lines[0].s_horisontal_line.setPosition(138, 600); lines[0].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;                                                 
                                                    green_number.zero++;
                                                    if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405);}
                                                    if (green_number.zero >= 100 && green_number.zero < 1000){green_number.green_text.setPosition(230, 405);}
                                                    if (green_number.zero >= 1000 && green_number.zero < 10000){green_number.green_text.setPosition(190, 405);}
                                                    green_number.green_string = std::to_string(green_number.zero);
                                                    green_number.green_text.setString(green_number.green_string); 
                                                    vibrate(bzzz);
                                                }
                                                if (s2_zero[3].check && s2_zero[4].check && s2_zero[5].check) { lines[1].s_horisontal_line.setPosition(138, 750); lines[1].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   green_number.zero++;
                                                   if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                   if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                   if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                   green_number.green_string = std::to_string(green_number.zero);
                                                   green_number.green_text.setString(green_number.green_string);
                                                   vibrate(bzzz);
                                                }
                                                if (s2_zero[6].check && s2_zero[7].check && s2_zero[8].check) { lines[2].s_horisontal_line.setPosition(138, 900); lines[2].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   green_number.zero++;
                                                   if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                   if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                   if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                   green_number.green_string = std::to_string(green_number.zero);
                                                   green_number.green_text.setString(green_number.green_string);
                                                   vibrate(bzzz);
                                                }

                                                if (s2_zero[0].check && s2_zero[3].check && s2_zero[6].check) { lines[3].s_vertical_line.setPosition(198, 540); lines[3].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   green_number.zero++;
                                                   if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                   if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                   if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                   green_number.green_string = std::to_string(green_number.zero);
                                                   green_number.green_text.setString(green_number.green_string);
                                                   vibrate(bzzz);
                                                }
                                                if (s2_zero[1].check && s2_zero[4].check && s2_zero[7].check) { lines[4].s_vertical_line.setPosition(350, 540); lines[4].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   green_number.zero++;
                                                   if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                   if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                   if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                   green_number.green_string = std::to_string(green_number.zero);
                                                   green_number.green_text.setString(green_number.green_string);
                                                   vibrate(bzzz);
                                                }
                                                if (s2_zero[2].check && s2_zero[5].check && s2_zero[8].check) { lines[5].s_vertical_line.setPosition(500, 540); lines[5].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   green_number.zero++;
                                                   if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                   if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                   if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                   green_number.green_string = std::to_string(green_number.zero);
                                                   green_number.green_text.setString(green_number.green_string);
                                                   vibrate(bzzz);
                                                }

                                                if (s2_zero[0].check && s2_zero[4].check && s2_zero[8].check) { lines[6].s_left_line.setPosition(140, 540); lines[6].left = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   green_number.zero++;
                                                   if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                   if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                   if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                   green_number.green_string = std::to_string(green_number.zero);
                                                   green_number.green_text.setString(green_number.green_string);
                                                   vibrate(bzzz);
                                                }
                                                if (s2_zero[2].check && s2_zero[4].check && s2_zero[6].check) { lines[7].s_right_line.setPosition(140, 540); lines[7].right = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                   green_number.zero++;
                                                   if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                   if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                   if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                   green_number.green_string = std::to_string(green_number.zero);
                                                   green_number.green_text.setString(green_number.green_string);
                                                  vibrate(bzzz);
                                                }

                                                selector = 0;
                                                cells[i] = false;

                                                if (std::all_of(cells.cbegin(), cells.cend(), [](int i) { return i == 0; }))
                                                {                                                  
                                                    button.check = true;
                                                    restart.check = true;                       
                                                }

                                            }
                                         }
            
                                         break;
                                    }
                                   
                                    }

                                }

                            }

                        }

                        if (event.type == sf::Event::MouseButtonPressed)
                        {
                            if (event.key.code == sf::Mouse::Left)
                            {
                                if (button.s_button.getGlobalBounds().contains(position))
                                {
                                    for (int i = 0; i < cells.size(); ++i)
                                    {
                                        cells[i] = true;
                                        s1_cross[i].check = false;
                                        s2_zero[i].check = false;
                                    }
                                    for (int i = 0; i < 8; ++i)
                                    {
                                        lines[i].horisontal = false;
                                        lines[i].vertical = false;
                                        lines[i].left = false;
                                        lines[i].right = false;
                                    }
                                    if (the_alternation_of_moves)
                                    {
                                        selector = 1;
                                        the_alternation_of_moves = false;
                                    }
                                    else {
                                        selector = 0;
                                        the_alternation_of_moves = true;
                                    }
                                    button.check = false;
                                    restart.check = false;

                                }

                            }

                        }
                        
                       
                                    sf::Vector2i touch_pixel_position = sf::Touch::getPosition(0,window);
                                    sf::Vector2f pixel_position = window.mapPixelToCoords(touch_pixel_position);

                                    if (event.type == sf::Event::TouchBegan)
                                    {
                                        for (int i = 0; i < 9; ++i)
                                        {

                                            switch (selector)
                                            {
                                            case 0:
                                            {
                                                if (s1_cross[i].s_cross.getGlobalBounds().contains(pixel_position))
                                                {
                                                    if (cells[i])
                                                    {
                                                        s1_cross[i].show();
                                                        if (s1_cross[0].check && s1_cross[1].check && s1_cross[2].check) {
                                                            lines[0].s_horisontal_line.setPosition(138, 600); lines[0].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }
                                                        if (s1_cross[3].check && s1_cross[4].check && s1_cross[5].check) {
                                                            lines[1].s_horisontal_line.setPosition(138, 750);  lines[1].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }
                                                        if (s1_cross[6].check && s1_cross[7].check && s1_cross[8].check) {
                                                            lines[2].s_horisontal_line.setPosition(138, 900);  lines[2].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }

                                                        if (s1_cross[0].check && s1_cross[3].check && s1_cross[6].check) {
                                                            lines[3].s_vertical_line.setPosition(198, 540); lines[3].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }
                                                        if (s1_cross[1].check && s1_cross[4].check && s1_cross[7].check) {
                                                            lines[4].s_vertical_line.setPosition(350, 540); lines[4].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }
                                                        if (s1_cross[2].check && s1_cross[5].check && s1_cross[8].check) {
                                                            lines[5].s_vertical_line.setPosition(500, 540); lines[5].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }

                                                        if (s1_cross[0].check && s1_cross[4].check && s1_cross[8].check) {
                                                            lines[6].s_left_line.setPosition(140, 540); lines[6].left = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }
                                                        if (s1_cross[2].check && s1_cross[4].check && s1_cross[6].check) {
                                                            lines[7].s_right_line.setPosition(140, 540); lines[7].right = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            ++red_number.crosses;
                                                            red_number.red_string = std::to_string(red_number.crosses);
                                                            red_number.red_text.setString(red_number.red_string);
                                                            vibrate(bzzz);

                                                        }

                                                        selector += 1;
                                                        cells[i] = false;

                                                        if (std::all_of(cells.cbegin(), cells.cend(), [](int i) { return i == 0; }))
                                                        {
                                                            button.check = true;
                                                            restart.check = true;
                                                        }

                                                    }
                                                }
                                                break;
                                            }

                                            case 1:
                                            {
                                                if (s2_zero[i].s_zero.getGlobalBounds().contains(pixel_position))
                                                {
                                                    if (cells[i])
                                                    {
                                                        s2_zero[i].show();
                                                        if (s2_zero[0].check && s2_zero[1].check && s2_zero[2].check) {
                                                            lines[0].s_horisontal_line.setPosition(138, 600); lines[0].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }
                                                        if (s2_zero[3].check && s2_zero[4].check && s2_zero[5].check) {
                                                            lines[1].s_horisontal_line.setPosition(138, 750); lines[1].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }
                                                        if (s2_zero[6].check && s2_zero[7].check && s2_zero[8].check) {
                                                            lines[2].s_horisontal_line.setPosition(138, 900); lines[2].horisontal = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }

                                                        if (s2_zero[0].check && s2_zero[3].check && s2_zero[6].check) {
                                                            lines[3].s_vertical_line.setPosition(198, 540); lines[3].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }
                                                        if (s2_zero[1].check && s2_zero[4].check && s2_zero[7].check) {
                                                            lines[4].s_vertical_line.setPosition(350, 540); lines[4].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }
                                                        if (s2_zero[2].check && s2_zero[5].check && s2_zero[8].check) {
                                                            lines[5].s_vertical_line.setPosition(500, 540); lines[5].vertical = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }

                                                        if (s2_zero[0].check && s2_zero[4].check && s2_zero[8].check) {
                                                            lines[6].s_left_line.setPosition(140, 540); lines[6].left = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }
                                                        if (s2_zero[2].check && s2_zero[4].check && s2_zero[6].check) {
                                                            lines[7].s_right_line.setPosition(140, 540); lines[7].right = true; sound_win.sound.play(); to_prevent_entry(cells); button.check = true; restart.check = true;
                                                            green_number.zero++;
                                                            if (green_number.zero >= 10 && green_number.zero < 100) { green_number.green_text.setPosition(270, 405); }
                                                            if (green_number.zero >= 100 && green_number.zero < 1000) { green_number.green_text.setPosition(230, 405); }
                                                            if (green_number.zero >= 1000 && green_number.zero < 10000) { green_number.green_text.setPosition(190, 405); }
                                                            green_number.green_string = std::to_string(green_number.zero);
                                                            green_number.green_text.setString(green_number.green_string);
                                                            vibrate(bzzz);
                                                        }
                                                        selector = 0;
                                                        cells[i] = false;

                                                        if (std::all_of(cells.cbegin(), cells.cend(), [](int i) { return i == 0; }))
                                                        {
                                                            button.check = true;
                                                            restart.check = true;
                                                        }

                                                    }
                                                }
                                                break;
                                            }
                                            }
                                        }
                                    }

                                    if (event.type == sf::Event::TouchBegan)
                                    {
                                        if (button.s_button.getGlobalBounds().contains(pixel_position))
                                        {
                                            for (int i = 0; i < cells.size(); ++i)
                                            {
                                                cells[i] = true;
                                                s1_cross[i].check = false;
                                                s2_zero[i].check = false;
                                            }
                                            for (int i = 0; i < 8; ++i)
                                            {
                                                lines[i].horisontal = false;
                                                lines[i].vertical = false;
                                                lines[i].left = false;
                                                lines[i].right = false;
                                            }

                                            if (the_alternation_of_moves)
                                            {
                                                selector = 1;
                                                the_alternation_of_moves = false;
                                            }
                                            else {
                                                selector = 0;
                                                the_alternation_of_moves = true;
                                            }
                                            button.check = false;
                                            restart.check = false;

                                        }

                                    }
                        
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        AutoHideNavBar();
        window.draw(lattice);
        window.draw(button);
        window.draw(restart);
        window.draw(red_number);
        window.draw(green_number);
        for (int i = 0; i < 9; i++)
        {
            window.draw(s1_cross[i]);
        }


        for (int i = 0; i < 9; i++)
        {
            window.draw(s2_zero[i]);
        }


        for (int i = 0; i < 8; i++)
        {
            window.draw(lines[i]);
        }





        window.display();
    }

    return 0;
}

























