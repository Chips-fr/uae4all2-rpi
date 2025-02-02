#include <algorithm>
#ifdef ANDROIDSDL
#include <android/log.h>
#endif
#include <guichan.hpp>
#include <iostream>
#include <sstream>
#include <SDL/SDL_ttf.h>
#include <guichan/sdl.hpp>
#include "sdltruetypefont.hpp"

#include "sysconfig.h"
#include "sysdeps.h"
#include "config.h"

#include "uaeradiobutton.hpp"
#include "uaedropdown.hpp"
#include "menu.h"
#include "menu_config.h"
#include "options.h"

namespace widgets
{
void show_settings_TabControl(void);

extern gcn::Color baseCol;
extern gcn::Color baseColLabel;
extern gcn::Container* top;
extern gcn::TabbedArea* tabbedArea;
extern gcn::Icon* icon_winlogo;
#ifdef ANDROIDSDL
extern gcn::contrib::SDLTrueTypeFont* font14;
#endif


gcn::Container *tab_control;
gcn::Window *group_control_config;
gcn::UaeRadioButton* radioButton_control_config_1;
gcn::UaeRadioButton* radioButton_control_config_2;
gcn::UaeRadioButton* radioButton_control_config_3;
gcn::UaeRadioButton* radioButton_control_config_4;

gcn::CheckBox* checkBox_statusline;

gcn::Label* label1_control_config;
gcn::Label* label2_control_config;
gcn::Window *group_joystick;
gcn::UaeRadioButton* radioButton_joy_port0;
gcn::UaeRadioButton* radioButton_joy_port1;
gcn::UaeRadioButton* radioButton_joy_both;

gcn::Window *group_autofirerate;
gcn::UaeRadioButton* radioButton_autofirerate_light;
gcn::UaeRadioButton* radioButton_autofirerate_medium;
gcn::UaeRadioButton* radioButton_autofirerate_heavy;
gcn::Window *group_mouseMultiplier;
gcn::UaeRadioButton* radioButton_mouseMultiplier_25;
gcn::UaeRadioButton* radioButton_mouseMultiplier_50;
gcn::UaeRadioButton* radioButton_mouseMultiplier_1;
gcn::UaeRadioButton* radioButton_mouseMultiplier_2;
gcn::UaeRadioButton* radioButton_mouseMultiplier_4;
#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
gcn::Window *group_tapDelay;
gcn::UaeRadioButton* radioButton_tapDelay_10;
gcn::UaeRadioButton* radioButton_tapDelay_5;
gcn::UaeRadioButton* radioButton_tapDelay_2;
gcn::Container* backgrd_offset;
gcn::Label* label_offset;
gcn::UaeDropDown* dropDown_offset;
#endif

#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
class OffsetListModel : public gcn::ListModel
{
public:
    int getNumberOfElements() {
        return 11;
    }

    std::string getElementAt(int i) {
        switch(i) {
        case 0:
            return std::string("none");
        case 1:
            return std::string("1 px");
        case 2:
            return std::string("2 px");
        case 3:
            return std::string("3 px");
        case 4:
            return std::string("4 px");
        case 5:
            return std::string("5 px");
        case 6:
            return std::string("6 px");
        case 7:
            return std::string("7 px");
        case 8:
            return std::string("8 px");
        case 9:
            return std::string("9 px");
        case 10:
            return std::string("10 px");
        }
        return std::string(""); // Keep the compiler happy
    }
};
OffsetListModel offsetList;
#endif

class SelConfigActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_control_config_1)
            mainMenu_joyConf=0;
        else if (actionEvent.getSource() == radioButton_control_config_2)
            mainMenu_joyConf=1;
        else if (actionEvent.getSource() == radioButton_control_config_3)
            mainMenu_joyConf=2;
        else if (actionEvent.getSource() == radioButton_control_config_4)
            mainMenu_joyConf=3;
        set_joyConf();
        show_settings_TabControl();
    }
};
SelConfigActionListener* selConfigActionListener;


class JoystickActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_joy_port0)
            mainMenu_joyPort=1;
        else  if (actionEvent.getSource() == radioButton_joy_port1)
            mainMenu_joyPort=2;
        else  if (actionEvent.getSource() == radioButton_joy_both)
            mainMenu_joyPort=0;
    }
};
JoystickActionListener* joystickActionListener;


class StatuslineActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == checkBox_statusline)
            if (checkBox_statusline->isSelected())
                mainMenu_showStatus=true;
            else
                mainMenu_showStatus=false;
    }
};
StatuslineActionListener* statuslineActionListener;


class AutofireActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_autofirerate_light)
            mainMenu_autofireRate=12;
        else if (actionEvent.getSource() == radioButton_autofirerate_medium)
            mainMenu_autofireRate=8;
        else if (actionEvent.getSource() == radioButton_autofirerate_heavy)
            mainMenu_autofireRate=4;
    }
};
AutofireActionListener* autofireActionListener;


class MouseMultActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_mouseMultiplier_25)
            mainMenu_mouseMultiplier=25;
        else if (actionEvent.getSource() == radioButton_mouseMultiplier_50)
            mainMenu_mouseMultiplier=50;
        else if (actionEvent.getSource() == radioButton_mouseMultiplier_1)
            mainMenu_mouseMultiplier=1;
        else if (actionEvent.getSource() == radioButton_mouseMultiplier_2)
            mainMenu_mouseMultiplier=2;
        else if (actionEvent.getSource() == radioButton_mouseMultiplier_4)
            mainMenu_mouseMultiplier=4;
    }
};
MouseMultActionListener* mouseMultActionListener;

#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
class TapDelayActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        if (actionEvent.getSource() == radioButton_tapDelay_10)
            mainMenu_tapDelay=10;
        else if (actionEvent.getSource() == radioButton_tapDelay_5)
            mainMenu_tapDelay=5;
        else if (actionEvent.getSource() == radioButton_tapDelay_2)
            mainMenu_tapDelay=2;
    }
};
TapDelayActionListener* tapDelayActionListener;

class OffsetActionListener : public gcn::ActionListener
{
public:
    void action(const gcn::ActionEvent& actionEvent) {
        mainMenu_stylusOffset = dropDown_offset->getSelected() * 2;
    }
};
OffsetActionListener* offsetActionListener;
#endif

void menuTabControl_Init()
{
    // Select Control config
    radioButton_control_config_1 = new gcn::UaeRadioButton("1", "radiocontrolconfiggroup");
    radioButton_control_config_1->setPosition(5,10);
    radioButton_control_config_1->setId("ControlCfg1");
    selConfigActionListener = new SelConfigActionListener();
    radioButton_control_config_1->addActionListener(selConfigActionListener);
    radioButton_control_config_2 = new gcn::UaeRadioButton("2", "radiocontrolconfiggroup");
    radioButton_control_config_2->setPosition(70,10);
    radioButton_control_config_2->setId("ControlCfg2");
    radioButton_control_config_2->addActionListener(selConfigActionListener);
    radioButton_control_config_3 = new gcn::UaeRadioButton("3", "radiocontrolconfiggroup");
    radioButton_control_config_3->setPosition(5,40);
    radioButton_control_config_3->setId("ControlCfg3");
    radioButton_control_config_3->addActionListener(selConfigActionListener);
    radioButton_control_config_4 = new gcn::UaeRadioButton("4", "radiocontrolconfiggroup");
    radioButton_control_config_4->setPosition(70,40);
    radioButton_control_config_4->setId("ControlCfg4");
    radioButton_control_config_4->addActionListener(selConfigActionListener);
    label1_control_config = new gcn::Label("control config scheme");
    label1_control_config->setPosition(10,70);
    label2_control_config = new gcn::Label("control config scheme");
    label2_control_config->setPosition(10,100);
#ifdef ANDROIDSDL
    label1_control_config->setFont(font14);
    label2_control_config->setFont(font14);
#endif
    group_control_config = new gcn::Window("Control config");
    group_control_config->setPosition(10,20);
    group_control_config->add(radioButton_control_config_1);
    group_control_config->add(radioButton_control_config_2);
    group_control_config->add(radioButton_control_config_3);
    group_control_config->add(radioButton_control_config_4);
    group_control_config->add(label1_control_config);
    group_control_config->add(label2_control_config);
    group_control_config->setMovable(false);
    group_control_config->setSize(145,145);
    group_control_config->setBaseColor(baseCol);

    // Select Statusline
    checkBox_statusline = new gcn::CheckBox("Status line");
    checkBox_statusline->setPosition(485,155);
    checkBox_statusline->setId("StatusLine");
    checkBox_statusline->setBaseColor(baseColLabel);
    statuslineActionListener = new StatuslineActionListener();
    checkBox_statusline->addActionListener(statuslineActionListener);

    // Select Joystick port
    radioButton_joy_port0 = new gcn::UaeRadioButton("Port0", "radiojoystickgroup");
    radioButton_joy_port0->setPosition(5,10);
    radioButton_joy_port0->setId("Port0");
    joystickActionListener = new JoystickActionListener();
    radioButton_joy_port0->addActionListener(joystickActionListener);
    radioButton_joy_port1 = new gcn::UaeRadioButton("Port1", "radiojoystickgroup");
    radioButton_joy_port1->setPosition(5,40);
    radioButton_joy_port1->setId("Port1");
    radioButton_joy_port1->addActionListener(joystickActionListener);
    radioButton_joy_both = new gcn::UaeRadioButton("Both", "radiojoystickgroup");
    radioButton_joy_both->setPosition(5,70);
    radioButton_joy_both->setId("Both");
    radioButton_joy_both->addActionListener(joystickActionListener);
    group_joystick = new gcn::Window("Joystick");
    group_joystick->setPosition(170,20);
    group_joystick->add(radioButton_joy_port0);
    group_joystick->add(radioButton_joy_port1);
    group_joystick->add(radioButton_joy_both);
    group_joystick->setMovable(false);
    group_joystick->setSize(80,115);
    group_joystick->setBaseColor(baseCol);

    // Select Autofire
    radioButton_autofirerate_light = new gcn::UaeRadioButton("Light", "radioautofirerategroup");
    radioButton_autofirerate_light->setPosition(5,10);
    radioButton_autofirerate_light->setId("Light");
    autofireActionListener = new AutofireActionListener();
    radioButton_autofirerate_light->addActionListener(autofireActionListener);
    radioButton_autofirerate_medium = new gcn::UaeRadioButton("Medium", "radioautofirerategroup");
    radioButton_autofirerate_medium->setPosition(5,40);
    radioButton_autofirerate_medium->setId("Medium");
    radioButton_autofirerate_medium->addActionListener(autofireActionListener);
    radioButton_autofirerate_heavy = new gcn::UaeRadioButton("Heavy", "radioautofirerategroup");
    radioButton_autofirerate_heavy->setPosition(5,70);
    radioButton_autofirerate_heavy->setId("Heavy");
    radioButton_autofirerate_heavy->addActionListener(autofireActionListener);
    group_autofirerate = new gcn::Window("Autofire rate");
    group_autofirerate->setPosition(265,20);
    group_autofirerate->add(radioButton_autofirerate_light);
    group_autofirerate->add(radioButton_autofirerate_medium);
    group_autofirerate->add(radioButton_autofirerate_heavy);
    group_autofirerate->setMovable(false);
    group_autofirerate->setSize(95,115);
    group_autofirerate->setBaseColor(baseCol);

    // Select Mouse multiplier
    radioButton_mouseMultiplier_25 = new gcn::UaeRadioButton(".25", "radiomouseMultipliergroup");
    radioButton_mouseMultiplier_25->setPosition(5,10);
    radioButton_mouseMultiplier_25->setId("Mouse.25");
    mouseMultActionListener = new MouseMultActionListener();
    radioButton_mouseMultiplier_25->addActionListener(mouseMultActionListener);
    radioButton_mouseMultiplier_50 = new gcn::UaeRadioButton(".5", "radiomouseMultipliergroup");
    radioButton_mouseMultiplier_50->setPosition(5,40);
    radioButton_mouseMultiplier_50->setId("Mouse.5");
    radioButton_mouseMultiplier_50->addActionListener(mouseMultActionListener);
    radioButton_mouseMultiplier_1 = new gcn::UaeRadioButton("1x", "radiomouseMultipliergroup");
    radioButton_mouseMultiplier_1->setPosition(5,70);
    radioButton_mouseMultiplier_1->setId("Mouse1x");
    radioButton_mouseMultiplier_1->addActionListener(mouseMultActionListener);
    radioButton_mouseMultiplier_2 = new gcn::UaeRadioButton("2x", "radiomouseMultipliergroup");
    radioButton_mouseMultiplier_2->setPosition(5,100);
    radioButton_mouseMultiplier_2->setId("Mouse2x");
    radioButton_mouseMultiplier_2->addActionListener(mouseMultActionListener);
    radioButton_mouseMultiplier_4 = new gcn::UaeRadioButton("4x", "radiomouseMultipliergroup");
    radioButton_mouseMultiplier_4->setPosition(5,130);
    radioButton_mouseMultiplier_4->setId("Mouse4x");
    radioButton_mouseMultiplier_4->addActionListener(mouseMultActionListener);
    group_mouseMultiplier = new gcn::Window("Mouse speed");
    group_mouseMultiplier->setPosition(375,20);
    group_mouseMultiplier->add(radioButton_mouseMultiplier_25);
    group_mouseMultiplier->add(radioButton_mouseMultiplier_50);
    group_mouseMultiplier->add(radioButton_mouseMultiplier_1);
    group_mouseMultiplier->add(radioButton_mouseMultiplier_2);
    group_mouseMultiplier->add(radioButton_mouseMultiplier_4);
    group_mouseMultiplier->setMovable(false);
    group_mouseMultiplier->setSize(100,175);
    group_mouseMultiplier->setBaseColor(baseCol);

#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
    // Select Tap delay
    radioButton_tapDelay_10 = new gcn::UaeRadioButton("normal", "radiotapdelaygroup");
    radioButton_tapDelay_10->setPosition(5,10);
    radioButton_tapDelay_10->setId("TapNormal");
    tapDelayActionListener = new TapDelayActionListener();
    radioButton_tapDelay_10->addActionListener(tapDelayActionListener);
    radioButton_tapDelay_5 = new gcn::UaeRadioButton("short", "radiotapdelaygroup");
    radioButton_tapDelay_5->setPosition(5,40);
    radioButton_tapDelay_5->setId("TapShort");
    radioButton_tapDelay_5->addActionListener(tapDelayActionListener);
    radioButton_tapDelay_2 = new gcn::UaeRadioButton("no", "radiotapdelaygroup");
    radioButton_tapDelay_2->setPosition(5,70);
    radioButton_tapDelay_2->setId("TapNo");
    radioButton_tapDelay_2->addActionListener(tapDelayActionListener);
    group_tapDelay = new gcn::Window("Tap delay");
    group_tapDelay->setPosition(490,20);
    group_tapDelay->add(radioButton_tapDelay_10);
    group_tapDelay->add(radioButton_tapDelay_5);
    group_tapDelay->add(radioButton_tapDelay_2);
    group_tapDelay->setMovable(false);
    group_tapDelay->setSize(90,115);
    group_tapDelay->setBaseColor(baseCol);

    // Stylus Offset
    label_offset = new gcn::Label("Stylus offset");
    label_offset->setPosition(4, 2);
    backgrd_offset = new gcn::Container();
    backgrd_offset->setOpaque(true);
    backgrd_offset->setBaseColor(baseColLabel);
    backgrd_offset->setPosition(375, 215);
    backgrd_offset->setSize(105, 21);
    backgrd_offset->add(label_offset);
    dropDown_offset = new gcn::UaeDropDown(&offsetList);
    dropDown_offset->setPosition(490,215);
    dropDown_offset->setWidth(90);
    dropDown_offset->setBaseColor(baseCol);
    dropDown_offset->setId("StylusOffset");
    offsetActionListener = new OffsetActionListener();
    dropDown_offset->addActionListener(offsetActionListener);
#endif

    tab_control = new gcn::Container();
    tab_control->add(icon_winlogo);
    tab_control->add(group_control_config);
    tab_control->add(group_joystick);
    tab_control->add(checkBox_statusline);
    tab_control->add(group_autofirerate);
    tab_control->add(group_mouseMultiplier);
#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
    tab_control->add(group_tapDelay);
    tab_control->add(backgrd_offset);
    tab_control->add(dropDown_offset);
#endif
    tab_control->setSize(600,280);
    tab_control->setBaseColor(baseCol);
}


void menuTabControl_Exit()
{
    delete tab_control;
    delete group_control_config;
    delete radioButton_control_config_1;
    delete radioButton_control_config_2;
    delete radioButton_control_config_3;
    delete radioButton_control_config_4;

    delete checkBox_statusline;

    delete label1_control_config;
    delete label2_control_config;
    delete group_joystick;
    delete radioButton_joy_port0;
    delete radioButton_joy_port1;
    delete radioButton_joy_both;
    delete group_autofirerate;
    delete radioButton_autofirerate_light;
    delete radioButton_autofirerate_medium;
    delete radioButton_autofirerate_heavy;
    delete group_mouseMultiplier;
    delete radioButton_mouseMultiplier_25;
    delete radioButton_mouseMultiplier_50;
    delete radioButton_mouseMultiplier_1;
    delete radioButton_mouseMultiplier_2;
    delete radioButton_mouseMultiplier_4;
#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
    delete group_tapDelay;
    delete radioButton_tapDelay_10;
    delete radioButton_tapDelay_5;
    delete radioButton_tapDelay_2;
    delete backgrd_offset;
    delete label_offset;
    delete dropDown_offset;
#endif

    delete selConfigActionListener;
    delete joystickActionListener;
    delete statuslineActionListener;
    delete autofireActionListener;
    delete mouseMultActionListener;
#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
    delete tapDelayActionListener;
    delete offsetActionListener;
#endif
}


void show_settings_TabControl()
{
    if (mainMenu_joyConf==0) {
        radioButton_control_config_1->setSelected(true);
        label1_control_config->setCaption("A=Autofire X=Fire");
        label2_control_config->setCaption("Y=Space B=2nd");
    } else if (mainMenu_joyConf==1) {
        radioButton_control_config_2->setSelected(true);
        label1_control_config->setCaption("A=Fire X=Autofire");
        label2_control_config->setCaption("Y=Space B=2nd");
    } else if (mainMenu_joyConf==2) {
        radioButton_control_config_3->setSelected(true);
        label1_control_config->setCaption("A=Autofire X=Jump");
        label2_control_config->setCaption("Y=Fire B=2nd");
    } else if (mainMenu_joyConf==3) {
        radioButton_control_config_4->setSelected(true);
        label1_control_config->setCaption("A=Fire X=Jump");
        label2_control_config->setCaption("Y=Autofire B=2nd");
    }

    if (mainMenu_joyPort==1)
        radioButton_joy_port0->setSelected(true);
    else if (mainMenu_joyPort==2)
        radioButton_joy_port1->setSelected(true);
    else if (mainMenu_joyPort==0)
        radioButton_joy_both->setSelected(true);


    if (mainMenu_showStatus)
        checkBox_statusline->setSelected(true);
    else
        checkBox_statusline->setSelected(false);

    if (mainMenu_autofireRate==12)
        radioButton_autofirerate_light->setSelected(true);
    else if (mainMenu_autofireRate==8)
        radioButton_autofirerate_medium->setSelected(true);
    else if (mainMenu_autofireRate==4)
        radioButton_autofirerate_heavy->setSelected(true);

    if (mainMenu_mouseMultiplier==25)
        radioButton_mouseMultiplier_25->setSelected(true);
    else if (mainMenu_mouseMultiplier==50)
        radioButton_mouseMultiplier_50->setSelected(true);
    else if (mainMenu_mouseMultiplier==1)
        radioButton_mouseMultiplier_1->setSelected(true);
    else if (mainMenu_mouseMultiplier==2)
        radioButton_mouseMultiplier_2->setSelected(true);
    else if (mainMenu_mouseMultiplier==4)
        radioButton_mouseMultiplier_4->setSelected(true);

#if defined(PANDORA) && !(defined(AROS) || defined(WIN32) || defined (RASPBERRY))
    if (mainMenu_tapDelay==10)
        radioButton_tapDelay_10->setSelected(true);
    else if (mainMenu_tapDelay==5)
        radioButton_tapDelay_5->setSelected(true);
    else if (mainMenu_tapDelay==2)
        radioButton_tapDelay_2->setSelected(true);

    if(dropDown_offset->getSelected() != mainMenu_stylusOffset / 2)
        dropDown_offset->setSelected(mainMenu_stylusOffset / 2);
#endif
}

}

